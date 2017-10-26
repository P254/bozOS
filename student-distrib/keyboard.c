// References are listed in keyboard.h
#include "keyboard.h"
#include "lib.h"
#include "IDT.h"
#include "i8259.h"
#include "x86_desc.h"

/* Scancodes taken from osdever.com*/
unsigned char scancode[KB_SIZE] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
  '9', '0', '-', '=', '\b',	/* Backspace */
  '\t',			/* Tab */
  'q', 'w', 'e', 'r',	/* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
    0,			/* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
 '\'', '`',   0,		/* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
  'm', ',', '.', '/',   0,				/* Right shift */
  '*',
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
  '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
  '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};


/*
 * kb_init
 *   DESCRIPTION: Main function that initializes the keyboard interrupt. 
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: void
 *   SIDE EFFECTS: modifies the KB entry in the IDT
 */
void kb_init(void){
    enable_irq(KB_IRQ); // the keyboard interrupt
    set_IDT_wrapper(SOFT_INT_START + 1, keyboard_handler_asm);
}


/*
 * getScanCode
 *   DESCRIPTION: Grabs the scancode from the keyboard and returns the code if it is non-zero.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: char -- the character scanned in from the keyboard
 *   SIDE EFFECTS: none
 */
char getScanCode() {
    // char c = 0 ;
    // do {
    //     if (inb(KB_DATA_PORT) != c ) {
    //         c = inb(KB_DATA_PORT);
    //         if (c>0) return c;
    //     }
    // } while(1);
    char code;
    // cli();
    code = inb(KB_DATA_PORT);
    // val =  inb(0x61);       // Get keyboard acknowledge
    // outb(0x61, val | 0x80); // Disable bit 7
    // outb(0x61, val);  
    // sti();
    return code;
}

/*
 * get_char
 *   DESCRIPTION: Grabs the character and prints it to the screen
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: void
 *   SIDE EFFECTS: Prints a character to screen
 */
void get_char() {
    // we have to use this somewhere to print to the screen.
    // outb smthing
    
    char c = getScanCode();
    char d = inb(0x64);
    printf("c");
    send_eoi(KB_IRQ);
    asm("hlt");
}
