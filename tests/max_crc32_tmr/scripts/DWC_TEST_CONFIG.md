# DWC Test Configuration Example

## Quick Start: Switch from TMR to DWC

To test DWC/DMR instead of TMR, simply change one line in the Makefile:

```makefile
# Change this line in tests/max_crc32_tmr/Makefile:
# OPT_PASSES = -passes=tmr
OPT_PASSES = -passes=dwc
```

## Build and Verify

```bash
cd tests/max_crc32_tmr
make clean
make exe

# Verify DWC output (should show only _DWC variables)
grep "_DWC" crc32_tmr.opt.ll | wc -l
grep "_TMR" crc32_tmr.opt.ll  # Should return nothing

# Generate assembly
riscv64-linux-gnu-objdump -d crc32_tmr.o > crc32_tmr.s
```

## Expected Results

- **DWC**: ~2x redundancy for CRC computation variables
- **TMR**: ~3x redundancy for CRC computation variables

## Switch Back to TMR

```makefile
# Change back to TMR:
OPT_PASSES = -passes=tmr
```

