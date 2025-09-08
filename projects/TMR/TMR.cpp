#include "../dataflowProtection/dataflowProtection.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {

struct TMRPass : PassInfoMixin<TMRPass> {
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &) {
    errs() << "TMR Pass starting on module: " << M.getName() << "\n";

    dataflowProtection DP;
    bool changed = DP.run(M, 3); // TMR mode

    errs() << "TMR Pass finished. Changed: " << (changed ? "YES" : "NO") << "\n";

    return PreservedAnalyses::none();
  }
};

} // namespace

extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
  errs() << "TMR Plugin Info requested - registering passes\n";

  return {LLVM_PLUGIN_API_VERSION, "TMR_Plugin", "1.0",
          [](PassBuilder &PB) {
            errs() << "Registering TMR pass in pipeline\n";

            PB.registerPipelineParsingCallback(
                [](StringRef Name, ModulePassManager &MPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  errs() << "Pipeline callback called with: '" << Name << "'\n";

                  if (Name == "tmr") {
                    errs() << "Adding TMR pass to pipeline\n";
                    MPM.addPass(TMRPass());
                    return true;
                  }
                  return false;
                });
          }};
}
