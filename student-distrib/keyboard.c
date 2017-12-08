// References are listed in keyboard.h
#include "keyboard.h"
#include "lib.h"
#include "IDT.h"
#include "i8259.h"
#include "x86_desc.h"
#include "multi_term.h"

static unsigned char kb_buf[KB_SIZE]; // Text buffer that holds whatever we've typed so far
static unsigned char int_buf[KB_SIZE]; // Intermediate buffer for copying to terminal buffer
static enum kb_t terminal_read_release[3]; // Keep track of enter being released for which keyboard
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
    key_status = 0;
    terminal_read_release[TERM_1] = ENTER_WAITING;
    terminal_read_release[TERM_2] = ENTER_WAITING;
    terminal_read_release[TERM_3] = ENTER_WAITING;
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
        // Check for newline character
        add_char_to_buf(scanCodeTable[c]);  // if new line, add it to buffer, then
        copy_kb_buf();                      // clear keyboard buf and add to intermediate buf
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
    
    // We want to write to the KB buffer that corresponds to the active terminal 
    uint8_t term_num = get_active_terminal();
    scanCode = inb(KB_DATA_PORT); //get data from port when key is pressed/released
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
            clear_screen(); //clear screen
            kb_buf[0] = '\0'; //reset keyboard buffer
            printf("391OS> ");
        }
        else if (scanCode == ALT_PRESSED) {key_status += ALT_FLAG;}
        else if (scanCode == FN_1 && (key_status & ALT_FLAG)) { switch_terminal(TERM_1); }
        else if (scanCode == FN_2 && (key_status & ALT_FLAG)) { switch_terminal(TERM_2); }
        else if (scanCode == FN_3 && (key_status & ALT_FLAG)) { switch_terminal(TERM_3); }


        else if (scanCode == ENTER_PRESSED) { //if \n is pressed
            terminal_read_release[term_num] = ENTER_RELEASED; //allow terminal to be read if we are calling that function
            position = (int) scanCode;
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
        x = get_screen_x(ACTIVE_TERM); //get screen coordinates
        y = get_screen_y(ACTIVE_TERM);

        if (c == '\n') { // if new line
            put_newln_kb();
            if ((buf_len + x) >= NUM_COLS) put_newln_kb(); //if we have an extended logical string, print another new line
        }
        else if (c != '\n') { // if not new line
            // calculate the index that we should write the character to
            add_idx = convert_to_vid_idx(x, y, buf_len);   
            // write the character from the buffer to video mem
            *(uint8_t *)(video_mem + (add_idx << 1)) = kb_buf[buf_len];
            *(uint8_t *)(video_mem + (add_idx << 1) + 1) = get_terminal_color(ACTIVE_TERM);
        }

        if (y == NUM_ROWS-1 && (buf_len + x) == NUM_COLS-1) { // if we are at bottom-right of screen
            video_scroll(ACTIVE_TERM); // scroll down
            set_screen_y(y-1, ACTIVE_TERM); // set "cursor" to second-last line
        }
    }
    // Deals with the case when the buffer is full
    else if (c == '\n'){ //if new line
        //print two new lines as we have exceeded buffer limit and terminal line limit
        put_newln_kb();
        put_newln_kb();
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
        x = get_screen_x(ACTIVE_TERM); //get screen coordinates
        y = get_screen_y(ACTIVE_TERM);

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
enum kb_t* kb_read_release() {
    return terminal_read_release;
}

/*
 * get_kb_buffer
 *   DESCRIPTION: Returns the pointer of the keyboard buffer. Used for terminal-switching.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: unsigned char -- pointer to keyboard buffer
 *   SIDE EFFECTS: none
 */
unsigned char* get_kb_buffer() {
    return (unsigned char*) kb_buf;
}


/*
 * get_int_buffer
 *   DESCRIPTION: Returns the pointer of the intermediate buffer. For use by terminal driver.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: unsigned char -- pointer to intermediate buffer
 *   SIDE EFFECTS: none
 */
unsigned char* get_int_buffer() {
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

/*
 * put_newln_kb
 *   DESCRIPTION: Puts a new line onto the active terminal. For use by keyboard driver ONLY. 
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: modifies the video memory of the active terminal
 */
void put_newln_kb() {
    int y = get_screen_y(ACTIVE_TERM);
    if (y == NUM_ROWS-1) {
        video_scroll(ACTIVE_TERM);
        set_screen_x(0, ACTIVE_TERM);
    }
    else {
        set_screen_y(y+1, ACTIVE_TERM);
        set_screen_x(0, ACTIVE_TERM);
    }
}
