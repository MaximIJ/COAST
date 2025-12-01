#!/bin/bash

# Simple wrapper script to use custom RISC-V compiler with existing Makefile system
# Now includes assembly file generation

set -e

# Get script and project directories
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"

# Change to project directory
cd "$PROJECT_DIR"

# Colors for output
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${YELLOW}Building with custom RISC-V compiler (including assembly)...${NC}"

# Use the custom Makefile with assembly generation
make -f Makefile.custom exe-with-assembly

echo -e "${GREEN}✓ Build completed with custom RISC-V compiler!${NC}"
echo "Executable: crc32_tmr"
echo "Assembly file: crc32_tmr_custom.s"

