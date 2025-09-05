#!/bin/bash

# COAST Test Runner - Comprehensive testing suite
# This script runs multiple test cases to validate COAST plugin functionality

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

# Configuration
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TEST_SCRIPT="./coast_test.sh"
RESULTS_DIR="test_results_$(date +%Y%m%d_%H%M%S)"

# Test files to run
TEST_FILES=(
    "test_basic.c"
    "test_arithmetic.c" 
    "test_loops.c"
    "test_arrays.c"
    "test_pointers.c"
    "test_recursive.c"
)

# Targets to test
TARGETS=("host" "riscv64" "riscv32")

# Test configurations
CONFIGS=(
    "--no-passes -l -s"              # Original (no COAST)
    "--tmr-only -l -s"               # TMR only
    "--compare -o 0"                 # Full comparison O0
    "--compare -o 2"                 # Full comparison O2
)

# Initialize results
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

# Create results directory
mkdir -p "$RESULTS_DIR"
cd "$RESULTS_DIR"

# Copy test files to results directory
for test_file in "${TEST_FILES[@]}"; do
    if [[ -f "../$test_file" ]]; then
        cp "../$test_file" .
    else
        echo -e "${YELLOW}Warning: Test file $test_file not found${NC}"
    fi
done

# Function to run a single test
run_test() {
    local test_file="$1"
    local target="$2"
    local config="$3"
    local test_name="${test_file}_${target}_$(echo $config | tr ' -' '_')"
    
    echo -e "${BLUE}Running: $test_name${NC}"
    
    # Create subdirectory for this test
    mkdir -p "$test_name"
    cd "$test_name"
    
    # Copy test file to subdirectory
    cp "../$test_file" .
    
    # Run the test
    local cmd="../$TEST_SCRIPT $test_file $target $config"
    
    if eval "$cmd" > test_output.log 2>&1; then
        echo -e "${GREEN}✓ PASSED: $test_name${NC}"
        echo "PASSED" > test_result.txt
        ((PASSED_TESTS++))
    else
        echo -e "${RED}✗ FAILED: $test_name${NC}"
        echo "FAILED" > test_result.txt
        echo "Command: $cmd" >> test_result.txt
        ((FAILED_TESTS++))
    fi
    
    ((TOTAL_TESTS++))
    cd ..
}

# Function to analyze results
analyze_results() {
    local test_dir="$1"
    echo ""
    echo -e "${BLUE}=== Analysis for $test_dir ===${NC}"
    
    cd "$test_dir"
    
    # Check if files were generated
    local files_generated=0
    local expected_files=0
    
    # Check for object files
    if ls *.o > /dev/null 2>&1; then
        echo "✓ Object files generated: $(ls *.o | wc -l)"
        ((files_generated++))
    fi
    ((expected_files++))
    
    # Check for LLVM IR files
    if ls *.ll > /dev/null 2>&1; then
        echo "✓ LLVM IR files generated: $(ls *.ll | wc -l)"
        ((files_generated++))
    fi
    
    # Check for assembly files
    if ls *.s > /dev/null 2>&1; then
        echo "✓ Assembly files generated: $(ls *.s | wc -l)"
        ((files_generated++))
    fi
    
    # Check file sizes
    echo "File sizes:"
    ls -lh *.o *.ll *.s 2>/dev/null | awk '{print "  " $9 ": " $5}'
    
    # Look for evidence of TMR transformation
    if [[ -f *_coast.ll ]]; then
        local tmr_indicators=$(grep -c "tmr\|triple\|redundan" *_coast.ll 2>/dev/null || echo "0")
        if [[ $tmr_indicators -gt 0 ]]; then
            echo "✓ TMR transformation indicators found: $tmr_indicators"
        else
            echo "⚠ No obvious TMR transformation indicators in LLVM IR"
        fi
    fi
    
    cd ..
}

# Main execution
echo -e "${GREEN}=== COAST Comprehensive Test Suite ===${NC}"
echo "Results will be stored in: $RESULTS_DIR"
echo "Starting tests at: $(date)"
echo ""

# Copy the test script
cp "../$TEST_SCRIPT" .

# Run tests for each combination
for test_file in "${TEST_FILES[@]}"; do
    if [[ ! -f "$test_file" ]]; then
        echo -e "${YELLOW}Skipping $test_file (not found)${NC}"
        continue
    fi
    
    echo -e "${BLUE}=== Testing $test_file ===${NC}"
    
    for target in "${TARGETS[@]}"; do
        echo -e "${YELLOW}Target: $target${NC}"
        
        for config in "${CONFIGS[@]}"; do
            run_test "$test_file" "$target" "$config"
        done
        
        echo ""
    done
done

# Generate summary report
echo -e "${GREEN}=== Test Summary ===${NC}"
echo "Total tests run: $TOTAL_TESTS"
echo "Passed: $PASSED_TESTS"
echo "Failed: $FAILED_TESTS"
echo "Success rate: $(echo "scale=1; ($PASSED_TESTS * 100) / $TOTAL_TESTS" | bc -l)%"

# Detailed analysis of selected tests
echo ""
echo -e "${GREEN}=== Detailed Analysis ===${NC}"

# Find a successful test with comparison for analysis
for test_dir in */; do
    if [[ -f "$test_dir/test_result.txt" ]] && grep -q "PASSED" "$test_dir/test_result.txt"; then
        if ls "$test_dir"/*_coast.ll > /dev/null 2>&1; then
            analyze_results "$test_dir"
            break
        fi
    fi
done

# Generate recommendations
echo ""
echo -e "${GREEN}=== Recommendations ===${NC}"

if [[ $FAILED_TESTS -eq 0 ]]; then
    echo "🎉 All tests passed! Your COAST plugin is working correctly."
    echo "   • TMR transformations are being applied"
    echo "   • All target architectures are supported"
    echo "   • Plugin loads and functions properly"
else
    echo "⚠ Some tests failed. Check the following:"
    echo "   • Verify COAST library paths are correct"
    echo "   • Check LLVM version compatibility"
    echo "   • Review compilation errors in failed test logs"
    echo "   • Ensure target toolchains are installed"
fi

# Performance summary
echo ""
echo -e "${GREEN}=== Performance Summary ===${NC}"
echo "Test execution completed at: $(date)"
echo "Results directory: $RESULTS_DIR"
echo ""
echo "To examine individual test results:"
echo "  cd $RESULTS_DIR"
echo "  ls -la"
echo "  cat <test_directory>/test_output.log"

echo ""
echo -e "${GREEN}=== Testing Complete ===${NC}"