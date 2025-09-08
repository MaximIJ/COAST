#define DEBUG_TYPE "DWC"

#include "../dataflowProtection/dataflowProtection.h"

#include <llvm/Pass.h>
#include <llvm/IR/PassManager.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>

#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/Debug.h>

using namespace llvm;

//--------------------------------------------------------------------------//
// Legacy Pass (for compatibility)
//--------------------------------------------------------------------------//
class DWC : public ModulePass {
public:
  static char ID;
  DWC() : ModulePass(ID) {}

  bool runOnModule(Module &M) override;
  void getAnalysisUsage(AnalysisUsage& AU) const override;
};

char DWC::ID = 0;
static RegisterPass<DWC> X("DWC",
		"Full DWC coverage pass", false, false);

bool DWC::runOnModule(Module &M) {
	dataflowProtection DP;
	DP.run(M,2);
	return true;
}

//set pass dependencies
void DWC::getAnalysisUsage(AnalysisUsage& AU) const {
	ModulePass::getAnalysisUsage(AU);
}

//--------------------------------------------------------------------------//
// New Pass Manager Implementation
//--------------------------------------------------------------------------//
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