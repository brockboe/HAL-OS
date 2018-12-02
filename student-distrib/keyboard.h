/* keyboard.h: Header file for all keyboard functions */
#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#define KEYBOARD_IRQ_ON_MASTER 0x01
#define KEYBOARD_PORT 0x60

void clear_tmp_buffer();
void handle_keyinput(unsigned char key_pressed);
void enter_pressed();
void backspace_pressed();
void keyboard_init(void);
void keyboard_interrupt_handler(void);

void populate_keymappings_upper();
void populate_keymappings();

#endif  /* _KEYBOARD_H */
