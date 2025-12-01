#!/bin/bash

# Script to build crc32 with custom RISC-V compiler
# This script runs the standard make exe and then recompiles with the custom compiler

set -e  # Exit on any error

# Configuration
CUSTOM_RISCV_CC="/home/m4x/riscv-compiler/riscv64-glibc-gcc-thead_20200702/bin/riscv64-unknown-linux-gnu-gcc"
TARGET="crc32"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"

# Change to project directory
cd "$PROJECT_DIR"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${YELLOW}=== COAST CRC32 RISC-V Build Script ===${NC}"
echo "Target: $TARGET"
echo "Custom RISC-V Compiler: $CUSTOM_RISCV_CC"
echo "Project Directory: $PROJECT_DIR"
echo

# Check if custom compiler exists
if [ ! -f "$CUSTOM_RISCV_CC" ]; then
    echo -e "${RED}Error: Custom RISC-V compiler not found at:${NC}"
    echo "  $CUSTOM_RISCV_CC"
    echo "Please verify the path is correct."
    exit 1
fi

echo -e "${GREEN}✓ Custom RISC-V compiler found${NC}"

# Step 1: Run the standard make exe to generate .o and .opt.ll files
echo -e "${YELLOW}Step 1: Running 'make exe' to generate LLVM IR and object files...${NC}"
make exe

if [ $? -ne 0 ]; then
    echo -e "${RED}Error: 'make exe' failed${NC}"
    exit 1
fi

echo -e "${GREEN}✓ Standard build completed${NC}"

# Step 1.5: Preserve the .opt.bc file before it gets deleted
if [ -f "${TARGET}.opt.bc" ]; then
    cp "${TARGET}.opt.bc" "${TARGET}_preserved.opt.bc"
    echo -e "${GREEN}✓ Preserved optimized LLVM IR: ${TARGET}_preserved.opt.bc${NC}"
fi

# Step 2: Check if required files exist
echo -e "${YELLOW}Step 2: Verifying generated files...${NC}"

if [ ! -f "${TARGET}.o" ]; then
    echo -e "${RED}Error: ${TARGET}.o not found${NC}"
    exit 1
fi

if [ ! -f "${TARGET}.opt.ll" ]; then
    echo -e "${RED}Error: ${TARGET}.opt.ll not found${NC}"
    exit 1
fi

echo -e "${GREEN}✓ Required files found: ${TARGET}.o, ${TARGET}.opt.ll${NC}"

# Step 3: Generate assembly file with custom RISC-V compiler
echo -e "${YELLOW}Step 3: Generating assembly file with custom RISC-V compiler...${NC}"

echo "  Generating assembly from LLVM IR"
# Use LLVM to generate assembly from the optimized LLVM IR
# Disable debug info (removes .loc and .uleb128) and M extension (removes mul/mulw)
echo "  Stripping debug info from LLVM IR..."
opt-16 -strip-debug "${TARGET}.opt.ll" -o "${TARGET}.opt.no-debug.ll"
llc-16 \
    -march=riscv64 \
    -mattr=-m \
    -filetype=asm \
    -o "${TARGET}_custom.s" \
    "${TARGET}.opt.no-debug.ll"

if [ $? -ne 0 ]; then
    echo -e "${RED}Error: Assembly generation failed${NC}"
    exit 1
fi

echo -e "${GREEN}✓ Assembly file generated: ${TARGET}_custom.s${NC}"

# Step 4: Recompile with custom RISC-V compiler
echo -e "${YELLOW}Step 4: Recompiling with custom RISC-V compiler...${NC}"

# Backup the original executable if it exists
if [ -f "$TARGET" ]; then
    mv "$TARGET" "${TARGET}.backup"
    echo -e "${YELLOW}  Backed up original executable to ${TARGET}.backup${NC}"
fi

# Compile with custom compiler
echo "  Compiling with: $CUSTOM_RISCV_CC"
"$CUSTOM_RISCV_CC" \
    -O2 \
    -fno-builtin-printf \
    -g \
    -march=rv64gc \
    -mabi=lp64d \
    -mcmodel=medany \
    -I.. \
    "${TARGET}.o" \
    -o "${TARGET}_custom" \
    -nostartfiles \
    -static

if [ $? -ne 0 ]; then
    echo -e "${RED}Error: Custom RISC-V compilation failed${NC}"
    exit 1
fi

echo -e "${GREEN}✓ Custom RISC-V compilation successful${NC}"

# Step 5: Verify the executable
echo -e "${YELLOW}Step 5: Verifying executable...${NC}"

if [ -f "${TARGET}_custom" ]; then
    echo -e "${GREEN}✓ Executable created: ${TARGET}_custom${NC}"
    
    # Show file info
    echo "  File size: $(ls -lh ${TARGET}_custom | awk '{print $5}')"
    echo "  File type: $(file ${TARGET}_custom)"
    
    # Check if it's a RISC-V executable
    if file "${TARGET}_custom" | grep -q "RISC-V"; then
        echo -e "${GREEN}✓ Confirmed RISC-V executable${NC}"
    else
        echo -e "${YELLOW}⚠ Warning: File may not be RISC-V executable${NC}"
    fi
else
    echo -e "${RED}Error: Executable not created${NC}"
    exit 1
fi

# Step 6: Summary
echo
echo -e "${GREEN}=== Build Summary ===${NC}"
echo "Generated files:"
echo "  - ${TARGET}.o (object file)"
echo "  - ${TARGET}.opt.ll (optimized LLVM IR)"
echo "  - ${TARGET}.opt.no-debug.ll (LLVM IR without debug info)"
echo "  - ${TARGET}_custom.s (assembly file without debug info and M extension)"
echo "  - ${TARGET}_custom (RISC-V executable with custom compiler)"
if [ -f "${TARGET}.backup" ]; then
    echo "  - ${TARGET}.backup (original executable backup)"
fi

echo
echo -e "${GREEN}✓ Build completed successfully!${NC}"
echo "To run the executable: ./${TARGET}_custom"
echo "To clean up: make clean"

