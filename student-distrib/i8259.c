/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */
#include "i8259.h"
#include "lib.h"

// http://minirighi.sourceforge.net/html/group__KInterrupt.html

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask = MASK; /* IRQs 0-7  */
uint8_t slave_mask = MASK;  /* IRQs 8-15 */

uint8_t slave_number;

//pthread_spinlock_t i8259A_lock = SPIN_LOCK_UNLOCKED;

/* source: http://wiki.osdev.org/8259_PIC
* Inputs: void
* Return Value: void
* Function: Initialize PICs by setting 4 ICWS on both master and slave PIC*/
void i8259_init(void) {

//  spinlock_irqsave(&i8259A_lock, flags); //WHT?

  outb(MASK, MASTER_8259_PORT + 1); //mask all irqs
  outb(MASK, SLAVE_8259_PORT + 1);

  outb(ICW1, MASTER_8259_PORT); //write all ICWs for master
  outb(ICW2_MASTER + 0, MASTER_8259_PORT + 1);
  outb(ICW3_MASTER, MASTER_8259_PORT + 1);
  outb(ICW4 , MASTER_8259_PORT + 1);

  outb(ICW1 , SLAVE_8259_PORT); //write all ICWs for slave
  outb(ICW2_SLAVE , SLAVE_8259_PORT + 1);
  outb(ICW3_SLAVE , SLAVE_8259_PORT + 1);
  outb(ICW4 , SLAVE_8259_PORT + 1);

  outb(master_mask, MASTER_8259_PORT + 1); //restore current IRQs
  outb(slave_mask, SLAVE_8259_PORT + 1);

  slave_number = ICW3_SLAVE;

  master_mask &= ~(1 << slave_number);
  outb(master_mask, MASTER_8259_PORT + 1); //unmask slave port on master

  //spin_lock_irqrestore(&i8259A_lock, flags);//WHT?

}

/* source: http://wiki.osdev.org/8259_PIC
* Inputs: IRQ line to enable
* Return Value: void
* Function: Check if IRQ line belongs to master or slave and unmask that line on DATA PORT*/
void enable_irq(uint32_t irq_num) {

  uint8_t mask;
  uint16_t data_port;
  if(irq_num>MAX_IRQ) return; //invalid IRQ, return
  if(irq_num<MIN_IRQ) return;

  if(irq_num<MAX_MASTER_IRQ){ //If master IRQ
      data_port =  MASTER_8259_PORT + 1;
      master_mask &= ~(1 << irq_num); //Set IRQ num position of master masks to 0
      mask = master_mask;
  }

  else{
        data_port = SLAVE_8259_PORT + 1;
        slave_mask &= ~(1 << (irq_num-MAX_MASTER_IRQ)); //Set IRQ num position of slave masks to 0
        mask = slave_mask;
  }

  outb(mask, data_port);
  return;
}

/* source: http://wiki.osdev.org/8259_PIC
* Inputs: IRQ line to disable
* Return Value: void
* Function: Check if IRQ line belongs to master or slave and mask that line on DATA PORT*/
void disable_irq(uint32_t irq_num) {

  uint8_t mask;
  uint16_t data_port;
  if(irq_num>MAX_IRQ) return; //invalid IRQ, return
  if(irq_num<MIN_IRQ) return;

  if(irq_num<MAX_MASTER_IRQ){ //If master IRQ
      data_port = MASTER_8259_PORT + 1;;
      master_mask |= (1 << irq_num); //Set IRQ num position of master masks to 0
      mask = master_mask;
  }

  else{
        data_port =  SLAVE_8259_PORT + 1;;
        slave_mask |= (1 << (irq_num-MAX_MASTER_IRQ)); //Set IRQ num position of slave masks to 0
        mask = slave_mask;
  }

  outb(mask, data_port);
  return;
}

/* source: http://wiki.osdev.org/8259_PIC
U If the IRQ came from the Master PIC, it is sufficient to issue this command only to the Master PIC;
* however if the IRQ came from the Slave PIC, it is necessary to issue the command to both PIC chips.
* Inputs: IRQ line to send EOI to
* Return Value: void
* Function: Check if IRQ line belongs to master or slave and send EOI of that line to correct PORT*/
void send_eoi(uint32_t irq_num) {

  if(irq_num>MAX_IRQ) return;  //invalid IRQ, return;
  if(irq_num<MIN_IRQ) return;

  if(irq_num<MAX_MASTER_IRQ){
      outb(EOI | irq_num , MASTER_8259_PORT); // OR irq number with EOI and write
  }

  else{
      outb(EOI | slave_number , MASTER_8259_PORT); //if slave, eoi to master bit that slave is on
      outb(EOI | (irq_num-MAX_MASTER_IRQ) , SLAVE_8259_PORT); //and to slave itself.
  }

  return;
}
