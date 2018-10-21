#include "keyboard.h"
#include "i8259.h"
#include "lib.h"

/* TODO: Implement keyboard initialization implementation */
void keyboard_init(void) {
    /* enable keyboard IRQ line on master PIC */
    enable_irq(KEYBOARD_IRQ_ON_MASTER);
}
