# RISC-V 64-bit DWC/DMR Compilation Guide

## Overview

This guide explains how to compile COAST applications for RISC-V 64-bit Linux using DWC (Duplication With Comparison) for DMR (Dual Modular Redundancy) fault tolerance.

## DWC vs TMR Comparison

### DWC/DMR (Dual Modular Redundancy)
- **Redundancy Level**: 2x (Original + DWC copy)
- **Variables**: Only `_DWC` suffix variables
- **Error Detection**: Compare original vs DWC copy
- **Error Handling**: Call `FAULT_DETECTED_DWC` on mismatch
- **Memory Overhead**: ~100% increase
- **Pass Name**: `dwc`

### TMR (Triple Modular Redundancy)
- **Redundancy Level**: 3x (Original + DWC + TMR copies)
- **Variables**: Both `_DWC` and `_TMR` suffix variables
- **Error Detection**: 2-out-of-3 majority voting
- **Error Correction**: Can correct single faults
- **Memory Overhead**: ~200% increase
- **Pass Name**: `tmr`

## Build System Configuration

### 1. RISC-V 64-bit Makefile Support

The build system has been updated to support both TMR and DWC passes:

```makefile
# tests/makefiles/Makefile.compile.riscv64
%.opt.bc: %.clang.bc $(PROJECT_LIBS)
	$(LLVM_OPT) -load-pass-plugin $(PROJECT_BUILD_DIR)/TMR/TMR.so -load-pass-plugin $(PROJECT_BUILD_DIR)/DWC/DWC.so $(OPT_PASSES) -o $@ $<
	$(LLVM_DIS) $@
```

### 2. Test Configuration

To use DWC/DMR instead of TMR, modify the test Makefile:

```makefile
# tests/matrixMul_tmr/Makefile
LEVEL = ..
TARGET = matrixMul_tmr
OPT_PASSES = -passes=dwc  # Use DWC instead of TMR
BOARD = riscv64

include $(LEVEL)/makefiles/Makefile.common
```

## Compilation Process

### Prerequisites

1. **RISC-V 64-bit Toolchain**:
   ```bash
   sudo apt install gcc-riscv64-linux-gnu
   ```

2. **COAST Build**:
   ```bash
   cd /path/to/COAST
   mkdir build && cd build
   cmake .. -DLLVM_DIR=/usr/lib/llvm-16/lib/cmake/llvm
   make -j$(nproc)
   ```

3. **Configuration File**:
   ```bash
   cp projects/dataflowProtection/functions.config tests/matrixMul_tmr/
   ```

### Build Commands

#### For DWC/DMR:
```bash
cd tests/matrixMul_tmr
# Edit Makefile: OPT_PASSES = -passes=dwc
make clean
make exe
```

#### For TMR:
```bash
cd tests/matrixMul_tmr
# Edit Makefile: OPT_PASSES = -passes=tmr
make clean
make exe
```

## Verification

### 1. Check LLVM IR for Redundancy

#### DWC/DMR Verification:
```bash
# Should show only _DWC variables
grep "_DWC" matrixMul_tmr.opt.ll | wc -l
# Should show no _TMR variables
grep "_TMR" matrixMul_tmr.opt.ll
```

#### TMR Verification:
```bash
# Should show both _DWC and _TMR variables
grep "_DWC\|_TMR" matrixMul_tmr.opt.ll | wc -l
```

### 2. Expected Variable Counts

For `matrixMul_tmr` test:
- **DWC/DMR**: ~39 variables (2x redundancy)
- **TMR**: ~64 variables (3x redundancy)

### 3. Assembly Verification

Generate assembly to verify fault tolerance logic:

```bash
# Generate assembly from object file
riscv64-linux-gnu-objdump -d matrixMul_tmr.o > matrixMul_tmr.s

# Check for fault detection calls
grep "FAULT_DETECTED" matrixMul_tmr.s
```

## Technical Details

### DWC Pass Implementation

The DWC pass uses the new LLVM 16 pass manager:

```cpp
// projects/DWC/DWC.cpp
struct DWCNewPMPass : PassInfoMixin<DWCNewPMPass> {
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &) {
    dataflowProtection DP;
    bool changed = DP.run(M, 2); // DWC mode (2x redundancy)
    return PreservedAnalyses::none();
  }
};
```

### TMR Pass Implementation

The TMR pass also uses the new pass manager:

```cpp
// projects/TMR/TMR.cpp
struct TMRPass : PassInfoMixin<TMRPass> {
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &) {
    dataflowProtection DP;
    bool changed = DP.run(M, 3); // TMR mode (3x redundancy)
    return PreservedAnalyses::none();
  }
};
```

### Pass Registration

Both passes register with the new pass manager:

```cpp
extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "Plugin_Name", "1.0",
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, ModulePassManager &MPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "dwc" || Name == "tmr") {
                    MPM.addPass(PassClass());
                    return true;
                  }
                  return false;
                });
          }};
}
```

## File Generation Order

After `make clean` and `make exe`:

1. **matrixMul_tmr.clang.bc** - LLVM IR from Clang
2. **matrixMul_tmr.opt.bc** - Optimized LLVM IR with fault tolerance
3. **matrixMul_tmr.opt.ll** - Human-readable optimized IR
4. **matrixMul_tmr.o** - RISC-V 64-bit object file
5. **matrixMul_tmr** - Executable binary

## Troubleshooting

### Common Issues

1. **"No configuration file found"**:
   ```bash
   cp ../../projects/dataflowProtection/functions.config .
   ```

2. **"Unknown pass name"**:
   - Ensure both TMR.so and DWC.so are loaded
   - Check pass name: `dwc` for DWC, `tmr` for TMR

3. **Missing system headers**:
   ```bash
   sudo apt install gcc-riscv64-linux-gnu
   ```

### Verification Commands

```bash
# Check if passes are loaded
opt-16 -load-pass-plugin ../../build/DWC/DWC.so --help | grep DWC
opt-16 -load-pass-plugin ../../build/TMR/TMR.so --help | grep TMR

# Test pass execution
opt-16 -load-pass-plugin ../../build/DWC/DWC.so -passes=dwc input.bc -o output.bc
```

## Performance Comparison

| Metric | DWC/DMR | TMR |
|--------|---------|-----|
| Memory Usage | 2x | 3x |
| Error Detection | Yes | Yes |
| Error Correction | No | Yes |
| Performance Overhead | Lower | Higher |
| Fault Tolerance | Single fault detection | Single fault correction |

## Next Steps

1. **Test with Different Applications**: Verify DWC works with other test cases
2. **Performance Analysis**: Measure overhead differences between DWC and TMR
3. **Fault Injection**: Test fault detection capabilities
4. **Integration**: Combine with other COAST passes if needed

## References

- [COAST Main Documentation](../README.md)
- [RISC-V 64 TMR Compilation Steps](./RISC-V_64_TMR_COMPILATION_STEPS.md)
- [LLVM 16 Pass Manager Migration](./DEVELOPMENT_CONTEXT.md)

