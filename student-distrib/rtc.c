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

/* Function that opens the RTC */
int32_t rtc_open() {
    /* if rtc hasn't been initialized, we want to initialize it */
    if(!rtc_init_check)
    {
        rtc_init();
    }
    /* Set frequency of RTC to 2 */
    char two_hertz = HERTZ_2;

    /* Save reg A value */
    outb(REGISTER_A, REG_NUM_PORT);

    /* Set frequency to 2Hz */
    two_hertz = two_hertz | (FREQ_MASK & inb(REG_CMOS));
    outb(two_hertz, REG_CMOS);

    return 0;
}

/* Function that writes to the RTC */
int32_t rtc_write(const void * buf, int32_t nbytes) {
    /* Variabl that will hold desired frequency from buf */
    uint32_t frequency;
    uint32_t rate;

    /* If nbytes is 4 (which it has to be), and the buf isn't empty,
     * We can set the frequency to the new desired value
     */
    if(nbytes == 4 && (uint32_t)buf != NULL)
    {   /* Extract desired frequency from buf */
        frequency = *(uint32_t*)(buf);

        /* Set appropriate based on frequency, we use
         * a switch statement to find proper rate
         */
        switch(frequency) {
            case 128:
                rate = 0x09;
                break;

            case 64:
                rate = 0x0A;
                break;

            case 32:
                rate = 0x0B;
                break;

            case 16:
                rate = 0x0C;
                break;

            case 8:
                rate = 0x0D;
                break;

            case 4:
                rate = 0x0E;
                break;

            case 2:
                rate = 0x0F;
                break;

            default:
                return -EINVAL;
        }

        /* Write new refresh rate to proper port */
        rate = rate | (FREQ_MASK & inb(REG_CMOS));
        /* Save reg A value */
        outb(REGISTER_A, REG_NUM_PORT);
        outb(rate, REG_CMOS);
    }

    else
    {
        /* Return -1 to indicate write failed */
        return -EINVAL;
    }

    return nbytes;
}

/* Function that reads the RTC */
int32_t rtc_read() {
    /* Spin while we wait for interrupts to get disabled */
    while(!rtc_interrupt_flag) {}

    /* When interrupt is completed, set to 0 */
    rtc_interrupt_flag = 0;

    return 0;
}

/* Function that closes the RTC */
int32_t rtc_close() {
    return 0;
}

int32_t rtc_io(uint32_t action, uint32_t inode_index, uint32_t offset, uint8_t * buf, uint32_t nbytes){
      switch(action){
            case 0:
                  return rtc_read();
            case 1:
                  return rtc_write(buf, nbytes);
            default:
                  return 1;
      }
      return 1;
}
