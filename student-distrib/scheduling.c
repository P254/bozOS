// This file will include code necessary for scheduling only
// Code for multi-terminal support will be in terminal.c 

#include "i8259.h"
#include "scheduling.h"
#include "IDT.h"
#include "terminal.h"
#include "lib.h"

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
    outb(0x36, CMD_REG);        // 0x36: channel 0 output, lobyte/hibyte access, operating mode 3, binary
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
    uint8_t curr_term = 1; // TODO: Change this
    uint8_t new_term = (curr_term % MAX_TERM_N) + MIN_TERM_N;

    // TODO: Complete this function
    // printf("If this prints continuously, then we have the PIT set up right. Yay!\n");


    // We want to block all other interrupts so that our task switch process is atomic
    send_eoi(PIT_IRQ_NUM);  
}
