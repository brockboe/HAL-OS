#include "lib.h"
#include "pit.h"
#include "i8259.h"
#include "keyboard.h"
#include "term_sched.h"

/* definition of different PIT ports */
#define PIT_REG       0x36
#define PIT_C0        0x40
#define PIT_C1        0x41
#define PIT_C2        0x42
#define PIT_MODE      0x43
#define BIT8_MASK     0xFF
#define BYTE_LENGTH   8
#define MAX_PIT_CLOCK 1193180

/* Initialization borrowed from here
 * http://www.jamesmolloy.co.uk/tutorial_html/5.-IRQs%20and%20the%20PIT.html
 */

/* Function to initialize the pit */
void pit_init(void)  {
  /* Variable of our desired frequemcy for PIT */
  uint32_t desiredFrequency = 100;  // 100Hz
  uint32_t dividedFrequency = MAX_PIT_CLOCK / desiredFrequency;

  /* We need to write the frequency as lower and upper byte
   * to make 16 bit frequency
   */
  uint8_t upper = (uint8_t) (dividedFrequency & BIT8_MASK);
  uint8_t lower = (uint8_t) ((dividedFrequency >> BYTE_LENGTH) & BIT8_MASK);

  /* Disable interrupts to set registers */
  cli();

  /* send initial command byte */
  outb(PIT_REG, PIT_MODE);

  /* write frequency to the proper ports */
  outb(lower, PIT_C0);
  outb(upper, PIT_C0);
  /* enable the PIT IRQ line */
  enable_irq(0);

  /* re-enable all interrupts */
  sti();
}

void pit_interrupt_handler(void)  {
  /* basically, keep global variable of active terminal number, and we'll check if
   * calculated terminal number aka dest = (curr +1) % TERMINAL_COUNT is equal
   * to the current terminal. If it is, send EOI, else, modify dest ID to
   * dest = (dest+1) % TERMINAL_COUNT.
   */

   send_eoi(0);

   running_display++;
   if(running_display > 2){
         running_display = 0;
   }

   task_switch(current_pid[running_display]);

   return;
}
