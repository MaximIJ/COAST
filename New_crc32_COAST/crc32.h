#ifndef CRC32_H
#define CRC32_H

#define DATA_SIZE 1024
#define MAX_RUNS 50

unsigned int cycle_counter;
unsigned int block_counter;
unsigned int run_counter;
unsigned int errors;
extern int64_t TMR_ERROR_CNT;

void init(void);
void test(void);
void fill(void);
void check(void);
void start_timer(void);
unsigned int stop_timer(void);
void send_status(unsigned int cycles);

#endif /* CRC32_H */