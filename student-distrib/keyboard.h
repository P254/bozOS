/* Keyboard / Terminal Driver
 * Sources:
 * 1. http://www.osdever.net/bkerndev/Docs/keyboard.htm
 * 2. http://minirighi.sourceforge.net/html/keyboard_8c.html
 * 3. http://wiki.osdev.org/%228042%22_PS/2_Controller
 * 4. http://wiki.osdev.org/PS/2_Keyboard
 */

/* Magic Numbers */
#define KB_DATA_PORT 0x60
#define KB_CONTROL_REG 0x64
#define KB_SIZE 128
#define KB_IRQ 1
#define KB_IDT_ENTRY (SOFT_INT_START + 1)
#define ENTER_KEY_IDX 28

#define VIDEO       0xB8000
#define NUM_COLS    80
#define NUM_ROWS    25

/* Forward Declarations */
extern void kb_init(void);
extern unsigned int getScanCode(void);
extern void keyboard_handler_asm(void);
extern void kb_int_handler(void);
extern void addCharToBuf(unsigned char c);
extern void delCharFrBuf(void);
extern int convertToVidIdx(int x, int y, int buf_len);
extern int* kb_read_release();
extern unsigned char* get_kb_buffer();
extern void copy_kb_buff();
