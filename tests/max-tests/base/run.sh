#!/bin/bash

# COAST Plugin Testing Script
# Tests LLVM 16 clang plugin functionality with comprehensive options

set -e  # Exit on any error

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
BUILD_DIR="../../../build-laptop"
DATAFLOW_SO="${BUILD_DIR}/dataflowProtection/DataflowProtection.so"
TMR_SO="${BUILD_DIR}/TMR/TMR.so"
DWC_SO="${BUILD_DIR}/DWC/DWC.so"  # If available

print_usage() {
    echo "Usage: $0 <file.c/file.cpp> <target> [options]"
    echo ""
    echo "Targets:"
    echo "  host     - Compile for host architecture (x86_64)"
    echo "  riscv64  - Compile for RISC-V 64-bit"
    echo "  riscv32  - Compile for RISC-V 32-bit"
    echo ""
    echo "Options:"
    echo "  -s, --assembly       Generate assembly (.s) files"
    echo "  -l, --llvm-ir        Generate LLVM IR (.ll) files"
    echo "  -c, --compare        Generate both original and transformed versions"
    echo "  -o, --optimize LEVEL Optimization level (0,1,2,3) [default: 0]"
    echo "  -e, --executable     Create executable (host only)"
    echo "  -d, --debug          Enable debug output"
    echo "  -t, --time           Time compilation process"
    echo "  -m, --memory         Monitor memory usage"
    echo "  --tmr-only           Load only TMR pass"
    echo "  --dwc-only           Load only DWC pass (if available)"
    echo "  --no-passes          Compile without COAST passes"
    echo "  -h, --help           Show this help"
    echo ""
    echo "Examples:"
    echo "  $0 test.c host -s -l -c          # Full comparison with assembly and IR"
    echo "  $0 test.c riscv64 --tmr-only -o 2  # RISC-V with TMR and O2 optimization"
    echo "  $0 test.c host -e -t             # Create executable with timing"
}

# Parse command line arguments
if [ $# -lt 2 ]; then
    print_usage
    exit 1
fi

SOURCE_FILE="$1"
TARGET="$2"
shift 2

# Default options
GENERATE_ASSEMBLY=false
GENERATE_LLVM_IR=false
COMPARE_VERSIONS=false
OPTIMIZATION_LEVEL="0"
CREATE_EXECUTABLE=false
DEBUG_OUTPUT=false
TIME_COMPILATION=false
MONITOR_MEMORY=false
TMR_ONLY=false
DWC_ONLY=false
NO_PASSES=false

# Parse options
while [[ $# -gt 0 ]]; do
    case $1 in
        -s|--assembly)
            GENERATE_ASSEMBLY=true
            shift
            ;;
        -l|--llvm-ir)
            GENERATE_LLVM_IR=true
            shift
            ;;
        -c|--compare)
            COMPARE_VERSIONS=true
            GENERATE_ASSEMBLY=true
            GENERATE_LLVM_IR=true
            shift
            ;;
        -o|--optimize)
            OPTIMIZATION_LEVEL="$2"
            shift 2
            ;;
        -e|--executable)
            CREATE_EXECUTABLE=true
            shift
            ;;
        -d|--debug)
            DEBUG_OUTPUT=true
            shift
            ;;
        -t|--time)
            TIME_COMPILATION=true
            shift
            ;;
        -m|--memory)
            MONITOR_MEMORY=true
            shift
            ;;
        --tmr-only)
            TMR_ONLY=true
            shift
            ;;
        --dwc-only)
            DWC_ONLY=true
            shift
            ;;
        --no-passes)
            NO_PASSES=true
            shift
            ;;
        -h|--help)
            print_usage
            exit 0
            ;;
        *)
            echo -e "${RED}Unknown option: $1${NC}"
            print_usage
            exit 1
            ;;
    esac
done

# Validate inputs
if [[ ! -f "$SOURCE_FILE" ]]; then
    echo -e "${RED}Error: Source file '$SOURCE_FILE' not found${NC}"
    exit 1
fi

if [[ "$TARGET" != "host" && "$TARGET" != "riscv64" && "$TARGET" != "riscv32" ]]; then
    echo -e "${RED}Error: Invalid target '$TARGET'${NC}"
    print_usage
    exit 1
fi

if [[ ! "$OPTIMIZATION_LEVEL" =~ ^[0-3]$ ]]; then
    echo -e "${RED}Error: Invalid optimization level '$OPTIMIZATION_LEVEL'${NC}"
    exit 1
fi

