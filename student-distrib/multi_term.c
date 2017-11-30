#include "multi_term.h"
#include "syscalls.h"
#include "lib.h"

/* Global Variables */
static uint8_t active_terminal;
static term_t* terminal_table[TERM_3];

/*
 * multi_term_init
 *   DESCRIPTION: Initializes multiple terminals. To be called from kernel.c
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Starts 3 terminals (each with shell), with #1 as active 
 */
void multi_term_init() {
    // TODO: Complete this function
    switch_terminal(TERM_1);
}

/*
 * get_PCB_tail
 *   DESCRIPTION: Returns the pointer to the tail of PCB linked list for a given terminal
 *   INPUTS: terminal_n -- the terminal # (1,2,3) we are interested in
 *   OUTPUTS: none
 *   RETURN VALUE: pcb_t
 *   SIDE EFFECTS: none
 */
pcb_t* get_PCB_tail(uint8_t terminal_n) {
    if (terminal_n < TERM_1 || terminal_n > TERM_3) return NULL;
    // TODO: Complete this function
    pcb_t* PCB_base; 

    return PCB_base;
}

/*
 * get_active_terminal
 *   DESCRIPTION: Returns the pointer to the active terminal struct. 
 *                Used by functions outside the scope of this file's namespace. 
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: term_t -- pointer to active terminal struct
 *   SIDE EFFECTS: none
 */
term_t* get_active_terminal() {
    // term_t* terminal_table[3] = {*term_1_struct, *term_2_struct, *term_3_struct};
    return terminal_table[active_terminal];
}

/*
 * switch_terminal
 *   DESCRIPTION: Performs switching of acive terminal. Called by keyboard.c
 *   INPUTS: new_term_n -- new terminal number to switch to
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Clears video memory to load new terminal
 */
void switch_terminal(uint8_t new_term_n) {
    // Check for bad inputs
    if (new_term_n < TERM_1 || new_term_n > TERM_3) return;
    // Switch terminals only if we're not already in the same terminal
    if (new_term_n == active_terminal) return;
    
    // TODO: Complete this function
    printf("Switching to terminal %u\n", new_term_n);
    active_terminal = new_term_n;
}
