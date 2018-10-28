#include "keyboard.h"
#include "i8259.h"
#include "lib.h"
#include "vc.h"
#include "video.h"

#define KEYBOARD 256

/*Mapping that matches the keyboard raw input to the char associated*/
unsigned char keymappings[KEYBOARD];
unsigned char keymappings_upper[KEYBOARD];

void populate_keymappings();
void populate_keymappings_upper();

static unsigned int ctrl_flag;
static unsigned int shift_flag;
static unsigned int cap_flag;

static unsigned char tmpbuffer[KEYBOARD];
static unsigned int next_available;

#define ENTER  0x1C
#define CTRL_L 0x1D
#define CTRL_R 0x1D
#define BACKSPACE 0x0E
#define SHIFT_L 0x2A
#define SHIFT_R 0x36
#define CAPS_LOCK 0x3A

#define U_S_L 0xAA
#define U_S_R 0xB6
#define U_C_L 0x9D
#define U_C_R 0x9D

#define L 0xA6

void clear_tmp_buffer();
void handle_keyinput(unsigned char key_pressed);
void enter_pressed();
void backspace_pressed();
void populate_keymappings_upper();

/* Initializes the keyboard and populates keymappings */
void keyboard_init(void) {
     cli();

     ctrl_flag = 0;
     shift_flag = 0;
     cap_flag = 0;

     next_available = 0;

    /* enable keyboard IRQ line on master PIC */
    enable_irq(KEYBOARD_IRQ_ON_MASTER);

    //clear tmp buffer so that people can type
    clear_tmp_buffer();
    //clear flags
    ctrl_flag = 0;
    shift_flag = 0;
    cap_flag = 0;
    /*Populate the keymappings table*/
    populate_keymappings();
    populate_keymappings_upper();

    sti();
    return;
}

/* Handles interrupt input when they keyboard generates an interrupt */
void keyboard_interrupt_handler(){
      unsigned char key_pressed;
      /*Get keyboard input*/
      key_pressed = inb(KEYBOARD_PORT);

      if(key_pressed == 0xE0){
            key_pressed = inb(KEYBOARD_PORT);
      }


      switch(key_pressed){
            case(ENTER): {
                  enter_pressed();
                  break;
            }

            //check for ctrl flag.
            case(CTRL_L | CTRL_R):{
                  ctrl_flag = 1;
                  break;
            }
            case(U_C_L | U_C_R):{
                  ctrl_flag = 0;
                  break;
            }

            //check for shift flag.
            case SHIFT_L:{
                  shift_flag = 1;
                  break;
            }

            case SHIFT_R:{
                  shift_flag = 1;
                  break;
            }

            case U_S_L:{
                  shift_flag = 0;
                  break;
            }

            case U_S_R:{
                  shift_flag = 0;
                  break;
            }

            //check for backspace
            case(BACKSPACE):{
                  backspace_pressed();
                  break;
            }

            case(CAPS_LOCK):{
                  /*when caps-lock pressed, set the caps-lock flag*/
                  if(cap_flag == 1){
                        cap_flag = 0;
                  }
                  else{
                        cap_flag = 1;
                  }
                  break;
            }

            default :{
                  handle_keyinput(key_pressed);
                  break;
            }

      }

      /*Return from interrupt*/
      send_eoi(1);
      enable_irq(1);
      return;
}


void clear_tmp_buffer(){
    next_available = 0;
}


void handle_keyinput(unsigned char key_pressed){
      /*check if it's ctrl + l to clear screen */
      if((key_pressed == L) && ctrl_flag){
            clear_term();
            next_available = 0;
            return;
      }
      /* otherwise, check if it's the upper case or lower case then to print to the screen */
      char tmp_k;
      if(shift_flag != cap_flag){
            tmp_k = keymappings_upper[key_pressed];
            printchar_term(tmp_k);
      }
      else{
            tmp_k = keymappings[key_pressed];
            printchar_term(tmp_k);
      }

      /* save it to the tmp buffer */
      /* left the last one char in the buffer as '\n' */
      if(next_available == BUFFER_SIZE-1){
            return;
      }
      else{
            tmpbuffer[next_available] = tmp_k;
            next_available ++;
            return;
      }
      return;
}

void enter_pressed(){
      /*add the newline at the end of the buffer */
      tmpbuffer[next_available] = '\n';
      next_available ++;

      printchar_term('\n');

      /*copy the keyboard buffer to the official buffer */
      char* official = get_buffer();
      if(official == NULL){
            return;
      }
      /*deep copy everything from the keyboard buffer to the official terminal buffer */
      unsigned int i = 0;
      for(i = 0; i < next_available; i++){
            official[i] = tmpbuffer[i];
      }
      /*clear the keyboard buffer */
      next_available = 0;
      return;
}

void backspace_pressed(){
      /*if nothing is in the keyboard buffer, only call delete video buffer */
      if(next_available == 0){
            backspace();
            return;
      }
      /*if there are things in the keyboard buffer, delete it and call the video buffer */
      else{
            next_available --;
            backspace();
            return;
      }
}

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
      keymappings[0x0C] = '-';
      keymappings[0x0D] = '=';



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
      keymappings[0x1A] = '[';
      keymappings[0x1B] = ']';



      keymappings[0x1E] = 'a';
      keymappings[0x1F] = 's';
      keymappings[0x20] = 'd';
      keymappings[0x21] = 'f';
      keymappings[0x22] = 'g';
      keymappings[0x23] = 'h';
      keymappings[0x24] = 'j';
      keymappings[0x25] = 'k';
      keymappings[0x26] = 'l';
      keymappings[0x27] = ';';
      keymappings[0x28] = '\'';
      keymappings[0x29] = '`';



      keymappings[0x1C] = '\n';

      keymappings[0x2B] = '\\';
      keymappings[0x2C] = 'z';
      keymappings[0x2D] = 'x';
      keymappings[0x2E] = 'c';
      keymappings[0x2F] = 'v';
      keymappings[0x30] = 'b';
      keymappings[0x31] = 'n';
      keymappings[0x32] = 'm';
      keymappings[0x33] = ',';
      keymappings[0x34] = '.';
      keymappings[0x35] = '/';
      keymappings[0x39] = ' ';

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

     keymappings[0x1C] = '\n';

     keymappings_upper[0x2B] = '|';

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
     keymappings_upper[0x39] = ' ';
}
