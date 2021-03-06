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
 *   SIDE EFFECTS: none
 */
void multi_term_init() {
    int32_t i, j;
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
    
    terminal_table[TERM_1].vidmap_addr = (uint32_t) (USER_VIDEO_MEM + (1 << ALIGN_4KB)*TERM_1);
    terminal_table[TERM_2].vidmap_addr = (uint32_t) (USER_VIDEO_MEM + (1 << ALIGN_4KB)*TERM_2);
    terminal_table[TERM_3].vidmap_addr = (uint32_t) (USER_VIDEO_MEM + (1 << ALIGN_4KB)*TERM_3);

    // We want to initialize the correct colors
    for (j = TERM_1; j <= TERM_3; j++) {
        char* video_mem = terminal_table[j].video;
        uint8_t term_color = terminal_table[j].color;
        
        for (i = 0; i < NUM_ROWS * NUM_COLS; i++) {
            *(uint8_t *)(video_mem + (i << 1)) = ' ';
            *(uint8_t *)(video_mem + (i << 1) + 1) = term_color;
        }
    }
}

/*
 * switch_terminal
 *   DESCRIPTION: Performs switching of acive terminal. Called by keyboard.c
 *   INPUTS: new_terminal -- new terminal number to switch to
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Clears video memory and keyboard buffer to load new terminal
 */
void switch_terminal(uint8_t new_terminal) {
    // Check for bad inputs
    if (new_terminal > TERM_3) return;
    // Switch terminals only if we're not already in the same terminal
    if (new_terminal == active_terminal) return;
    
    // Copy active terminal data to the terminal table 
    char* active_video = (char*) VIDEO;
    uint8_t* active_kb_buf = get_kb_buffer();

    memcpy(terminal_table[active_terminal].video, active_video, VIDEO_SIZE);
    memcpy(terminal_table[active_terminal].kb_buf, active_kb_buf, KB_SIZE);

    // Clear keyboard buffer and terminal video buffer
    memset(active_kb_buf, '\0', KB_SIZE);
    memset(active_video, ' ', VIDEO_SIZE);

    // Copy new terminal data from the terminal table 
    memcpy(active_video, terminal_table[new_terminal].video, VIDEO_SIZE);
    memcpy(active_kb_buf, terminal_table[new_terminal].kb_buf, KB_SIZE);

    // Update cursor status
    int buf_len = strlen((int8_t*) active_kb_buf);
    update_cursor(terminal_table[new_terminal].x + buf_len % NUM_COLS, terminal_table[new_terminal].y + buf_len / NUM_COLS);

    // Update paging for vidmap
    vidmap_ptable[TERM_1] = (TERM_1_VIDEO) | 0x7; // 4 KiB page, user access, r/w access, present
    vidmap_ptable[TERM_2] = (TERM_2_VIDEO) | 0x7; // 4 KiB page, user access, r/w access, present
    vidmap_ptable[TERM_3] = (TERM_3_VIDEO) | 0x7; // 4 KiB page, user access, r/w access, present
    vidmap_ptable[new_terminal] = (VIDEO_MEM) | 0x7; // 4 KiB page, user access, r/w access, present

    // Flush the TLB
    asm volatile(
        "movl %%cr3, %%eax;"
        "movl %%eax, %%cr3;"
        : /*no outputs*/
        : /*no inputs*/
        : "eax"
    );

    // Update bookkeeping information 
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


/*
 * get_PCB_tail
 *   DESCRIPTION: Returns the pointer to the tail of PCB linked list for a given terminal
 *   INPUTS: terminal_n -- the terminal # (0,1,2) we are interested in
 *   OUTPUTS: none
 *   RETURN VALUE: pcb_t -- pointer to the tail of the PCB linked list
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
 *   DESCRIPTION: Adds a process to a given terminal. Used by system call EXECUTE.
 *   INPUTS: term_num -- the terminal # we want to add the process to
 *   OUTPUTS: none
 *   RETURN VALUE: process number of the newly-added PCB, else -1 on failure
 *   SIDE EFFECTS: Modifies the process_usage table, terminal_table and
 *                 adds a child_pcb to the PCB linked list of the active terminal
 */
int8_t add_PCB(uint8_t term_num) {
    int i, process_num = -1;
    for (i = 0; i < MAX_PROCESSES; i++) {
        if (process_usage[i] == NOT_USED) {
            process_num = i;
            break;
        };
    }
    if (process_num == -1) return -1;

    process_usage[process_num] = IN_USE;
    pcb_t* pcb_ptr = terminal_table[term_num].pcb_head;
    
    // Adding the first process for a given terminal
    if (pcb_ptr == NULL) {
        terminal_table[term_num].pcb_head = get_PCB_base(process_num);
    }
    
    else {
        pcb_ptr = terminal_table[term_num].pcb_head;
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
