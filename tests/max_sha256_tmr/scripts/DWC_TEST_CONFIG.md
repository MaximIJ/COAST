# DWC Test Configuration Example

## Quick Start: Switch from TMR to DWC

To test DWC/DMR instead of TMR, simply change one line in the Makefile:

```makefile
# Change this line in tests/max_sha256_tmr/Makefile:
# OPT_PASSES = -passes=tmr -countErrors -storeDataSync
OPT_PASSES = -passes=dwc
```

## Build and Verify

```bash
cd tests/max_sha256_tmr
make clean
make exe

# Verify DWC output (should show only _DWC variables)
grep "_DWC" sha256_tmr.opt.ll | wc -l
grep "_TMR" sha256_tmr.opt.ll  # Should return nothing

# Generate assembly
riscv64-linux-gnu-objdump -d sha256_tmr.o > sha256_tmr.s
```

## Expected Results

- **DWC**: ~2x redundancy for hash computation variables
- **TMR**: ~3x redundancy for hash computation variables

## Switch Back to TMR

```makefile
# Change back to TMR:
OPT_PASSES = -passes=tmr -countErrors -storeDataSync
```

