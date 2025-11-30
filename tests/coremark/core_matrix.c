#include "coremark.h"


void matrix_test(MAT A, MAT B, MAT C, int c);
void   matrix_mul_const(MAT A, MAT B, int c);
void   matrix_mul_vect(MAT A, MAT B, MAT C);
void   matrix_mul_matrix(MAT A, MAT B, MAT C);
void   matrix_mul_matrix_bitextract(MAT A, MAT B, MAT C);
void   matrix_add_const(MAT A, int c);

#define bit_extract(x, from, to) (((x) >> (from)) & (~(0xffffffff << (to))))



// Un popquito caotico para que se use todo
void
matrix_test(MAT A, MAT B, MAT C, int c)
{
    matrix_add_const(A, c); /* make sure data changes  */

    matrix_mul_const(A, C, c);

    matrix_mul_vect(B, C, A);

    matrix_mul_matrix(C, A, B);

    matrix_mul_matrix_bitextract(A, B, C);

    return;
}

void
core_init_matrix(MAT A, int seed)
{
    int order = 1;
    int val, i, j;
    for (i = 0; i < MATRIX_N; i++) {
        for (j = 0; j < MATRIX_N; j++) {
            order = ((order * seed) % LIMIT);
            val = seed + order;
            A[i][j] = val;
            order++;
        }
    }

    return;
}


void
matrix_mul_const(MAT A, MAT B, int c)
{
    int i, j;
    for (i = 0; i < MATRIX_N; i++) {
        for (j = 0; j < MATRIX_N; j++) {
            B[i][j] = A[i][j] * c;
        }
    }
    return;
}


void
matrix_add_const(MAT A, int c)
{
    int i, j;
    for (i = 0; i < MATRIX_N; i++) {
        for (j = 0; j < MATRIX_N; j++) {
            A[i][j] += c;
        }
    }
}


void
matrix_mul_vect(MAT A, MAT B, MAT C)
{
    int i, j;
    for (i = 0; i < MATRIX_N; i++) {
        C[0][i] = 0;
        for (j = 0; j < MATRIX_N; j++) {
            C[0][i] += A[i][j] * B[j][0];
        }
    }
}


void matrix_mul_matrix_tmr_1(MAT A, MAT B, MAT C) {
    int i, j, k;
    for (i = 0; i < MATRIX_N; i++) {
        for (j = 0; j < MATRIX_N; j++) {
            C[i][j] = 0;
            for (k = 0; k < MATRIX_N; k++) {
                C[i][j] += A[i][k] * B[k][i];
            }
        }
    }
}

void matrix_mul_matrix_tmr_2(MAT A, MAT B, MAT C) {
    int i, j, k;
    for (i = 0; i < MATRIX_N; i++) {
        for (j = 0; j < MATRIX_N; j++) {
            C[i][j] = 0;
            for (k = 0; k < MATRIX_N; k++) {
                C[i][j] += A[i][k] * B[k][i];
            }
        }
    }
}

void matrix_mul_matrix_tmr_3(MAT A, MAT B, MAT C) {
    int i, j, k;
    for (i = 0; i < MATRIX_N; i++) {
        for (j = 0; j < MATRIX_N; j++) {
            C[i][j] = 0;
            for (k = 0; k < MATRIX_N; k++) {
                C[i][j] += A[i][k] * B[k][i];
            }
        }
    }
}

void matrix_mul_matrix(MAT A, MAT B, MAT C) {
    #define copy_MAT(a, b) for (int _i1 = 0; _i1 < MATRIX_N; ++_i1) { for (int _i2 = 0; _i2 < MATRIX_N; ++_i2) { a[_i1][_i2] = b[_i1][_i2]; }}

    MAT A_copy1;
    MAT A_copy2;
    MAT A_copy3;
    MAT B_copy1;
    MAT B_copy2;
    MAT B_copy3;
    MAT C_copy1;
    MAT C_copy2;
    MAT C_copy3;
    copy_MAT(A_copy1, A);
    copy_MAT(A_copy2, A);
    copy_MAT(A_copy3, A);
    copy_MAT(B_copy1, B);
    copy_MAT(B_copy2, B);
    copy_MAT(B_copy3, B);
    copy_MAT(C_copy1, C);
    copy_MAT(C_copy2, C);
    copy_MAT(C_copy3, C);

    matrix_mul_matrix_tmr_1(A_copy1, B_copy1, C_copy1);
    matrix_mul_matrix_tmr_2(A_copy2, B_copy2, C_copy2);
    matrix_mul_matrix_tmr_3(A_copy3, B_copy3, C_copy3);

    for (int _i1 = 0; _i1 < MATRIX_N; ++_i1) {
        for (int _i2 = 0; _i2 < MATRIX_N; ++_i2) {
            if (A_copy1[_i1][_i2] == A_copy2[_i1][_i2] || A_copy1[_i1][_i2] == A_copy3[_i1][_i2]) {
                A[_i1][_i2] = A_copy1[_i1][_i2];
            } else if (A_copy2[_i1][_i2] == A_copy3[_i1][_i2]) {
                A[_i1][_i2] = A_copy2[_i1][_i2];
            } else {
                A[_i1][_i2] = A_copy1[_i1][_i2];
            }
        }
    }
    for (int _i1 = 0; _i1 < MATRIX_N; ++_i1) {
        for (int _i2 = 0; _i2 < MATRIX_N; ++_i2) {
            if (B_copy1[_i1][_i2] == B_copy2[_i1][_i2] || B_copy1[_i1][_i2] == B_copy3[_i1][_i2]) {
                B[_i1][_i2] = B_copy1[_i1][_i2];
            } else if (B_copy2[_i1][_i2] == B_copy3[_i1][_i2]) {
                B[_i1][_i2] = B_copy2[_i1][_i2];
            } else {
                B[_i1][_i2] = B_copy1[_i1][_i2];
            }
        }
    }
    for (int _i1 = 0; _i1 < MATRIX_N; ++_i1) {
        for (int _i2 = 0; _i2 < MATRIX_N; ++_i2) {
            if (C_copy1[_i1][_i2] == C_copy2[_i1][_i2] || C_copy1[_i1][_i2] == C_copy3[_i1][_i2]) {
                C[_i1][_i2] = C_copy1[_i1][_i2];
            } else if (C_copy2[_i1][_i2] == C_copy3[_i1][_i2]) {
                C[_i1][_i2] = C_copy2[_i1][_i2];
            } else {
                C[_i1][_i2] = C_copy1[_i1][_i2];
            }
        }
    }
    #undef copy_MAT
}


void
matrix_mul_matrix_bitextract(MAT A, MAT B, MAT C)
{
    int i, j, k;
    for (i = 0; i < MATRIX_N; i++) {
        for (j = 0; j < MATRIX_N; j++) {
            C[i][j] = 0;
            for (k = 0; k < MATRIX_N; k++) {
                int tmp = A[i][k] * B[k][j];
                C[i][j] += bit_extract(tmp, 2, 4) * bit_extract(tmp, 5, 7);
            }
        }
    }
}
