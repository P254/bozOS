// http://wiki.osdev.org/PS/2_Keyboard
#include "keyboard.h"
#include "lib.h"
#include "IDT.h"
#include "i8259.h"
#include "x86_desc.h"
unsigned char scancode []={'\0', '\0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\0', '\0',
	 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\'', '\0', 'a', 's',
	 'd', 'f', 'g', 'h', 'j', 'k', 'l' , ';', '\'', '`', '\0', '\\', 'z', 'x', 'c', 'v',
	 'b', 'n', 'm',',', '.', '/', '\0', '*', '\0', ' ', '\0'};

void kb_init(void){
    enable_irq(1); // the keyboard interrupt
    set_IDT_wrapper(SOFT_INT_START + 1, get_char);
}



char getScancode() {
    char c = 0 ;
    do {
        if (inb(0x60) != c ) {
            c = inb(0x60);
            if (c>0) return c;
        }
    } while(1);
}

void get_char() {
    // we have to use this somewhere to print to the screen.
    // outb smthing

    send_eoi(1);
    putc(scancode[getScancode()]);

}
