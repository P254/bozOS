#include "lib.h"
#include "IDT.h"
#include "mouse.h"
#include "i8259.h"
#include "keyboard.h"
#include "multi_term.h"

// Source: https://houbysoft.com/download/ps2mouse.html
uint8_t mouse_cycle = 0;    
int8_t mouse_byte[MOUSE_BYTES];

void mouse_handler() {
    static uint8_t cycle = 0;
    static char mouse_bytes[MOUSE_N_BYTES];
    int mouse_x, mouse_y;
    mouse_bytes[cycle++] = inb(KB_DATA_PORT);

    /* The mouse sends three 1-byte packets:
     * Byte 1: [ Y overflow | X overflow | Y sign bit | X sign bit | 1 | Middle Btn | Right Btn | Left Btn ]
     * Byte 2: [ X movement (delta-X) ] -- range [-256 : +255], typically 1-2 for slow and 20 for fast movement
     * Byte 3: [ Y movement (delta-Y) ] -- range [-256 : +255], typically 1-2 for slow and 20 for fast movement
     */

    if (cycle == MOUSE_N_BYTES) { // if we have all the 3 bytes...
        cycle = 0; // reset the counter

        // do what you wish with the bytes, this is just a sample
        if ((mouse_bytes[0] & 0x80) || (mouse_bytes[0] & 0x40))
            return; // the mouse only sends information about overflowing, do not care about it and return
        if (!(mouse_bytes[0] & 0x20))
            // y |= 0xFFFFFF00; //delta-y is a negative value
            puts("Delta-y.\n");
        if (!(mouse_bytes[0] & 0x10))
            // x |= 0xFFFFFF00; //delta-x is a negative value
            puts("Delta-x.\n");

        // Middle button is pressed
        if (mouse_bytes[0] & 0x4) {
            switch_terminal(TERM_2);
        }
        // Right button is pressed
        if (mouse_bytes[0] & 0x2) {
            switch_terminal(TERM_3);
        }
        // Left button is pressed 
        if (mouse_bytes[0] & 0x1) {
            switch_terminal(TERM_1);
        }

        printf("(%d,%d)\n", mouse_bytes[1], mouse_bytes[2]);
        
        // to use the coordinate data, use mouse_bytes[1] for delta-x, and mouse_bytes[2] for delta-y
    }
    send_eoi(MOUSE_IRQ);
}

void mouse_wait(uint8_t type) {
    uint32_t time_out = TIMEOUT;
    if (type == 0) { // Data
        while(time_out--) {
            if ((inb(KB_CONTROL_REG) & 1) == 1) return;
        }
    return;
    }
    
    else if (type == 1) { // Signal
        while(time_out--) {
            if((inb(KB_CONTROL_REG) & 2) == 0) return;
        }
    return;
    }
}

void mouse_write(uint8_t cmd) {
    mouse_wait(1);      // Wait to be able to send a command
    outb(0xD4, KB_CONTROL_REG);   // Tell the mouse we are sending a command
    mouse_wait(1);      // Wait for the final part
    outb(cmd, KB_DATA_PORT);    // Finally write
}

uint8_t mouse_read() {
    // Get response from mouse
    mouse_wait(0); 
    return inb(KB_DATA_PORT);
}

// Enable the auxiliary mouse device
void mouse_init() {
    uint8_t status_byte;
    
    mouse_wait(1);
    outb(0xA8, KB_CONTROL_REG); 

    mouse_wait(1);
    outb(0x20, KB_CONTROL_REG);

    mouse_wait(0);
    status_byte = inb(KB_DATA_PORT) | 2;

    mouse_wait(1);
    outb(KB_DATA_PORT, 0x64);

    mouse_wait(1);
    outb(status_byte, KB_DATA_PORT);

    mouse_write(0xF6);
    mouse_read();

    mouse_write(0xF4);
    mouse_read();
    
    // Setup the mouse handler and eanble interrupts
    enable_irq(MOUSE_IRQ);
    set_IDT_wrapper(MOUSE_IDT_ENTRY , mouse_handler_asm);
}
