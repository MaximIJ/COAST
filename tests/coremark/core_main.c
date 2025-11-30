#include "coremark.h"

#define BRIDGE 0x7F00
#define N 500

#define END_TEST_ADDR   0x20000000


MAT A, B, C;

int main(void)
{
    int test_flag;
    int res, i, j;

    int test_counter = 0;

    uint32_t size = TOTAL_DATA_SIZE;
    void *memblock;
    struct list_head_s *list;
    uint32_t res2;


    int *bridge;
    bridge = (int*)BRIDGE;
    int *test_periph;
    test_periph = (int*)END_TEST_ADDR;


    // Bucle infinito
    while (1) {
        // Bench matrices
        res = 0;
        // Inicializa matrices
        core_init_matrix(A, SEED);
        core_init_matrix(B, SEED+2);
        core_init_matrix(C, SEED+4);

        // Ejecuta batch matrices
        matrix_test(A, B, C, 25);

        // Hace una suma de todos los resultados para comprobar si test passed
        for (i = 0; i < MATRIX_N; i++) {
            for (j = 0; j < MATRIX_N; j++) {
                res += A[i][j];
                res += B[i][j];
                res += C[i][j];
            }
        }

        // Comprueba el resultado
        // SIMULACION VIVADO
        bridge[0] = res;
        if (res != RES_MATRIX) {
            test_periph[0] = 1;
        }


        // Bench Listas
        memblock = malloc(size);
        list = core_list_init(size, memblock, SEED+3);
        res2 = core_bench_list(list, 0x66, SEED+3);

        // Comprueba el resultado
        // SIMULACION VIVADO
        bridge[0] = res2;
        if (res2 == RES_LIST) {
            test_counter += 1;
            if (test_counter == TEST_PASSED_C) {
                test_periph[0] = 187;
                test_counter = 0;
            }
        }
        else {
            test_periph[0] = 1;
            test_counter = 0;
        }

        // Libera la memoria
        free(memblock);
    }


    return 0;
}
