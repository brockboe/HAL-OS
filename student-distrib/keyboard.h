/* keyboard.h: Header file for all keyboard functions */
#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#define KEYBOARD_IRQ_ON_MASTER 0x01
#define KEYBOARD_PORT 0x60

void keyboard_init(void);

void keyboard_handler(void);

#endif  /* _KEYBOARD_H */
