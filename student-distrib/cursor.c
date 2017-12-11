#include "cursor.h"
#include "types.h"
#include "lib.h"

/*
 * cursor_init
 *   DESCRIPTION: Initializes the cursor
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: void
 *   SIDE EFFECTS: none
 */
void cursor_init() {
    outb(CURSOR_INIT_A, VGA_CMD_PORT);  
	outb((inb(VGA_DATA_PORT) & CURSOR_INIT_C) | CURSOR_START, VGA_DATA_PORT);
 
	outb(CURSOR_INIT_B, VGA_CMD_PORT);
	outb((inb(CURSOR_INIT_3E) & CURSOR_INIT_E) | CURSOR_END, VGA_DATA_PORT);
}


/*
 * update_cursor
 *   DESCRIPTION: Updates the cursor position based on x,y. If the position is out of range, the cursor won't appear onscreen.
 *   INPUTS: x -- x position, y -- y position
 *   OUTPUTS: none
 *   RETURN VALUE: void
 *   SIDE EFFECTS: 
 */
void update_cursor(int x, int y) {
    uint16_t pos = y * NUM_COLS + x;
    
    outb(CURSOR_LOW, VGA_CMD_PORT); // Cursor low
	outb((uint8_t) (pos & MASK_8_BIT), VGA_DATA_PORT);
	outb(CURSOR_HIGH, VGA_CMD_PORT); // Cursor high
	outb((uint8_t) ((pos >> SHIFT_8) & MASK_8_BIT), VGA_DATA_PORT);
}
