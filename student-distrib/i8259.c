/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7  */
uint8_t slave_mask;  /* IRQs 8-15 */

/* Initialize the 8259 PIC */
void i8259_init(void) {

    /* Initialize PIC masks */
    master_mask = 0xFF;
    slave_mask = 0xFF;

    /* Mask all interrupts */
    outb(slave_mask, SLAVE_8259_PORT + 1);
	  outb(master_mask, MASTER_8259_PORT + 1);

    // ICW1
    /* Initialize master port */
    outb(ICW1, MASTER_8259_PORT);

    /* Initialize slave port */
    outb(ICW1, SLAVE_8259_PORT);

    // ICW2
    /* Loads master offset into IDT */
    outb(MASTER_8259_PORT, MASTER_8259_PORT+1);

    /* Loads slave offset into IDT */
    outb(SLAVE_IDT_OFFSET, SLAVE_8259_PORT+1);

    //ICW3
    /* Send data to master informing it of slave pic */
    outb(MASTER_IRQ2, MASTER_8259_PORT+1);

    /* Tell slave that it's the first cascaded device */
    outb(FIRST_SLAVE, SLAVE_8259_PORT+1);

    // ICW4
    /* Set ISA for master and slave pic */
    outb(ICW_8086_MODE, MASTER_8259_PORT+1);
    outb(ICW_8086_MODE, SLAVE_8259_PORT+1);

    /* Restore all masks */
    outb(slave_mask, SLAVE_8259_PORT + 1);
	  outb(master_mask, MASTER_8259_PORT + 1);

    enable_irq(SLAVE_IRQ_ON_MASTER);
}

/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num) {
    /* First check if below irq_num bounds */
    if(irq_num < 0 || irq_num > 15)
    {
        return;
    }

    /* if irq > 8, then we know we're working with slave PIC */
    if(irq_num > 8)
    {
        /* Lower irqnum to traditional irq value range */
        irq_num -= 8;
        slave_mask = inb(SLAVE_8259_PORT + 1);

        /* Create IRQ mask */
        slave_mask &= ~(1 << irq_num);

        /* Write new mask to data port */
        outb(slave_mask, SLAVE_8259_PORT + 1);
    }

    /* else, we'll be working with the master PIC */
    else
    {
        master_mask = inb(MASTER_8259_PORT + 1);
        master_mask &= ~(1 << irq_num);

        /* Write master mask to master port */
        outb(master_mask, MASTER_8259_PORT + 1);
    }
}

/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num) {
  /* First check if below irq_num bounds */
  if(irq_num < 0 || irq_num > 15)
  {
      return;
  }

  /* if irq > 8, then we know we're working with slave PIC */
  if(irq_num > 8)
  {
      /* Lower irqnum to traditional irq value range */
      irq_num -= 8;
      slave_mask = inb(SLAVE_8259_PORT + 1);

      /* Create IRQ mask */
      slave_mask |= (1 << irq_num);

      /* Write new mask to data port */
      outb(slave_mask, SLAVE_8259_PORT + 1);
  }

  /* else, we'll be working with the master PIC */
  else
  {
      master_mask = inb(MASTER_8259_PORT + 1);
      master_mask |= (1 << irq_num);

      /* Write master mask to master port */
      outb(master_mask, MASTER_8259_PORT + 1);
  }
}

/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num) {
    /* First check if below irq_num bounds */
    if(irq_num < 0 || irq_num > 15)
    {
        return;
    }

    /* send EOI on slave PIC */
    if(irq_num >= 8)
    {
        irq_num -= 8;
        outb(EOI | irq_num, SLAVE_8259_PORT);
        outb(EOI | 2, MASTER_8259_PORT);
    }

    /* send EOI on master PIC */
    else
    {
        outb(EOI | irq_num, MASTER_8259_PORT);
    }
}
