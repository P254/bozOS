// This file will include code necessary for scheduling only
// Code for multi-terminal support will be in terminal.c

#include "i8259.h"
#include "scheduling.h"
#include "IDT.h"
#include "syscalls.h"
#include "lib.h"
#include "x86_desc.h"
#include "multi_term.h"
#include "pcb.h"

// Global variable that holds the terminal # where the task is active (1,2,3)
volatile uint8_t active_task;

/*
 * pit_init
 *   DESCRIPTION: Initializes the Programmable Interrupt Timer (PIT) to tick once every 10ms
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
void pit_init() {
    set_active_task(0);
    // Source: http://www.osdever.net/bkerndev/Docs/pit.htm
    outb(PIT_INIT_CMD, CMD_REG);
    outb(SET_FREQ_L, CH0_PORT); // send low byte
    outb(SET_FREQ_H, CH0_PORT); // send high byte

    enable_irq(PIT_IRQ_NUM);    // enable IRQ line for PIT
    set_IDT_wrapper(SOFT_INT_START + PIT_IRQ_NUM, pit_handler_asm);
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
    uint8_t outgoing_task, incoming_task;
    pcb_t* incoming_pcb;
    pcb_t* outgoing_pcb;

    // For use with mono-tasking
    // outgoing_task = get_active_task();
    // incoming_task = get_active_terminal();
    // if (incoming_task == outgoing_task) {
    //     send_eoi(PIT_IRQ_NUM);
    //     return;
    // }

    // For use with multi-tasking
    outgoing_task = get_active_task();
    incoming_task = (outgoing_task + 1) % MAX_TERM_N;

    incoming_pcb = get_PCB_tail(incoming_task);
    outgoing_pcb = get_PCB_tail(outgoing_task);

    // Check that we have launched our first shell in terminal.c 
    if (outgoing_pcb == NULL) {
        send_eoi(PIT_IRQ_NUM);
        return;
    }

    // Update bookkeeping information first
    set_active_task(incoming_task);

    if (incoming_pcb == NULL) {
        // Launch a new shell 
        // Save outgoing esp/ebp first
        asm volatile(
            "movl %%esp, %0;"
            "movl %%ebp, %1;"
            : "=r" (outgoing_pcb->esp_switch), "=r" (outgoing_pcb->ebp_switch)
        );
        send_eoi(PIT_IRQ_NUM);

        // Execute new user program
        execute((uint8_t*) "shell");

        // Execute might fail (return -1): we will need to return to old terminal
        // TODO: Add this later
    }

    else {
        /******* TASK SWITCH CODE BEGINS HERE ******/
        // Set up incoming paging and tss.esp0
        tss.esp0 = incoming_pcb->self_k_stack;
        page_directory[(USER_MEM_V >> ALIGN_4MB)] = (incoming_pcb->self_page) | USER_PAGE_SET_BITS;
        asm volatile(
            "movl %%cr3, %%eax;"
            "movl %%eax, %%cr3;"
            : /* no outputs */
            : /* no inputs */
            : "eax"
        );

        // Save outgoing esp/ebp
        asm volatile(
            "movl %%esp, %0;"
            "movl %%ebp, %1;"
            : "=r" (outgoing_pcb->esp_switch), "=r" (outgoing_pcb->ebp_switch)
        );


        // Load incoming esp/ebp
        asm volatile(
            "movl %0, %%esp;"
            "movl %1, %%ebp;"
            : /* no outputs */
            : "r" (incoming_pcb->esp_switch), "r" (incoming_pcb->ebp_switch)
            : "esp", "ebp"
        );
        send_eoi(PIT_IRQ_NUM);
    }
}

/*
 * get_active_task
 *   DESCRIPTION: Returns the terminal # that the current active task resides in.
 *                To be called by external functions.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: uint8_t -- terminal # of the current active task
 *   SIDE EFFECTS: none
 */
uint8_t get_active_task() {
    return active_task;
}

/*
 * set_active_task
 *   DESCRIPTION: Sets the active task.
 *   INPUTS: uint8_t -- the active task # that we want to set
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
void set_active_task(uint8_t new_task) {
    if (new_task >= MAX_TERM_N) return;
    else active_task = new_task;
}
