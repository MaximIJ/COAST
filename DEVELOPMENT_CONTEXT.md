# COAST LLVM 16 Migration - Development Context

## Project State Summary

### 🎯 **Objective**
Migrate COAST compiler from LLVM 7 to LLVM 16+ to enable RISC-V support and modern LLVM features.

### 📊 **Progress: ~85% Complete**
- ✅ Core API migrations done
- ✅ New Pass Manager implementation complete
- ✅ Build system updated
- 🔄 Testing and validation needed
- 🔄 Documentation updates pending

## What Was Accomplished

### 1. **New Pass Manager (NPM) Implementation**
**Files Modified:**
- `projects/TMR/TMRNewPM.cpp` (NEW) - Modern plugin-based TMR pass
- `projects/TMR/CMakeLists.txt` - Updated for NPM plugin build
- `projects/CMakeLists.txt` - Added compatibility layer

**Key Changes:**
```cpp
// Modern plugin structure
struct TMRPass : PassInfoMixin<TMRPass> {
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &) {
    dataflowProtection DP;
    DP.run(M, 3);  // enable TMR
    return PreservedAnalyses::none();
  }
};
```

### 2. **Core LLVM API Migrations**
**Files Modified:**
- `projects/dataflowProtection/dataflowProtection.h`
- `projects/dataflowProtection/synchronization.cpp`
- `projects/dataflowProtection/cloning.cpp`
- `projects/dataflowProtection/verification.cpp`
- `projects/dataflowProtection/utils.cpp`
- `projects/dataflowProtection/interface.cpp`
- `projects/dataflowProtection/inspection.cpp`

**Major API Changes Applied:**
```cpp
// OLD → NEW
TerminatorInst* → Instruction* (with isTerminator() checks)
getNumArgOperands() → arg_size()
getCalledValue() → getCalledOperand()
VectorType::get() → FixedVectorType::get()
LoadInst(ptr, name) → LoadInst(type, ptr, name, false, Align(1))
StoreInst(val, ptr) → StoreInst(val, ptr, false, Align(1))
getBasicBlockList().size() → empty() or size()
```

### 3. **Build System Modernization**
**Files Modified:**
- `projects/CMakeLists.txt` - Main build configuration
- All subproject `CMakeLists.txt` files

**Key Updates:**
- LLVM requirement: 7.0 → 16+
- CMake minimum: 3.13
- Added `add_llvm_loadable_module_shim` for compatibility
- Conditional legacy pass building

## Current Issues & Next Steps

### 🔧 **Immediate Fixes Needed**

1. **verification.cpp - GEPOperator Issues**
   ```cpp
   // CURRENT (BROKEN):
   if (CE->getOpcode() == Instruction::GetElementPtr && !cast<GEPOperator>(CE)->hasIndices())
   
   // NEEDS:
   if (CE->getOpcode() == Instruction::GetElementPtr && !cast<Operator>(CE)->hasIndices())
   ```

2. **Intrinsic ID References**
   ```cpp
   // CURRENT (BROKEN):
   if (calledF->getIntrinsicID() != Intrinsic::ID::not_intrinsic)
   
   // NEEDS:
   if (calledF->getIntrinsicID() != Intrinsic::not_intrinsic)
   ```

3. **ConstantExpr API Updates**
   ```cpp
   // CURRENT (BROKEN):
   if (CE->isGEPWithNoNotionalOverIndexing())
   
   // NEEDS:
   if (CE->getOpcode() == Instruction::GetElementPtr)
   ```

### 🧪 **Testing Requirements**

1. **Compilation Testing**
   - [ ] Build with LLVM 16
   - [ ] Build with LLVM 17
   - [ ] Build with LLVM 18
   - [ ] Verify no compilation errors

2. **Functional Testing**
   - [ ] Test TMR pass on RISC-V targets
   - [ ] Test DWC pass functionality
   - [ ] Run existing test suites
   - [ ] Validate fault injection tests

3. **Integration Testing**
   - [ ] Test with different optimization levels
   - [ ] Test with different target architectures
   - [ ] Verify pass ordering works correctly

## File Structure & Key Components

### Core Passes
```
projects/
├── dataflowProtection/     # Core fault tolerance logic
│   ├── dataflowProtection.h
│   ├── synchronization.cpp # Error detection/correction
│   ├── cloning.cpp         # Instruction/function cloning
│   ├── verification.cpp    # Safety checks
│   └── utils.cpp          # Utility functions
├── TMR/                   # Triple Modular Redundancy
│   ├── TMRNewPM.cpp       # NEW: Modern NPM plugin
│   └── TMR.cpp           # Legacy pass (conditional)
└── DWC/                   # Duplication With Comparison
    └── DWC.cpp           # Legacy pass (conditional)
```

### Build Configuration
```
CMakeLists.txt              # Main build config
projects/CMakeLists.txt     # Project-level config
projects/*/CMakeLists.txt   # Individual pass configs
```

## Development Workflow

### 1. **Fix Remaining Compile Errors**
```bash
cd build
make clean
make -j$(nproc) 2>&1 | tee build.log
```

### 2. **Test TMR Pass**
```bash
# Test with opt tool
opt -load-pass-plugin=./libTMR.so -passes=tmr input.ll -o output.ll

# Test with clang
clang -Xclang -load -Xclang ./libTMR.so -c input.c -o output.o
```

### 3. **Validate RISC-V Support**
```bash
# Test with RISC-V target
clang --target=riscv64-unknown-elf -Xclang -load -Xclang ./libTMR.so -c input.c
```

## Common Patterns & Solutions

### LLVM 16 API Patterns
```cpp
// Instruction creation
IRBuilder<> Builder(InsertPoint);
LoadInst* LI = Builder.CreateLoad(PtrType, Ptr, "name");

// Function calls
for (unsigned i = 0; i < CI->arg_size(); i++) {
    Value* Arg = CI->getArgOperand(i);
}

// Type checking
if (Instruction* I = dyn_cast<Instruction>(V)) {
    if (I->isTerminator()) {
        // Handle terminator
    }
}
```

### Error Handling Patterns
```cpp
// Module access for intrinsics
Module& M = *BB->getModule();
Function* Intrinsic = Intrinsic::getDeclaration(&M, IntrinsicID, Types);
```

## Resources & References

### LLVM Documentation
- [LLVM 16 Migration Guide](https://llvm.org/docs/ReleaseNotes.html)
- [New Pass Manager](https://llvm.org/docs/NewPassManager.html)
- [LLVM API Changes](https://llvm.org/docs/APIChanges.html)

### COAST Documentation
- [COAST Documentation](https://coast-compiler.readthedocs.io/)
- [Fault Tolerance Techniques](https://coast-compiler.readthedocs.io/en/latest/)

### Testing Resources
- `tests/` directory contains validation tests
- `tests/TMRregression/` has TMR-specific tests
- `tests/hifive1/` has RISC-V specific examples

## Success Criteria

### ✅ **Complete When:**
1. All passes compile without errors on LLVM 16+
2. TMR pass works correctly on RISC-V targets
3. DWC pass maintains functionality
4. Existing test suites pass
5. Documentation is updated
6. CI/CD pipeline supports multiple LLVM versions

### 🎯 **Quality Gates:**
- Zero compilation warnings
- All existing tests pass
- RISC-V target validation
- Performance regression < 5%
- Memory usage regression < 10%

---

**Last Updated:** Current session
**Next Review:** After fixing remaining compile errors
**Status:** Active Development - LLVM 16 Migration
