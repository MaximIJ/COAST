#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include <llvm/Pass.h>
#include <llvm/PassSupport.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>
#include "llvm/Support/raw_ostream.h"
#include <llvm/IR/Constants.h>
#include <llvm/Support/Debug.h>
#include <llvm/Support/CommandLine.h>

namespace {

    struct SmallProfileNewPMPass : PassInfoMixin<SmallProfileNewPMPass> {
      PreservedAnalyses run(Module &M, ModuleAnalysisManager &) {
        errs() << "SmallProfile Pass starting on module: " << M.getName() << "\n";
    
        Type* type_i32 = Type::getInt32Ty(M.getContext());
        Function* mainFunc = nullptr;
        ReturnInst* mainReturn = nullptr;
        std::vector<std::pair<Function*, GlobalVariable*> > profPairs;
        std::set<Function*> funcsToLookFor;
    
        // Helper functions as lambdas
        auto createGlobalCounter = [&](Function* fn) -> GlobalVariable* {
            std::string glblNm = "__" + fn->getName().str() + "_profCnt";
            GlobalVariable* nextCnt = M.getGlobalVariable(StringRef(glblNm));
    
            if (nextCnt == nullptr) {
                nextCnt = cast<GlobalVariable>(M.getOrInsertGlobal(StringRef(glblNm), type_i32));
                nextCnt->setConstant(false);
                nextCnt->setInitializer(ConstantInt::getNullValue(type_i32));
                nextCnt->setUnnamedAddr(GlobalValue::UnnamedAddr::Global);
                nextCnt->setAlignment(Align(4));
                profPairs.push_back(std::make_pair(fn, nextCnt));
            }
            return nextCnt;
        };
    
        auto incrementCounter = [&](GlobalVariable* cntr, Instruction* insertHere, bool extCall) {
            LoadInst* LI = new LoadInst(type_i32, cntr, "cntLoad", false, Align(1));
            Constant* one = ConstantInt::get(LI->getType(), 1, false);
            BinaryOperator* BI = BinaryOperator::CreateAdd(LI, one, "incCnt");
            StoreInst* SI = new StoreInst(BI, cntr, false, Align(1));
            
            LI->insertBefore(insertHere);
            BI->insertAfter(LI);
            SI->insertAfter(BI);
    
            if (extCall) {
                if (auto dbgLoc = insertHere->getDebugLoc()) {
                    LI->setDebugLoc(dbgLoc);
                    BI->setDebugLoc(dbgLoc);
                    SI->setDebugLoc(dbgLoc);
                }
            }
        };
    
        // Profile local functions
        for (auto &F : M) {
            StringRef fnName = F.getName();
            if (fnName.startswith("llvm.dbg") || fnName.startswith("llvm.lifetime."))
                continue;
    
            if(F.empty()) {
                funcsToLookFor.insert(&F);
            } else {
                GlobalVariable* nextCnt = createGlobalCounter(&F);
                Instruction* insertHere = F.getEntryBlock().getFirstNonPHIOrDbg();
                if (isa<LandingPadInst>(insertHere)) {
                    insertHere = insertHere->getNextNode();
                }
                incrementCounter(nextCnt, insertHere, false);
            }
    
            if (F.getName() == "main") {
                mainFunc = &F;
                for(auto &bb : F){
                    if(ReturnInst* RI = dyn_cast<ReturnInst>(bb.getTerminator())){
                        mainReturn = RI;
                    }
                }
            }
        }
    
        // Profile external functions
        for (auto &F : M) {
            for (auto &bb : F) {
                for (auto &I : bb) {
                    if (CallInst* CI = dyn_cast<CallInst>(&I)) {
                        Function* calledF = CI->getCalledFunction();
                        if (!calledF) continue;
    
                        if (funcsToLookFor.find(calledF) != funcsToLookFor.end()) {
                            GlobalVariable* nextCnt = createGlobalCounter(calledF);
                            incrementCounter(nextCnt, CI, true);
                        }
                    }
                }
            }
        }
    
        bool changed = !profPairs.empty();
        errs() << "SmallProfile Pass finished. Changed: " << (changed ? "YES" : "NO") << "\n";
    
        return changed ? PreservedAnalyses::none() : PreservedAnalyses::all();
      }
    };
    
} // namespace
    
extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
  errs() << "SmallProfile Plugin Info requested - registering passes\n";

  return {LLVM_PLUGIN_API_VERSION, "SmallProfile_Plugin", "1.0",
          [](PassBuilder &PB) {
            errs() << "Registering SmallProfile pass in pipeline\n";

            PB.registerPipelineParsingCallback(
                [](StringRef Name, ModulePassManager &MPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  errs() << "Pipeline callback called with: '" << Name << "'\n";

                  if (Name == "smallprofile") {
                    errs() << "Adding SmallProfile pass to pipeline\n";
                    MPM.addPass(SmallProfileNewPMPass());
                    return true;
                  }
                  return false;
                });
          }};
}