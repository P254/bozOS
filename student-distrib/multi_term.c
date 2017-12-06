#include "multi_term.h"
#include "pcb.h"
#include "syscalls.h"
#include "lib.h"
#include "paging.h"
#include "terminal.h"
#include "keyboard.h"
#include "scheduling.h"
#include "i8259.h"

/*
 * multi_term_init
 *   DESCRIPTION: Initializes multiple terminals. To be called from kernel.c
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Starts 3 terminals (each with shell), with #1 as active
 */
void multi_term_init() {
    uint8_t i;
    for (i = 0; i < MAX_PROCESSES; i++) {
        process_usage[i] = NOT_USED;
    }
    set_active_terminal(0);

    terminal_table[TERM_1].pcb_head = NULL;
    terminal_table[TERM_2].pcb_head = NULL;
    terminal_table[TERM_3].pcb_head = NULL;

    terminal_table[TERM_1].video = (char*) TERM_1_VIDEO;
    terminal_table[TERM_2].video = (char*) TERM_2_VIDEO;
    terminal_table[TERM_3].video = (char*) TERM_3_VIDEO;

    terminal_table[TERM_1].x = 0;
    terminal_table[TERM_1].y = 0;
    terminal_table[TERM_2].x = 0;
    terminal_table[TERM_2].y = 0;
    terminal_table[TERM_3].x = 0;
    terminal_table[TERM_3].y = 0;

    terminal_table[TERM_1].color = COLOR_1;
    terminal_table[TERM_2].color = COLOR_2;
    terminal_table[TERM_3].color = COLOR_3;
}

/*
 * switch_terminal
 *   DESCRIPTION: Performs switching of acive terminal. Called by keyboard.c
 *   INPUTS: new_terminal -- new terminal number to switch to
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Clears video memory to load new terminal
 */
void switch_terminal(uint8_t new_terminal) {
    // Check for bad inputs
    if (new_terminal > TERM_3) return;
    // Switch terminals only if we're not already in the same terminal
    if (new_terminal == active_terminal) return;

    incoming_pcb = get_PCB_tail(new_terminal);
    outgoing_pcb = get_PCB_tail(active_terminal);

    copy_terminal(new_terminal);
    set_active_terminal(new_terminal);

    // Check if our terminal is empty - if yes, launch a new shell
    if (incoming_pcb == NULL) {
        // Save outgoing esp/ebp first
        asm volatile(
            "movl %%esp, %0;"
            "movl %%ebp, %1;"
            : "=r" (outgoing_pcb->esp_switch), "=r" (outgoing_pcb->ebp_switch)
        );
        // Execute new user program
        execute((uint8_t*) "shell");

        // Execute might fail (return -1): we will need to return to old terminal
        // TODO: Add this later
    }
}

/*
 * copy_terminal
 *   DESCRIPTION: Helper function for copy_terminal
 *   INPUTS: new_terminal -- new terminal number to switch to
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Clears video memory to load new terminal
 */
void copy_terminal(uint8_t new_terminal) {
    // Copy active terminal data to the terminal table
    char* active_video = (char*) VIDEO;
    uint8_t* active_kb_buf = get_kb_buffer();

    memcpy(terminal_table[active_terminal].video, active_video, VIDEO_SIZE);
    memcpy(terminal_table[active_terminal].kb_buf, active_kb_buf, KB_SIZE);

    // Clear keyboard buffer
    memset(active_kb_buf, '\0', KB_SIZE);

    // Copy new terminal data from the terminal table
    memcpy(active_video, terminal_table[new_terminal].video, VIDEO_SIZE);
    memcpy(active_kb_buf, terminal_table[new_terminal].kb_buf, KB_SIZE);

    set_active_terminal(new_terminal);
}

/*
 * get_terminal_ptr
 *   DESCRIPTION: Returns the (read-only) pointer to the active terminal struct.
 *                Used by functions outside the scope of this file's namespace.
 *   INPUTS: terminal_n -- terminal # that we are interested in
 *   OUTPUTS: none
 *   RETURN VALUE: term_t -- pointer to active terminal struct
 *   SIDE EFFECTS: none
 */
term_t* get_terminal_ptr(uint8_t terminal_n) {
    if (terminal_n > TERM_3) return NULL;
    // NOTE: This pointer is read-only
    return &terminal_table[terminal_n];
}

/*
 * unset_process_usage
 *   DESCRIPTION: Unsets the process usage of a given pid. Used by system call HALT.
 *   INPUTS: pid -- the process # we want to mark as NOT_USED
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
void unset_process_usage(uint8_t pid) {
    if (pid > MAX_PROCESSES) return;
    process_usage[pid] = NOT_USED;
}

/*
 * get_active_terminal
 *   DESCRIPTION: Returns the # of the active terminal, for use as a mono-tasking synchronisation mechanism with scheduling.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: uint8_t -- # of the active terminal
 *   SIDE EFFECTS: none
 */
uint8_t get_active_terminal() {
    return active_terminal;
}

/*
 * set_active_terminal
 *   DESCRIPTION: Sets the active terminal.
 *   INPUTS: uint8_t -- the new terminal # that we want to set
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
void set_active_terminal(uint8_t new_terminal) {
    active_terminal = new_terminal;
}
