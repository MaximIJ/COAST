#include "../../COAST.h"
#include <stdio.h>

int __xMR factorial(int n) {
    if (n <= 1) return 1;
    int result = 1;
    for (int i = 2; i <= n; i++) {
        result *= i;
    }
    return result;
}

int __xMR main() {
    int n = 5;
    int fact = factorial(n);
    printf("Loop test: %d! = %d\n", n, fact);
    return 0;
}