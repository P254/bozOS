#ifndef IDT_H
#define IDT_H

/* Exception handlers are defined here */

#define EMPTY_EXCEPTION 15
#define N_EXCEPTIONS 20
#define N_EXCEPTIONS_RESERVED 32

void init_idt_exceptions();
void set_IDT_wrapper(uint8_t idt_num, void* handler_function);

#endif /* IDT_H */
