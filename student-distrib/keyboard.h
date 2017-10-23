/* http://www.osdever.net/bkerndev/Docs/keyboard.htm
*http://minirighi.sourceforge.net/html/keyboard_8c.html
* http://wiki.osdev.org/%228042%22_PS/2_Controller
*
*/

/*Magic Numbers*/
#define KEYBOARD_PORT 0x60

/*Forward Declarations*/
void kb_init(void);
char getScancode(void);
void get_char(void);

extern void keyboard_handler();
