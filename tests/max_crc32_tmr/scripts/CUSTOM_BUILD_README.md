# Custom RISC-V Compiler Build Scripts

This directory contains scripts to build the `crc32_tmr` test with your custom RISC-V compiler located at:
```
/home/m4x/riscv-compiler/riscv64-glibc-gcc-thead_20200702/bin/riscv64-unknown-linux-gnu-gcc
```

## Available Build Methods

### Method 1: Two-Step Build Script (`scripts/build_riscv.sh`)
This script runs the standard `make exe` and then recompiles the object file with your custom compiler, **including assembly file generation**.

**Usage:**
```bash
# From the crc32_tmr directory
./scripts/build_riscv.sh

# Or from anywhere
cd tests/max_crc32_tmr && ./scripts/build_riscv.sh
```

**What it does:**
1. Runs `make exe` to generate LLVM IR and object files
2. **Generates assembly file** with your custom RISC-V compiler
3. Recompiles the `.o` file with your custom RISC-V compiler
4. Creates `crc32_tmr_custom` executable
5. Backs up the original executable as `crc32_tmr.backup`

**Output files:**
- `crc32_tmr.o` - Object file
- `crc32_tmr.opt.ll` - Optimized LLVM IR
- `crc32_tmr_custom.s` - **Assembly file with custom compiler**
- `crc32_tmr_custom` - RISC-V executable with custom compiler
- `crc32_tmr.backup` - Original executable backup

### Method 2: Custom Makefile (`scripts/build_custom.sh`)
This script uses a modified Makefile that directly uses your custom compiler throughout the build process, **including assembly generation**.

**Usage:**
```bash
# From the crc32_tmr directory
./scripts/build_custom.sh

# Or from anywhere
cd tests/max_crc32_tmr && ./scripts/build_custom.sh
```

**What it does:**
1. Uses `Makefile.custom` which overrides the RISC-V compiler
2. Runs the complete build process with your custom compiler
3. **Generates assembly file** with your custom compiler
4. Creates `crc32_tmr` executable directly

**Output files:**
- `crc32_tmr.o` - Object file
- `crc32_tmr.opt.ll` - Optimized LLVM IR  
- `crc32_tmr_custom.s` - **Assembly file with custom compiler**
- `crc32_tmr` - RISC-V executable with custom compiler

## Assembly File Generation

Both methods now generate RISC-V assembly files using your custom compiler with the following features:

- **Compiler:** Your custom GCC 8.1.0 RISC-V compiler
- **Architecture:** RISC-V 64-bit (rv64gc)
- **ABI:** LP64D (64-bit integer, 64-bit pointer, double-precision floating point)
- **Optimization:** -O2 (optimized for performance)
- **Debug info:** Included for debugging
- **File format:** Standard RISC-V assembly (.s)

### Assembly File Contents

The generated assembly file (`crc32_tmr_custom.s`) contains:
- RISC-V assembly instructions
- Function definitions (testing, crc32, etc.)
- TMR-redundant code (due to the TMR pass)
- Debug information
- Proper RISC-V calling conventions

## File Descriptions

- `scripts/build_riscv.sh` - Two-step build script with assembly generation (recommended for testing)
- `scripts/build_custom.sh` - Simple wrapper for custom Makefile with assembly generation
- `Makefile.custom` - Modified Makefile with custom compiler path and assembly targets
- `scripts/DWC_TEST_CONFIG.md` - Instructions for switching between TMR and DWC

## Additional Makefile Targets

The `Makefile.custom` includes additional targets for more control:

```bash
# Generate only assembly file
make -f Makefile.custom assembly

# Generate executable only (no assembly)
make -f Makefile.custom exe

# Generate both executable and assembly
make -f Makefile.custom exe-with-assembly
```

## Switching Between TMR and DWC

To test DWC instead of TMR, edit the `OPT_PASSES` line in the Makefile:

```makefile
# For TMR (current)
OPT_PASSES = -passes=tmr

# For DWC
OPT_PASSES = -passes=dwc
```

## Verification

Both scripts verify that:
- The custom compiler exists and is accessible
- Generated files are RISC-V executables
- Build process completes successfully

## Cleanup

To clean up generated files:
```bash
make clean
```

## Notes

- Both methods produce valid RISC-V 64-bit executables
- The linker warning about `_start` symbol is normal for this type of embedded application
- The custom compiler is GCC 8.1.0 with RISC-V support
- All executables are statically linked for embedded deployment

