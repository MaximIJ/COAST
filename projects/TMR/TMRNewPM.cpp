#include "../dataflowProtection/dataflowProtection.h"
#include "llvm/IR/Module.h"

#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

using namespace llvm;

namespace {

struct TMRPass : PassInfoMixin<TMRPass> {
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &) {
    dataflowProtection DP;
    DP.run(M, 3); // Triplication
    return PreservedAnalyses::none();
  }
};

struct DWCNewPMPass : PassInfoMixin<DWCNewPMPass> {
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &) {
    dataflowProtection DP;
    DP.run(M, 2); // Duplication with compare
    return PreservedAnalyses::none();
  }
};

} // namespace

extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "COAST_TMR_DWC", "0.1",
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, ModulePassManager &MPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "tmr") {
                    MPM.addPass(TMRPass());
                    return true;
                  }
                  if (Name == "dwc") {
                    MPM.addPass(DWCNewPMPass());
                    return true;
                  }
                  return false;
                });
          }};
}



