#include "kernelsdk.h"
#include "gpio.h"
#include "mmu.h"
#include "uart.h"
#include "interrupt.h"
#include "clock-init.h"
#include "quicksort.h"
#include "utils.h"

void print_str(const char *str) {
    while (*str != '\0') {
        uart_putc(uart0_ctl, *str++); 
    }
}

void setup_cache_and_mmu(void) {
    uint64_t mcor_csr = read_csr(MCOR); 
    uint64_t mhcr_csr = read_csr(MHCR); 
    uint64_t mhint_csr = read_csr(MHINT); 
    
    print_str("Configuring cache and MMU...\n");
    
    // Enable all caches
    cache_enable_status_set(1, 1, 1, 1, 1, 1, 1); 
    cache_prefetch_status_set(1, 1, DCACHE_PREFETCH2);
    
    clint_enable_supervisor_interrupt();
    plic_enable_supervisor_access();
    enable_supervisor_counter_access();
    
    interrupt_delegate_to_supervisor_mode(0xffff);
    exception_delegate_to_supervisor_mode(0xffff);
    
    uintptr_t mstatus = read_csr("mstatus"); 
    mstatus &= (~((uintptr_t)MSTATUS_MPP)); 
    mstatus |= (uintptr_t)(1 << 11); 
    write_csr("mstatus", mstatus);
    
    // Configure memory permissions
    write_csr("satp", 0);
    asm volatile("csrw pmpaddr0, %0" : : "r" (0xffffffffULL));
    asm volatile("csrw pmpcfg0, %0" : : "r" (0x0FULL));
    
    print_str("Cache and MMU configured!\n");
}

void enter_supervisor_mode_and_setup_mmu(void) {
    uintptr_t stvec = set_supervisor_interrupt_vector((uintptr_t)&interrupt_vector, INTERRUPT_HANDLER_VECTOR);
    enable_all_supervisor_interrupts();
    
    vmem_kernel_init();
    vmem_kernel_set_satp();
    
    // Clear caches to ensure coherence with new MMU settings
    dcache_clear_invalidate_all();
    icache_invalidate_all_broadcast();
    
    print_str("Quicksort Benchmark Initialized!\n");
    // BLOCK RUNS ERRORS CYCLES TMR_MITIGATIONS

#if CACHE_PROFILING
    print_str("Initializing cache profiling in S-mode...\n");
    init_cache_profiling();
#endif

    init();  // Initialize array data
    fill();  // Compute golden sorted arrays

    block_counter = 0;
    run_counter = 0;
    TMR_ERROR_CNT = 0;

    int unmitigated;

    while (1) { // BLOCKS (variable depending on program lifetime)
        errors = 0;
        unmitigated = 0;
        TMR_ERROR_CNT = 0;

        // <----- TIMER START
        start_timer(); 
        test();  // Run quicksort
        check(); // Verify result
        // <------ TIMER STOP
        unsigned int cycles = stop_timer(); 
        
        if (run_counter >= MAX_RUNS || errors != 0 || unmitigated != 0) { // RUNS
                        
            send_status(cycles); // log
            
            if (errors != 0 || unmitigated != 0) {
                init();
                fill(); // Data changes between blocks when there's an error
            }
            
#if CACHE_PROFILING
            // Collect and print cache statistics
            cache_stats_t stats = collect_cache_stats();
            print_cache_stats(&stats);

            // Reset counters for next block
            reset_cache_counters();

            // Print block completion
            break;
#endif
#if TIME_PROFILING
            break;
#endif
            run_counter = 0;
            block_counter++;
        }

        run_counter++;
        
    }
}

int main(void) {
    sys_clock_init();
    uart0_ctl = uart_init(0, 1);
    
    // Setup cache and enter supervisor mode
    setup_cache_and_mmu();
    
    // Switch to supervisor mode
    write_csr("mepc", (uintptr_t)&enter_supervisor_mode_and_setup_mmu);
    print_str("Switching to supervisor mode...\n");
    asm volatile ("mret":::"memory");
        
    return 0;
}

