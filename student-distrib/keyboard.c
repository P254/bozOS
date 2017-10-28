// References are listed in keyboard.h
#include "keyboard.h"
#include "lib.h"
#include "IDT.h"
#include "i8259.h"
#include "x86_desc.h"

// Text buffer that holds whatever we've typed so far
static unsigned char kb_buf[KB_SIZE];
// Scroll flag that is held until we hit 'enter'
static int scroll_flag;

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
    // TODO: Add macro for  KB_IDT_ENTRY
    enable_irq(KB_IRQ); // the keyboard interrupt
    set_IDT_wrapper(KB_IDT_ENTRY, keyboard_handler_asm);
    kb_buf[0] = '\0';
}


/*
 * getScanCode
 *   DESCRIPTION: Grabs the scancode from the keyboard and ret the code if it is non-zero.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: char -- the character scanned in from the keyboard
 *   SIDE EFFECTS: none
 */
char getScanCode() {
    char code;
    code = inb(KB_DATA_PORT);
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
    send_eoi(KB_IRQ);
    unsigned int c = (unsigned int) getScanCode(); // This will be replaced by Abhishek's function
    if (scancode[c] == '\b') { 
        // Check for backspace
        delCharFrBuf();
    }
    else if (scancode[c] == '\n') { 
        // Newline character: clear text buffer and reset scroll flag
        putc('\n');
        kb_buf[0] = '\0';
        scroll_flag = 0;
    }

    else if (scancode[c] != 0) {
        addCharToBuf(scancode[c]);
    }
    
}

/*
 * addCharToBuf
 *   DESCRIPTION: Adds a character to the keyboard buffer
 *   INPUTS: c -- input character we want to print
 *   OUTPUTS: none
 *   RETURN VALUE: void
 *   SIDE EFFECTS: prints character to the screen and modifies kb_buf accordingly
 */
void addCharToBuf(unsigned char c) {
    int add_idx, x, y;
    char* video_mem = (char *) VIDEO; 

    uint32_t buf_len = strlen((int8_t*) kb_buf); 
    if (buf_len < KB_SIZE-1) {
        kb_buf[buf_len] = c;
        kb_buf[buf_len+1] = '\0';

        x = getScreenX();
        y = getScreenY();

        if (y == NUM_ROWS-1 && buf_len == NUM_COLS-1 && scroll_flag == 0) {
            videoScroll();
            scroll_flag = 1;
        }
        // Calculate the index that we should write the character to
        add_idx = convertToVidIdx(x, y-scroll_flag, buf_len);
        *(uint8_t *)(video_mem + (add_idx << 1)) = kb_buf[buf_len];
    }
}

/*
 * addCharToBuf
 *   DESCRIPTION: Deletes a character from the buffer when 'backspace' key is pressed
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: void
 *   SIDE EFFECTS: removes character from the screen and modifies kb_buf accordingly
 */
void delCharFrBuf() {
    int erase_idx, x, y;
    char* video_mem = (char *) VIDEO;

    uint32_t buf_len = strlen((int8_t*) kb_buf); 
    if (buf_len > 0) {
        kb_buf[buf_len-1] = '\0';

        x = getScreenX();
        y = getScreenY();

        // Calculate index that we should erase the character from
        erase_idx = convertToVidIdx(x, y-scroll_flag, buf_len) - 1;
        *(uint8_t *)(video_mem + (erase_idx << 1)) = ' ';
    }
}

/*
 * convertToVidIdx
 *   DESCRIPTION: Converts a set of coordinates to the index to video memory
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: int -- the video memory index we are interested in
 *   SIDE EFFECTS: none
 */
int convertToVidIdx(int x, int y, int buf_len) {
    return ((NUM_COLS * y) + x + buf_len);
}

