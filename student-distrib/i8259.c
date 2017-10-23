/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */
#include "i8259.h"
#include "lib.h"

// http://minirighi.sourceforge.net/html/group__KInterrupt.html

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask = MASK; /* IRQs 0-7  */
uint8_t slave_mask = MASK;  /* IRQs 8-15 */

uint8_t slave_number; //which port the slave is on the master

/*
 * i8259_init
 *   DESCRIPTION: Initializes the PIC by sending the 4 ICWs as well as unmasks slave IRQ on master PIC.
 *   INPUTS: void
 *   OUTPUTS: writes to master data port.
 *   RETURN VALUE: void
 *   SIDE EFFECTS: none
 */
void i8259_init(void) {
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

  return;
}

/*
 * enable_irq
 *   DESCRIPTION: unmasks IRQ line that is sent, either on slave or master.
 *   INPUTS: IRQ number to unmask
 *   OUTPUTS: writes to master or slave data port.
 *   RETURN VALUE: void
 *   SIDE EFFECTS: none
 */
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

/*
 * disable_irq
 *   DESCRIPTION: masks IRQ line that is sent, either on slave or master.
 *   INPUTS: IRQ number to mask
 *   OUTPUTS: writes to master or slave data port.
 *   RETURN VALUE: void
 *   SIDE EFFECTS: none
 */
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

/*
 * send_eoi
 *   DESCRIPTION: Sends EOI to either master or both slave and master depending on IRQ input.
 *   INPUTS: IRQ line number to send EOI.
 *   OUTPUTS: writes to master and/or slave port. 
 *   RETURN VALUE: void
 *   SIDE EFFECTS: none
 */
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