# Check if COAST libraries exist
check_library() {
    local lib="$1"
    local name="$2"
    if [[ ! -f "$lib" ]]; then
        echo -e "${YELLOW}Warning: $name library not found at $lib${NC}"
        return 1
    fi
    return 0
}

# Prepare compiler flags
CLANG="clang++-16"
BASE_NAME=$(basename "$SOURCE_FILE" .c)
BASE_NAME=$(basename "$BASE_NAME" .cpp)

# Target-specific flags
case "$TARGET" in
    host)
        TARGET_FLAGS=""
        LINKER="clang++-16"
        ;;
    riscv64)
        TARGET_FLAGS="--target=riscv64-unknown-elf"
        LINKER="riscv64-unknown-elf-gcc"
        ;;
    riscv32)
        TARGET_FLAGS="--target=riscv32-unknown-elf"
        LINKER="riscv32-unknown-elf-gcc"
        ;;
esac

# Optimization flags
OPT_FLAGS="-O${OPTIMIZATION_LEVEL}"

# Debug flags
DEBUG_FLAGS=""
if [[ "$DEBUG_OUTPUT" == "true" ]]; then
    DEBUG_FLAGS="-mllvm -debug -v"
fi

# COAST plugin flags
COAST_FLAGS=""
if [[ "$NO_PASSES" == "false" ]]; then
    if [[ "$TMR_ONLY" == "true" ]]; then
        if check_library "$TMR_SO" "TMR"; then
            COAST_FLAGS="-Xclang -load -Xclang $TMR_SO"
        fi
    elif [[ "$DWC_ONLY" == "true" ]]; then
        if check_library "$DWC_SO" "DWC"; then
            COAST_FLAGS="-Xclang -load -Xclang $DWC_SO"
        fi
    else
        # Load all available passes
        if check_library "$DATAFLOW_SO" "DataflowProtection"; then
            COAST_FLAGS="$COAST_FLAGS -Xclang -load -Xclang $DATAFLOW_SO"
        fi
        if check_library "$TMR_SO" "TMR"; then
            COAST_FLAGS="$COAST_FLAGS -Xclang -load -Xclang $TMR_SO"
        fi
    fi
fi

# Timing and memory monitoring setup
TIME_CMD=""
if [[ "$TIME_COMPILATION" == "true" || "$MONITOR_MEMORY" == "true" ]]; then
    if [[ "$MONITOR_MEMORY" == "true" ]]; then
        TIME_CMD="/usr/bin/time -v"
    else
        TIME_CMD="time"
    fi
fi

# Function to run compilation
run_compilation() {
    local suffix="$1"
    local extra_flags="$2"
    local output_name="${BASE_NAME}${suffix}"
    
    echo -e "${BLUE}Compiling $SOURCE_FILE -> ${output_name}.o${NC}"
    
    # Full command
    local cmd="$TIME_CMD $CLANG $TARGET_FLAGS $OPT_FLAGS $COAST_FLAGS $DEBUG_FLAGS $extra_flags -c $SOURCE_FILE -o ${output_name}.o"
    
    echo "Command: $cmd"
    eval "$cmd"
    
    if [[ $? -eq 0 ]]; then
        echo -e "${GREEN}✓ Object file created: ${output_name}.o${NC}"
    else
        echo -e "${RED}✗ Compilation failed${NC}"
        return 1
    fi
}

# Function to generate LLVM IR
generate_llvm_ir() {
    local suffix="$1"
    local extra_flags="$2"
    local output_name="${BASE_NAME}${suffix}"
    
    echo -e "${BLUE}Generating LLVM IR: ${output_name}.ll${NC}"
    
    local cmd="$CLANG $TARGET_FLAGS $OPT_FLAGS $COAST_FLAGS $extra_flags -emit-llvm -S $SOURCE_FILE -o ${output_name}.ll"
    
    echo "Command: $cmd"
    eval "$cmd"
    
    if [[ $? -eq 0 ]]; then
        echo -e "${GREEN}✓ LLVM IR created: ${output_name}.ll${NC}"
    else
        echo -e "${RED}✗ LLVM IR generation failed${NC}"
        return 1
    fi
}

# Function to generate assembly
generate_assembly() {
    local suffix="$1"
    local extra_flags="$2"
    local output_name="${BASE_NAME}${suffix}"
    
    echo -e "${BLUE}Generating assembly: ${output_name}.s${NC}"
    
    local cmd="$CLANG $TARGET_FLAGS $OPT_FLAGS $COAST_FLAGS $extra_flags -S $SOURCE_FILE -o ${output_name}.s"
    
    echo "Command: $cmd"
    eval "$cmd"
    
    if [[ $? -eq 0 ]]; then
        echo -e "${GREEN}✓ Assembly created: ${output_name}.s${NC}"
    else
        echo -e "${RED}✗ Assembly generation failed${NC}"
        return 1
    fi
}

