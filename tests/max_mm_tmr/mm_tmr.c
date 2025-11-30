#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "../COAST.h"
#define MATRIX_SIZE 64
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
    int i, j, k, sum;
    for (i = 0; i < MATRIX_SIZE; i++) {
        for (j = 0; j < MATRIX_SIZE; j++) {
            sum = 0;
            for ( k = 0; k < MATRIX_SIZE; k++) {
                if (i == 6 && j == 6 && k == 6) {
                    sum = -1; // Fault injection point
                } else {
                    sum += matrix_a[i][k] * matrix_b[k][j];
                }
                //sum += matrix_a[i][k] * matrix_b[k][j];
            }
            matrix_c[i][j] = sum;
        }
    }
}

void mm() {
    testing();
}
