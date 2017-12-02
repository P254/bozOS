// This file will include code necessary for scheduling only
// Code for multi-terminal support will be in terminal.c 

#include "i8259.h"
#include "scheduling.h"
#include "IDT.h"
#include "syscalls.h"
#include "lib.h"
#include "x86_desc.h"
#include "multi_term.h"

// Global variable that holds the terminal # where the task is active (1,2,3)
static uint8_t active_task_n;   

/*
 * pit_init
 *   DESCRIPTION: Initializes the Programmable Interrupt Timer (PIT) to tick once every 10ms
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
void pit_init() {
    // Source: http://www.osdever.net/bkerndev/Docs/pit.htm
    outb(PIT_INIT_CMD, CMD_REG);        
    outb(SET_FREQ_L, CH0_PORT); // send low byte
    outb(SET_FREQ_H, CH0_PORT); // send high byte
    
    enable_irq(PIT_IRQ_NUM);    // enable IRQ line for PIT
    set_IDT_wrapper(SOFT_INT_START + PIT_IRQ_NUM, pit_handler_asm);
    active_task_n = 0;
}

/*
 * task_switch
 *   DESCRIPTION: Perform task switching. This is also the C interrupt handler for the PIT
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Modifies the ESP, EBP, TSS and paging structure
 */
void task_switch() {
    // TODO: Complete this function

    // For use with mono-tasking
    uint8_t new_task_n = get_active_terminal(); 
    if (new_task_n == active_task_n) {
        send_eoi(PIT_IRQ_NUM);
        return;
    }
    // For use with multi-tasking
    // uint8_t new_task_n = (active_task_n+1) % MAX_TERM_N; 

    // We want to block all other interrupts so that our task switch process is atomic
    /******* TASK SWITCH CODE BEGINS HERE ******/
    pcb_t* new_task_pcb = get_PCB_tail(new_task_n);
    if (new_task_pcb == NULL) {
        send_eoi(PIT_IRQ_NUM);
        return;
    }

    uint32_t new_task_physical_mem = new_task_pcb->self_page;
    page_directory[(USER_MEM_V >> ALIGN_4MB)] = new_task_physical_mem | USER_PAGE_SET_BITS;

    asm volatile(
        "movl %%cr3, %%eax;"
        "movl %%eax, %%cr3;"
        : /* no outputs */
        : /* no inputs */
        : "eax"
    );

    tss.esp0 = new_task_pcb->self_k_stack;
    asm volatile(
        "movl %0, %%esp;"
        "movl %1, %%ebp;"
        : /* no outputs */
        : "r" (new_task_pcb->self_esp), "r" (new_task_pcb->self_ebp)
        : "esp", "ebp"
    );
    active_task_n = new_task_n;
    send_eoi(PIT_IRQ_NUM);
}

/*
 * get_active_task
 *   DESCRIPTION: Returns the terminal that the current active task resides in.
 *                To be called by external functions. 
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: uint8_t -- the terminal of the current active task
 *   SIDE EFFECTS: none
 */
uint8_t get_active_task() {
    return active_task_n;
}
