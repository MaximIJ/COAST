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
int array_tmr_1[array_elements];
int array_tmr_2[array_elements];
int array_tmr_3[array_elements];
int golden_array[array_elements];
int golden_array_rev[array_elements];
unsigned int errors_12;
unsigned int errors_13;
unsigned int errors_23;
unsigned int unmitigated;

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
        array_tmr_1[i] = val;
        array_tmr_2[i] = val;
        array_tmr_3[i] = val;
        golden_array[i] = val;
        golden_array_rev[i] = val;
    }
}

//*****************************************************************************
//
// Quick sort code from http://rosettacode.org/wiki/Sorting_algorithms/Quicksort#C
//
//*****************************************************************************

void quick_sort_tmr_1(int *A, int len) {
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

    quick_sort_tmr_1(A, i);
    quick_sort_tmr_1(A + i, len - i);
}

void quick_sort_tmr_2(int *A, int len) {
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

    quick_sort_tmr_2(A, i);
    quick_sort_tmr_2(A + i, len - i);
}

void quick_sort_tmr_3(int *A, int len) {
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

    quick_sort_tmr_3(A, i);
    quick_sort_tmr_3(A + i, len - i);
}

void quick_sort_rev_tmr_1(int *a, int n) {
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
    quick_sort_rev_tmr_1(a, i);
    quick_sort_rev_tmr_1(a + i, n - i);
}

void quick_sort_rev_tmr_2(int *a, int n) {
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
    quick_sort_rev_tmr_2(a, i);
    quick_sort_rev_tmr_2(a + i, n - i);
}

void quick_sort_rev_tmr_3(int *a, int n) {
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
    quick_sort_rev_tmr_3(a, i);
    quick_sort_rev_tmr_3(a + i, n - i);
}

// Element-wise voter for arrays
void custom_Voter(int *result, int len) {
    int i;
    for (i = 0; i < len; i++) {
        if (array_tmr_1[i] == array_tmr_2[i] 
            && array_tmr_1[i] == array_tmr_3[i]) { // all agree
            result[i] = array_tmr_1[i];
        } else if (array_tmr_2[i] == array_tmr_3[i]) { // error in 1
            result[i] = array_tmr_2[i];
        } else { // error in 2 or 3
            result[i] = array_tmr_1[i];
        }
    }
}

// TMR wrapper for forward sort - runs three times and votes
void quick_sort(int *A, int len) {
    int i;
    
    // Copy input to all three TMR arrays
    for (i = 0; i < len; i++) {
        array_tmr_1[i] = A[i];
        array_tmr_2[i] = A[i];
        array_tmr_3[i] = A[i];
    }
    
    // Sort all three copies
    quick_sort_tmr_1(array_tmr_1, len);
    quick_sort_tmr_2(array_tmr_2, len);
    quick_sort_tmr_3(array_tmr_3, len);
    
    // Vote and store result
    custom_Voter(A, len);
}

// TMR wrapper for reverse sort - runs three times and votes
void quick_sort_rev(int *a, int n) {
    int i;
    
    // Copy input to all three TMR arrays
    for (i = 0; i < n; i++) {
        array_tmr_1[i] = a[i];
        array_tmr_2[i] = a[i];
        array_tmr_3[i] = a[i];
    }
    
    // Sort all three copies
    quick_sort_rev_tmr_1(array_tmr_1, n);
    quick_sort_rev_tmr_2(array_tmr_2, n);
    quick_sort_rev_tmr_3(array_tmr_3, n);
    
    // Vote and store result
    custom_Voter(a, n);
}

void check_tmr(int len) {
    int i;
    for (i = 0; i < len; i++) {
        if (array_tmr_1[i] != array_tmr_2[i]) {
            errors_12++;
        }
        if (array_tmr_1[i] != array_tmr_3[i]) {
            errors_13++;
        }
        if (array_tmr_2[i] != array_tmr_3[i]) {
            errors_23++;
        }
    }
    if (errors_12 != 0 && errors_13 != 0 && errors_23 != 0) {
        unmitigated++;
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
                check_tmr(n);
                local_errors = checker(golden_array, array, i);
            }
            else {
                //the last two sorts are reverse
                quick_sort_rev(array, n);
                check_tmr(n);
                local_errors = checker(golden_array_rev, array, i);
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
    printf("mit: TMR\r\n");
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

