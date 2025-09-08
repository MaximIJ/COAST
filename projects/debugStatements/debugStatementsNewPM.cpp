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

    struct DebugStatementsNewPMPass : PassInfoMixin<DebugStatementsNewPMPass> {
      PreservedAnalyses run(Module &M, ModuleAnalysisManager &) {
        errs() << "DebugStatements Pass starting on module: " << M.getName() << "\n";
    
        // Get a reference to the print statement - LLVM 16 compatible
        Type *charPointerType = PointerType::get(IntegerType::get(M.getContext(), 8), 0);
        Type* type_i32 = Type::getInt32Ty(M.getContext());
        FunctionType *printfTy = FunctionType::get(type_i32, std::vector<Type*> (1, charPointerType), true);
        auto callee = M.getOrInsertFunction("printf", printfTy);
        Function* print = dyn_cast<Function>(callee.getCallee());
        assert(print && "Print function not defined");
    
        // Define constant strings
        StringRef arrow = StringRef("-->");
        StringRef newLineChar = StringRef("\n");
    
        bool specificFlag = (fnPrintList.size() > 0);
        bool changed = false;
    
        for (auto &F : M) {
            if (F.empty())
                continue;
    
            // if there's something in the list, and this function isn't, continue
            if (specificFlag && (std::find(fnPrintList.begin(), fnPrintList.end(), F.getName().str()) == fnPrintList.end()) ) {
                continue;
            }
    
            // Helper lambda to create GEP for print
            auto getGEPforPrint = [&](StringRef* varName, BasicBlock* bb) -> GetElementPtrInst* {
                Type* type_i8 = Type::getInt8Ty(M.getContext());
                ArrayType * type_i8_array = ArrayType::get(type_i8,(unsigned long long int)(varName->size()+1));
                Constant * dataInit = ConstantDataArray::getString(M.getContext(), *varName);
    
                GlobalVariable * globalVal = dyn_cast<GlobalVariable>(
                        M.getOrInsertGlobal(*varName, type_i8_array));
                globalVal->setConstant(true);
                globalVal->setInitializer(dataInit);
                globalVal->setLinkage(GlobalVariable::PrivateLinkage);
                globalVal->setUnnamedAddr(GlobalValue::UnnamedAddr::Global);
                globalVal->setAlignment(Align(1));
    
                ConstantInt* zeroCI = ConstantInt::get(IntegerType::getInt32Ty(M.getContext()),0,false);
                Value* zeroVal = dyn_cast<Value>(zeroCI);
    
                std::vector<Value*> gepArgs;
                gepArgs.push_back(zeroVal);
                gepArgs.push_back(zeroVal);
                ArrayRef<Value*>* gepArgsArray = new ArrayRef<Value*>(gepArgs);
    
                GetElementPtrInst* gep = GetElementPtrInst::CreateInBounds(type_i8_array,
                        globalVal,*gepArgsArray,varName->str(),bb->getTerminator());
                return gep;
            };
    
            BasicBlock* entryBlock = &F.getEntryBlock();
            StringRef fnName = F.getName();
            std::vector<Value*> fnArgs;
            std::vector<Value*> argsArrow;
            std::vector<Value*> newlineArgs;
    
            // Arguments for printing the function name
            GetElementPtrInst* fnGEP = getGEPforPrint(&fnName, entryBlock);
            fnArgs.push_back(fnGEP);
            ArrayRef<Value*>* fnCallArgs = new ArrayRef<Value*>(fnArgs);
    
            // Arguments for printing an arrow
            GetElementPtrInst* arrowGEP = getGEPforPrint(&arrow, entryBlock);
            argsArrow.push_back(arrowGEP);
            ArrayRef<Value*>* callArgsArrow = new ArrayRef<Value*>(argsArrow);
    
            // Arguments for printing a new line
            GetElementPtrInst* newlineGEP = getGEPforPrint(&newLineChar, entryBlock);
            newlineArgs.push_back(newlineGEP);
            ArrayRef<Value*>* callArgsNewline = new ArrayRef<Value*>(newlineArgs);
    
            for (auto & bb : F) {
                StringRef bbName = bb.getName();
                BasicBlock* currBB = &bb;
                std::vector<Value*> bbArgs;
    
                // Arguments for printing the name of the basic block
                GetElementPtrInst* bbGEP = getGEPforPrint(&bbName, currBB);
                bbArgs.push_back(bbGEP);
                ArrayRef<Value*>* bbCallArgs = new ArrayRef<Value*>(bbArgs);
    
                // Create all the function calls: function arrow bb newline
                Instruction* insertionPoint = bb.getFirstNonPHI();
                if (isa<LandingPadInst>(insertionPoint)) {
                    insertionPoint = insertionPoint->getNextNode();
                }
                CallInst* newlinePrint = CallInst::Create(print, *callArgsNewline, "", insertionPoint);
                CallInst* bbPrint = CallInst::Create(print, *bbCallArgs, "", newlinePrint);
                CallInst* arrowPrint = CallInst::Create(print, *callArgsArrow, "", bbPrint);
                CallInst* fnNamePrint = CallInst::Create(print, *fnCallArgs, "", arrowPrint);
    
                // Make bbGEP dominate all uses
                bbGEP->moveBefore(fnNamePrint);
                changed = true;
            }
    
            // Move all the GEPs to the front of the entry block to dominate all uses
            Instruction* insertionPoint = entryBlock->getFirstNonPHI();
            if (isa<LandingPadInst>(insertionPoint)) {
                insertionPoint = insertionPoint->getNextNode();
            }
            newlineGEP->moveBefore(insertionPoint);
            arrowGEP->moveBefore(newlineGEP);
            fnGEP->moveBefore(arrowGEP);
        }
    
        errs() << "DebugStatements Pass finished. Changed: " << (changed ? "YES" : "NO") << "\n";
    
        return changed ? PreservedAnalyses::none() : PreservedAnalyses::all();
      }
    };
    
} // namespace
    
extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
  errs() << "DebugStatements Plugin Info requested - registering passes\n";

  return {LLVM_PLUGIN_API_VERSION, "DebugStatements_Plugin", "1.0",
          [](PassBuilder &PB) {
            errs() << "Registering DebugStatements pass in pipeline\n";

            PB.registerPipelineParsingCallback(
                [](StringRef Name, ModulePassManager &MPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  errs() << "Pipeline callback called with: '" << Name << "'\n";

                  if (Name == "debugstatements") {
                    errs() << "Adding DebugStatements pass to pipeline\n";
                    MPM.addPass(DebugStatementsNewPMPass());
                    return true;
                  }
                  return false;
                });
          }};
}