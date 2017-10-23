#ifndef _RTC_H
#define _RTC_H

#include "types.h"

/*Magic numbers*/
#define NMI_MASK 0x80
#define REG_A 0x0A
#define REG_B 0x0B
#define REG_C 0x0C
#define RTC_REG 0x70
#define RTC_IRQ_ADDR 8
#define BIT_6_SWITCH 0x40
#define UPPER_MASK 0xF0
#define MAX_RATE 15

/*forward declarations*/
void rtc_init(void);
void rtc_handler(void);

#endif