# Function to create executable
create_executable() {
    local object_file="$1"
    local executable_name="$2"
    
    if [[ "$TARGET" != "host" ]]; then
        echo -e "${YELLOW}Warning: Executable creation only supported for host target${NC}"
        return 0
    fi
    
    echo -e "${BLUE}Creating executable: $executable_name${NC}"
    
    local cmd="$LINKER -o $executable_name $object_file"
    
    echo "Command: $cmd"
    eval "$cmd"
    
    if [[ $? -eq 0 ]]; then
        echo -e "${GREEN}✓ Executable created: $executable_name${NC}"
        echo -e "${BLUE}To run: ./$executable_name${NC}"
    else
        echo -e "${RED}✗ Executable creation failed${NC}"
        return 1
    fi
}

# Main compilation process
echo -e "${GREEN}=== COAST Plugin Testing ===${NC}"
echo "Source: $SOURCE_FILE"
echo "Target: $TARGET"
echo "Optimization: O$OPTIMIZATION_LEVEL"
echo "COAST Flags: $COAST_FLAGS $COAST_PASS_FLAGS"
echo ""

# Compile with COAST passes
if [[ "$NO_PASSES" == "false" ]]; then
    run_compilation "_coast" ""
    
    if [[ "$GENERATE_LLVM_IR" == "true" ]]; then
        generate_llvm_ir "_coast" ""
    fi
    
    if [[ "$GENERATE_ASSEMBLY" == "true" ]]; then
        generate_assembly "_coast" ""
    fi
    
    if [[ "$CREATE_EXECUTABLE" == "true" ]]; then
        create_executable "${BASE_NAME}_coast.o" "${BASE_NAME}_coast"
    fi
fi

# Compile without COAST passes (for comparison)
if [[ "$COMPARE_VERSIONS" == "true" || "$NO_PASSES" == "true" ]]; then
    echo ""
    echo -e "${BLUE}=== Compiling without COAST passes (reference) ===${NC}"
    
    # Temporarily disable COAST flags
    COAST_FLAGS_BACKUP="$COAST_FLAGS"
    COAST_FLAGS=""
    
    run_compilation "_original" ""
    
    if [[ "$GENERATE_LLVM_IR" == "true" ]]; then
        generate_llvm_ir "_original" ""
    fi
    
    if [[ "$GENERATE_ASSEMBLY" == "true" ]]; then
        generate_assembly "_original" ""
    fi
    
    if [[ "$CREATE_EXECUTABLE" == "true" ]]; then
        create_executable "${BASE_NAME}_original.o" "${BASE_NAME}_original"
    fi
    
    # Restore COAST flags
    COAST_FLAGS="$COAST_FLAGS_BACKUP"
fi

# Generate file comparison report
if [[ "$COMPARE_VERSIONS" == "true" ]]; then
    echo ""
    echo -e "${GREEN}=== Comparison Report ===${NC}"
    
    # Compare object file sizes
    if [[ -f "${BASE_NAME}_coast.o" && -f "${BASE_NAME}_original.o" ]]; then
        coast_size=$(stat -c%s "${BASE_NAME}_coast.o")
        orig_size=$(stat -c%s "${BASE_NAME}_original.o")
        size_diff=$((coast_size - orig_size))
        size_percent=$(echo "scale=2; ($size_diff * 100) / $orig_size" | bc -l)
        
        echo "Object file sizes:"
        echo "  Original: $orig_size bytes"
        echo "  COAST:    $coast_size bytes"
        echo "  Difference: $size_diff bytes (${size_percent}%)"
    fi
    
    # Compare LLVM IR if generated
    if [[ -f "${BASE_NAME}_coast.ll" && -f "${BASE_NAME}_original.ll" ]]; then
        echo ""
        echo "LLVM IR differences (first 20 lines):"
        diff "${BASE_NAME}_original.ll" "${BASE_NAME}_coast.ll" | head -20
    fi
    
    # Compare assembly if generated
    if [[ -f "${BASE_NAME}_coast.s" && -f "${BASE_NAME}_original.s" ]]; then
        echo ""
        echo "Assembly differences (first 20 lines):"
        diff "${BASE_NAME}_original.s" "${BASE_NAME}_coast.s" | head -20
    fi
fi

echo ""
echo -e "${GREEN}=== Generated Files ===${NC}"
ls -la ${BASE_NAME}*

echo ""
echo -e "${GREEN}=== Testing Complete ===${NC}"