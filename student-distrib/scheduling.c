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
static volatile uint8_t active_task;
static volatile uint8_t term_flags[3]= {1,0,0};

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
    // printf("outgoing: %d, incoming: %d\n", outgoing_task, incoming_task);
    // if (incoming_task == outgoing_task) {
    //     send_eoi(PIT_IRQ_NUM);
    //     return;
    // }

    // For use with multi-tasking

    // NOTE: active task represents the last task that was being executed in the quantum
    outgoing_task = get_active_terminal();
    // outgoing task shoudl be the one that was being executed

    while(incoming_task != outgoing_task){
      // if that terminal exists then set incoming task to that
      if(term_flags[(outgoing_task + 1) % MAX_TERM_N]==1){
        incoming_task = (outgoing_task + 1) % MAX_TERM_N;
        break;
      }
      incoming_task = (outgoing_task + 1) % MAX_TERM_N;
    }
    //set to the next terminal iff has started with a flag
    printf("outgoing: %d, incoming: %d\n", outgoing_task, incoming_task);

    // if no other terminal exists task_switch is not required
    if(incoming_task == outgoing_task){
      send_eoi(PIT_IRQ_NUM);
      return;
    }

    set_active_task(incoming_task);

    /******* TASK SWITCH CODE BEGINS HERE ******/
    // We want to block all other interrupts so that our task switch process is atomic
    incoming_pcb = get_PCB_tail(incoming_task);
    outgoing_pcb = get_PCB_tail(outgoing_task);

    if (incoming_pcb == NULL || outgoing_pcb == NULL) {
        send_eoi(PIT_IRQ_NUM);
        return;
    }
    // Update bookkeeping information first

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

/*
 * set_terminal_flag
 *   DESCRIPTION: Sets the active task.
 *   INPUTS: uint8_t -- terminal_num (0,1,2) terminal we have Initialized
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
void set_terminal_flag(uint8_t terminal_num){
  if(terminal_num<2)
    term_flags[terminal_num]=1;
  else
    return;
}
