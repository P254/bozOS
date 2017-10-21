http://wiki.osdev.org/PS/2_Keyboard
void init_kb(void){
    enable_irq(/*  */); // the keyboard interrupt
}

void


char getScancode() {
    char c = 0 ;
    do {
        if (inb(0x60) != c ) {
            c = inb(0x60);
            if (c>0) return c;
        }
    } while(1);
}

char getchar() {
    return scancode[getScancode() + 1];
}
