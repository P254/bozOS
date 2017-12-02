#ifndef MULTI_TERM_H
#define MULTI_TERM_H

#include "types.h"
#include "syscalls.h"

/*Magic Numbers*/
#define TERM_1  1
#define TERM_2  2
#define TERM_3  3

enum pu_t {NOT_USED, IN_USE};

typedef struct term {
    pcb_t* pcb_head;    // Pointer to head of PCB linked list
    uint8_t* kb_buf;    // Keyboard buffer
    char* video;        // Pointer to video memory
    int x;              // X-position for printing
    int y;              // Y-position for printing
    uint32_t* vidmap;   // For use with vidmap
} term_t;

/*Forward declarations*/
pcb_t* get_PCB_tail(uint8_t terminal_n);
term_t* get_terminal(uint8_t terminal_n);
void switch_terminal(uint8_t new_term_n);
void multi_term_init();
void reset_pcb_head(uint8_t terminal_n);
void unset_process_usage(uint8_t pid);

int8_t add_PCB();
pcb_t* get_PCB_base(int8_t process_num);

#endif /*MULTI_TERM_H*/
