#include "keyboard.h"
#include "i8259.h"
#include "lib.h"

/*Mapping that matches the keyboard raw input to the char associated*/
unsigned char keymappings[128];


void populate_keymappings();

/* TODO: Implement keyboard initialization implementation */
void keyboard_init(void) {
    /* enable keyboard IRQ line on master PIC */
    enable_irq(KEYBOARD_IRQ_ON_MASTER);

    /*Populate the keymappings table*/
    populate_keymappings();
    return;
}

void keyboard_handler(){
      unsigned char key_pressed;

      /*Get keyboard input*/
      key_pressed = inb(KEYBOARD_PORT);

      /*Ensure the byte isn't a key release and print it*/
      if(key_pressed < 0x40){
            printf("%c", keymappings[key_pressed]);
      }

      /*Return from interrupt*/
      send_eoi(1);
      enable_irq(1);
      return;
}

/*Fills the table with the values associated with keyboard input*/
void populate_keymappings(){
      keymappings[0x02] = '1';
      keymappings[0x03] = '2';
      keymappings[0x04] = '3';
      keymappings[0x05] = '4';
      keymappings[0x06] = '5';
      keymappings[0x07] = '6';
      keymappings[0x08] = '7';
      keymappings[0x09] = '8';
      keymappings[0x0A] = '9';
      keymappings[0x0B] = '0';

      keymappings[0x10] = 'q';
      keymappings[0x11] = 'w';
      keymappings[0x12] = 'e';
      keymappings[0x13] = 'r';
      keymappings[0x14] = 't';
      keymappings[0x15] = 'y';
      keymappings[0x16] = 'u';
      keymappings[0x17] = 'i';
      keymappings[0x18] = 'o';
      keymappings[0x19] = 'p';

      keymappings[0x1E] = 'a';
      keymappings[0x1F] = 's';
      keymappings[0x20] = 'd';
      keymappings[0x21] = 'f';
      keymappings[0x22] = 'g';
      keymappings[0x23] = 'h';
      keymappings[0x24] = 'j';
      keymappings[0x25] = 'k';
      keymappings[0x26] = 'l';

      keymappings[0x2A] = '\n';

      keymappings[0x2C] = 'z';
      keymappings[0x2D] = 'x';
      keymappings[0x2E] = 'c';
      keymappings[0x2F] = 'v';
      keymappings[0x30] = 'b';
      keymappings[0x31] = 'n';
      keymappings[0x32] = 'm';
}
