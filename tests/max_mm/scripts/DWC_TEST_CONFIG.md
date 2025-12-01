# DWC Test Configuration Example

## Quick Start: Switch from TMR to DWC

To test DWC/DMR instead of TMR, simply change one line in the Makefile:

```makefile
# Change this line in tests/matrixMul_tmr/Makefile:
# OPT_PASSES = -passes=tmr
OPT_PASSES = -passes=dwc
```

## Build and Verify

```bash
cd tests/matrixMul_tmr
make clean
make exe

# Verify DWC output (should show only _DWC variables)
grep "_DWC" matrixMul_tmr.opt.ll | wc -l
grep "_TMR" matrixMul_tmr.opt.ll  # Should return nothing

# Generate assembly
riscv64-linux-gnu-objdump -d matrixMul_tmr.o > matrixMul_tmr.s
```

## Expected Results

- **DWC**: ~39 variables (2x redundancy)
- **TMR**: ~64 variables (3x redundancy)

## Switch Back to TMR

```makefile
# Change back to TMR:
OPT_PASSES = -passes=tmr
```

