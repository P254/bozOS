/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"
//#include <linux/delay.h>
// do we need to include sys/io.h to get outb ?

// http://minirighi.sourceforge.net/html/group__KInterrupt.html

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask = 0xFF; /* IRQs 0-7  */
uint8_t slave_mask = 0xFF;  /* IRQs 8-15 */

uint8_t slave_number;

//pthread_spinlock_t i8259A_lock = SPIN_LOCK_UNLOCKED;

/* Initialize the 8259 PIC */
void i8259_init(void) {

  unsigned long flags;
  int master_counter;

//  spinlock_irqsave(&i8259A_lock, flags); //WHT?

  outb(0xFF, MASTER_8259_PORT + 1); //mask all irqs
  outb(0xFF, SLAVE_8259_PORT + 1);

  outb(ICW1, MASTER_8259_PORT); //write all ICWs for master
  outb(ICW2_MASTER + 0, MASTER_8259_PORT + 1);
  outb(ICW3_MASTER, MASTER_8259_PORT + 1);
  outb(ICW4 , MASTER_8259_PORT + 1);


  outb(ICW1 , SLAVE_8259_PORT); //write all ICWs for slave
  outb(ICW2_SLAVE , SLAVE_8259_PORT + 1);
  outb(ICW3_SLAVE , SLAVE_8259_PORT + 1);
  outb(ICW4 , SLAVE_8259_PORT + 1);

 /* udelay(100);*/

  outb(master_mask, MASTER_8259_PORT + 1); //restore current IRQs
  outb(slave_mask, SLAVE_8259_PORT + 1);


  master_counter = 0;
  slave_number = 0;
  // while(!(master_counter & ICW3_MASTER)){
  //   slave_number++;
  //   master_counter<<= 1;
  // }
  slave_number = ICW3_SLAVE;

  master_mask &= ~(1 << slave_number);
  outb(master_mask, MASTER_8259_PORT + 1); //unmask slave port on master

  //spin_lock_irqrestore(&i8259A_lock, flags);//WHT?


}

/* Enable (unmask) the specified IRQ */
/*derived from http://wiki.osdev.org/8259_PIC Masking code*/
void enable_irq(uint32_t irq_num) {
  uint8_t mask;
  uint16_t data_port;
  if(irq_num>15) return; //invalid IRQ, return
  if(irq_num<0) return;

  if(irq_num<8){ //If master IRQ
      data_port = 0x21;
      master_mask &= ~(1 << irq_num); //Set IRQ num position of master masks to 0
      mask = master_mask;
  }

  else{
        data_port = 0xA1;
        slave_mask &= ~(1 << (irq_num-8)); //Set IRQ num position of slave masks to 0
        mask = slave_mask;
  }

  outb(mask, data_port);

return;

}

/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num) {
  uint8_t mask;
  uint16_t data_port;
  if(irq_num>15) return; //invalid IRQ, return
  if(irq_num<0) return;

  if(irq_num<8){ //If master IRQ
      data_port = 0x21;
      master_mask |= (1 << irq_num); //Set IRQ num position of master masks to 0
      mask = master_mask;
  }

  else{
        data_port = 0xA1;
        slave_mask |= (1 << (irq_num-8)); //Set IRQ num position of slave masks to 0
        mask = slave_mask;
  }

  outb(mask, data_port);

return;

}

/* Send end-of-interrupt signal for the specified IRQ
http://wiki.osdev.org/8259_PIC
If the IRQ came from the Master PIC, it is sufficient to issue this command only to the Master PIC;
however if the IRQ came from the Slave PIC, it is necessary to issue the command to both PIC chips.*/
void send_eoi(uint32_t irq_num) {
  uint8_t mask;
  if(irq_num>15) return;  //invalid IRQ, return;
  if(irq_num<0) return;

  if(irq_num<8){
      outb(EOI | irq_num , MASTER_8259_PORT); // OR irq number with EOI and write
  }

  else{
      outb(EOI | slave_number , MASTER_8259_PORT); //if slave, eoi to master bit that slave is on
      outb(EOI | (irq_num-8) , SLAVE_8259_PORT); //and to slave itself.

  }
}
