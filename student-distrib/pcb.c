#include "pcb.h"
#include "multi_term.h"
#include "syscalls.h"
#include "lib.h"
#include "paging.h"
#include "terminal.h"
#include "keyboard.h"
#include "scheduling.h"
#include "i8259.h"

// TODO: interface global variabels through get setter functions instead

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
    int i, process_num = -1;
    for (i = 0; i < MAX_PROCESSES; i++) {
        if (process_usage[i] == NOT_USED) {
            process_num = i;
            break;
        };
    }
    if (process_num == -1) return -1;

    process_usage[process_num] = IN_USE;
    pcb_t* pcb_ptr = terminal_table[active_terminal].pcb_head;

    // Adding the first process for a given terminal
    if (pcb_ptr == NULL) {
        terminal_table[active_terminal].pcb_head = get_PCB_base(process_num);
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
