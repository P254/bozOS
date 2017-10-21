/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"
//#include <linux/delay.h>
// do we need to include sys/io.h to get outb ?

// http://minirighi.sourceforge.net/html/group__KInterrupt.html

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7  */
uint8_t slave_mask;  /* IRQs 8-15 */

/* Initialize the 8259 PIC */
void i8259_init(void) {
    /* add a spinlock over here */

    outb(0xff , MASTER_8259_PORT ); /* mask all of MASTER_8259_PORT */
    outb(0xff , SLAVE_8259_PORT ); /* mask all of SLAVE_8259_PORT */

    outb( ICW1 , MASTER_8259_PORT); // select 8259A-1 init
    outb( ICW2_MASTER , MASTER_8259_PORT + 1 ); // 8259A-1 IR0-7 mappend to 0x20-0x27
    outb( ICW3_MASTER , MASTER_8259_PORT + 1 ); // 8259A-1 has a slave on IR2
    outb( ICW4 , MASTER_8259_PORT + 1 ); //

    outb( ICW1 , SLAVE_8259_PORT); // select 8259A-2 init
    outb( ICW2_SLAVE , SLAVE_8259_PORT + 1 ); // 8259A-2 IR0-7 mapped to 0x28 - 0x2f
    outb( ICW3_SLAVE , SLAVE_8259_PORT + 1 ); // 8259A-2 is a slave on master's IR2
    outb( ICW4 , SLAVE_8259_PORT + 1 );

    //delay to wait for the 8259A to Initialize
    udelay(1000);

    /* spinklock unlock over here? */

}

/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num) {
    uint32_t mask;
    if (irq_num > 15 || irq_num < 0) return ;
    if (irq_num < 8 ){
        mask = inb(ICW1);
        mask &= ~(1 << irq_num); // is it ~
        outb(mask,MASTER_8259_PORT);
    }
    else {
        mask = inb(ICW2_SLAVE);
        mask &= ~(1 << (irq_num - 8 )); // is it ~
        outb(mask,SLAVE_8259_PORT);
    }
}

/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num) {
    uint32_t mask; // maybe we could set mask = 0x1 . not sure how to use inb
    if (irq_num > 15 || irq_num < 0 ) return;
    if (irq_num < 8) {
        mask = inb(ICW1); // ?
        mask |= 1 << irq_num;
        /*outb(ICW1,mask);*/
        outb(mask, MASTER_8259_PORT);
    }
    else {
        mask = inb(ICW2);
        mask |= 1 << (irq_num - 8);
        // outb(ICW2,mask);
        outb(mask,SLAVE_8259_PORT);
    }
}

/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num) {
    if (irq_num > 8 ){
            outb(EOI , SLAVE_8259_PORT);
        outb(EOI , MASTER_8259_PORT );
    }
}
