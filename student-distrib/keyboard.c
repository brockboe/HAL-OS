#include "keyboard.h"
#include "i8259.h"
#include "lib.h"

/* TODO: Implement keyboard initialization implementation */
void keyboard_init(void) {
    /* enable keyboard IRQ line on master PIC */
    enable_irq(KEYBOARD_IRQ_ON_MASTER);
}

void keyboard_handler(){
      char input;

      input = inb(KEYBOARD_PORT);
      send_eoi(1);
      enable_irq(1);
      return;
}
