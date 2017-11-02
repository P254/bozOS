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

/* Exception handler prototype in C*/
void handle_e0();
void handle_e1();
void handle_e2();
void handle_e3();
void handle_e4();
void handle_e5();
void handle_e6();
void handle_e7();
void handle_e8();
void handle_e9();
void handle_e10();
void handle_e11();
void handle_e12();
void handle_e13();
void handle_e14();
void handle_e15();
void handle_e16();
void handle_e17();
void handle_e18();
void handle_e19();
void handle_sys_call(); // TODO Sean: This will need to be placed into assembly linkage
void handle_default();  // TODO Sean: This will need to be placed into assembly linkage
void print_error_code();

/* Exception handler prototype in ASM */
extern void handle_e0_asm();
extern void handle_e1_asm();
extern void handle_e2_asm();
extern void handle_e3_asm();
extern void handle_e4_asm();
extern void handle_e5_asm();
extern void handle_e6_asm();
extern void handle_e7_asm();
extern void handle_e8_asm();
extern void handle_e9_asm();
extern void handle_e10_asm();
extern void handle_e11_asm();
extern void handle_e12_asm();
extern void handle_e13_asm();
extern void handle_e14_asm();
extern void handle_e15_asm();
extern void handle_e16_asm();
extern void handle_e17_asm();
extern void handle_e18_asm();
extern void handle_e19_asm();

/* Interrupt linkage prototype */
extern void rtc_handler_asm();
extern void keyboard_handler_asm();

#endif /* IDT_H */
