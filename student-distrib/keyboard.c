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

/*
 * kb_init
 *   DESCRIPTION: enables IRQ line 1 and writes keyboard handler into IDT table.
 *   INPUTS: void
 *   OUTPUTS: writes handler into IDT table.
 *   RETURN VALUE: void
 *   SIDE EFFECTS: none
 */
void kb_init(void){
    enable_irq(1); // the keyboard interrupt
    set_IDT_wrapper(SOFT_INT_START + 1, get_char); //add to IDT table at pos 33
}

/*
 * getScancode
 *   DESCRIPTION: Recieves byte from keyboard port and returns it if not 0.
 *   INPUTS: void
 *   OUTPUTS: none
 *   RETURN VALUE: char to write to screen
 *   SIDE EFFECTS: none
 */
unsigned char getScancode(void) {
    unsigned char c = 0;
    do {
        if (inb(KEYBOARD_PORT) != c ) { //if value of keyboard port is not 0
            c = inb(KEYBOARD_PORT); //get char and save
            if (c>0) return c; // if character is not 0, return it.
        }
    } while(1); //we continue checking port until we have a non-zero char.
}

/*
 * get_char
 *   DESCRIPTION: calls getScancode and prints its return value onto screen.
 *   INPUTS: void
 *   OUTPUTS: puts char onto screen
 *   RETURN VALUE: void
 *   SIDE EFFECTS: none
 */
void get_char(void) {
    putc(scancode[getScancode()]); //get character to print and putc it.
	send_eoi(1); //signal that interrupt is complete.
}
