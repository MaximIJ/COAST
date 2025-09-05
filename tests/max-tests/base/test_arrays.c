#include "../../COAST.h"
#include <stdio.h>

int __xMR sum_array(int arr[], int size) {
    int sum = 0;
    for (int i = 0; i < size; i++) {
        sum += arr[i];
    }
    return sum;
}

int __xMR main() {
    int numbers[] = {1, 2, 3, 4, 5};
    int size = sizeof(numbers) / sizeof(numbers[0]);
    int total = sum_array(numbers, size);
    printf("Array test: sum = %d\n", total);
    return 0;
}