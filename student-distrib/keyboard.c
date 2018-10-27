#include "keyboard.h"
#include "i8259.h"
#include "lib.h"

/*Mapping that matches the keyboard raw input to the char associated*/
unsigned char keymappings[128];
unsigned char keymappings_lower[128];
unsigned char keymappings_upper[128];

void populate_keymappings();
void populate_keymappings_lower();
void populate_keymappings_upper();

/* Initializes the keyboard and populates keymappings */
void keyboard_init(void) {
    /* enable keyboard IRQ line on master PIC */
    enable_irq(KEYBOARD_IRQ_ON_MASTER);

    /*Populate the keymappings table*/
    populate_keymappings();
    return;
}

/* Handles interrupt input when they keyboard generates an interrupt */
void keyboard_interrupt_handler(){
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

void populate_keymappings_upper(){
     keymappings_upper[0x29] = '~';
     keymappings_upper[0x02] = '!';
     keymappings_upper[0x03] = '@';
     keymappings_upper[0x04] = '#';
     keymappings_upper[0x05] = '$';
     keymappings_upper[0x06] = '%';
     keymappings_upper[0x07] = '^';
     keymappings_upper[0x08] = '&';
     keymappings_upper[0x09] = '*';
     keymappings_upper[0x0A] = '(';
     keymappings_upper[0x0B] = ')';
     keymappings_upper[0x0C] = '_';
     keymappings_upper[0x0D] = '+';

     keymappings_upper[0x10] = 'Q';
     keymappings_upper[0x11] = 'W';
     keymappings_upper[0x12] = 'E';
     keymappings_upper[0x13] = 'R';
     keymappings_upper[0x14] = 'T';
     keymappings_upper[0x15] = 'Y';
     keymappings_upper[0x16] = 'U';
     keymappings_upper[0x17] = 'I';
     keymappings_upper[0x18] = 'O';
     keymappings_upper[0x19] = 'P';
     keymappings_upper[0x1A] = '{';
     keymappings_upper[0x1B] = '}';

     keymappings_upper[0x1E] = 'A';
     keymappings_upper[0x1F] = 'S';
     keymappings_upper[0x20] = 'D';
     keymappings_upper[0x21] = 'F';
     keymappings_upper[0x22] = 'G';
     keymappings_upper[0x23] = 'H';
     keymappings_upper[0x24] = 'J';
     keymappings_upper[0x25] = 'K';
     keymappings_upper[0x26] = 'L';
     keymappings_upper[0x27] = ':';
     keymappings_upper[0x28] = '"';

     keymappings_upper[0x2C] = 'Z';
     keymappings_upper[0x2D] = 'X';
     keymappings_upper[0x2E] = 'C';
     keymappings_upper[0x2F] = 'V';
     keymappings_upper[0x30] = 'B';
     keymappings_upper[0x31] = 'N';
     keymappings_upper[0x32] = 'M';
     keymappings_upper[0x33] = '<';
     keymappings_upper[0x34] = '>';
     keymappings_upper[0x35] = '?';
}
}


