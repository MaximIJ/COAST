#include "mmult.h"

void start_timer(void)
{
    unsigned int cc_start;
    asm volatile("rdcycle %0" : "=r"(cc_start));
    cycle_counter = cc_start;
}

unsigned int stop_timer(void)
{
    unsigned int cc_end;
    asm volatile("rdcycle %0" : "=r"(cc_end));
    return cc_end - cycle_counter;
}