#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define		data_size				1024
#define		robust_printing				1
#define		change_rate				100

unsigned long int ind = 0;
int local_errors = 0;
int in_block = 0;
int seed_value = -1;
uint8_t data[data_size];
uint32_t crc_result_tmr_1;
uint32_t crc_result_tmr_2;
uint32_t crc_result_tmr_3;
uint32_t crc_result;
uint32_t golden_crc;
unsigned int errors_12;
unsigned int errors_13;
unsigned int errors_23;
unsigned int unmitigated;

// CRC32 lookup table
static uint32_t crc32_table[256];

void init_crc32_table(void) {
    uint32_t polynomial = 0xEDB88320;
    for (uint32_t i = 0; i < 256; i++) {
        uint32_t crc = i;
        for (int j = 0; j < 8; j++) {
            if (crc & 1) {
                crc = (crc >> 1) ^ polynomial;
            } else {
                crc >>= 1;
            }
        }
        crc32_table[i] = crc;
    }
}

void init_data() {
    int i = 0;

    //seed the random number generator
    //the input data is reset on error to the same values, so
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

    //fill the data buffer
    for (i = 0; i < data_size; i++) {
        data[i] = (uint8_t)(rand() & 0xFF);
    }
}

//*****************************************************************************
//
// CRC32 computation function
//
//*****************************************************************************

uint32_t compute_crc32_tmr_1(uint8_t *data, size_t len) {
    uint32_t crc = 0xFFFFFFFF;
    
    for (size_t i = 0; i < len; i++) {
        uint8_t byte = data[i];
        uint32_t index = (crc ^ byte) & 0xFF;
        crc = (crc >> 8) ^ crc32_table[index];
    }
    
    return crc ^ 0xFFFFFFFF;
}

uint32_t compute_crc32_tmr_2(uint8_t *data, size_t len) {
    uint32_t crc = 0xFFFFFFFF;
    
    for (size_t i = 0; i < len; i++) {
        uint8_t byte = data[i];
        uint32_t index = (crc ^ byte) & 0xFF;
        crc = (crc >> 8) ^ crc32_table[index];
    }
    
    return crc ^ 0xFFFFFFFF;
}

uint32_t compute_crc32_tmr_3(uint8_t *data, size_t len) {
    uint32_t crc = 0xFFFFFFFF;
    
    for (size_t i = 0; i < len; i++) {
        uint8_t byte = data[i];
        uint32_t index = (crc ^ byte) & 0xFF;
        crc = (crc >> 8) ^ crc32_table[index];
    }
    
    return crc ^ 0xFFFFFFFF;
}

void custom_Voter(void) {
    if (crc_result_tmr_1 == crc_result_tmr_2 
        && crc_result_tmr_1 == crc_result_tmr_3) { // all agree
        crc_result = crc_result_tmr_1;
    } else if (crc_result_tmr_2 == crc_result_tmr_3) { // error in 1
        crc_result = crc_result_tmr_2;
    } else { // error in 2 or 3
        crc_result = crc_result_tmr_1;
    }
}

uint32_t compute_crc32(uint8_t *data, size_t len) {
    // Compute CRC32 on all three TMR arrays
    crc_result_tmr_1 = compute_crc32_tmr_1(data, len);
    crc_result_tmr_2 = compute_crc32_tmr_2(data, len);
    crc_result_tmr_3 = compute_crc32_tmr_3(data, len);
    
    custom_Voter();
    
    return crc_result;
}

void check_tmr(void) {
    if (crc_result_tmr_1 != crc_result_tmr_2) {
        errors_12++;
    }
    if (crc_result_tmr_1 != crc_result_tmr_3) {
        errors_13++;
    }
    if (crc_result_tmr_2 != crc_result_tmr_3) {
        errors_23++;
    }
    if(errors_12 != 0 && errors_13 != 0 && errors_23 != 0) {
        unmitigated++;
    }
}

int checker(uint32_t golden_crc, uint32_t dut_crc, int sub_test) {
    int first_error = 0;
    int num_of_errors = 0;

    if (golden_crc != dut_crc) {
        //an error is found, print the results
        if (!in_block && robust_printing) {
            printf(" - i: %lu, %i\r\n", ind, sub_test);
            printf("   E: {CRC: [%08x, %08x]}\r\n", golden_crc, dut_crc);
            first_error = 1;
            in_block = 1;
        }
        else if (in_block && robust_printing) {
            printf("   E: {CRC: [%08x, %08x]}\r\n", golden_crc, dut_crc);
            first_error = 1;
        }
        num_of_errors = 1;
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

void crc32_test() {
    //initialize variables
    int total_errors = 0;
    int i = 0;

    //init CRC32 table
    init_crc32_table();

    //init data
    init_data();

    //compute the golden CRC
    golden_crc = compute_crc32(data, data_size);

    while (1) {
        for (i = 0; i < 4; i++) {
            //compute CRC32
            crc_result = compute_crc32(data, data_size);
            check_tmr();
            local_errors = checker(golden_crc, crc_result, i);

            //if there is an error, fix the input data
            //and recompute the golden CRC.
            if (local_errors > 0) {
                init_data();
                golden_crc = compute_crc32(data, data_size);
            }

            total_errors += local_errors;
            local_errors = 0;
            in_block = 0;
        }

        //ack and change input data
        if (ind % change_rate == 0) {
            printf("# %lu, %i\r\n", ind, total_errors);
            seed_value = -1;

            //init data with new values
            init_data();

            //compute the new golden CRC
            golden_crc = compute_crc32(data, data_size);
        }

        //reset vars and such
        ind++;
    }
}

int main() {
    //print the YAML header
    printf("\r\n---\r\n");
    printf("hw: msp430f2619\r\n");
    printf("test: CRC32\r\n");
    printf("mit: TMR\r\n");
    printf("printing: %i\r\n", robust_printing);
    printf("input change rate: %i\r\n", change_rate);
    printf("Data size: %i\r\n", data_size);
    printf("ver: 0.1\r\n");
    printf("fac: LANSCE Nov 2015\r\n");
    printf("d:\r\n");

    //start the test
    crc32_test();

    return 0;
}

