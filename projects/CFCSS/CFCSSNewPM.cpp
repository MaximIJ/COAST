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
#include <list>
#include <vector>

using namespace llvm;

namespace {

    struct CFCSSNewPMPass : PassInfoMixin<CFCSSNewPMPass> {
      PreservedAnalyses run(Module &M, ModuleAnalysisManager &) {
        errs() << "CFCSS Pass starting on module: " << M.getName() << "\n";
    
        // Helper lambda for checking if function should be skipped
        auto shouldSkipF = [](StringRef name) -> bool {
            if(name.size() == 0) return false;
            std::list<StringRef> skipFList = {StringRef("EDDI_FAULT_DETECTED"),
                    StringRef("CF_FAULT_DETECTED")};
            for(StringRef sr : skipFList){
                if(sr == name) return true;
            }
            return false;
        };
    
        // Helper lambda for creating error blocks
        auto createErrorBlocks = [&](Function &F) {
            Module* Mod = F.getParent();
            auto FTy = FunctionType::get(Type::getVoidTy(Mod->getContext()), /*isVarArg=*/false);
            auto callee = Mod->getOrInsertFunction("FAULT_DETECTED_CFC", FTy);
            Function* cfFn = dyn_cast<Function>(callee.getCallee());
    
            BasicBlock* lastBlock = &(F.back());
            BasicBlock* errBlock = BasicBlock::Create(lastBlock->getContext(),
                    "CFerrorHandler." + Twine(F.getName()), &F, lastBlock);
            errBlock->moveAfter(lastBlock);
    
            CallInst* cfFailCall = CallInst::Create(cfFn, "", errBlock);
            UnreachableInst* term = new UnreachableInst(errBlock->getContext(), errBlock);
        };
    
        int BBCount = 0;
        bool changed = false;
    
        // Process each function
        for(auto &F : M){
            // Create error blocks if needed
            if(!F.empty() && !shouldSkipF(F.getName())) {
                createErrorBlocks(F);
                changed = true;
            }
    
            // Count basic blocks and analyze control flow
            for(auto & BB : F){
                BBCount++;
                
                // Process instructions in each basic block
                for(auto &I : BB){
                    if(CallInst* CallI = dyn_cast<CallInst>(&I)){
                        // Process call instructions for control flow analysis
                        // This is where signature checking would be implemented
                    }
                    if(ReturnInst* RetI = dyn_cast<ReturnInst>(&I)){
                        if(F.getName() != "main"){
                            // Process return instructions for control flow analysis
                        }
                    }
                }
            }
        }
    
        errs() << "CFCSS: Processed " << BBCount << " basic blocks\n";
        errs() << "CFCSS Pass finished. Changed: " << (changed ? "YES" : "NO") << "\n";
    
        return changed ? PreservedAnalyses::none() : PreservedAnalyses::all();
      }
    };
    
} // namespace
    
extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
  errs() << "CFCSS Plugin Info requested - registering passes\n";

  return {LLVM_PLUGIN_API_VERSION, "CFCSS_Plugin", "1.0",
          [](PassBuilder &PB) {
            errs() << "Registering CFCSS pass in pipeline\n";

            PB.registerPipelineParsingCallback(
                [](StringRef Name, ModulePassManager &MPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  errs() << "Pipeline callback called with: '" << Name << "'\n";

                  if (Name == "cfcss") {
                    errs() << "Adding CFCSS pass to pipeline\n";
                    MPM.addPass(CFCSSNewPMPass());
                    return true;
                  }
                  return false;
                });
          }};
}