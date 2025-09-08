#include "../dataflowProtection/dataflowProtection.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {

    struct DWCNewPMPass : PassInfoMixin<DWCNewPMPass> {
      PreservedAnalyses run(Module &M, ModuleAnalysisManager &) {
        errs() << "DWC Pass starting on module: " << M.getName() << "\n";
    
        dataflowProtection DP;
        bool changed = DP.run(M, 2); // DWC mode
    
        errs() << "DWC Pass finished. Changed: " << (changed ? "YES" : "NO") << "\n";
    
        return PreservedAnalyses::none();
      }
    };
    
} // namespace
    
extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
  errs() << "DWC Plugin Info requested - registering passes\n";

  return {LLVM_PLUGIN_API_VERSION, "DWC_Plugin", "1.0",
          [](PassBuilder &PB) {
            errs() << "Registering DWC pass in pipeline\n";

            PB.registerPipelineParsingCallback(
                [](StringRef Name, ModulePassManager &MPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  errs() << "Pipeline callback called with: '" << Name << "'\n";

                  if (Name == "dwc") {
                    errs() << "Adding DWC pass to pipeline\n";
                    MPM.addPass(DWCNewPMPass());
                    return true;
                  }
                  return false;
                });
          }};
}