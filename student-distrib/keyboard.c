// References are listed in keyboard.h
#include "keyboard.h"
#include "lib.h"
#include "IDT.h"
#include "i8259.h"
#include "x86_desc.h"

// Text buffer that holds whatever we've typed so far
static unsigned char keyboard_buf[KB_SIZE];

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
    keyboard_buf[0] = '\0';
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
    if (scancode[c] == '\b') { // Check for backspace
        delete_char_from_buf();
        // print_buf();
    }
    else if (scancode[c] == '\n') { 
        // Newline character, call printf and clear text buffer
        printf((int8_t*) keyboard_buf);
        putc('\n');
        keyboard_buf[0] = '\0';
    }

    else if (scancode[c] != 0) {
        add_char_to_buf(scancode[c]);
        // print_buf();
    }
    
}

// TODO Sean: Add function definition and comments
// Adds a character to the buffer when any alphanumeric key is pressed
void add_char_to_buf(unsigned char c) {
    int add_idx, x, y;
    char* video_mem = (char *) VIDEO; 

    uint32_t buf_len = strlen((int8_t*) keyboard_buf); 
    if (buf_len < KB_SIZE-1) {
        keyboard_buf[buf_len] = c;
        keyboard_buf[buf_len+1] = '\0';

        x = get_screen_x();
        y = get_screen_y();

        if (y == NUM_ROWS-1 && buf_len == NUM_COLS-1) {
            video_scroll();
        }
        // Calculate the index that we should write the character to
        add_idx = convert_buf_idx(x, y, buf_len);
        *(uint8_t *)(video_mem + (add_idx << 1)) = keyboard_buf[buf_len];
    }
}

// TODO Sean: Add function definition and comments
// Deletes a character from the buffer when 'backspace' key is pressed
void delete_char_from_buf() {
    int erase_idx, x, y;
    char* video_mem = (char *) VIDEO;

    uint32_t buf_len = strlen((int8_t*) keyboard_buf); 
    if (buf_len > 0) {
        keyboard_buf[buf_len-1] = '\0';

        x = get_screen_x();
        y = get_screen_y();

        // Calculate index that we should erase the character from
        erase_idx = convert_buf_idx(x, y, buf_len) - 1;
        *(uint8_t *)(video_mem + (erase_idx << 1)) = ' ';
    }
}

// TODO Sean: Add function definition and comments
// Converts a buffer index to the video memory index
int convert_buf_idx(int x, int y, int buf_len) {
    int yval = (buf_len >= NUM_COLS-1) ? (y-1) : y;
    return ((NUM_COLS * yval) + x + buf_len);
}

