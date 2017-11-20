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
  0,         /* Tab */
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
  0,         /* Tab */
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
  0,         /* Tab */
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
  0,         /* Tab */
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
    set_IDT_wrapper(KB_IDT_ENTRY, keyboard_handler_asm); //add handler to IDT table
    kb_buf[0] = '\0';//NULL terminate intermediate and keyboard buffer
    int_buf[0] = '\0';
    terminal_read_release = 0; //set flags
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
    send_eoi(KB_IRQ); //send EOI signal
    unsigned int c = get_scan_code(); //get ascii code character
    if (scanCodeTable[c] == '\b') {
        // Check for backspace
        del_char_from_buf(); //if backspace, delete char
    }
    else if(scanCodeTable[c] == '\n'){
        //Chec for new line
      add_char_to_buf(scanCodeTable[c]); //if new line, add it to buffer
      copy_kb_buf(); //then clear keybaord buffer and add to intermidate buffer
    }
    else if (scanCodeTable[c] != 0) { //all other scancodes
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
    scanCode = inb(KB_DATA_PORT); //get data from port when key is pressed/released
    //printf("%x", key_status);
    //printf("%x", scanCode);
    if (scanCode & RELEASED_KEY_MASK) { //check if any key is released

        if (scanCode == SHIFT_RELEASE) {key_status &= CLEAR_SHIFT_FLAG;}
        //if shift is release, clear shift status
        else if (scanCode == CTRL_RELEASE)  {key_status &= CLEAR_CTRL_FLAG;}
        //if control is released, clear control status
        else if (scanCode == ALT_RELEASE)  {key_status &= CLEAR_ALT_FLAG;}

    }
    else { //else if a key is pressed
        if (scanCode == SHIFT_PRESSED) { key_status += SHIFT_FLAG; }
        //if shift is pressed, set shift status
        else if (scanCode == CAPS_PRESSED) { key_status ^= CAPS_FLAG; }
        //if caps loc is pressed, set OR clear caps status, depending on previous state
        else if (scanCode == CTRL_PRESSED) { key_status += CTRL_FLAG; }
        //if ctrl is pressed, set ctrl status
        else if (scanCode == L_PRESSED && (key_status & CTRL_FLAG)) { //if CTRL+L is pressed
            clear(); //clear screen
            kb_buf[0] = '\0'; //reset keyboard buffer
        }
        else if(scanCode == F_ONE && (key_status & ALT_FLAG) ) {/*TERM1*/}
        else if(scanCode == F_TWO && (key_status & ALT_FLAG) ) {/*TERM2*/}
        else if(scanCode == F_THREE && (key_status & ALT_FLAG) ) {/*TERM3*/}

        else if (scanCode == ALT_PRESSED) {key_status += ALT_FLAG;}

        else if (scanCode == ENTER_PRESSED) { //if \n is pressed
            terminal_read_release = 1; //allow terminal to be read if we are calling that function
            position  = (int) scanCode;
            return position; //send scan code to handler
        }



        else if (key_status == 0) { //if no special keys are pressed
            position = (int) (scanCode);
            if (position < BASE_KB_SIZE && 0 <= position) {
                return position; //send basic scan code to handler
            }
        }

        else if (key_status == SHIFT_FLAG) { //if shift is pressed
            position = (int) (scanCode) + BASE_KB_SIZE; //acquire shift table
            if (position < (SHIFT_EXTEND*BASE_KB_SIZE) && BASE_KB_SIZE <= position) { //check for invalid scan codes
                return position; //send shift scan code to handler
            }
        }

        else if (key_status == CAPS_FLAG) { //if caps lock is pressed
            position = (int) (scanCode) + (SHIFT_EXTEND*BASE_KB_SIZE); //acquire caps lock table
            if(position < (CAPS_EXTEND*BASE_KB_SIZE) && BASE_KB_SIZE <= position) { //check for invalid scan codes
                return position; //send caps lock scan code to handler
            }
        }

        else if (key_status && CAPS_FLAG && SHIFT_FLAG) {//if caps lock AND shift is pressed
            position = (int) (scanCode) + (CAPS_EXTEND*BASE_KB_SIZE); //acquire shift+caps lock table
            if(position < (BOTH_EXTEND*BASE_KB_SIZE) && (CAPS_EXTEND*BASE_KB_SIZE) <= position) { //check for invalid scan codes
                return position; //send combined scan code to handler
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
    uint32_t buf_len = strlen((int8_t*) kb_buf); //get length of keyboard buffer
    if (buf_len < KB_SIZE-1) { //only if we have not reached the limit
        kb_buf[buf_len+1] = '\0'; //null terminate our string
        kb_buf[buf_len] = c; //add the character we just proccessed to buf

        int add_idx, x, y;
        char* video_mem = (char *) VIDEO; //get mem loc
        x = get_screen_x(); //get screen coordinates
        y = get_screen_y();

        if (y == NUM_ROWS-1 && buf_len == NUM_COLS-1) { //if we are at bottom of screen
            video_scroll(); //scroll down
            set_screen_y(y-1); //set "cursor" to last line
        }

        else if (c == '\n') { // if new line
            putc('\n'); //print new line
            if (buf_len >= NUM_COLS) putc('\n'); //if we have an extended logical string, print another new line
        }

        else if(c != '\n') { //if not new line
          add_idx = convert_to_vid_idx(x, y, buf_len); // calculate the index that we should write the character to
          *(uint8_t *)(video_mem + (add_idx << 1)) = kb_buf[buf_len]; //write the character from the buffer to video mem
        }
    }
    // Deals with the case when the buffer is full
    else if (c == '\n'){ //if new line
        printf("\n\n"); //print two new lines as we have exceeded buffer limit and terminal line limit.
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
    uint32_t buf_len = strlen((int8_t*) kb_buf); //get length of keyboard buffer
    if (buf_len > 0) { //if we have a valid buf len
        kb_buf[buf_len-1] = '\0'; //null terminal our buffer

        int erase_idx, x, y;
        char* video_mem = (char *) VIDEO; //get mem loc
        x = get_screen_x(); //get screen coordinates
        y = get_screen_y();

        erase_idx = convert_to_vid_idx(x, y, buf_len) - 1; // Calculate index that we should erase the character from
        *(uint8_t *)(video_mem + (erase_idx << 1)) = ' '; //erase char from video mem loc
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
      for (i = 0; i < KB_SIZE; i++) { //max copy length is kb size
        int_buf[i] = kb_buf[i]; //copy char by char
        if (kb_buf[i] == '\n') {
            kb_buf[i] = '\0'; //we must also remove the new line from the kb  buffer
            break; //if we encounter a new line, we stop copying
        }
        else kb_buf[i] = '\0'; // Flush-as-you-go
      }
  return;
}
