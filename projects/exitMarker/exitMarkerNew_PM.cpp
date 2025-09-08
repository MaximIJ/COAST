#include "../dataflowProtection/dataflowProtection.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include <llvm/Pass.h>
#include <llvm/PassSupport.h>
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include <llvm/IR/Constants.h>
#include <llvm/Support/Debug.h>

using namespace llvm;

namespace {

    struct ExitMarkerNewPMPass : PassInfoMixin<ExitMarkerNewPMPass> {
      PreservedAnalyses run(Module &M, ModuleAnalysisManager &) {
        errs() << "ExitMarker Pass starting on module: " << M.getName() << "\n";
    
        Function* mainFn = nullptr;
        std::vector<ReturnInst*> returnInsts;
    
        // Get main function and its return instructions
        for(auto &F : M){
            if(F.getName() == "main"){
                mainFn = &F;
    
                for(auto &bb : F){
                    if(ReturnInst* RI = dyn_cast<ReturnInst>(bb.getTerminator())){
                        returnInsts.push_back(RI);
                    }
                }
                break;
            }
        }
    
        if (!mainFn) {
            errs() << "No main function found, skipping ExitMarker\n";
            return PreservedAnalyses::all();
        }
    
        // Assemble the proper function type
        std::vector<Type*> params;
        params.push_back(mainFn->getReturnType());
        ArrayRef<Type*> args(params);
        FunctionType* markerTy = FunctionType::get(mainFn->getReturnType(),args,false);
    
        // Create EXIT_MARKER function
        auto callee = M.getOrInsertFunction("EXIT_MARKER", markerTy);
        Function* exitMarkerFn = dyn_cast<Function>(callee.getCallee());
        assert(exitMarkerFn && "Exit marker function is non-void");
    
        // Create a basic block that returns the value passed into it
        if (exitMarkerFn->empty()) {
            BasicBlock* bb = BasicBlock::Create(M.getContext(), Twine("entry"), exitMarkerFn, NULL);
            assert(exitMarkerFn->arg_size() == 1);
            Argument* arg = &*(exitMarkerFn->arg_begin());
            ReturnInst* term = ReturnInst::Create(M.getContext(), arg, bb);
        }
    
        // Insert the call instructions before all return instructions
        bool changed = false;
        for(auto &I : returnInsts){
            CallInst* exitMarkerCall = CallInst::Create(exitMarkerFn, I->getReturnValue(), "", I);
            changed = true;
        }
    
        errs() << "ExitMarker Pass finished. Changed: " << (changed ? "YES" : "NO") << "\n";
    
        return changed ? PreservedAnalyses::none() : PreservedAnalyses::all();
      }
    };
    
} // namespace
    
extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
  errs() << "ExitMarker Plugin Info requested - registering passes\n";

  return {LLVM_PLUGIN_API_VERSION, "ExitMarker_Plugin", "1.0",
          [](PassBuilder &PB) {
            errs() << "Registering ExitMarker pass in pipeline\n";

            PB.registerPipelineParsingCallback(
                [](StringRef Name, ModulePassManager &MPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  errs() << "Pipeline callback called with: '" << Name << "'\n";

                  if (Name == "exitmarker") {
                    errs() << "Adding ExitMarker pass to pipeline\n";
                    MPM.addPass(ExitMarkerNewPMPass());
                    return true;
                  }
                  return false;
                });
          }};
}