#ifndef SHA256_H
#define SHA256_H

#define HASH_DATA_LEN 10
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

#endif /* SHA256_H */

