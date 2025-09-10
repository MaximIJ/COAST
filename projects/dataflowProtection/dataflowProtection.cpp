#define DEBUG_TYPE "dataflowProtection"

#include "dataflowProtection.h"

#include <llvm/Support/raw_ostream.h>
#include "llvm/Support/CommandLine.h"

using namespace llvm;

//--------------------------------------------------------------------------//
// Command line options are now defined in commandLineOptions.cpp
// to avoid multiple registration when multiple plugins are loaded
//--------------------------------------------------------------------------//

// External declarations for command line options
extern cl::opt<bool> verboseFlag;


//--------------------------------------------------------------------------//
// Top level behavior
//--------------------------------------------------------------------------//
char dataflowProtection::ID = 0;
static RegisterPass<dataflowProtection> X("DataflowProtection",
		"Insert copies of IR to protect dataflow", false, false);

bool dataflowProtection::runOnModule(Module &M) {
	// Needed for the dataflowProtection pass to compile, never used in practice
	run(M,2);
	return true;
}

bool dataflowProtection::run(Module &M, int numClones) {
	llvm::outs() << "Running dataflowProtection passssss\n";
	// Remove user functions that are never called in the module to reduce code size, processing time
	// These are mainly inlined by prior optimizations
	if (verboseFlag)
		PRINT_STRING("The following functions are unused, removing them:");
	removeUnusedFunctions(M);

	// Process user commands inside of the source code
	// Must happen before processCommandLine to make sure we don't clone things if not needed
	processAnnotations(M);

	// Make original globals PIE compatible before cloning
	makeGlobalsPIECompatible(M);

	// Remove annotations here so they aren't cloned
	removeAnnotations(M);

	// Make sure that the command line options are correct
	processCommandLine(M, numClones);

	// Populate the list of functions to touch
	populateFnWorklist(M);

	// First figure out which instructions are going to be cloned
	populateValuesToClone(M);

	// validate that the configuration parameters can be followed safely
	verifyOptions(M);

	// Now add new arguments to functions
	// (In LLVM you can't change a function signature, so we have to make new functions)
	// populateValuesToClone has to be called before this so we know which
	// instructions are cloned, and thus when functions need to have extra arguments
	cloneFunctionArguments(M);
	cloneFunctionReturnVals(M);

	// deal with function wrappers
	updateFnWrappers(M);

	// Parse the annotations on local variables within functions so that
	//  list of values to clone is up to date
	processLocalAnnotations(M);
	removeLocalAnnotations(M);

	// Once again figure out which instructions are going to be cloned
	// This need to be re-run after creating the new functions as the old
	// pointers will be stale
	populateValuesToClone(M);

	// Do the actual cloning
	cloneGlobals(M);
	cloneConstantExpr();
	cloneInsns();
	verifyPIECompatibility(M);

	// Change clones to depend on the duplications
	updateCallInsns(M);
	updateInvokeInsns(M);

	// Insert error detection/handling
	insertErrorFunction(M, numClones);
	createErrorBlocks(M, numClones);

	// Determine where synchronization logic needs to be
	populateSyncPoints(M);

	// Insert synchronization statements
	processSyncPoints(M, numClones);

	// Global runtime initialization
	addGlobalRuntimeInit(M);
	updateRRFuncs(M);

	// stack protection
	insertStackProtection(M);

	// Clean up
	removeUnusedErrorBlocks(M);
	checkForUnusedClones(M);
	removeOrigFunctions();
	removeUnusedGlobals(M);

	// This is executed if code is segmented instead of interleaved
	moveClonesToEndIfSegmented(M);

	if (verboseFlag)
		PRINT_STRING("Removing unused functions...");
	/*
	 * Final check for unused functions.
	 * It's possible that there are circular dependencies here.
	 * For example, not removing a function because it's still used in a call,
	 *  but then removing the function that had that call in it right after.
	 * Keep calling until nothing new is removed.
	 */
	int numRemoved = 0;
	do {
		numRemoved = removeUnusedFunctions(M);
	} while (numRemoved > 0);
	// Make sure old calls to functions with replicated return values are removed
	validateRRFuncs();

	// Option executed when -dumpModule is passed in
	dumpModule(M);

	return true;
}

// set pass dependencies
void dataflowProtection::getAnalysisUsage(AnalysisUsage& AU) const {
	ModulePass::getAnalysisUsage(AU);
}
