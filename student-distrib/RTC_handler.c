#include "lib.h"
#include "RTC_handler.h"
#include "i8259.h"
#include "IDT.h"

volatile int count=0;

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
    rate = INIT_RATE; //set rate to 15 or 1HZ. 32768 >> (rate-1);

    outb(NMI_MASK+REG_B, RTC_REG); //select status register B and disable interuppts using x80.
    previous = inb(RTC_REG+1); //read immediately after or the RTC may be left in an unknown state.
    outb(NMI_MASK+REG_B, RTC_REG);
    outb(previous | BIT_6_SWITCH, RTC_REG+1);  // write the previous value ORed with 0x40. This turns on bit 6 of register B
    enable_irq(RTC_IRQ_ADDR); //enable 8th IRQ
    set_IDT_wrapper(SOFT_INT_START + RTC_IRQ_ADDR, rtc_handler);
    outb(REG_B, RTC_REG);  //enable 8 bit NMI

    //TODO: ask the TA if we need a CLI/STI thing
    // unsigned int flags;
    // cli_and_save(flags);
//    outb(NMI_MASK+REG_A, RTC_REG); //remask NMI //
//    previous = inb(RTC_REG+1); //read immediately after or the RTC may be left in an unknown state.
//    outb(NMI_MASK+REG_A, RTC_REG);
//    outb((previous & UPPER_MASK) | rate, RTC_REG+1);////write only our rate to A. Rate is the bottom 4 bits.
//    outb(REG_A, RTC_REG);  //enable 80 bit NMI
    // restore_flags(flags);
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
  //  test_interrupts(); //we must prove this function is being called.
  printf("1");
  count++;
   send_eoi(RTC_IRQ_ADDR); //end 8th IRQ
   outb(REG_C, RTC_REG); // select register C
    inb(RTC_REG+1); // just throw away contents, we must do this otherwise IRQ8 will never be called again.
    // while(1);
}

int32_t read (int32_t fd, void* buf, int32_t nbytes){
    while(interupt_flag!=0){
      //means interuppt_flag==1 so do nothing until it changes
    }
    interupt_flag=0;
    return 0;
}

int32_t write (int32_t fd, const void* buf, int32_t nbytes){

    char previous;
    unsigned int rate;
    unsigned int input_rate;

    if(buf==NULL)
        return -1;  // make sure input buffer is valid

    input_rate= (int)(*(int*)buf);

    switch(input_rate) {
      case 2:
        rate= 0x0F; break;
      case 4:
        rate= 0x0E; break;
      case 8:
        rate= 0x0D; break;
      case 16:
        rate= 0x0C; break;
      case 32:
        rate= 0x0B; break;
      case 64:
        rate= 0x0A; break;
      case 128:
        rate= 0x09; break;
      case 256:
        rate= 0x08; break;
      case 512:
        rate= 0x07; break;
      default:
        return -1;
    }
    // if(input_rate==2)
    // else if(input_rate==4)
    // else if(input_rate==8)
    // else if(input_rate==16)
    // else if(input_rate==32)
    // else if(input_rate==64)
    // else if(input_rate==128)
    // else if(input_rate==256)
    // else if(input_rate==512)
    // else if(input_rate==1024)
    // else if(input_rate>1024 || input_rate<2)

    //TODO: ask the TA if we need a CLI/STI thing
    // unsigned int flags;
    // cli_and_save(flags);
    outb(NMI_MASK+REG_A, RTC_REG); //remask NMI
    previous = inb(RTC_REG+1); //read immediately after or the RTC may be left in an unknown state.
    outb(NMI_MASK+REG_A, RTC_REG);
    outb((previous & UPPER_MASK) | rate, RTC_REG+1);////write only our rate to A. Rate is the bottom 4 bits.
    outb(REG_A, RTC_REG);  //enable 80 bit NMI
    // restore_flags(flags);
    return 0;
}

int ret_count(){
  return count;
}

void setRetCount(int setter){
    count = setter;
    return;
}
int32_t open (const int8_t* filename){
    // if(rtc_init_flag==0)
    //     rtc_init();
    // else
    //     return -1;
    // rtc_init_flag=1;
    unsigned int rate;
    rate= INIT_RATE;
    write(NULL, &rate, 4);
    return 0; //success
}

int32_t close (int32_t fd){
    // if(!rtc_init_flag){
    //     return -1;
    // }
    // rtc_init_flag= 0;
    // disable_irq(RTC_IRQ_ADDR); // TODO: disables RTC_IRQ_ADDR
    return 0;
}
