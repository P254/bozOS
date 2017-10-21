#include "lib.h"
#include "exceptions.h"
#include "x86_desc.h"

/* Functions to handle exceptions
 * Source: http://www.osdever.net/bkerndev/Docs/isrs.htm
 */

/* Exception handler prototype */
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
void handle_e16();
void handle_e17();
void handle_e18();
void handle_e19();

void handle_default();

// Function pointer array
void (*handle_exceptions[N_EXCEPTIONS])() = {handle_e0, handle_e1, handle_e2, handle_e3, handle_e4, handle_e5, handle_e6, handle_e7, handle_e8, handle_e9, handle_e10, handle_e11, handle_e12, handle_e13, handle_e14, handle_e16, handle_e17, handle_e18, handle_e19};

// Initialize the exception handlers in the IDT
void init_idt_exceptions() {
    unsigned int i;
    // Initalizes the exceptions by writing them to the IDT
    for (i = 0; i < N_EXCEPTIONS + 1; i++) {
        if (i < 15) SET_IDT_ENTRY(idt[i], handle_exceptions[i]);
        else if (i == 15) SET_IDT_ENTRY(idt[i], handle_default); // Entry 15 is empty according to documentation
        else SET_IDT_ENTRY(idt[i], handle_exceptions[i-1]);
    }
    // Indices 32-255 are user-defined, so we write a default handler. 
    for (i = N_EXCEPTIONS + 1; i < NUM_VEC; i++) {
        SET_IDT_ENTRY(idt[i], handle_default);   
    }
}

/* Function defintions here */
void handle_e0() {
    printf("Interrupt 0 - Divide Error Exception (#DE) \n");
    while(1);
}

void handle_e1() {
    printf("Interrupt 1 - Debug Exception (#DB) \n");
    while(1);
}

void handle_e2() {
    printf("Interrupt 2 - NMI Interrupt\n");
    while(1);
}
void handle_e3() {
    printf("Interrupt 3 - Breakpoint Exception (#BP)\n");
    while(1);
}
void handle_e4() {
    printf("Interrupt 4 - Overflow Exception (#OF)\n");
    while(1);
}
void handle_e5() {
    printf("Interrupt 5 - BOUND Range Exceeded Exception (#BR)\n");
    while(1);
}
void handle_e6() {
    printf("Interrupt 6 - Invalid Opcode Exception (#UD)\n");
    while(1);
}
void handle_e7() {
    printf("Interrupt 7 - Device Not Available Exception (#NM)\n");
    while(1);
}
void handle_e8() {
    printf("Interrupt 8 - Double Fault Exception (#DF)\n");
    while(1);
}
void handle_e9() {
    // Exception Class Abort. (Intel reserved; do not use. Recent IA-32 processors do not generate this exception.)
    printf("Interrupt 9 - Coprocessor Segment Overrun\n");
    while(1);
}
void handle_e10() {
    printf("Interrupt 10 - Invalid TSS Exception (#TS)\n");
    while(1);
}
void handle_e11() {
    printf("Interrupt 11 - Segment Not Present (#NP)\n");
    while(1);
}
void handle_e12() {
    printf("Interrupt 12 - Stack Fault Exception (#SS)\n");
    while(1);
}
void handle_e13() {
    printf("Interrupt 13 - General Protection Exception (#GP)\n");
    while(1);
}
void handle_e14() {
    printf("Interrupt 14 - Page-Fault Exception (#PF)\n");
    while(1);
}
void handle_e16() {
    printf("Interrupt 16 - x87 FPU Floating-Point Error (#MF)\n");
    while(1);
}
void handle_e17() {
    printf("Interrupt 17 - Alignment Check Exception (#AC)\n");
    while(1);
}
void handle_e18() {
    printf("Interrupt 18 - Machine-Check Exception (#MC)\n");
    while(1);
}
void handle_e19() {
    printf("Interrupt 19 - SIMD Floating-Point Exception (#XF)\n");
    while(1);
}

void handle_default() {
    printf("Default function handler. Nothing specified.\n");
    while(1);
}