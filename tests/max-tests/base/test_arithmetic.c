#include "../../COAST.h"
#include <stdio.h>

int __xMR critical_computation(int x) {
    return x * x + 2 * x + 1; // Should be triplicated
}

int __xMR main() {
    int input = 5;
    int result = critical_computation(input);
    printf("Arithmetic test: f(%d) = %d\n", input, result);
    return result;
}