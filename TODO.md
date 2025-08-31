# COAST LLVM 16 Migration - TODO

## 🔥 **IMMEDIATE PRIORITY (Fix Compile Errors)**

### 1. Fix verification.cpp Issues
- [ ] **Line 756**: Replace `cast<GEPOperator>(CE)->hasIndices()` with `cast<Operator>(CE)->hasIndices()`
- [ ] **Line 823**: Same fix as above
- [ ] **Line 1005**: Fix `Intrinsic::ID::not_intrinsic` → `Intrinsic::not_intrinsic`

### 2. Fix cloning.cpp Issues  
- [ ] **Line 920**: Replace `callInst->getNumArgOperands()` with `callInst->arg_size()`
- [ ] **Line 1416**: Replace `callInst->getNumArgOperands()` with `callInst->arg_size()`
- [ ] **Line 1452**: Replace `invInst->getNumArgOperands()` with `invInst->arg_size()`

### 3. Fix synchronization.cpp Issues
- [ ] **Line 1229**: Replace `errFn->getBasicBlockList().size() != 0` with `!errFn->empty()`
- [ ] **Line 1295**: Replace `F.getBasicBlockList().size() == 0` with `F.empty()`
- [ ] **Line 1528**: Add `Module &M = *thisBlock->getModule();` before `Intrinsic::getDeclaration`
- [ ] **Line 1765**: Replace `TerminatorInst*` with `Instruction*` for `curTerminator`
- [ ] **Line 1778**: Replace `TerminatorInst*` with `Instruction*` for `newTerm0`

## 🧪 **TESTING PHASE**

### 4. Build & Compile Testing
- [ ] Clean build directory: `rm -rf build && mkdir build`
- [ ] Configure with LLVM 16: `cmake .. -DLLVM_DIR=/path/to/llvm/lib/cmake/llvm`
- [ ] Build: `make -j$(nproc)`
- [ ] Verify no compilation errors
- [ ] Test with LLVM 17 if available
- [ ] Test with LLVM 18 if available

### 5. Functional Testing
- [ ] Test TMR pass with simple C program
- [ ] Test DWC pass with simple C program
- [ ] Run existing test suites in `tests/` directory
- [ ] Test with RISC-V target specifically
- [ ] Validate fault injection tests still work

### 6. Integration Testing
- [ ] Test pass with different optimization levels (-O0, -O1, -O2, -O3)
- [ ] Test with different target architectures (x86, ARM, RISC-V)
- [ ] Verify pass ordering works correctly
- [ ] Test with different input sizes and complexity

## 📚 **DOCUMENTATION & CLEANUP**

### 7. Update Documentation
- [ ] Update README.md with new LLVM requirements
- [ ] Update installation instructions
- [ ] Document new build process
- [ ] Update API documentation if needed
- [ ] Create migration guide for users

### 8. Code Cleanup
- [ ] Remove any remaining legacy code paths
- [ ] Clean up unused includes
- [ ] Fix any remaining compiler warnings
- [ ] Add missing comments for complex LLVM API usage
- [ ] Standardize code formatting

## 🚀 **ENHANCEMENTS**

### 9. Performance Optimization
- [ ] Profile TMR pass performance
- [ ] Optimize memory usage
- [ ] Reduce compilation time overhead
- [ ] Benchmark against original LLVM 7 version

### 10. CI/CD Setup
- [ ] Set up GitHub Actions or similar CI
- [ ] Test with multiple LLVM versions (16, 17, 18)
- [ ] Test with multiple target architectures
- [ ] Add automated testing for fault injection scenarios

## 🔍 **VALIDATION CHECKLIST**

### Before Marking Complete:
- [ ] All compilation errors fixed
- [ ] All tests pass
- [ ] RISC-V target works correctly
- [ ] Performance is acceptable (< 5% regression)
- [ ] Memory usage is acceptable (< 10% regression)
- [ ] Documentation is updated
- [ ] Code is clean and well-commented

## 📝 **NOTES FOR NEXT SESSION**

### Current Build Status:
- **Progress**: ~85% complete
- **Main Issue**: Compilation errors in verification.cpp and cloning.cpp
- **Next Focus**: Fix remaining API issues, then test thoroughly

### Key Files Modified:
- `projects/dataflowProtection/` (all .cpp files)
- `projects/TMR/TMRNewPM.cpp` (new file)
- `projects/CMakeLists.txt` (build system)
- All subproject `CMakeLists.txt` files

### LLVM API Changes Applied:
- ✅ TerminatorInst → Instruction*
- ✅ LoadInst/StoreInst constructors
- ✅ VectorType → FixedVectorType
- ✅ getNumArgOperands → arg_size
- ✅ getCalledValue → getCalledOperand
- 🔄 GEPOperator → Operator (in progress)
- 🔄 Intrinsic::ID::not_intrinsic → Intrinsic::not_intrinsic (in progress)

---

**Last Updated**: Current session
**Next Session Goal**: Fix remaining compile errors and begin testing
**Estimated Time to Complete**: 2-3 hours for fixes, 1-2 days for testing
