#include "uart.h"
#include "quicksort.h"
#include "kernelsdk.h"

// Array buffers - main and TMR copies
extern int array[ARRAY_SIZE];
extern int array_DWC[ARRAY_SIZE];
extern int array_TMR[ARRAY_SIZE];

// Golden arrays (computed without TMR)
int golden_array[ARRAY_SIZE];
int golden_array_rev[ARRAY_SIZE];

// Result flag to indicate which sort was done
int last_sort_forward;

unsigned int tmr_mitigations = 0;
void quicksort();

// Local quicksort for golden computation (not TMR protected)
void local_quick_sort(int *A, int len) {
    if (len < 2) return;

    int pivot = A[len / 2];
    int i, j;
    for (i = 0, j = len - 1; ; i++, j--) {
        while (A[i] < pivot) i++;
        while (A[j] > pivot) j--;

        if (i >= j) break;

        int temp = A[i];
        A[i] = A[j];
        A[j] = temp;
    }

    local_quick_sort(A, i);
    local_quick_sort(A + i, len - i);
}

void local_quick_sort_rev(int *a, int n) {
    if (n < 2) return;

    int pivot = a[n / 2];
    int i, j;
    for (i = 0, j = n - 1; ; i++, j--) {
        while (a[i] > pivot) i++;
        while (a[j] < pivot) j--;

        if (i >= j) break;
        int temp = a[i];
        a[i] = a[j];
        a[j] = temp;
    }
    local_quick_sort_rev(a, i);
    local_quick_sort_rev(a + i, n - i);
}

void init(void)
{
    int64_t seed = block_counter * 1000;
    
    // Initialize arrays with pseudo-random values
    for (int i = 0; i < ARRAY_SIZE; i++) {
        // Pseudo-random numbers different for each block
        seed = (seed * 1103515245 + 12345) & 0x7FFFFFFF;
        int val = (int)(seed & 0x7FFFFFFF);
        array[i] = val;
        array_DWC[i] = val;
        array_TMR[i] = val;
        golden_array[i] = val;
        golden_array_rev[i] = val;
    }
    
    last_sort_forward = 1;
}

void test(void)
{
    // Call the assembly quicksort function
    quicksort();
}

void fill(void)
{
    // Compute golden arrays (forward and reverse sorted)
    local_quick_sort(golden_array, ARRAY_SIZE);
    local_quick_sort_rev(golden_array_rev, ARRAY_SIZE);
}

void check(void)
{
    // Check against forward sorted golden (assuming forward sort was done)
    // For a more complete test, you would alternate between forward/reverse
    for (int i = 0; i < ARRAY_SIZE; i++) {
        if (array[i] != golden_array[i]) {
            errors++;
        }
    }
}

void send_status(unsigned int cycles)
{
    // BLOCK RUN ERRORS CYCLES TMR_MITIGATIONS
    printf("%u\t%u\t%u\t%u\t%u\n", block_counter, run_counter, cycles, errors, TMR_ERROR_CNT);
}

