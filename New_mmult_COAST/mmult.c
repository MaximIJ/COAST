#include "uart.h"
#include "mmult.h"
#include "kernelsdk.h"

extern int64_t matrix_a[MATRIX_SIZE][MATRIX_SIZE];
extern int64_t matrix_a_DWC[MATRIX_SIZE][MATRIX_SIZE];
extern int64_t matrix_a_TMR[MATRIX_SIZE][MATRIX_SIZE];
extern int64_t matrix_b[MATRIX_SIZE][MATRIX_SIZE];
extern int64_t matrix_b_DWC[MATRIX_SIZE][MATRIX_SIZE];
extern int64_t matrix_b_TMR[MATRIX_SIZE][MATRIX_SIZE];
extern int64_t matrix_c[MATRIX_SIZE][MATRIX_SIZE];
extern int64_t matrix_c_DWC[MATRIX_SIZE][MATRIX_SIZE];
extern int64_t matrix_c_TMR[MATRIX_SIZE][MATRIX_SIZE];
int64_t matrix_golden[MATRIX_SIZE][MATRIX_SIZE];

unsigned int tmr_mitigations = 0; // Initialize the counter
void mm();

void init(void)
{
    int64_t seed = block_counter * 1000;
    
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            // números pseudoaleatorios diferentes para cada bloque
            seed = (seed * 1103515245 + 12345) & 0x7FFFFFFFFFFFFFFF;
            // range: 1-10
            matrix_a[i][j] = (seed % 100) + 1;  
	    matrix_a_DWC[i][j] = (seed % 100) + 1;  
            matrix_a_TMR[i][j] = (seed % 100) + 1;  
            matrix_b[i][j] = (seed % 100) + 1;
            matrix_b_DWC[i][j] = (seed % 100) + 1;
            matrix_b_TMR[i][j] = (seed % 100) + 1;
            matrix_golden[i][j] = 0;
        }
    }
}

void test(void)
{
    // Call the assembly matrix multiplication function
    mm();
}

void fill(void)
{
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            matrix_golden[i][j] = 0;
            for (int k = 0; k < MATRIX_SIZE; k++) {
                matrix_golden[i][j] += matrix_a[i][k] * matrix_b[k][j];
            }
	    //printf("%u\t",matrix_golden[i][j]);
        }
    }
}

void check(void)
{    
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
	    //printf("%u - %u - %u - %u\n", matrix_a[i][j], matrix_b[i][j], matrix_c[i][j], matrix_golden[i][j]);
            if (matrix_c[i][j] != matrix_golden[i][j]) {
                errors++;
            }
        }
    }
}

void send_status(unsigned int cycles)
{
// BLOCK RUN ERRORS CYCLES TMR_MITIGATIONS
    printf("%u\t%u\t%u\t%u\t%u\n", block_counter, run_counter, cycles, errors, TMR_ERROR_CNT);
}
