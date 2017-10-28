#ifndef _TERMINAL_H
#define _TERMINAL_H

int terminal_read(int fd, unsigned char* buf, int nbytes);
int terminal_write(int fd, const unsigned char* buf, int nbytes);
int terminal_open();
int terminal_close();


#endif
