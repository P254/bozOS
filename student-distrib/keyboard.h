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

#define VIDEO       0xB8000
#define NUM_COLS    80
#define NUM_ROWS    25

/* Forward Declarations */
void kb_init(void);
unsigned int getScanCode(void);
void kb_int_handler(void);
void addCharToBuf(unsigned char c);
void delCharFrBuf(void);
int convertToVidIdx(int x, int y, int buf_len);
