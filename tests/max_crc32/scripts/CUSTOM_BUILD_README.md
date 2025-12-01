# Custom RISC-V Compiler Build Scripts

This directory contains scripts to build the `crc32` test (non-TMR baseline) with your custom RISC-V compiler located at:
```
/home/m4x/riscv-compiler/riscv64-glibc-gcc-thead_20200702/bin/riscv64-unknown-linux-gnu-gcc
```

## Available Build Methods

### Method 1: Two-Step Build Script (`scripts/build_riscv.sh`)
This script runs the standard `make exe` and then recompiles the object file with your custom compiler, **including assembly file generation**.

**Usage:**
```bash
# From the crc32 directory
./scripts/build_riscv.sh

# Or from anywhere
cd tests/max_crc32 && ./scripts/build_riscv.sh
```

**What it does:**
1. Runs `make exe` to generate LLVM IR and object files
2. **Generates assembly file** with your custom RISC-V compiler
3. Recompiles the `.o` file with your custom RISC-V compiler
4. Creates `crc32_custom` executable
5. Backs up the original executable as `crc32.backup`

**Output files:**
- `crc32.o` - Object file
- `crc32.opt.ll` - Optimized LLVM IR
- `crc32_custom.s` - **Assembly file with custom compiler**
- `crc32_custom` - RISC-V executable with custom compiler
- `crc32.backup` - Original executable backup

### Method 2: Custom Makefile (`scripts/build_custom.sh`)
This script uses a modified Makefile that directly uses your custom compiler throughout the build process, **including assembly generation**.

**Usage:**
```bash
# From the crc32 directory
./scripts/build_custom.sh

# Or from anywhere
cd tests/max_crc32 && ./scripts/build_custom.sh
```

## File Descriptions

- `scripts/build_riscv.sh` - Two-step build script with assembly generation (recommended for testing)
- `scripts/build_custom.sh` - Simple wrapper for custom Makefile with assembly generation
- `Makefile.custom` - Modified Makefile with custom compiler path and assembly targets

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

## Notes

- This is the baseline (non-TMR) version of the CRC32 test
- For TMR version, see `max_crc32_tmr/`
- Both methods produce valid RISC-V 64-bit executables
- All executables are statically linked for embedded deployment

