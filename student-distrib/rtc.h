/* rtc.h: Header file for all rtc functions */
#ifndef _RTC_H
#define _RTC_H

#define REGISTER_A          0x8A
#define REGISTER_B          0x8B
#define REGISTER_C          0x8C
#define REG_NUM_PORT        0x70
#define REG_CMOS            0x71
#define BIT_6_MASK          0x40
#define RTC_IRQ_ON_MASTER   0x08

/* Function to initialize the rtc */
void rtc_init(void);

/* Function that handles rtc-generated interrupts */
void rtc_interrupt_handler(void);

#endif  /* _RTC_H */
