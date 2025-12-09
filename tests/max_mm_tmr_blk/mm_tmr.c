#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "../COAST.h"
#define MATRIX_SIZE 20
#define TMR_ERROR_CNT
#define BLK_SIZE 5

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
    register int i, j, k, i_blk, j_blk, k_blk, sum;
    for (i = 0; i < MATRIX_SIZE; i += BLK_SIZE) {
        for (j = 0; j < MATRIX_SIZE; j += BLK_SIZE) {
            for (k = 0; k < MATRIX_SIZE; k += BLK_SIZE) {
                for (i_blk = i; i_blk < i + BLK_SIZE && i_blk < MATRIX_SIZE; i_blk++) {
                    for (j_blk = j; j_blk < j + BLK_SIZE && j_blk < MATRIX_SIZE; j_blk++) {
                        sum = 0;
                        for (k_blk = k; k_blk < k + BLK_SIZE && k_blk < MATRIX_SIZE; k_blk++) {
                            sum += matrix_a[i_blk][k_blk] * matrix_b[k_blk][j_blk];
                        }
                        matrix_c[i_blk][j_blk] = sum;
                    }
                }
            }
        }
    }
}

void mm() {
    testing();
}
