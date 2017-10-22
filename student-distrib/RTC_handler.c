#include "lib.h"
#include "RTC_handler.h"
#include "i8259.h"
#include "IDT.h"

/* source from http://wiki.osdev.org/RTC
 * Inputs: void
 * Return Value: void
 * Function: Initialize real time clock and add RTC to IDT*/
void rtc_init(void){
  char previous;
  outb(NMI_MASK+REG_B, RTC_REG); //select status register B and disable interuppts using x80.
  previous = inb(RTC_REG+1); //	  read immediately after or the RTC may be left in an unknown state.
  outb(NMI_MASK+REG_B, RTC_REG);
  outb(previous | BIT_6_SWITCH, RTC_REG+1);  // write the previous value ORed with 0x40. This turns on bit 6 of register B
  enable_irq(RTC_IRQ_ADDR); //enable 8th IRQ
  set_IDT_wrapper(SOFT_INT_START + RTC_IRQ_ADDR, rtc_handler);
  outb(REG_B, RTC_REG);  //enable 80 bit NMI
}

/* Inputs: void
 * Return Value: void
 * Function: Handles RTC interrupts by call test_interrupts and sending EOI signal to PIC */
void rtc_handler(void) {
  // printf("Calling RTC handler\n");
  test_interrupts(); //we must prove this function is being called.
  send_eoi(RTC_IRQ_ADDR); //end 8th IRQ
  outb(REG_C, RTC_REG);	// select register C
  inb(RTC_REG+1);		// just throw away contents, we must do this otherwise IRQ8 will never be called again.
}
