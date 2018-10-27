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

volatile unsigned int rtc_count;

/* Function to initialize the rtc */
void rtc_init(void);

/* Function that handles rtc-generated interrupts */
void rtc_interrupt_handler(void);

/* Function that opens the RTC */
int32_t rtc_open();

/* Function that writes to the RTC */
int32_t rtc_write(const void * buf, int32_t nbytes);

/* Function that reads the RTC */
int32_t rtc_read();

/* Function that closes the RTC */
int32_t rtc_close();
#endif  /* _RTC_H */
