// http://wiki.osdev.org/PS/2_Keyboard
#include "keyboard.h"
#include "lib.h"
#include "IDT.h"
#include "i8259.h"
#include "x86_desc.h"
// scancode taken from osdever.com
int pos = 0;
volatile int TERMINAL_READ_RELEASE=0;
unsigned char keyboard_buffer[128];
volatile int KEY_STATUS = 0;
unsigned char scancode[KB_SIZE*3] =
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






    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
    '9', '0', '-', '=', '\b',	/* Backspace */
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
    int i;
    for(i=0 ; i<128 ;i++){
        keyboard_buffer[i]=NULL;
    }
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
    unsigned int position;
    scanCode = inb(KEYBOARD_PORT);
    if(scanCode&0x80){
        //printf("released");
        if(scanCode==0xAA) KEY_STATUS &= 0x10;
        //if(scanCode==0xBA) KEY_STATUS&= 0x01;
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
      if(scanCode==0x2A){KEY_STATUS=0x1;}
      if(scanCode==0x3A){KEY_STATUS^=0x10;}
      if(scanCode==0x1C){TERMINAL_READ_RELEASE=1;}
      else if(KEY_STATUS==0x0){
          position = (int)(scanCode);
          if(position<90 && 0<=position)
          //putc(position);
          pos++;
          if(pos<128)
            keyboard_buffer[pos] = position;
        }
      else if(KEY_STATUS==0x1){
          position = (int)(scanCode)+90;
          if(position<180 && 90<=position)
          //putc(position);
          pos++;
          if(pos<128)
            keyboard_buffer[pos] = position;

        }
        else if(KEY_STATUS==0x10){
            position = (int)(scanCode)+180;
            if(position<270 && 180<=position)
            //putc(position);
            pos++;
            if(pos<128)
                keyboard_buffer[pos] = position;
          }
    }
    send_eoi(1);
    while(1);
}

int terminal_open(){
    return 0;
}

int terminal_close(){
    return 0;
}

int terminal_read(int fd, unsigned char* buf, int nbytes){
   int j=0;
   while(!TERMINAL_READ_RELEASE){}
   while(curr_char!='\n'){
       buf[j] = keyboard_buffer[j];
       j++;
   }
   int i=0;
   for(i=0 ; i<j ; i++){
       keyboard_buffer[i]=NULL;
       keyboard_buffer[i] = keyboard_buffer[i+1];
   }
   return j;


}

int terminal_write(int fd, const unsigned char* buf, int nbytes)
{
    int i;
    for(i=0 ; i<nbytes; i++){
        putc(buf[i]);
    }
    putc('_');
    return nbytes;
}
