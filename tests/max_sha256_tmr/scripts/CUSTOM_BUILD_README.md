# Custom RISC-V Compiler Build Scripts

This directory contains scripts to build the `sha256_tmr` test with your custom RISC-V compiler located at:
```
/home/m4x/riscv-compiler/riscv64-glibc-gcc-thead_20200702/bin/riscv64-unknown-linux-gnu-gcc
```

## Available Build Methods

### Method 1: Two-Step Build Script (`scripts/build_riscv.sh`)
This script runs the standard `make exe` and then recompiles the object file with your custom compiler, **including assembly file generation**.

**Usage:**
```bash
# From the sha256_tmr directory
./scripts/build_riscv.sh

# Or from anywhere
cd tests/max_sha256_tmr && ./scripts/build_riscv.sh
```

**Output files:**
- `sha256_tmr.o` - Object file
- `sha256_tmr.opt.ll` - Optimized LLVM IR
- `sha256_tmr_custom.s` - **Assembly file with TMR variables**
- `sha256_tmr_custom` - RISC-V executable with custom compiler

### Method 2: Custom Makefile (`scripts/build_custom.sh`)
This script uses a modified Makefile that directly uses your custom compiler throughout the build process.

**Usage:**
```bash
# From the sha256_tmr directory
./scripts/build_custom.sh
```

## Additional Makefile Targets

```bash
# Generate only assembly file
make -f Makefile.custom assembly

# Generate executable only (no assembly)
make -f Makefile.custom exe

# Generate both executable and assembly
make -f Makefile.custom exe-with-assembly
```

## Switching Between TMR and DWC

See `scripts/DWC_TEST_CONFIG.md` for instructions.

## Cleanup

```bash
make clean
```

