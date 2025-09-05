#include "../../COAST.h"
#include <stdio.h>

int __xMR fibonacci(int n) {
    if (n <= 1) return n;
    return fibonacci(n-1) + fibonacci(n-2);
}

int __xMR main() {
    int n = 10;
    int fib = fibonacci(n);
    printf("Recursive test: fib(%d) = %d\n", n, fib);
    return 0;
}