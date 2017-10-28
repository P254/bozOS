
#include "keyboard.h"
#include "terminal.h"
#include "x86_desc.h"
#include "lib.h"


int terminal_open(){
    return 0;
}

int terminal_close(){
    return 0;
}

int terminal_read(int fd, unsigned char* buf, int nbytes){
   int j=0;
   while(!kb_read_release());
   unsigned char* kb_buff = get_kb_buffer();
   while(kb_buff[j]!='\n'){
       buf[j] = kb_buff[j];
       j++;
   }
   int i=0;
   for(i=0 ; i<j ; i++){
       kb_buff[i]='\0';
       kb_buff[i] = kb_buff[i+1];
   }
   return j;

}

int terminal_write(int fd, const unsigned char* buf, int nbytes)
{
    int i;
    i=0;
    while(buf[i]!='\n'){
        putc(buf[i]);
        i++;
    }
    putc('\n');
    putc('_');

    return nbytes;
}
