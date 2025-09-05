#include "../../COAST.h"
#include <stdio.h>
#include <stdlib.h>

int __xMR pointer_arithmetic(int* ptr, int offset) {
    return *(ptr + offset);
}

int __xMR main() {
    int* data = malloc(5 * sizeof(int));
    for (int i = 0; i < 5; i++) {
        data[i] = i * 10;
    }
    
    int value = pointer_arithmetic(data, 2);
    printf("Pointer test: data[2] = %d\n", value);
    
    free(data);
    return 0;
}