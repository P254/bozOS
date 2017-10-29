#include "keyboard.h"
#include "terminal.h"
#include "x86_desc.h"
#include "lib.h"

int32_t terminal_open(const uint8_t* filename) {
    return 0;
}

int32_t terminal_close(int32_t fd) {
    return 0;
}

int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes) {
    unsigned char* source = get_kb_buffer();
    unsigned char* dest = (unsigned char*) buf;

    int32_t i, bytes_copied, count=0;
    int* enter_flag = kb_read_release();

    while(!(*enter_flag));

    for (i = 0; i < nbytes; i++) {
        dest[i] = source[i];
        if (source[i] == '\n') break;
        else source[i] = '\0'; // Flush-as-you-go
    }

    *enter_flag = 0; // Reset the keyboard flag
    bytes_copied = i;
    i++;

    // Move the KB buffer 
    while (source[i]!='\0') {
        source[count] = source[i];
        source[i] = '\0';
        count++;
        i++;
    }

    return bytes_copied;
}

int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes) {
    int i = 0;
    unsigned char* dest = (unsigned char*) buf;
    while (i < nbytes){
        putc(dest[i]);
        if (dest[i] == '\n' || dest[i] == '\0') return (i+1);
        i++;
    }
    return i;
}
