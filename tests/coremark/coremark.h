#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


#define MATRIX_N 10
#define SEED 1
#define LIMIT 128 // Limite para no pasarse con las operaciones de matrices
#define TOTAL_DATA_SIZE 2048

#define RES_MATRIX 0x7E10B293 // Resultado final del batch de matrices (sacado de simulacion Vivado)
#define RES_LIST 0x028284F

#define TEST_PASSED_C 300


typedef int MAT[MATRIX_N][MATRIX_N];

typedef struct list_data_s
{
    int data;
    uint32_t idx;
} list_data;

typedef struct list_head_s
{
    struct list_head_s *next;
    struct list_data_s *info;
} list_head;


// Matrices
void core_init_matrix(MAT A, int seed);
void matrix_test(MAT A, MAT B, MAT C, int c);

// Listas
list_head *core_list_init(uint32_t blksize, list_head *memblock, int seed);
uint32_t core_bench_list(list_head *list, uint32_t finder_idx, int seed);
