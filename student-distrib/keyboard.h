/* http://www.osdever.net/bkerndev/Docs/keyboard.htm
*http://minirighi.sourceforge.net/html/keyboard_8c.html
* http://wiki.osdev.org/%228042%22_PS/2_Controller
*
*/

/*Magic Numbers*/
#define KEYBOARD_PORT 0x60
#define KB_SIZE 128
#define KB_IRQ 1

/*Forward Declarations*/
void kb_init(void);
char getScanCode(void);
void get_char(void);

extern void keyboard_handler();
