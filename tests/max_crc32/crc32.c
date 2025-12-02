#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "../COAST.h"
#define DATA_SIZE 1024
#define TMR_ERROR_CNT

#if INJECT_FAULTS
#include <time.h>
#include <unistd.h>
#include <signal.h>
#endif

// Data buffer
uint8_t data[DATA_SIZE];

// CRC32 lookup table
uint32_t crc32_table[256];

// Result variable
uint32_t crc_result;

void testing(void)
{
    uint32_t crc = 0xFFFFFFFF;
    
    for (int i = 0; i < DATA_SIZE; i++) {
        uint8_t byte = data[i];
        uint32_t index = (crc ^ byte) & 0xFF;
        crc = (crc >> 8) ^ crc32_table[index];
    }
    
    crc_result = crc ^ 0xFFFFFFFF;
}

void crc32() {
    testing();
}