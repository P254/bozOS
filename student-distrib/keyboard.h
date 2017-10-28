/* http://www.osdever.net/bkerndev/Docs/keyboard.htm
*http://minirighi.sourceforge.net/html/keyboard_8c.html
* http://wiki.osdev.org/%228042%22_PS/2_Controller
*
*/

#ifndef _KEYBOARD_H
#define _KEYBOARD_H
/*Magic Numbers*/
#define KEYBOARD_PORT 0x60
#define KB_SIZE 128
#define KB_IRQ 1

/*Forward Declarations*/
void kb_init(void);
void getScanCode(void);

extern void keyboard_handler_asm();
int terminal_read(int fd, unsigned char* buf, int nbytes);
int terminal_write(int fd, const unsigned char* buf, int nbytes);
int terminal_open(void);
int terminal_close(void);


#endif
