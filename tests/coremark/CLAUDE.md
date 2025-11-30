# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is a modified CoreMark benchmark implementation designed for embedded system testing, specifically targeting hardware simulation in Vivado. The code runs two primary benchmarks in an infinite loop:
1. **Matrix operations** - Tests on 10x10 integer matrices
2. **Linked list operations** - Tests on dynamically allocated linked lists

The benchmark communicates with simulation environment via memory-mapped I/O at specific addresses (`BRIDGE` at 0x7F00 and `END_TEST_ADDR` at 0x20000000).

## Architecture

### Core Components

**core_main.c** - Main benchmark loop
- Runs infinite test loop executing matrix and list benchmarks
- Validates results against expected checksums (RES_MATRIX=0x7E10B293, RES_LIST=0x028284F)
- Communicates results to simulation environment via memory-mapped bridge
- Counts consecutive successful tests (TEST_PASSED_C=300) before signaling completion

**core_matrix.c** - Matrix benchmark operations
- Matrix size: 10x10 (MATRIX_N)
- Operations: initialization, constant multiplication/addition, vector multiplication, matrix multiplication, bitfield extraction
- `matrix_test()`: Orchestrates sequence of matrix operations
- `bit_extract()` macro: Extracts bit ranges for specialized operations

**core_list_join.c** - Linked list benchmark
- Implements singly-linked list with head/data structure separation
- Operations: find, reverse, insert, remove, mergesort
- `core_bench_list()`: Main benchmark performing find/reverse/sort/remove/reinsert operations
- Memory allocated via `malloc()` based on TOTAL_DATA_SIZE (2048 bytes)

**core_state.c** - State machine benchmark (from original EEMBC CoreMark)
- Implements Moore machine for parsing numeric formats (int, float, scientific notation)
- Validates input strings and transitions between states
- Used in original CoreMark; may not be actively used in this embedded variant

**core_util.c** - CRC utility functions
- Implements 8/16/32-bit CRC calculations for result validation
- Originally from EEMBC CoreMark benchmark suite

**coremark.h** - Common definitions
- Type definitions: MAT (matrix type), list_data, list_head
- Configuration constants: MATRIX_N, SEED, LIMIT, TOTAL_DATA_SIZE
- Expected result checksums for validation
- Function prototypes

### Data Structures

```c
// Matrix: 10x10 integer array
typedef int MAT[MATRIX_N][MATRIX_N];

// Linked list node
typedef struct list_head_s {
    struct list_head_s *next;
    struct list_data_s *info;
} list_head;

// List data payload
typedef struct list_data_s {
    int data;
    uint32_t idx;
} list_data;
```

## Hardware/Simulation Interface

The code targets an embedded system with memory-mapped I/O:

- **BRIDGE (0x7F00)**: Writes benchmark results for monitoring
- **END_TEST_ADDR (0x20000000)**: Test status peripheral
  - Write 1: Test failed
  - Write 187: Test passed after TEST_PASSED_C successful iterations

## Build and Execution

**Note**: This repository does not contain a Makefile. The code is compiled as part of a larger embedded toolchain targeting a specific hardware platform.

**Binary artifact**: `coremark.hex` - Pre-compiled Intel HEX format binary for loading into simulation/hardware

To work with this code:
- Cross-compilation required for target architecture (compiler/flags not included here)
- Likely built with embedded toolchain (arm-none-eabi-gcc, riscv-gcc, or similar)
- Final output format: Intel HEX for direct memory loading

## Key Constants and Configuration

Located in coremark.h:
- `MATRIX_N = 10`: Matrix dimensions
- `SEED = 1`: Initial seed for deterministic random generation
- `LIMIT = 128`: Bounds for matrix value generation
- `TOTAL_DATA_SIZE = 2048`: Memory allocation size for linked lists
- `RES_MATRIX = 0x7E10B293`: Expected checksum for matrix test
- `RES_LIST = 0x028284F`: Expected checksum for list test
- `TEST_PASSED_C = 300`: Number of consecutive passes before success signal

## Code Characteristics

- **Language**: C (C89/C99 compatible)
- **Memory management**: Uses `malloc()`/`free()` for list benchmark
- **Deterministic**: All operations seeded for reproducible results
- **No standard I/O**: Uses memory-mapped I/O instead of printf/scanf
- **Infinite loop**: Designed to run continuously for hardware stress testing
- **EEMBC heritage**: State machine and utility code derived from official CoreMark

## Licensing

Code contains Apache 2.0 licensed components from EEMBC (Embedded Microprocessor Benchmark Consortium). Original author: Shay Gal-on. See headers in core_state.c and core_util.c.
