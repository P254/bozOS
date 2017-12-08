#include "keyboard.h"
#include "terminal.h"
#include "x86_desc.h"
#include "lib.h"
#include "syscalls.h"
#include "scheduling.h"
#include "multi_term.h"

/*
 * terminal_open
 *   DESCRIPTION: Opens the terminal driver. Does nothing for now.
 *   INPUTS: filename -- ignored
 *   OUTPUTS: none
 *   RETURN VALUE: int32_t -- always returns 0
 *   SIDE EFFECTS: none
 */
int32_t terminal_open(const uint8_t* filename) {
    return 0;
}

/*
 * terminal_close
 *   DESCRIPTION: Closes the terminal driver. Does nothing for now.
 *   INPUTS: fd -- ignored
 *   OUTPUTS: none
 *   RETURN VALUE: int32_t -- always returns 0
 *   SIDE EFFECTS: none
 */
int32_t terminal_close(int32_t fd) {
    return 0;
}

/*
 * terminal_read
 *   DESCRIPTION: Copies data from the intermediate buffer to terminal buffer
 *   INPUTS: fd -- ignored
 *           buf -- pointer to terminal buffer
 *           nbytes -- number of bytes we want to read
 *   OUTPUTS: none
 *   RETURN VALUE: int32_t -- number of bytes copied, or -1 on failure
 *   SIDE EFFECTS: flushes the intermediate buffer
 */
int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes) {
    // Check for bad inputs
    if (buf == NULL || nbytes < 0) return -1; //check invalid input

    unsigned char* source = get_int_buffer(); //get intermediate buffer
    unsigned char* dest = (unsigned char*) buf; //set system buffer as dest
    uint8_t term_num = get_active_task();

    int32_t i, bytes_copied, bytes_to_copy, c = 0;
    bytes_to_copy = (nbytes < KB_BUF_SIZE) ? nbytes : KB_BUF_SIZE; //check how much we need to copy

    enum kb_t* enter_flag = kb_read_release();
    while(enter_flag[term_num] != ENTER_RELEASED); //spin until \n is pressed

    for (i = 0; i < bytes_to_copy; i++) {
        dest[i] = source[i]; //copy over into system buffer
        if (source[i] == '\n') break;
        else source[i] = '\0'; // Flush-as-you-go
    }
    dest[bytes_to_copy-1] = '\n'; // We want to terminate our intermediate buffer with '\n' instead of '\0'
    enter_flag[term_num] = ENTER_WAITING; // Reset the keyboard flag
    bytes_copied = i;
    i++;

    // shift the KB buffer
    while (source[i]!='\0') {
        source[c] = source[i];
        source[i] = '\0';
        c++;
        i++;
    }
    return bytes_copied;
}

/*
 * terminal_write
 *   DESCRIPTION: Writes data from terminal buffer to the screen
 *   INPUTS: fd -- ignored
 *           buf -- pointer to terminal buffer
 *           nbytes -- number of bytes we want to write
 *   OUTPUTS: none
 *   RETURN VALUE: int32_t -- number of bytes written, or -1 on failure
 *   SIDE EFFECTS: flushes the keyboard buffer up until '\n'
 */
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes) {
    // Check for bad inputs
    if (buf == NULL || nbytes < 0) return -1; //check invalid input

    uint8_t term_num = get_active_task();
    pcb_t* PCB_base = get_PCB_tail(term_num);
    uint8_t text_file_flag = PCB_base->fd_arr[fd].text_file_flag;

    uint32_t i, bytes_to_write;
    i = 0;
    bytes_to_write = (nbytes < KB_BUF_SIZE) ? nbytes : KB_BUF_SIZE; //check how much we need to copy
    unsigned char* dest = (unsigned char*) buf; //set system buffer as dest

    while (i < nbytes){
        putc(dest[i]); //write buffer to terminal 

        if (text_file_flag == 1 && dest[i] == '\0') return (i+1); //stop if we encounter null or new line
        i++;
    }
    return i;
}

// Does nothing - just used to fill in the FOTP
int32_t terminal_empty() {
    return -1;
}
