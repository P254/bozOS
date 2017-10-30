/* Keyboard Driver
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
#define KB_OPTIONS 4
#define SHIFT_EXTEND 2
#define CAPS_EXTEND 3
#define BOTH_EXTEND 4
#define BASE_KB_SIZE 90
#define SHIFT_FLAG 0x1
#define CAPS_FLAG 0x10
#define CTRL_FLAG 0x100
#define BOTH_FLAG 0x011
#define CLEAR_CTRL_FLAG 0x011
#define CLEAR_SHIFT_FLAG 0x110
#define EXPANDED_KB_SIZE (KB_SIZE * KB_OPTIONS)
#define KB_IRQ 1
#define KB_IDT_ENTRY (SOFT_INT_START + 1)
#define ENTER_KEY_IDX 28
#define RELEASED_KEY_MASK 0x80
#define SHIFT_RELEASE 0xAA
#define CTRL_RELEASE 0x9D
#define SHIFT_PRESSED 0x2A
#define CAPS_PRESSED 0x3A
#define CTRL_PRESSED 0x1D
#define L_PRESSED 0x26
#define ENTER_PRESSED 0x1C

#define VIDEO       0xB8000
#define NUM_COLS    80
#define NUM_ROWS    25

/* Forward Declarations */
void kb_init(void);
unsigned int get_scan_code(void);
void kb_int_handler(void);
void add_char_to_buf(unsigned char c);
void del_char_from_buf(void);
int convert_to_vid_idx(int x, int y, int buf_len);
int* kb_read_release();
unsigned char* get_kb_buffer();
void copy_kb_buf();
