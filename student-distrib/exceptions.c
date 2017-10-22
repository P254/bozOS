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

void set_idt_reserved(idt_desc_t * item) {
    item->reserved4 = 0;
    item->reserved3 = 1;
    item->reserved2 = 1;
    item->reserved1 = 1;
    item->reserved0 = 0;
    item->present = 1;
    item->dpl = 0;
    item->seg_selector = KERNEL_CS;
}

// Function pointer array
// Entry #15 is empty, (according to documentation)
void (*handle_exceptions[N_EXCEPTIONS-1])() = {handle_e0, handle_e1, handle_e2, handle_e3, handle_e4, handle_e5, handle_e6, handle_e7, handle_e8, handle_e9, handle_e10, handle_e11, handle_e12, handle_e13, handle_e14, handle_e16, handle_e17, handle_e18, handle_e19};

// Initialize the exception handlers in the IDT
void init_idt_exceptions() {
    unsigned int i;
    // Initalizes the exceptions by writing them to the IDT
    for (i = 0; i < N_EXCEPTIONS_RESERVED + 1; i++) {
        if (i < EMPTY_EXCEPTION) {
            SET_IDT_ENTRY(idt[i], handle_exceptions[i]);
            set_idt_reserved(&idt[i]);
        }
        else if (i == EMPTY_EXCEPTION) {
            // Entry 15 (EMPTY_EXCEPTION) is empty, according to documentation
            SET_IDT_ENTRY(idt[i], handle_default);
            set_idt_reserved(&idt[i]);
        }
        else if (i > EMPTY_EXCEPTION && i < N_EXCEPTIONS) {
            SET_IDT_ENTRY(idt[i], handle_exceptions[i-1]);
            set_idt_reserved(&idt[i]);
        }
        else {
            // Indices 20-31 are reserved for some other purpose (according to the spec), so we write a default handler
            SET_IDT_ENTRY(idt[i], handle_default);
        }
    }

    /* Initialize hardware interrupts, which start at IDT entry 32.
     * Source: https://courses.engr.illinois.edu/ece391/fa2017/secure/references/descriptors.pdf
     * According to spec, there are 15 default hardware interrupts.
     * Source: http://wiki.osdev.org/Interrupts#General_IBM-PC_Compatible_Interrupt_Information
     */
    // TODO Sean: Move this segment of code below to i8259.c
    unsigned int irq_num;
    for (irq_num = 32; irq_num < 32+15; irq_num++ ) {
        // SET_IDT_ENTRY(idt[i], <insert hardware int handler function here>);
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
