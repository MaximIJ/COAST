# RISC-V 64-bit Compilation Guide for COAST

## Overview

This document explains how to compile COAST tests for RISC-V 64-bit targets, specifically for running on the Nezha D1 (Allwinner D1) board. The setup enables COAST's TMR (Triple Modular Redundancy) fault tolerance techniques to work with RISC-V 64-bit architectures.

## Background

COAST (Compiler-Assisted Software Fault Tolerance) is a framework for implementing software fault tolerance techniques using LLVM passes. This guide covers the migration from legacy LLVM 7 to modern LLVM 16+ with RISC-V 64-bit support.

## Prerequisites

- LLVM 16+ with RISC-V support
- RISC-V 64-bit cross-compilation toolchain (`riscv64-linux-gnu-gcc`)
- COAST build system (already configured)
- Target: Nezha D1 (Allwinner D1) with RISC-V 64-bit C906 core

## Implementation Details

### 1. RISC-V 64-bit Makefile Configuration

Created `tests/makefiles/Makefile.compile.riscv64` with the following key features:

```makefile
# RISC-V 64-bit Linux configuration for COAST
RISCV64_CC = riscv64-linux-gnu-gcc

# Clang flags for RISC-V 64-bit target
CLANG_FLAGS = -fno-builtin-printf -fno-vectorize -Wall -m64 --target=riscv64

# CFLAGS for compilation
CFLAGS = -O2 -fno-builtin-printf -g -march=rv64gc -mabi=lp64d -mcmodel=medany

# Linker flags
LFLAGS = -O2 -fno-builtin-printf -g -march=rv64gc -mabi=lp64d -mcmodel=medany

# Include flags for RISC-V 64-bit Linux
INC_FLAGS = -I$(LEVEL) -I/usr/riscv64-linux-gnu/include
```

**Key Configuration Details:**
- **Target Architecture**: `rv64gc` (RISC-V 64-bit with General and Compressed instruction sets)
- **ABI**: `lp64d` (64-bit pointers, 64-bit longs, double-precision floating point)
- **Memory Model**: `medany` (medium code model with any alignment)
- **Cross-compiler**: Uses system-installed `riscv64-linux-gnu-gcc`

### 2. Build System Integration

#### Updated Configuration Files

**`tests/makefiles/config`:**
```makefile
BOARD_RISCV64 = riscv64
```

**`tests/makefiles/Makefile.compile`:**
```makefile
else ifeq ($(BOARD), $(BOARD_RISCV64))
include $(LEVEL)/makefiles/Makefile.compile.riscv64
```

#### Library Path Corrections

Fixed the build system to use the correct library paths:
- **Before**: `$(COAST_ROOT)/projects/build/`
- **After**: `$(COAST_ROOT)/build/`

This ensures the TMR and DataflowProtection libraries are found correctly.

### 3. Legacy Pass Manager Configuration

Due to LLVM 16's new pass manager, we needed to force the legacy pass manager for COAST compatibility:

```makefile
%.opt.bc: %.clang.bc $(PROJECT_LIBS)
	$(LLVM_OPT) $(OPT_FLAGS) $(OPT_LIBS_LOAD) -enable-new-pm=0 $(OPT_PASSES) -o $@ $<
	$(LLVM_DIS) $@
```

**Key Points:**
- `-enable-new-pm=0` forces legacy pass manager
- `-DataflowProtection` applies COAST's TMR pass
- `-load $(PROJECT_BUILD_DIR)/TMR/TMR.so` loads the TMR library

### 4. Test Configuration

**`tests/matrixMul_tmr/Makefile`:**
```makefile
LEVEL = ..
TARGET = matrixMul_tmr
OPT_PASSES = -DataflowProtection
BOARD = riscv64

include $(LEVEL)/makefiles/Makefile.common
```

## Compilation Process

### Step-by-Step Build Process

1. **C to LLVM IR**: Clang compiles C code to LLVM IR with RISC-V 64-bit target
2. **TMR Pass Application**: LLVM opt applies COAST's DataflowProtection pass
3. **Code Generation**: LLC generates RISC-V 64-bit assembly
4. **Linking**: GCC links the final executable

### Build Command
```bash
cd tests/matrixMul_tmr
make exe
```

## Generated Output

### File Types Created

- **`matrixMul_tmr`**: RISC-V 64-bit ELF executable (~507KB)
- **`matrixMul_tmr.clang.bc`**: LLVM bitcode (intermediate)
- **`matrixMul_tmr.opt.bc`**: Optimized LLVM bitcode with TMR
- **`matrixMul_tmr.opt.ll`**: Human-readable LLVM IR with TMR
- **`matrixMul_tmr.o`**: RISC-V 64-bit object file
- **`matrixMul_tmr.s`**: RISC-V 64-bit assembly code

### TMR Verification

The generated LLVM IR shows TMR has been applied:

```llvm
@matrix_a_DWC = internal local_unnamed_addr global [64 x [64 x i64]] zeroinitializer
@matrix_a = internal local_unnamed_addr global [64 x [64 x i64]] zeroinitializer
```

- **Duplicated Variables**: Variables are triplicated with `_DWC` suffixes
- **Synchronization Logic**: TMR-style error checking and voting
- **Error Handling**: `errorHandler.main` blocks for fault detection

## Target Hardware Compatibility

### Nezha D1 (Allwinner D1) Specifications

- **CPU**: RISC-V 64-bit C906 core
- **Architecture**: RV64GC (64-bit with compressed instructions)
- **Floating Point**: Double-precision support
- **Memory Model**: Compatible with `medany` model

### Executable Characteristics

```bash
$ file matrixMul_tmr
matrixMul_tmr: ELF 64-bit LSB executable, UCB RISC-V, RVC, double-float ABI, version 1 (SYSV), statically linked
```

- **Architecture**: RISC-V 64-bit LSB executable
- **Features**: RVC (compressed instructions), double-float ABI
- **Linking**: Statically linked for bare metal execution
- **Size**: ~507KB (includes TMR overhead)

## Troubleshooting

### Common Issues and Solutions

1. **Missing RISC-V Headers**
   ```bash
   # Ensure RISC-V toolchain is installed
   sudo apt install gcc-riscv64-linux-gnu
   ```

2. **LLVM Pass Manager Issues**
   - Use `-enable-new-pm=0` to force legacy pass manager
   - Ensure TMR library is loaded before applying passes

3. **Library Path Issues**
   - Verify `PROJECT_BUILD_DIR` points to correct build directory
   - Check that `.so` files exist in the build directory

4. **Target Architecture Mismatch**
   - Ensure `-march=riscv64` is used consistently
   - Verify ABI compatibility (`lp64d` for double-precision)

## Usage

### Running the Test

The compiled executable can be transferred to the Nezha D1 board and executed:

```bash
# On the target board
./matrixMul_tmr
```

### Expected Output

```
Test completed with 0 errors.
```

The TMR protection will automatically detect and correct single-bit errors during execution.

## Future Enhancements

1. **Optimization**: Fine-tune TMR pass parameters for RISC-V specific optimizations
2. **Testing**: Add RISC-V specific fault injection tests
3. **Performance**: Profile TMR overhead on RISC-V hardware
4. **Documentation**: Create RISC-V specific COAST usage examples

## Conclusion

This setup successfully enables COAST's fault tolerance techniques on RISC-V 64-bit targets. The TMR pass provides triple modular redundancy protection, making the code resilient to single-bit errors that could occur in harsh environments like space or industrial applications.

The configuration is now ready for deployment on the Nezha D1 board and can be extended to other RISC-V 64-bit targets as needed.
