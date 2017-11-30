#ifndef MULTI_TERM_H
#define MULTI_TERM_H

#include "types.h"
#include "syscalls.h"

/*Magic Numbers*/
#define TERM_1  1
#define TERM_2  2
#define TERM_3  3

typedef struct term {
    uint8_t tid;        // Terminal ID (1,2,3)
    pcb_t* pcb_head;    // Pointer to head of PCB linked list
    uint8_t* kb_buf;    // Keyboard buffer
    char* video;        // Pointer to video memory
    int x;              // X-position for printing
    int y;              // Y-position for printing
    uint32_t* vidmap;   // For use with vidmap
} term_t;

/*Forward declarations*/
pcb_t* get_PCB_tail(uint8_t terminal_n);
term_t* get_active_terminal();
void switch_terminal(uint8_t new_term_n);
void multi_term_init();

#endif /*MULTI_TERM_H*/
