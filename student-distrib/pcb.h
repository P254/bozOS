#ifndef PCB_H
#define PCB_H

#include "types.h"
#include "syscalls.h"
#include "multi_term.h"


/*Forward declarations*/
pcb_t* get_PCB_tail(uint8_t terminal_n);
void reset_pcb_head(uint8_t terminal_n);
int8_t add_PCB();
pcb_t* get_PCB_base(int8_t process_num);

#endif /*MULTI_TERM_H*/
