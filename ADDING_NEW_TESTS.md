# Adding New Tests to COAST

This guide explains how to add new test cases to the COAST (Compiler-Assisted Software Fault Tolerance) framework.

## Overview

COAST supports two main types of fault tolerance techniques:
- **TMR (Triple Modular Redundancy)**: Triplicates functions and compares results
- **DWC (Duplication With Comparison)**: Duplicates functions and compares results

## Quick Start

### 1. Create Test Directory

```bash
mkdir /home/m4x/Documents/Work/UA/coast/COAST/tests/your_test_name
cd /home/m4x/Documents/Work/UA/coast/COAST/tests/your_test_name
```

### 2. Create Source File

Create a C file (e.g., `your_test.c`) with your test code:

```c
#include <stdio.h>
#include <stdint.h>

// Global variables for your test
int result;
int expected_result;

// Function to be protected with TMR
#pragma triple
void your_function() {
    // Your computation here
    result = 42;  // Example computation
}

// Optional: Prevent evaluation of specific variables during TMR
#pragma triple no_evaluate
int protected_variable;

int main() {
    your_function();
    
    // Validation logic
    expected_result = 42;
    if (result == expected_result) {
        printf("Test completed with 0 errors.\n");
    } else {
        printf("Test completed with 1 errors.\n");
    }
    
    return 0;
}
```

### 3. Create Makefile

Create a `Makefile` in your test directory:

```makefile
LEVEL = ..
TARGET = your_test_name
OPT_PASSES = -TMR

include $(LEVEL)/makefiles/Makefile.common
```

**Key Makefile variables:**
- `TARGET`: Name of your test (should match your source file name)
- `OPT_PASSES`: COAST passes to apply (`-TMR`, `-DWC`, or both)
- `LEVEL`: Relative path to tests directory (always `..`)

### 4. Test Your Implementation

```bash
# Clean previous builds
make clean

# Compile for x86 with TMR
make exe BOARD=x86

# Run the test
./your_test_name.out
```

### 5. Add to Test Suite (Optional)

To include your test in automated testing, edit `/home/m4x/Documents/Work/UA/coast/COAST/unittest/cfg/fast.yml`:

```yaml
benchmarks:
  - path: matrixMultiply
  
  - path: your_test_name
    re: "Test completed with 0 errors."

OPT_PASSES:
  - ""
  - "-DWC"
  - "-TMR"
```

## COAST Pragmas Reference

### `#pragma triple`
- **Purpose**: Marks a function for TMR (Triple Modular Redundancy)
- **Usage**: Place before function definition
- **Effect**: Function is triplicated and results are compared

```c
#pragma triple
void critical_function() {
    // This function will be triplicated
}
```

### `#pragma triple no_evaluate`
- **Purpose**: Prevents TMR from evaluating/comparing a variable
- **Usage**: Place before variable declaration
- **Effect**: Variable is not compared during TMR validation

```c
#pragma triple no_evaluate
int large_array[1000];  // Won't be compared during TMR
```

## Test Configuration Options

### OPT_PASSES Options

| Pass | Description |
|------|-------------|
| `""` | No COAST passes (baseline) |
| `-TMR` | Triple Modular Redundancy |
| `-DWC` | Duplication With Comparison |
| `-TMR -countErrors` | TMR with error counting |
| `-DWC -noMemReplication` | DWC without memory replication |
| `-TMR -noLoadSync` | TMR without load synchronization |

### Board Targets

| BOARD | Description |
|-------|-------------|
| `x86` | Native x86 compilation |
| `hiFive1` | RISC-V HiFive1 (requires Freedom SDK) |
| `lli` | LLVM JIT execution |

## Example: Complete Test Case

Here's a complete example of a matrix multiplication test:

### `tests/matrixMul_tmr/matrixMul_tmr.c`
```c
#include <stdint.h>
#include <stdio.h>

#define MATRIX_SIZE 64

int64_t matrix_a[MATRIX_SIZE][MATRIX_SIZE];
int64_t matrix_b[MATRIX_SIZE][MATRIX_SIZE];
#pragma triple no_evaluate
int64_t matrix_c[MATRIX_SIZE][MATRIX_SIZE];
int64_t matrix_golden[MATRIX_SIZE][MATRIX_SIZE];

unsigned int errors = 0;

#pragma triple
void test(void) {
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            matrix_c[i][j] = 0;
            for (int k = 0; k < MATRIX_SIZE; k++) {
                matrix_c[i][j] += matrix_a[i][k] * matrix_b[k][j];
            }
        }
    }
}

void check(void) {
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            if (matrix_c[i][j] != matrix_golden[i][j]) {
                errors++;
            }
        }
    }
}

int main() {
    test();
    check();
    printf("Test completed with %u errors.\n", errors);
    return 0;
}
```

### `tests/matrixMul_tmr/Makefile`
```makefile
LEVEL = ..
TARGET = matrixMul_tmr
OPT_PASSES = -TMR

include $(LEVEL)/makefiles/Makefile.common
```

## Running Tests

### Manual Testing
```bash
cd /home/m4x/Documents/Work/UA/coast/COAST/tests/your_test_name
make clean
make exe BOARD=x86 OPT_PASSES="-TMR"
./your_test_name.out
```

### Automated Testing
```bash
cd /home/m4x/Documents/Work/UA/coast/COAST

# Run fast test suite
make test_fast

# Run full TMR tests
make test_full_tmr

# Run comprehensive tests
make test_full
```

## Troubleshooting

### Common Issues

1. **Compilation Errors**
   - Check that your `TARGET` in Makefile matches your source file name
   - Ensure all required headers are included
   - Verify pragma syntax is correct

2. **Runtime Errors**
   - Check that your test outputs the expected validation string
   - Ensure error counting logic is correct
   - Verify that protected functions are properly marked

3. **TMR/DWC Not Working**
   - Verify pragma directives are correctly placed
   - Check that OPT_PASSES includes the desired pass
   - Ensure the COAST build is up to date

### Debugging Tips

- Use `make clean` before each test to ensure fresh compilation
- Check the generated `.opt.ll` file to see if TMR/DWC was applied
- Use `printf` statements to trace execution flow
- Test without COAST passes first (`OPT_PASSES=""`) to verify baseline functionality

## File Structure

```
tests/
├── your_test_name/
│   ├── your_test_name.c    # Source code
│   ├── Makefile            # Build configuration
│   ├── your_test_name.out  # Compiled executable
│   └── *.bc, *.ll, *.s     # Intermediate files
└── makefiles/
    ├── Makefile.common     # Common build rules
    ├── Makefile.compile.x86
    └── Makefile.compile.hiFive1
```

## Best Practices

1. **Test Design**
   - Keep tests simple and focused
   - Use clear, descriptive names
   - Include validation logic that reports errors

2. **Pragma Usage**
   - Only protect functions that need fault tolerance
   - Use `no_evaluate` for large arrays or complex data structures
   - Test both with and without COAST passes

3. **Output Format**
   - Use consistent output format: "Test completed with X errors."
   - Include error counts for validation
   - Make output parseable for automated testing

4. **Documentation**
   - Comment your test code
   - Document the purpose of each protected function
   - Explain any special pragma usage

## Advanced Features

### Custom Pass Combinations
```makefile
OPT_PASSES = -TMR -countErrors -noMemReplication
```

### RISC-V Testing
```makefile
BOARD = hiFive1
# Requires Freedom SDK installation
```

### Memory Protection
```c
#pragma triple no_evaluate
int sensitive_data[LARGE_SIZE];
```

This guide should help you create effective test cases for the COAST framework. For more examples, examine the existing tests in the `tests/` directory.
