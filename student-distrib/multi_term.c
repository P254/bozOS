#include "multi_term.h"
#include "syscalls.h"
#include "lib.h"
#include "paging.h"
#include "terminal.h"
#include "keyboard.h"
#include "scheduling.h"
#include "i8259.h"

/* Global Variables */
volatile uint8_t active_terminal;
static term_t terminal_table[TERM_SIZE];
static enum pu_t process_usage[MAX_PROCESSES];

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

    terminal_table[TERM_1].color = ATTRIB_1;
    terminal_table[TERM_2].color = ATTRIB_2;
    terminal_table[TERM_3].color = ATTRIB_3;
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
    cli();
    // Check for bad inputs
    if (new_terminal > TERM_3) return;
    // Switch terminals only if we're not already in the same terminal
    if (new_terminal == active_terminal) return;
    
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
    sti();    
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
 * reset_pcb_head
 *   DESCRIPTION: Resets 'pcb_head' of a given terminal ID to NULL.
 *                Used by system call HALT. 
 *   INPUTS: terminal_n -- terminal # that we are interested in
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
void reset_pcb_head(uint8_t terminal_n) {
    if (terminal_n > TERM_3) return;
    else terminal_table[terminal_n].pcb_head = NULL;
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

// TODO: Move all PCB-related things to a PCB.c file
/*
 * get_PCB_tail
 *   DESCRIPTION: Returns the pointer to the tail of PCB linked list for a given terminal
 *   INPUTS: terminal_n -- the terminal # (0,1,2) we are interested in
 *   OUTPUTS: none
 *   RETURN VALUE: pcb_t
 *   SIDE EFFECTS: none
 */
pcb_t* get_PCB_tail(uint8_t terminal_n) {
    if (terminal_n > TERM_3) return NULL;
    
    pcb_t* PCB_base = terminal_table[terminal_n].pcb_head; 
    if (PCB_base == NULL) return NULL;
    
    while (PCB_base->child_pcb != NULL) {
        PCB_base = PCB_base->child_pcb;
    }
    return PCB_base;
}

/*
 * add_PCB
 *   DESCRIPTION: Adds a process to the active terminal. Used by system call EXECUTE.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: process number of the newly-added PCB, else -1 on failure
 *   SIDE EFFECTS: Modifies the process_usage table, terminal_table and
 *                 adds a child_pcb to the PCB linked list of the active terminal
 */
int8_t add_PCB() {
    int i, task_n, process_num = -1;
    for (i = 0; i < MAX_PROCESSES; i++) {
        if (process_usage[i] == NOT_USED) {
            process_num = i;
            break;
        };
    }
    if (process_num == -1) return -1;

    process_usage[process_num] = IN_USE;
    task_n = get_active_task();
    pcb_t* pcb_ptr = terminal_table[task_n].pcb_head;
    
    // Adding the first process for a given terminal
    if (pcb_ptr == NULL) {
        terminal_table[task_n].pcb_head = get_PCB_base(process_num);
    }
    
    else {
        while (pcb_ptr->child_pcb != NULL) {
            pcb_ptr = pcb_ptr->child_pcb;
        }
        // Add the new PCB address to the child
        pcb_ptr->child_pcb = get_PCB_base(process_num);
    }
        
    // Return the process_num that corresponds to the newly-added child
    return process_num;
}

/*
 * get_PCB_base
 *   DESCRIPTION: Returns the PCB base pointer as specified by process_num
 *   INPUTS: process_num -- process number we are interested in (0-5)
 *   OUTPUTS: none
 *   RETURN VALUE: pcb_t -- pointer to the topmost PCB on the kernel stack
 *   SIDE EFFECTS: none
 */
pcb_t* get_PCB_base(int8_t process_num) {
    if (process_num >= 0 && process_num < MAX_PROCESSES) {
        uint32_t pcb_addr = KERNEL_BASE - (process_num + 1) * PCB_OFFSET; // find where program stack starts
        pcb_t* PCB_base = (pcb_t*) pcb_addr; // cast it to PCB so start of program stack contains PCB.
        return PCB_base;
    } 
    else return NULL;
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
