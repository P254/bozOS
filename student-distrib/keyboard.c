// References are listed in keyboard.h
#include "keyboard.h"
#include "lib.h"
#include "IDT.h"
#include "i8259.h"
#include "x86_desc.h"

static unsigned char kb_buf[KB_SIZE]; // Text buffer that holds whatever we've typed so far
static unsigned char int_buf[KB_SIZE]; // Intermediate buffer for copying to terminal buffer
static int terminal_read_release;
static int key_status;

unsigned char scanCodeTable[EXPANDED_KB_SIZE] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', /* 9 */
  '9', '0', '-', '=', '\b', /* Backspace */
  '\t',         /* Tab */
  'q', 'w', 'e', 'r',   /* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', /* Enter key */
    0,          /* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', /* 39 */
 '\'', '`',   0,        /* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',            /* 49 */
  'm', ',', '.', '/',   0,              /* Right shift */
  '*',
    0,  /* Alt */
  ' ',  /* Space bar */
    0,  /* Caps lock */
    0,  /* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,  /* < ... F10 */
    0,  /* 69 - Num lock*/
    0,  /* Scroll Lock */
    0,  /* Home key */
    0,  /* Up Arrow */
    0,  /* Page Up */
  '-',
    0,  /* Left Arrow */
    0,
    0,  /* Right Arrow */
  '+',
    0,  /* 79 - End key*/
    0,  /* Down Arrow */
    0,  /* Page Down */
    0,  /* Insert Key */
    0,  /* Delete Key */
    0,   0,   0,
    0,  /* F11 Key */
    0,  /* F12 Key */
    0,  /* All other keys are undefined */



    0,  27, '!', '@', '#', '$', '%', '^', '&', '*', /* 9 */ /*SHIFT TABLE*/
  '(', ')', '_', '+', '\b', /* Backspace */
  '\t',         /* Tab */
  'Q', 'W', 'E', 'R',   /* 19 */
  'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', /* Enter key */
    0,          /* 29   - Control */
  'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', /* 39 */
 '\"', '~',   0,        /* Left shift */
 '|', 'Z', 'X', 'C', 'V', 'B', 'N',            /* 49 */
  'M', '<', '>', '?',   0,              /* Right shift */
  '*',
    0,  /* Alt */
  ' ',  /* Space bar */
    0,  /* Caps lock */
    0,  /* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,  /* < ... F10 */
    0,  /* 69 - Num lock*/
    0,  /* Scroll Lock */
    0,  /* Home key */
    0,  /* Up Arrow */
    0,  /* Page Up */
  '-',
    0,  /* Left Arrow */
    0,
    0,  /* Right Arrow */
  '+',
    0,  /* 79 - End key*/
    0,  /* Down Arrow */
    0,  /* Page Down */
    0,  /* Insert Key */
    0,  /* Delete Key */
    0,   0,   0,
    0,  /* F11 Key */
    0,  /* F12 Key */
    0,  /* All other keys are undefined */



    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', /* 9 */
    '9', '0', '-', '=', '\b',   /* Backspace */
  '\t',         /* Tab */
  'Q', 'W', 'E', 'R',   /* 19 */
  'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', '\n', /* Enter key */
    0,          /* 29   - Control */
  'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', /* 39 */
 '\'', '~',   0,        /* Left shift */
 '\\', 'Z', 'X', 'C', 'V', 'B', 'N',            /* 49 */
  'M', '<', '>', '?',   0,              /* Right shift */
  '*',
    0,  /* Alt */
  ' ',  /* Space bar */
    0,  /* Caps lock */
    0,  /* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,  /* < ... F10 */
    0,  /* 69 - Num lock*/
    0,  /* Scroll Lock */
    0,  /* Home key */
    0,  /* Up Arrow */
    0,  /* Page Up */
  '-',
    0,  /* Left Arrow */
    0,
    0,  /* Right Arrow */
  '+',
    0,  /* 79 - End key*/
    0,  /* Down Arrow */
    0,  /* Page Down */
    0,  /* Insert Key */
    0,  /* Delete Key */
    0,   0,   0,
    0,  /* F11 Key */
    0,  /* F12 Key */
    0,  /* All other keys are undefined */


    0,  27, '!', '@', '#', '$', '%', '^', '&', '*', /* 9 */
    '(', ')', '_', '+', '\b',   /* Backspace */
  '\t',         /* Tab */
  'q', 'w', 'e', 'r',   /* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '{', '}', '\n', /* Enter key */
    0,          /* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ':', /* 39 */
 '\"', '`',   0,        /* Left shift */
 '|', 'z', 'x', 'c', 'v', 'b', 'n',            /* 49 */
  'm', '<', '>', '?',   0,              /* Right shift */
  '*',
    0,  /* Alt */
  ' ',  /* Space bar */
    0,  /* Caps lock */
    0,  /* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,  /* < ... F10 */
    0,  /* 69 - Num lock*/
    0,  /* Scroll Lock */
    0,  /* Home key */
    0,  /* Up Arrow */
    0,  /* Page Up */
  '-',
    0,  /* Left Arrow */
    0,
    0,  /* Right Arrow */
  '+',
    0,  /* 79 - End key*/
    0,  /* Down Arrow */
    0,  /* Page Down */
    0,  /* Insert Key */
    0,  /* Delete Key */
    0,   0,   0,
    0,  /* F11 Key */
    0,  /* F12 Key */
    0,  /* All other keys are undefined */
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
    set_IDT_wrapper(KB_IDT_ENTRY, keyboard_handler_asm);

    /****************** Testing code *********************/
    // unsigned char* teststr = "Hello world!\nThis is ECE 391\n";
    // strncpy(kb_buf, teststr, strlen((int8_t*) teststr));

    kb_buf[0] = '\0';
    int_buf[0] = '\0';
    terminal_read_release = 0;
    key_status = 0;
}

