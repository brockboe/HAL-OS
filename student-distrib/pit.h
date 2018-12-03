/* pit.h: Header file for all rtc functions */
#ifndef _PIT_H
#define _PIT_H

/* Function to initialize the pit */
void pit_init(void);

/* Function that handles pit-generated interrupts */
void pit_interrupt_handler(void);

#endif  /* _PIT_H */
