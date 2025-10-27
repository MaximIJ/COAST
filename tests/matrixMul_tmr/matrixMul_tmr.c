#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define MATRIX_SIZE 64

#if INJECT_FAULTS
#include <time.h>
#include <unistd.h>
#include <signal.h>
#endif

uint64_t cycle_counter;
uint64_t cycles_test;
uint64_t cycles_check;
unsigned int block_counter;
unsigned int run_counter;
unsigned int errors;

int64_t matrix_a[MATRIX_SIZE][MATRIX_SIZE];
int64_t matrix_b[MATRIX_SIZE][MATRIX_SIZE];
#pragma triple no_evaluate
int64_t matrix_c[MATRIX_SIZE][MATRIX_SIZE];
int64_t matrix_golden[MATRIX_SIZE][MATRIX_SIZE];

void init(void)
{
    int64_t seed = block_counter * 1000;

    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            // different pseudorandom numbers for each block
            seed = (seed * 1103515245 + 12345) | 0x7FFFFFFFFFFFFFFF;
            // range: 1-10
            matrix_a[i][j] = seed;

            //seed = (seed * 1103515245 + 12345) & 0x7FFFFFFF;
            matrix_b[i][j] = seed;

            matrix_c[i][j] = 0;
            matrix_golden[i][j] = 0;
        }
    }
}

#pragma triple
void test(void)
{
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            matrix_c[i][j] = 0;
            for (int k = 0; k < MATRIX_SIZE; k++) {
                matrix_c[i][j] += matrix_a[i][k] * matrix_b[k][j];
            }
        }
    }
}

void fill(void)
{
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            matrix_golden[i][j] = 0;
            for (int k = 0; k < MATRIX_SIZE; k++) {
                matrix_golden[i][j] += matrix_a[i][k] * matrix_b[k][j];
            }
        }
    }
}

void check(void)
{
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            if (matrix_c[i][j] != matrix_golden[i][j]) {
                errors++;
            }
        }
    }
}

void send_status(unsigned int cycles)
{
    // BLOCK RUN ERRORS CYCLES
    printf("%u\t%u\t%u\t%u\n", block_counter, run_counter, cycles, errors);
}

int matrixMul_tmr_main() {
    test();
    check();
    printf("Test completed with %u errors.\n", errors);
    return 0;
}
