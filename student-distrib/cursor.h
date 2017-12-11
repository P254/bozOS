#ifndef CURSOR_H
#define CURSOR_H

/*Magic Numbers*/
#define CURSOR_START    0
#define CURSOR_END      25

#define VGA_CMD_PORT    0x3D4
#define VGA_DATA_PORT   0x3D5

#define CURSOR_INIT_A 	0x0A
#define CURSOR_INIT_B 	0x0B
#define CURSOR_INIT_C 	0xC0
#define CURSOR_INIT_E 	0xE0
#define CURSOR_INIT_3E 	0x03E0

#define CURSOR_LOW		0x0F
#define CURSOR_HIGH		0x0E

#define MASK_8_BIT		0xFF
#define SHIFT_8			8

/*Forward declarations*/
void cursor_init();
void update_cursor(int x, int y);

#endif /*CURSOR_H*/
