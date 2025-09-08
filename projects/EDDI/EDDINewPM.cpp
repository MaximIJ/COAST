#include "../dataflowProtection/dataflowProtection.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {

    struct EDDINewPMPass : PassInfoMixin<EDDINewPMPass> {
      PreservedAnalyses run(Module &M, ModuleAnalysisManager &) {
        errs() << "\n------------------------------------------------\n";
        errs() << "---    WARNING: EDDI is deprecated.          ---\n";
        errs() << "---             Please use DWC instead.      ---\n";
        errs() << "------------------------------------------------\n\n";
    
        errs() << "EDDI Pass starting on module: " << M.getName() << "\n";
    
        // EDDI is deprecated, but we redirect to DWC for compatibility
        dataflowProtection DP;
        bool changed = DP.run(M, 2); // Same as DWC mode
    
        errs() << "EDDI Pass finished. Changed: " << (changed ? "YES" : "NO") << "\n";
    
        return PreservedAnalyses::none();
      }
    };
    
} // namespace
    
extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
  errs() << "EDDI Plugin Info requested - registering passes\n";

  return {LLVM_PLUGIN_API_VERSION, "EDDI_Plugin", "1.0",
          [](PassBuilder &PB) {
            errs() << "Registering EDDI pass in pipeline\n";

            PB.registerPipelineParsingCallback(
                [](StringRef Name, ModulePassManager &MPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  errs() << "Pipeline callback called with: '" << Name << "'\n";

                  if (Name == "eddi") {
                    errs() << "Adding EDDI pass to pipeline\n";
                    MPM.addPass(EDDINewPMPass());
                    return true;
                  }
                  return false;
                });
          }};
}