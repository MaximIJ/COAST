#include "uart.h"
#include "crc32.h"
#include "kernelsdk.h"

// Data buffers - main and TMR copies
extern uint8_t data[DATA_SIZE];
extern uint8_t data_DWC[DATA_SIZE];
extern uint8_t data_TMR[DATA_SIZE];

// CRC32 lookup table - main and TMR copies
extern uint32_t crc32_table[256];
extern uint32_t crc32_table_DWC[256];
extern uint32_t crc32_table_TMR[256];

// Result variables
extern uint32_t crc_result;
extern uint32_t crc_result_DWC;
extern uint32_t crc_result_TMR;

// Golden CRC (computed without TMR)
uint32_t golden_crc;

unsigned int tmr_mitigations = 0;
void crc32();

// Initialize CRC32 table (local copy for golden computation)
static uint32_t local_crc32_table[256];

void init_local_crc32_table(void) {
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
        local_crc32_table[i] = crc;
    }
}

// Compute golden CRC32 (without TMR)
uint32_t compute_golden_crc32(uint8_t *input_data, int len) {
    uint32_t crc = 0xFFFFFFFF;
    
    for (int i = 0; i < len; i++) {
        uint8_t byte = input_data[i];
        uint32_t index = (crc ^ byte) & 0xFF;
        crc = (crc >> 8) ^ local_crc32_table[index];
    }
    
    return crc ^ 0xFFFFFFFF;
}

void init(void)
{
    int64_t seed = block_counter * 1000;
    
    // Initialize CRC32 lookup table
    init_local_crc32_table();
    
    // Initialize data with pseudo-random values
    for (int i = 0; i < DATA_SIZE; i++) {
        // Pseudo-random numbers different for each block
        seed = (seed * 1103515245 + 12345) & 0x7FFFFFFFFFFFFFFF;
        // Range: 0-255
        uint8_t val = (uint8_t)(seed & 0xFF);
        data[i] = val;
        data_DWC[i] = val;
        data_TMR[i] = val;
    }
    
    // Initialize CRC32 table copies
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
        crc32_table_DWC[i] = crc;
        crc32_table_TMR[i] = crc;
    }
    
    // Initialize result variables
    crc_result = 0;
    crc_result_DWC = 0;
    crc_result_TMR = 0;
}

void test(void)
{
    // Call the assembly CRC32 computation function
    crc32();
}

void fill(void)
{
    // Compute the golden CRC32 value
    golden_crc = compute_golden_crc32(data, DATA_SIZE);
}

void check(void)
{    
    // Check if computed CRC matches golden
    if (crc_result != golden_crc) {
        errors++;
    }
}

void send_status(unsigned int cycles)
{
    // BLOCK RUN ERRORS CYCLES TMR_MITIGATIONS
    printf("%u\t%u\t%u\t%u\t%u\n", block_counter, run_counter, cycles, errors, TMR_ERROR_CNT);
}

