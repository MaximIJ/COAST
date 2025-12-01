#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define		array_elements				580
#define		robust_printing				1
#define		change_rate				100

unsigned long int ind = 0;
int local_errors = 0;
int in_block = 0;
int seed_value = -1;
int array[array_elements];
int array_dmr_1[array_elements];
int array_dmr_2[array_elements];
int golden_array[array_elements];
int golden_array_rev[array_elements];

void init_array() {
    int i = 0;

    //seed the random number generator
    //the input arrays are reset on error to the same values, so
    //the seed value is not always new.  The seed value also
    //changes with the change rate so that new values are used
    //every few seconds during the test.
    if (seed_value == -1) {
        srand(ind);
        seed_value = ind;
    }
    else {
        srand(seed_value);
    }

    //fill the arrays
    for (i = 0; i < array_elements; i++) {
        int val = rand();
        array[i] = val;
        array_dmr_1[i] = val;
        array_dmr_2[i] = val;
        golden_array[i] = val;
        golden_array_rev[i] = val;
    }
}

//*****************************************************************************
//
// Quick sort code from http://rosettacode.org/wiki/Sorting_algorithms/Quicksort#C
//
//*****************************************************************************

void quick_sort_dmr(int *A, int len) {
    if (len < 2) return;

    int pivot = A[len / 2];

    int i, j;
    for (i = 0, j = len - 1; ; i++, j--) {
        while (A[i] < pivot) i++;
        while (A[j] > pivot) j--;

        if (i >= j) break;

        int temp = A[i];
        A[i]     = A[j];
        A[j]     = temp;
    }

    quick_sort_dmr(A, i);
    quick_sort_dmr(A + i, len - i);
}

void quick_sort_rev_dmr(int *a, int n) {
    if (n < 2) return;

    int pivot = a[n / 2];
    int i, j;
    for (i = 0, j = n - 1; ; i++, j--) {
        while (a[i] > pivot) i++;
        while (a[j] < pivot) j--;

        if (i >= j) break;
        int temp = a[i];
        a[i]     = a[j];
        a[j]     = temp;
    }
    quick_sort_rev_dmr(a, i);
    quick_sort_rev_dmr(a + i, n - i);
}

// DMR wrapper for forward sort - runs twice and compares
void quick_sort(int *A, int len) {
    int i;
    int diff = 0;
    
    // Copy input to both DMR arrays
    for (i = 0; i < len; i++) {
        array_dmr_1[i] = A[i];
        array_dmr_2[i] = A[i];
    }
    
    // Sort both copies
    quick_sort_dmr(array_dmr_1, len);
    quick_sort_dmr(array_dmr_2, len);
    
    // Compare results
    for (i = 0; i < len; i++) {
        if (array_dmr_1[i] != array_dmr_2[i]) {
            diff = 1;
            break;
        }
    }
    
    // If different, run a third time to get correct result
    if (diff) {
        for (i = 0; i < len; i++) {
            array_dmr_1[i] = A[i];
        }
        quick_sort_dmr(array_dmr_1, len);
        local_errors++;
    }
    
    // Copy result back
    for (i = 0; i < len; i++) {
        A[i] = array_dmr_1[i];
    }
}

// DMR wrapper for reverse sort - runs twice and compares
void quick_sort_rev(int *a, int n) {
    int i;
    int diff = 0;
    
    // Copy input to both DMR arrays
    for (i = 0; i < n; i++) {
        array_dmr_1[i] = a[i];
        array_dmr_2[i] = a[i];
    }
    
    // Sort both copies
    quick_sort_rev_dmr(array_dmr_1, n);
    quick_sort_rev_dmr(array_dmr_2, n);
    
    // Compare results
    for (i = 0; i < n; i++) {
        if (array_dmr_1[i] != array_dmr_2[i]) {
            diff = 1;
            break;
        }
    }
    
    // If different, run a third time to get correct result
    if (diff) {
        for (i = 0; i < n; i++) {
            array_dmr_1[i] = a[i];
        }
        quick_sort_rev_dmr(array_dmr_1, n);
        local_errors++;
    }
    
    // Copy result back
    for (i = 0; i < n; i++) {
        a[i] = array_dmr_1[i];
    }
}

int checker(int golden_array[], int dut_array[], int sub_test) {
    int first_error = 0;
    int num_of_errors = 0;
    int i = 0;

    for (i = 0; i < array_elements; i++) {
        if (golden_array[i] != dut_array[i]) {
            //an error is found, print the results
            if (!first_error) {
                if (!in_block && robust_printing) {
                    printf(" - i: %lu, %i\r\n", ind, sub_test);
                    printf("   E: {%i: [%x, %x],", i, golden_array[i], dut_array[i]);
                    first_error = 1;
                    in_block = 1;
                }
                else if (in_block && robust_printing) {
                    printf("   E: {%i: [%x, %x],", i, golden_array[i], dut_array[i]);
                    first_error = 1;
                }
            }
            else {
                if (robust_printing)
                    printf("%i: [%x, %x],", i, golden_array[i], dut_array[i]);
            }
            num_of_errors++;
        }
    }

    //more printing
    if (first_error) {
        printf("}\r\n");
        first_error = 0;
    }

    //non-robust printing
    if (!robust_printing && (num_of_errors > 0)) {
        if (!in_block) {
            printf(" - i: %lu, %i\r\n", ind, sub_test);
            printf("   E: %i\r\n", num_of_errors);
            in_block = 1;
        }
        else {
            printf("   E: %i\r\n", num_of_errors);
        }
    }

    return num_of_errors;
}

void qsort_test() {
    //initialize variables
    int total_errors = 0;
    int n = sizeof array / sizeof array[0];
    int i = 0;

    //init arrays
    init_array();

    //compute the goldens for the forward and reverse sorts
    quick_sort(golden_array, n);
    quick_sort_rev(golden_array_rev, n);

    while (1) {
        for (i = 0; i < 4; i++) {
            //the first two sorts are forward
            if (i < 2) {
                quick_sort(array, n);
                local_errors += checker(golden_array, array, i);
            }
            else {
                //the last two sorts are reverse
                quick_sort_rev(array, n);
                local_errors += checker(golden_array_rev, array, i);
            }

            //if there is an error, fix the input arrays
            //and recompute the two goldens.
            if (local_errors > 0) {
                init_array();
                quick_sort(golden_array, n);
                quick_sort_rev(golden_array_rev, n);
            }

            total_errors += local_errors;
            local_errors = 0;
            in_block = 0;
        }

        //ack and change input arrays
        if (ind % change_rate == 0) {
            printf("# %lu, %i\r\n", ind, total_errors);
            seed_value = -1;

            //init arrays with new values
            init_array();

            //compute the two new golden arrays
            quick_sort(golden_array, n);
            quick_sort_rev(golden_array_rev, n);
        }

        //reset vars and such
        ind++;
    }
}

int main() {
    //print the YAML header
    printf("\r\n---\r\n");
    printf("hw: msp430f2619\r\n");
    printf("test: QSort\r\n");
    printf("mit: DMR\r\n");
    printf("printing: %i\r\n", robust_printing);
    printf("input change rate: %i\r\n", change_rate);
    printf("Array size: %i\r\n", array_elements);
    printf("ver: 0.1\r\n");
    printf("fac: LANSCE Nov 2015\r\n");
    printf("d:\r\n");

    //start the test
    qsort_test();

    return 0;
}

