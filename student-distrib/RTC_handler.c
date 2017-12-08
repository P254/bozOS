#include "lib.h"
#include "RTC_handler.h"
#include "i8259.h"
#include "IDT.h"
#include "tests.h"
#include "scheduling.h"
#include "multi_term.h"

volatile int rtc_count = 0;
volatile int interrupt_flag[3];

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
    outb(NMI_MASK+REG_B, RTC_REG); //select status register B and disable interuppts using x80.
    previous = inb(RTC_REG+1); //read immediately after or the RTC may be left in an unknown state.
    outb(NMI_MASK+REG_B, RTC_REG);
    outb(previous | BIT_6_SWITCH, RTC_REG+1);  // write the previous value ORed with 0x40. This turns on bit 6 of register B
    enable_irq(RTC_IRQ_ADDR); //enable 8th IRQ
    set_IDT_wrapper(SOFT_INT_START + RTC_IRQ_ADDR, rtc_handler_asm);
    outb(REG_B, RTC_REG);  //enable 8 bit NMI
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
    //  test_interrupts(); // we must prove this function is being called.
    #if (RTC_TEST_ENABLE == 1)
    printf("1");
    rtc_count++;
    #endif

    interrupt_flag[0] = 1;     // set the interrupt flag
    interrupt_flag[1] = 1;     // set the interrupt flag
    interrupt_flag[2] = 1;     // set the interrupt flag
    send_eoi(RTC_IRQ_ADDR); // end 8th IRQ
    outb(REG_C, RTC_REG);   // select register C
    inb(RTC_REG + 1);       // just throw away contents, we must do this otherwise IRQ8 will never be called again.
}

/*
 * rtc_read
 *   DESCRIPTION: Blocks RTC interuppts until next one arrives
 *   INPUTS: file descriptor, buffer, bytes to write
 *   OUTPUTS: none
 *   RETURN VALUE: 0
 *   SIDE EFFECTS: none
 */
int32_t rtc_read (int32_t fd, void* buf, int32_t nbytes){
    uint8_t curr_task= get_active_task();
    while (interrupt_flag[curr_task] == 0);    // Wait (block) for flag to be set by rtc_handler
    interrupt_flag[curr_task] = 0;             // Reset the flag
    return 0;
}

/*
 * rtc_write
 *   DESCRIPTION: converts rate to HZ and writes it to RTC device
 *   INPUTS: file descriptor, buffer containing rate, bytes to write
 *   OUTPUTS: changed rate of RTC
 *   RETURN VALUE: 0 or -1 if invalid rate
 *   SIDE EFFECTS: none
 */
int32_t rtc_write (int32_t fd, const void* buf, int32_t nbytes) {
    char previous;
    unsigned int rate, input_rate;

    if(buf == NULL) return -1;  // make sure input buffer is valid

    input_rate= (int)(*(int*)buf);

    switch (input_rate) {
        case IN_RATE_2:
            rate = OUT_RATE_2;
            break;
        case IN_RATE_4:
            rate = OUT_RATE_4;
            break;
        case IN_RATE_8:
            rate = OUT_RATE_8;
            break;
        case IN_RATE_16:
            rate = OUT_RATE_16;
            break;
        case IN_RATE_32:
            rate = OUT_RATE_32;
            break;
        case IN_RATE_64:
            rate = OUT_RATE_64;
            break;
        case IN_RATE_128:
            rate = OUT_RATE_128;
            break;
        case IN_RATE_256:
            rate = OUT_RATE_256;
            break;
        case IN_RATE_512:
            rate = OUT_RATE_512;
            break;
        default:
            return -1;
    }
    outb(NMI_MASK+REG_A, RTC_REG); // remask NMI
    previous = inb(RTC_REG+1); // read immediately after or the RTC may be left in an unknown state.
    outb(NMI_MASK+REG_A, RTC_REG);
    outb((previous & UPPER_MASK) | rate, RTC_REG+1);// write only our rate to A. Rate is the bottom 4 bits.
    outb(REG_A, RTC_REG);  // enable 80 bit NMI
    return 0;
}

/*
 * rtc_open
 *   DESCRIPTION: opens RTC device, sets RTC to intial rate.
 *   INPUTS: file descriptor
 *   OUTPUTS: 0
 *   RETURN VALUE: void
 *   SIDE EFFECTS: none
 */
int32_t rtc_open (const uint8_t* filename){
    unsigned int rate;
    rate = INIT_RATE;
    rtc_write(NULL, &rate, 0);
    return 0; //success
}

/*
 * rtc_close
 *   DESCRIPTION: closes RTC device
 *   INPUTS: file descriptor
 *   OUTPUTS: 0
 *   RETURN VALUE: void
 *   SIDE EFFECTS: none
 */
int32_t rtc_close (int32_t fd){
    return 0;
}
