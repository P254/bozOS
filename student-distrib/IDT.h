#ifndef IDT_H
#define IDT_H

/*Magic Numbers*/
#define EMPTY_EXCEPTION 15
#define N_EXCEPTIONS 20
#define N_EXCEPTIONS_RESERVED 32
#define USER_INT_START 32
#define SYS_CALL 0x80

/*Forward Declerations*/
void set_IDT_wrapper(uint8_t idt_num, void* handler_function);
void init_IDT();

#endif /* IDT_H */