/*
 * kb_int_handler
 *   DESCRIPTION: Main keyboard interrupt handler grabs the character and prints it to the screen
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: void
 *   SIDE EFFECTS: Prints a character to screen
 */
void kb_int_handler() {
    // we have to use this somewhere to print to the screen.
    send_eoi(KB_IRQ);
    unsigned int c = get_scan_code();
    if (scanCodeTable[c] == '\b') {
        // Check for backspace
        del_char_from_buf();
    }
    else if(scanCodeTable[c] == '\n'){
      add_char_to_buf(scanCodeTable[c]);
      copy_kb_buf();
    }
    else if (scanCodeTable[c] != 0) {
        // Adds characters, including the line feed '\n' character
        add_char_to_buf(scanCodeTable[c]);
    }
}

/*
 * get_scan_code
 *   DESCRIPTION: Grabs the scancode from the keyboard and updates the keyboard buffer.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: unsigned int -- index into the scancode table
 *   SIDE EFFECTS: updates the keyboard buffer
 */
unsigned int get_scan_code() {
    unsigned char scanCode;
    unsigned int position;
    // unsigned int pos = strlen(kb_buf);
    scanCode = inb(KB_DATA_PORT);
    if (scanCode & 0x80) {
        /* check release of shift, alt or ctrl */
        //printf("released");
        if (scanCode == 0xAA) { key_status &= 0x110; }

        else if (scanCode == 0xA6)  { key_status &= 0x011; }
        //if(scanCode==0xBA) key_status&= 0x01;
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
        if (scanCode == 0x2A) { key_status += 0x1; }

        else if (scanCode == 0x3A) { key_status ^= 0x10; }

        else if (scanCode == 0x1D) { key_status += 0x100; }

        else if (scanCode == 0x26 && (key_status & 0x100)) {
            clear();
            kb_buf[0] = '\0';
        }

        else if (scanCode == 0x1C) {
            terminal_read_release = 1;
            position  = (int) scanCode;
            return position;
        }

        else if (key_status == 0x0) {
            position = (int) (scanCode);
            if (position < 90 && 0 <= position) {
                return position;
            }
        }

        else if (key_status == 0x1) {
            position = (int) (scanCode) + 90;
            if (position < 180 && 90 <= position) {
                return position;
            }
        }

        else if (key_status == 0x10) {
            position = (int) (scanCode) + 180;
            if(position < 270 && 180 <= position) {
                return position;
            }
        }

        else if (key_status == 0x011) {
            position = (int) (scanCode) + 270;
            if(position < 360 && 270 <= position) {
                return position;
            }
        }
    }
    return 0;
}


