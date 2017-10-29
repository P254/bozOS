#include "lib.h"
#include "RTC_handler.h"
#include "i8259.h"
#include "IDT.h"

/*
 * rtc_init
 *   DESCRIPTION: Enables periodic interrupts and changes frequency of RTC. Also writes RTC handler into IDT table.
 *   INPUTS: void
 *   OUTPUTS: value of RTC_REGs are changed.
 *   RETURN VALUE: void
 *   SIDE EFFECTS: none
 */
void rtc_init(void){
    char previous;
    unsigned int rate;
    rate = MAX_RATE; //set rate to 15 or 1HZ. 32768 >> (rate-1);

    outb(NMI_MASK+REG_B, RTC_REG); //select status register B and disable interuppts using x80.
    previous = inb(RTC_REG+1); //read immediately after or the RTC may be left in an unknown state.
    outb(NMI_MASK+REG_B, RTC_REG);
    outb(previous | BIT_6_SWITCH, RTC_REG+1);  // write the previous value ORed with 0x40. This turns on bit 6 of register B
    enable_irq(RTC_IRQ_ADDR); //enable 8th IRQ
    set_IDT_wrapper(SOFT_INT_START + RTC_IRQ_ADDR, rtc_handler);

    outb(REG_B, RTC_REG);  //enable 80 bit NMI
    outb(NMI_MASK+REG_A, RTC_REG); //remask NMI
    previous = inb(RTC_REG+1); //read immediately after or the RTC may be left in an unknown state.
    outb(NMI_MASK+REG_A, RTC_REG);
    outb((previous & UPPER_MASK) | rate, RTC_REG+1);////write only our rate to A. Rate is the bottom 4 bits.
    outb(REG_A, RTC_REG);  //enable 80 bit NMI
}

/*
 * rtc_handler
 *   DESCRIPTION: sends EOI to IRQ line 8 as well as call test_interrupts for CP1.
 *   INPUTS: void
 *   OUTPUTS: test_interrupts shifts video memory.
 *   RETURN VALUE: void
 *   SIDE EFFECTS: none
 */
void rtc_handler(void) {
    test_interrupts(); //we must prove this function is being called.
    send_eoi(RTC_IRQ_ADDR); //end 8th IRQ
    outb(REG_C, RTC_REG); // select register C
    inb(RTC_REG+1); // just throw away contents, we must do this otherwise IRQ8 will never be called again.
    while(1);
}
