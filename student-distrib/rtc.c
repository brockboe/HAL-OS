#include "lib.h"
#include "rtc.h"
#include "i8259.h"
#include "keyboard.h"

#define REGISTER_A          0x8A
#define REGISTER_B          0x8B
#define REGISTER_C          0x8C
#define REG_NUM_PORT        0x70
#define REG_CMOS            0x71
#define BIT_6_MASK          0x40
#define RTC_IRQ_ON_MASTER   0x08

volatile unsigned int rtc_count = 0;

/* Function to initialize the rtc */
void rtc_init(void) {
    /* Declare local variables */
    unsigned char curr_reg_b_val;
    unsigned char reg_b_bit_6;
    /* Disable interrupts to set registers */
    cli();

    /* Disable NMI for Register A and B */
    outb(REGISTER_A, REG_NUM_PORT);
    outb(REGISTER_B, REG_NUM_PORT);

    /* Get current value of Register B and reset */
    curr_reg_b_val = inb(REG_CMOS);
    outb(REGISTER_B, REG_NUM_PORT);

    /* Activate Bit 6 of Register B */
    reg_b_bit_6 = curr_reg_b_val | BIT_6_MASK;
    outb(reg_b_bit_6, REG_CMOS);

    /* Once initialized, enable the IRQ line for the RTC */
    enable_irq(RTC_IRQ_ON_MASTER);

    /* Re-enable interrupts */
    sti();
}

/* Function that handles rtc-generated interrupts */
void rtc_interrupt_handler(void) {
  /* Disable all interrupts */
  cli();
  /* send E0I on RTC line */
  send_eoi(RTC_IRQ_ON_MASTER);

  rtc_count++;

  /* Make sure register C is not read again after IRQ8,
   * otherwise other interrupts will not occur.
   */
  outb(REGISTER_C, REG_NUM_PORT);
  inb(REG_CMOS);

  /* Re-enable interrupts */
  sti();
}
