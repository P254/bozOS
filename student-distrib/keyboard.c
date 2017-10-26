// http://wiki.osdev.org/PS/2_Keyboard
#include "keyboard.h"
#include "lib.h"
#include "IDT.h"
#include "i8259.h"
#include "x86_desc.h"
// scancode taken from osdever.com
volatile int KEY_STATUS = 0;
unsigned char scancode[KB_SIZE*2] =
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
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*',	/* 9 */ /*SHIFT TABLE*/
  '(', ')', '_', '+', '\b',	/* Backspace */
  '\t',			/* Tab */
  'Q', 'W', 'E', 'R',	/* 19 */
  'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', '\n',	/* Enter key */
    0,			/* 29   - Control */
  'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';',	/* 39 */
 '\'', '~',   0,		/* Left shift */
 '\\', 'Z', 'X', 'C', 'V', 'B', 'N',			/* 49 */
  'M', '<', '>', '?',   0,				/* Right shift */
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
    set_IDT_wrapper(SOFT_INT_START + 1, getScanCode);
}


/*
 * getScanCode
 *   DESCRIPTION: Grabs the scancode from the keyboard and returns the code if it is non-zero.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: char -- the character scanned in from the keyboard
 *   SIDE EFFECTS: none
 */
void getScanCode() {
    unsigned char scanCode;
    scanCode = inb(KEYBOARD_PORT);
    if(scanCode&0x80){
      KEY_STATUS = 0;
      /*check release of shift, alt or ctrl*/
    }
    else {
      /* Here, a key was just pressed. Please note that if you
      *  hold a key down, you will get repeated key press
      *  interrupts. */

      /* Just to show you how this works, we simply translate
      *  the keyboard scancode into an ASCII value, and then
      *  display it to the screen. You can get creative and
      *  use some flags to see if a shift is pressed and use a
      *  different layout, or you can add another 128 entries
      *  to the above layout to correspond to 'shift' being
      *  held. If shift is held using the larger lookup table,
      *  you would add 128 to the scancode when you look for it */
      if(scanCode==0x2A){KEY_STATUS=1;}
      else if(KEY_STATUS==1){putc(scancode[(int)(scanCode)+128]);}
      else if(KEY_STATUS==0){putc(scancode[(int)scanCode]);}

    }
    while(1);
}

/*
 * get_char
 *   DESCRIPTION: Grabs the character and prints it to the screen
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: void
 *   SIDE EFFECTS: Prints a character to screenq
 */
/*void get_char() {
    // we have to use this somewhere to print to the screen.
    else if(CAPSFLG==0) putc(scancode[(int)getScanCode()]);
    send_eoi(1);
    // while(1);
    asm("hlt");
}*/
