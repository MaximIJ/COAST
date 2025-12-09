#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../COAST.h"
#define MATRIX_SIZE 64
#define TMR_ERROR_CNT

#if INJECT_FAULTS
#include <unistd.h>
#include <signal.h>
#endif

int64_t matrix_a[MATRIX_SIZE][MATRIX_SIZE];
int64_t matrix_b[MATRIX_SIZE][MATRIX_SIZE];
int64_t matrix_c[MATRIX_SIZE][MATRIX_SIZE];

void testing(void)
{
    int i, j, k, sum;
    for (i = 0; i < MATRIX_SIZE; i++) {
        for (j = 0; j < MATRIX_SIZE; j++) {
            sum = 0;
            for ( k = 0; k < MATRIX_SIZE; k++) {
                sum += matrix_a[i][k] * matrix_b[k][j];
            }
            matrix_c[i][j] = sum;
        }
    }
}

void mm() {
    testing();
}

int main() {
    // Initialize matrices with some values
    int i, j;
    for (i = 0; i < MATRIX_SIZE; i++) {
        for (j = 0; j < MATRIX_SIZE; j++) {
            matrix_a[i][j] = i + j;
            matrix_b[i][j] = i - j;
            matrix_c[i][j] = 0;
        }
    }
    
    // Measure execution time
    clock_t start = clock();
    testing();
    clock_t end = clock();
    
    double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    
    printf("testing() execution time: %f seconds\n", cpu_time_used);
    printf("testing() execution time: %f milliseconds\n", cpu_time_used * 1000.0);
    
    // Print a sample result to verify correctness
    printf("Sample result: matrix_c[0][0] = %ld\n", matrix_c[0][0]);
    
    return 0;
}





