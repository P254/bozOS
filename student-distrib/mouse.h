#ifndef MOUSE_H
#define MOUSE_H

#include "types.h"

#define MOUSE_N_BYTES   3
#define MOUSE_IRQ       12
#define MOUSE_IDT_ENTRY (SOFT_INT_START + MOUSE_IRQ)
#define TIMEOUT         100000

extern void mouse_handler_asm();
void mouse_wait(uint8_t type);
void mouse_handler();
void mouse_write(uint8_t cmd);
uint8_t mouse_read();
void mouse_init();

#endif /* MOUSE_H */
