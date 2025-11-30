#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "../COAST.h"
#define MATRIX_SIZE 64
#define BLOCK_SIZE 8
#define TMR_ERROR_CNT

#if INJECT_FAULTS
#include <time.h>
#include <unistd.h>
#include <signal.h>
#endif

int64_t matrix_a[MATRIX_SIZE][MATRIX_SIZE];
int64_t matrix_b[MATRIX_SIZE][MATRIX_SIZE];
int64_t matrix_c[MATRIX_SIZE][MATRIX_SIZE];

void testing(void)
{
    register int i, j, k, i_block, j_block, k_block, sum;
    for (i = 0; i < MATRIX_SIZE; i++) {
        for (j = 0; j < MATRIX_SIZE; j++) {
            matrix_c[i][j] = 0;
        }
    }

    for (i = 0; i < MATRIX_SIZE; i += BLOCK_SIZE) {
        for (j = 0; j < MATRIX_SIZE; j += BLOCK_SIZE) {
            for (k = 0; k < MATRIX_SIZE; k += BLOCK_SIZE) {
                for (i_block = i; i_block < i + BLOCK_SIZE && i_block < MATRIX_SIZE; i_block++) {
                    for (j_block = j; j_block < j + BLOCK_SIZE && j_block < MATRIX_SIZE; j_block++) {
                        sum = 0;
                        for (k_block = k; k_block < k + BLOCK_SIZE && k_block < MATRIX_SIZE; k_block++) {
                            sum += matrix_a[i_block][k_block] * matrix_b[k_block][j_block];
                        }
                        matrix_c[i_block][j_block] = sum;
                    }
                }
            }
        }
    }
}

void mm() {
    testing();
}