/*
 * add_char_to_buf
 *   DESCRIPTION: Adds a character to the keyboard buffer
 *   INPUTS: c -- input character we want to print
 *   OUTPUTS: none
 *   RETURN VALUE: void
 *   SIDE EFFECTS: prints character to the screen and modifies kb_buf accordingly
 */
void add_char_to_buf(unsigned char c) {
    uint32_t buf_len = strlen((int8_t*) kb_buf);
    if (buf_len < KB_SIZE-1) {
        kb_buf[buf_len+1] = '\0';
        kb_buf[buf_len] = c;

        int add_idx, x, y;
        char* video_mem = (char *) VIDEO;
        x = get_screen_x();
        y = get_screen_y();

        if (y == NUM_ROWS-1 && buf_len == NUM_COLS-1) {
            video_scroll();
            set_screen_y(y-1);
        }
        // if new line
        else if (c == '\n') { 
            putc('\n');
            if (buf_len >= NUM_COLS) putc('\n');
        }

        // Calculate the index that we should write the character to
        else if(c != '\n') {
          add_idx = convert_to_vid_idx(x, y, buf_len);
          *(uint8_t *)(video_mem + (add_idx << 1)) = kb_buf[buf_len];
        }
    }
    // Deals with the case when the buffer is full
    else if (c == '\n'){
        printf("\n\n");
    }
}


/*
 * del_char_from_buf
 *   DESCRIPTION: Deletes a character from the buffer when 'backspace' key is pressed
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: void
 *   SIDE EFFECTS: removes character from the screen and modifies kb_buf accordingly
 */
void del_char_from_buf() {
    uint32_t buf_len = strlen((int8_t*) kb_buf);
    if (buf_len > 0) {
        kb_buf[buf_len-1] = '\0';

        int erase_idx, x, y;
        char* video_mem = (char *) VIDEO;
        x = get_screen_x();
        y = get_screen_y();

        // Calculate index that we should erase the character from
        erase_idx = convert_to_vid_idx(x, y, buf_len) - 1;
        *(uint8_t *)(video_mem + (erase_idx << 1)) = ' ';
    }
}

/*
 * convert_to_vid_idx
 *   DESCRIPTION: Converts a set of coordinates to the index to video memory
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: int -- the video memory index we are interested in
 *   SIDE EFFECTS: none
 */
int convert_to_vid_idx(int x, int y, int buf_len) {
    return ((NUM_COLS * y) + x + buf_len);
}

/*
 * kb_read_release
 *   DESCRIPTION: Returns the pointer to terminal_read_release. For use by terminal driver.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: int -- pointer to terminal_read_release
 *   SIDE EFFECTS: none
 */
int* kb_read_release() {
    return (int*) &terminal_read_release;
}

/*
 * get_kb_buffer
 *   DESCRIPTION: Returns the pointer of terminal_read_release. For use by terminal driver.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: unsigned char -- pointer to intermediate buffer
 *   SIDE EFFECTS: none
 */
unsigned char* get_kb_buffer() {
  return (unsigned char*) int_buf;
}

/*
 * copy_kb_buf
 *   DESCRIPTION: Copies keyboard buffer into intermediate buffer. 
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: modifies int_buf
 */
void copy_kb_buf() {
      int i = 0;
      for (i = 0; i < 128; i++) {
        int_buf[i] = kb_buf[i];
        if (kb_buf[i] == '\n') {
            kb_buf[i] = '\0';
            break;
        }
        else kb_buf[i] = '\0'; // Flush-as-you-go
      }
  return;
}
