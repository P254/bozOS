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
#define INIT_RATE 2

#define IN_RATE_2 2
#define IN_RATE_4 4
#define IN_RATE_8 8
#define IN_RATE_16 16
#define IN_RATE_32 32
#define IN_RATE_64 64
#define IN_RATE_128 128
#define IN_RATE_256 256
#define IN_RATE_512 512
 
#define OUT_RATE_2 0x0F
#define OUT_RATE_4 0x0E
#define OUT_RATE_8 0x0D
#define OUT_RATE_16 0x0C
#define OUT_RATE_32 0x0B
#define OUT_RATE_64 0x0A
#define OUT_RATE_128 0x09
#define OUT_RATE_256 0x08
#define OUT_RATE_512 0x07

/*forward declarations*/
void rtc_init(void);
void rtc_handler(void);

int32_t rtc_read (int32_t rtc_fd, void* buf, int32_t rtc_nbytes);
int32_t rtc_write (int32_t rtc_fd, const void* buf, int32_t rtc_nbytes);
int32_t rtc_open (const int8_t* filename);
int32_t rtc_close(int32_t rtc_fd);

#endif /* _RTC_H */
