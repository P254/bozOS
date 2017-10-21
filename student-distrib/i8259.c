/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask = 0xFF; /* IRQs 0-7  */
uint8_t slave_mask = 0xFF;  /* IRQs 8-15 */

pthread_spinlock_t i8259A_lock = SPIN_LOCK_UNLOCKED;

/* Initialize the 8259 PIC */
void i8259_init(void) {

  unsigned long flags;

  spinlock_irqsave(&i8259A_lock, flags); //WHT?

  outb(0xFF, MASTER_8259_PORT + 1); //mask all irqs
  outb(0xFF, SLAVE_8259_PORT + 1);

  outb_p(ICW1, MASTER_8259_PORT); //write all ICWs for master
  outb_p(ICW2_MASTER + 0, MASTER_8259_PORT + 1);
  outb_p(ICW3_MASTER, MASTER_8259_PORT + 1);
  outb_p(ICW4 , MASTER_8259_PORT + 1);


  outb_p(ICW1 , SLAVE_8259_PORT); //write all ICWs for slave
  outb_p(ICW2_SLAVE , SLAVE_8259_PORT + 1);
  outb_p(ICW3_SLAVE , SLAVE_8259_PORT + 1);
  outb_p(ICW4 , SLAVE_8259_PORT + 1);

  udelay(100);

  outb(master_mask, MASTER_8259_PORT + 1); //restore current IRQs
  outb(slave_mask, SLAVE_8259_PORT + 1);

  spin_lock_irqrestore(&i8259A_lock, flags);//WHT?


}

/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num) {
  uint8_t mask;
  if(irq_num>15) return; //invalid IRQ, return
  if(irq_num<0) return;

  if(irq_num<8){ //If master IRQ
      master_mask &= ~(1 << irq_num); //Set IRQ num position of master masks to 0
      outb_p(master_mask, MASTER_8259_PORT + 1);
  }

  else{
        slave_mask &= ~(1 << (irq_num-8)); //Set IRQ num position of slave masks to 0
        outb_p(slave_mask, SLAVE_8259_PORT + 1)
  }

return;

}

/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num) {
  uint8_t mask
  if(irq_num>15) return; //invalid IRQ, return;
  if(irq_num<0) return;

  if(irq_num<8){
      mask = master_mask;
      mask |= (1 << irq_num);  //Set IRQ num position of master masks to 1
      outb_p(master_mask, MASTER_8259_PORT + 1)
  }

  else{
        slave_mask |= (1 << (irq_num-8)); //Set IRQ num position of slave masks to 1
        outb_p(slave_mask, SLAVE_8259_PORT + 1)
  }
}

/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num) {
  uint8_t mask;
  if(irq_num>15) return;  //invalid IRQ, return;
  if(irq_num<0) return;

  if(irq_num<8){
      outb_p(EOI | irq_num , MASTER_8259_PORT); // OR irq number with EOI and write
  }

  else{
      outb_p(EOI | (irq_num-8) , SLAVE_8259_PORT);
  }
}
