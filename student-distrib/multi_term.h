#ifndef MULTI_TERM_H
#define MULTI_TERM_H

#include "types.h"
#include "syscalls.h"

/*Magic Numbers*/
#define TERM_1      0
#define TERM_2      1
#define TERM_3      2
#define TERM_SIZE   3


enum pu_t {NOT_USED, IN_USE};


typedef struct term {
    pcb_t* pcb_head;                // Pointer to head of PCB linked list
    uint8_t kb_buf[KB_BUF_SIZE];    // Keyboard buffer

    char* video;            // Pointer to video memory
    int x;                  // X-position for printing
    int y;                  // Y-position for printing
    int color;              // Makes it easier to see which terminal I'm in
    // uint32_t* vidmap_addr;  // For use with vidmap
} term_t;

/* Global Variables */
volatile uint8_t active_terminal;
static term_t terminal_table[TERM_SIZE];
static enum pu_t process_usage[MAX_PROCESSES];

/*Forward declarations*/
term_t* get_terminal_ptr(uint8_t terminal_n);
void switch_terminal(uint8_t new_terminal);
void multi_term_init();
void unset_process_usage(uint8_t pid);
uint8_t get_active_terminal();
void set_active_terminal(uint8_t new_terminal);


#endif /*MULTI_TERM_H*/
