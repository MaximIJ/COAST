//--------------------------------------------------------------------------//
// Command line options for the dataflow protection passes
//--------------------------------------------------------------------------//
// This file contains the definitions of all command line options used by
// the dataflow protection system. It's compiled separately to avoid
// multiple registration when multiple plugins are loaded.

#include "llvm/Support/CommandLine.h"

using namespace llvm;

//--------------------------------------------------------------------------//
// Command line options for the pass
//--------------------------------------------------------------------------//
// Replication rules
cl::opt<bool> noMemReplicationFlag ("noMemReplication", cl::desc("Do not duplicate variables in memory"));
cl::opt<bool> noLoadSyncFlag ("noLoadSync", cl::desc("Do not synchronize on data loads"));
cl::opt<bool> noStoreDataSyncFlag ("noStoreDataSync", cl::desc("Do not synchronize data on data stores"));
cl::opt<bool> noStoreAddrSyncFlag ("noStoreAddrSync", cl::desc("Do not synchronize address on data stores"));
cl::opt<bool> storeDataSyncFlag ("storeDataSync", cl::desc("Force synchronize data on data stores (not default)"));

// Replication scope
// note: any changes to list names must also be changed at the top of interface.cpp
cl::list<std::string> skipFnCl ("ignoreFns", cl::desc("Specify function to not protect. Defaults to none."), cl::CommaSeparated, cl::ZeroOrMore);
cl::list<std::string> ignoreGlblCl ("ignoreGlbls", cl::desc("Specify global variables to not protect. Defaults to none."), cl::CommaSeparated, cl::ZeroOrMore);
cl::list<std::string> skipLibCallsCl ("skipLibCalls", cl::desc("Specify library calls to not clone. Defaults to none."), cl::CommaSeparated, cl::ZeroOrMore);
cl::list<std::string> replicateUserFunctionsCallCl ("replicateFnCalls", cl::desc("Specify user calls where the call, not the function body, should be triplicated. Defaults to none."), cl::CommaSeparated, cl::ZeroOrMore);
cl::list<std::string> isrFunctionListCl ("isrFunctions", cl::desc("These functions are considered Interrupt Service Handlers and will be treated differently."), cl::CommaSeparated, cl::ZeroOrMore);
// should also be able to specify functions/globals to clone from command line
cl::list<std::string> cloneFnCl ("cloneFns", cl::desc("Specify function(s) to protect. Defaults to none."), cl::CommaSeparated, cl::ZeroOrMore);
cl::list<std::string> cloneGlblCl ("cloneGlbls", cl::desc("Specify global(s) to protect. Defaults to none."), cl::CommaSeparated, cl::ZeroOrMore);
// specify function names which should return multiple values
cl::list<std::string> replReturnCl ("cloneReturn", cl::desc("Specify function(s) which should return multiple values. Defaults to none."), cl::CommaSeparated, cl::ZeroOrMore);
cl::list<std::string> cloneAfterCallCl ("cloneAfterCall", cl::desc("Specify function(s) of which the argument(s) should be cloned after the function is called once (ie. scanf)"), cl::CommaSeparated, cl::ZeroOrMore);
cl::list<std::string> protectedLibCl ("protectedLibFn", cl::desc("Specify function(s) which should be treated as protected library functions."), cl::CommaSeparated, cl::ZeroOrMore);

// Other options
cl::opt<std::string> configFileLocation ("configFile", cl::desc("Location of configuration file"));
cl::opt<bool> ReportErrorsFlag ("countErrors", cl::desc("Instrument TMR'd code so it counts the number of corrections"), cl::value_desc("TMR error counting"));
cl::opt<bool> OriginalReportErrorsFlag ("reportErrors", cl::desc("Instrument TMR'd code so it reports if TMR corrected an error (deprecated)"), cl::value_desc("TMR error signaling (deprecated)"));
cl::opt<bool> InterleaveFlag ("i", cl::desc("Interleave instructions, rather than segmenting within a basic block. Default behavior."));
cl::opt<bool> SegmentFlag ("s", cl::desc("Segment instructions, rather than interleaving within a basic block"));
cl::list<std::string> globalsToRuntimeInitCl ("runtimeInitGlobals", cl::CommaSeparated, cl::ZeroOrMore);
cl::opt<bool> dumpModuleFlag ("dumpModule", cl::desc("Print out the module immediately before pass concludes. Option is for pass debugging."));
cl::opt<bool> verboseFlag ("verbose", cl::desc("Increase the amount of output"));
cl::opt<bool> noMainFlag ("noMain", cl::desc("There is no 'main' function in this module"));
cl::opt<bool> noCloneOperandsCheckFlag ("noCloneOpsCheck", cl::desc("Continue compilation even if instruction operands weren't correctly cloned."));
cl::opt<bool> countSyncsFlag ("countSyncs", cl::desc("Dynamic count of synchronization points"));
cl::opt<bool> protectStackFlag ("protectStack", cl::desc("Vote on values of return address and frame pointer before returning from function call."));
