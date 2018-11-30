/* keyboard.h: Header file for all keyboard functions */
#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#define KEYBOARD_IRQ_ON_MASTER 0x01
#define KEYBOARD_PORT 0x60
#define MOUSE_INT     0x64 //Keyboard and mouse share port 0x60, but mouse writes a 1 to 0x64 if the interrupt is from the mouse

void clear_tmp_buffer();
void handle_keyinput(unsigned char key_pressed);
void enter_pressed();
void backspace_pressed();
void keyboard_init(void);
void keyboard_interrupt_handler(void);

void populate_keymappings_upper();
void populate_keymappings();

#endif  /* _KEYBOARD_H */
