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
volatile uint8_t active_task;   
static uint8_t init_flag;

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
    init_flag = 0;
    clear_screen();
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
    uint32_t incoming_esp, incoming_ebp;
    pcb_t* incoming_pcb;
    pcb_t* outgoing_pcb;
    
    // Initialize a shell for each terminal if we haven't done so already
    if (init_flag == 0) { 
        init_shells();
        send_eoi(PIT_IRQ_NUM);
    }
    else {
        // For use with mono-tasking
        outgoing_task = get_active_task();
        incoming_task = get_active_terminal(); 
        if (incoming_task == outgoing_task) {
            send_eoi(PIT_IRQ_NUM);
            return;
        }

        // For use with multi-tasking
        // outgoing_task = get_active_task();
        // incoming_task = (outgoing_task + 1) % MAX_TERM_N; 

        /******* TASK SWITCH CODE BEGINS HERE ******/
        // Update bookkeeping information first
        set_active_task(incoming_task);

        // We want to block all other interrupts so that our task switch process is atomic
        incoming_pcb = get_PCB_tail(incoming_task);
        outgoing_pcb = get_PCB_tail(outgoing_task);

        if (incoming_pcb == NULL || outgoing_pcb == NULL) {
            send_eoi(PIT_IRQ_NUM);
            return;
        }

        // Save outgoing esp/ebp
        asm volatile(
            "movl %%esp, %0;"
            "movl %%ebp, %1;"
            : "=r" (outgoing_pcb->esp_switch), "=r" (outgoing_pcb->ebp_switch)
        );

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

        if (incoming_pcb->esp_switch == NULL || incoming_pcb->ebp_switch == NULL) {
            incoming_esp = incoming_pcb->self_esp;
            incoming_ebp = incoming_pcb->self_ebp;
        } else {
            incoming_esp = incoming_pcb->esp_switch;
            incoming_ebp = incoming_pcb->ebp_switch;
        }

        // Load incoming esp/ebp
        asm volatile(
            "movl %0, %%esp;"
            "movl %1, %%ebp;"
            : /* no outputs */
            : "r" (incoming_esp), "r" (incoming_ebp)
            : "esp", "ebp"
        );
        send_eoi(PIT_IRQ_NUM);
    }
}

/*
 * init_shells
 *   DESCRIPTION: Initializes a shell for each terminal. 
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
void init_shells() {
    int init, count = 0;
    term_t* init_term;
    for (init = 0; init < MAX_TERM_N; init++) {
        init_term = get_terminal_ptr(init);
        if (init_term->pcb_head == NULL) {
            set_active_task(init);
            send_eoi(PIT_IRQ_NUM);
            execute((uint8_t*) "shell");
            return;
        } else {
            count++;
        }
    }
    // Reset active task # to 0 if we have finished initializing all the terminals 
    if (count == MAX_TERM_N) {
        set_active_task(0);
        init_flag = 1;
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
