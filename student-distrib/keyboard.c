#include "keyboard.h"
#include "i8259.h"
#include "lib.h"
#include "vc.h"
#include "video.h"
#include "term_sched.h"

#define KEYBOARD 256

/*Mapping that matches the keyboard raw input to the char associated*/
unsigned char keymappings[KEYBOARD];       /* regular keymappings - no control words */
unsigned char keymappings_shift[KEYBOARD]; /* shift pressed down - no caps lock      */
unsigned char keymappings_caps[KEYBOARD];  /* caps lock pressed down - no shift      */
unsigned char keymappings_sc[KEYBOARD];    /* both shift and caps lock               */

//store which key codes map to alphanumeric characters
// 1 -> key is a character, number, or symbol
// 0 -> otherwise
uint8_t alphanumeric[KEYBOARD];

void populate_keymappings();
void populate_keymappings_shift();
void populate_keymappings_caps();
void populate_keymappings_sc();
void populate_alphanumeric();

static unsigned int ctrl_flag;
static unsigned int shift_flag;
static unsigned int cap_flag;
static uint32_t     alt_flag;

static unsigned char tmpbuffer[3][128];
static unsigned int next_available[3];

#define ENTER     0x1C
#define CTRL_L    0x1D
#define CTRL_R    0x1D
#define BACKSPACE 0x0E
#define SHIFT_L   0x2A
#define SHIFT_R   0x36
#define CAPS_LOCK 0x3A
#define TAB       0x0F
#define UPARW     0x48
#define DNARW     0x50
#define PGEUP     0x49
#define PGEDN     0x51
#define R_ARW     0x4D
#define L_ARW     0x4B
#define MAXCH     0x81
#define FUN_1     0x3B
#define FUN_2     0x3C
#define FUN_3     0x3D
#define ALT_L     0x38
#define ALT_R     0x38
#define U_ALT     0xB8

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
void switch_terminal(uint32_t fn_num);

/* keyboard_init
 * Description: Initialize the keyboard driver
 * Input: none
 * Output: none
 * Side effects: initializes buffer and flags, populates keyboard arrays and enables irq on pic
 * Return: none
 */
void keyboard_init(void) {
     cli();

     ctrl_flag = 0;
     shift_flag = 0;
     cap_flag = 0;
     int i;
     int j;
     for(i = 0; i < 3; i++){
           next_available[i] = 0;
     }

     for(j = 0; j < 3; j++){
       {
         for(i = 0; i < BUFFER_SIZE; i++){
           tmpbuffer[j][i] = '\0';
         }
       }
     }

     next_available[current_display] = 0;

    /* enable keyboard IRQ line on master PIC */
    enable_irq(KEYBOARD_IRQ_ON_MASTER);

    //clear tmp buffer so that people can type
    //clear flags
    ctrl_flag = 0;
    shift_flag = 0;
    cap_flag = 0;
    /*Populate the keymappings table*/
    populate_keymappings();
    populate_keymappings_shift();
    populate_keymappings_sc();
    populate_keymappings_caps();
    populate_alphanumeric();

    sti();
    return;
}

/* keyboard_interrupt_handler
 * Description: Handles interrupt input generated by the keyboard
 * Input: none
 * Output: none
 * Side effects: echos key to terminal
 * Return: none
 */

void keyboard_interrupt_handler(){

      int old_display;
      cli();

      unsigned char key_pressed;
      /*Get keyboard input*/
      key_pressed = inb(KEYBOARD_PORT);

      /* arrow keys and page up / page down are non functional atm, poll for new input */
      //TODO: add magic numbers for arrow keys page up and page down - Mike
      if(key_pressed == UPARW || key_pressed == DNARW || key_pressed == L_ARW || key_pressed == R_ARW ||
            key_pressed == PGEDN || key_pressed == PGEUP){

              send_eoi(1);
              enable_irq(1);
              return;
          }



      if(key_pressed == 0xE0){
            //printchar_term(0x02); //FIXME: fairly certain we never actually call this code and the unpress would be the polled key_pressed anyway
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

            case(TAB): {
                   tab();
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

            case (ALT_L):{
                  alt_flag = 1;
                  break;
            }

            case (U_ALT):{
                  alt_flag = 0;
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

      sti();
      
      cli();
      if(alt_flag && (key_pressed == FUN_1)){
            old_display = current_display;
            current_display = 0;
            vidchange(old_display, current_display);
      }

      if(alt_flag && (key_pressed == FUN_2)){
            old_display = current_display;
            current_display = 1;
            vidchange(old_display, current_display);
      }

      if(alt_flag && (key_pressed == FUN_3)){
            old_display = current_display;
            current_display = 2;
            vidchange(old_display, current_display);
      }
      sti();

      return;
}

/* clear_tmp_buffer
 * Description: clears buffer by setting index to 0
 * Input: none
 * Output: none
 * Side effects: none
 * Return: none
 */

void clear_tmp_buffer(){
    next_available[current_display] = 0;
}


/* handle_keyinput
 * Description: helper function for keyboard interrupt handler that handles key press
 * Input: scan code of key pressed
 * Output: none
 * Side effects: can change caps, shift, and ctrl flag, echos character to screen
 * Return: none
 */

void handle_keyinput(unsigned char key_pressed){
      /*check if it's ctrl + l to clear screen */
      if((key_pressed == L) && ctrl_flag){
            clear_term();
            next_available[current_display] = 0;
            return;
      }

      if(key_pressed > MAXCH){
            return;
      }


      /* otherwise, check if it's the upper case or lower case then to print to the screen */
      char tmp_k;
      if(shift_flag && cap_flag){
            tmp_k = keymappings_sc[key_pressed];
            printchar_term(tmp_k);
      }
      else if(cap_flag){
            tmp_k = keymappings_caps[key_pressed];
            printchar_term(tmp_k);
      }
      else if(shift_flag){
            tmp_k = keymappings_shift[key_pressed];
            printchar_term(tmp_k);
      }
      else{
            tmp_k = keymappings[key_pressed];
            printchar_term(tmp_k);
      }

      /* save it to the tmp buffer */
      /* left the last one char in the buffer as '\n' */
      if(next_available[current_display] == BUFFER_SIZE-1){
            return;
      }
      else{
            //ensure what we have is either a number, character, or symbol
            if(alphanumeric[key_pressed]){
                  tmpbuffer[current_display][next_available[current_display]] = tmp_k;
                  next_available[current_display] ++;
            }
            return;
      }
      return;
}


/* enter_pressed
 * Description: helper function for keyboard interrupt handler that handles enter
 * Input: none
 * Output: none
 * Side effects: copies keyboard buffer to the terminal buffer. prints new line to screen
 * Return: none
 */

void enter_pressed(){
      /*add the newline at the end of the buffer */
      tmpbuffer[current_display][next_available[current_display]] = '\n';
      next_available[current_display] ++;

      printchar_term('\n');

      /*copy the keyboard buffer to the official buffer */
      char* official = get_buffer();
      if(official == NULL){
            return;
      }
      /*deep copy everything from the keyboard buffer to the official terminal buffer */
      unsigned int i = 0;
      for(i = 0; i < next_available[current_display]; i++){
            official[i] = tmpbuffer[current_display][i];
      }
      /*clear the keyboard buffer */
      next_available[current_display] = 0;
      return;
}


/* backspace_pressed
 * Description: helper function for keyboard interrupt handler that handles backspace
 * Input: none
 * Output: none
 * Side effects: deletes character from screen, adjusts index of buffer
 * Return: none
 */

void backspace_pressed(){
      /*if nothing is in the keyboard buffer, return */
      if(next_available[current_display] == 0){
            return;
      }
      /*if there are things in the keyboard buffer, delete it and call the video buffer */
      else{
            next_available[current_display] --;
            backspace();
            return;
      }
}

/* populate_keymappings
 * Description: maps keyboard scan codes to ascii characters, no shift or caps
 * Input: none
 * Output: none
 * Side effects: none
 * Return: none
 */

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

/* populate_keymappings_caps
 * Description: maps keyboard scan codes to ascii characters, no shift, cap lock pressed
 * Input: none
 * Output: none
 * Side effects: none
 * Return: none
 */

void populate_keymappings_caps(){

  keymappings_caps[0x02] = '1';
  keymappings_caps[0x03] = '2';
  keymappings_caps[0x04] = '3';
  keymappings_caps[0x05] = '4';
  keymappings_caps[0x06] = '5';
  keymappings_caps[0x07] = '6';
  keymappings_caps[0x08] = '7';
  keymappings_caps[0x09] = '8';
  keymappings_caps[0x0A] = '9';
  keymappings_caps[0x0B] = '0';
  keymappings_caps[0x0C] = '-';
  keymappings_caps[0x0D] = '=';

  keymappings_caps[0x10] = 'Q';
  keymappings_caps[0x11] = 'W';
  keymappings_caps[0x12] = 'E';
  keymappings_caps[0x13] = 'R';
  keymappings_caps[0x14] = 'T';
  keymappings_caps[0x15] = 'Y';
  keymappings_caps[0x16] = 'U';
  keymappings_caps[0x17] = 'I';
  keymappings_caps[0x18] = 'O';
  keymappings_caps[0x19] = 'P';
  keymappings_caps[0x1A] = '[';
  keymappings_caps[0x1B] = ']';

  keymappings_caps[0x1E] = 'A';
  keymappings_caps[0x1F] = 'S';
  keymappings_caps[0x20] = 'D';
  keymappings_caps[0x21] = 'F';
  keymappings_caps[0x22] = 'G';
  keymappings_caps[0x23] = 'H';
  keymappings_caps[0x24] = 'J';
  keymappings_caps[0x25] = 'K';
  keymappings_caps[0x26] = 'L';
  keymappings_caps[0x27] = ';';
  keymappings_caps[0x28] = '\'';
  keymappings_caps[0x29] = '`';

  keymappings_caps[0x2B] = '\\';
  keymappings_caps[0x2C] = 'Z';
  keymappings_caps[0x2D] = 'X';
  keymappings_caps[0x2E] = 'C';
  keymappings_caps[0x2F] = 'V';
  keymappings_caps[0x30] = 'B';
  keymappings_caps[0x31] = 'N';
  keymappings_caps[0x32] = 'M';
  keymappings_caps[0x33] = ',';
  keymappings_caps[0x34] = '.';
  keymappings_caps[0x35] = '/';
  keymappings_caps[0x39] = ' ';
}

/* populate_keymappings_shift
 * Description: maps keyboard scan codes to ascii characters, shift pressed, no caps
 * Input: none
 * Output: none
 * Side effects: none
 * Return: none
 */


void populate_keymappings_shift(){
     keymappings_shift[0x29] = '~';
     keymappings_shift[0x02] = '!';
     keymappings_shift[0x03] = '@';
     keymappings_shift[0x04] = '#';
     keymappings_shift[0x05] = '$';
     keymappings_shift[0x06] = '%';
     keymappings_shift[0x07] = '^';
     keymappings_shift[0x08] = '&';
     keymappings_shift[0x09] = '*';
     keymappings_shift[0x0A] = '(';
     keymappings_shift[0x0B] = ')';
     keymappings_shift[0x0C] = '_';
     keymappings_shift[0x0D] = '+';

     keymappings_shift[0x10] = 'Q';
     keymappings_shift[0x11] = 'W';
     keymappings_shift[0x12] = 'E';
     keymappings_shift[0x13] = 'R';
     keymappings_shift[0x14] = 'T';
     keymappings_shift[0x15] = 'Y';
     keymappings_shift[0x16] = 'U';
     keymappings_shift[0x17] = 'I';
     keymappings_shift[0x18] = 'O';
     keymappings_shift[0x19] = 'P';
     keymappings_shift[0x1A] = '{';
     keymappings_shift[0x1B] = '}';

     keymappings_shift[0x1E] = 'A';
     keymappings_shift[0x1F] = 'S';
     keymappings_shift[0x20] = 'D';
     keymappings_shift[0x21] = 'F';
     keymappings_shift[0x22] = 'G';
     keymappings_shift[0x23] = 'H';
     keymappings_shift[0x24] = 'J';
     keymappings_shift[0x25] = 'K';
     keymappings_shift[0x26] = 'L';
     keymappings_shift[0x27] = ':';
     keymappings_shift[0x28] = '"';

     keymappings_shift[0x2B] = '|';

     keymappings_shift[0x2C] = 'Z';
     keymappings_shift[0x2D] = 'X';
     keymappings_shift[0x2E] = 'C';
     keymappings_shift[0x2F] = 'V';
     keymappings_shift[0x30] = 'B';
     keymappings_shift[0x31] = 'N';
     keymappings_shift[0x32] = 'M';
     keymappings_shift[0x33] = '<';
     keymappings_shift[0x34] = '>';
     keymappings_shift[0x35] = '?';
     keymappings_shift[0x39] = ' ';
}

/* populate_keymappings_sc
 * Description: maps keyboard scan codes to ascii characters, shift and caps pressed
 * Input: none
 * Output: none
 * Side effects: none
 * Return: none
 */

void populate_keymappings_sc(){

  keymappings_sc[0x29] = '~';
  keymappings_sc[0x02] = '!';
  keymappings_sc[0x03] = '@';
  keymappings_sc[0x04] = '#';
  keymappings_sc[0x05] = '$';
  keymappings_sc[0x06] = '%';
  keymappings_sc[0x07] = '^';
  keymappings_sc[0x08] = '&';
  keymappings_sc[0x09] = '*';
  keymappings_sc[0x0A] = '(';
  keymappings_sc[0x0B] = ')';
  keymappings_sc[0x0C] = '_';
  keymappings_sc[0x0D] = '+';



  keymappings_sc[0x10] = 'q';
  keymappings_sc[0x11] = 'w';
  keymappings_sc[0x12] = 'e';
  keymappings_sc[0x13] = 'r';
  keymappings_sc[0x14] = 't';
  keymappings_sc[0x15] = 'y';
  keymappings_sc[0x16] = 'u';
  keymappings_sc[0x17] = 'i';
  keymappings_sc[0x18] = 'o';
  keymappings_sc[0x19] = 'p';
  keymappings_sc[0x1A] = '{';
  keymappings_sc[0x1B] = '}';



  keymappings_sc[0x1E] = 'a';
  keymappings_sc[0x1F] = 's';
  keymappings_sc[0x20] = 'd';
  keymappings_sc[0x21] = 'f';
  keymappings_sc[0x22] = 'g';
  keymappings_sc[0x23] = 'h';
  keymappings_sc[0x24] = 'j';
  keymappings_sc[0x25] = 'k';
  keymappings_sc[0x26] = 'l';
  keymappings_sc[0x27] = ':';
  keymappings_sc[0x28] = '"';
  keymappings_sc[0x2B] = '|';


  keymappings_sc[0x2C] = 'z';
  keymappings_sc[0x2D] = 'x';
  keymappings_sc[0x2E] = 'c';
  keymappings_sc[0x2F] = 'v';
  keymappings_sc[0x30] = 'b';
  keymappings_sc[0x31] = 'n';
  keymappings_sc[0x32] = 'm';
  keymappings_sc[0x33] = '<';
  keymappings_sc[0x34] = '>';
  keymappings_sc[0x35] = '?';
  keymappings_sc[0x39] = ' ';

}


void populate_alphanumeric(){
      (void) memset((void *)alphanumeric, 0, KEYBOARD);

      alphanumeric[0x29] = (uint8_t)1;
      alphanumeric[0x02] = (uint8_t)1;
      alphanumeric[0x03] = (uint8_t)1;
      alphanumeric[0x04] = (uint8_t)1;
      alphanumeric[0x05] = (uint8_t)1;
      alphanumeric[0x06] = (uint8_t)1;
      alphanumeric[0x07] = (uint8_t)1;
      alphanumeric[0x08] = (uint8_t)1;
      alphanumeric[0x09] = (uint8_t)1;
      alphanumeric[0x0A] = (uint8_t)1;
      alphanumeric[0x0B] = (uint8_t)1;
      alphanumeric[0x0C] = (uint8_t)1;
      alphanumeric[0x0D] = (uint8_t)1;
      alphanumeric[0x10] = (uint8_t)1;
      alphanumeric[0x11] = (uint8_t)1;
      alphanumeric[0x12] = (uint8_t)1;
      alphanumeric[0x13] = (uint8_t)1;
      alphanumeric[0x14] = (uint8_t)1;
      alphanumeric[0x15] = (uint8_t)1;
      alphanumeric[0x16] = (uint8_t)1;
      alphanumeric[0x17] = (uint8_t)1;
      alphanumeric[0x18] = (uint8_t)1;
      alphanumeric[0x19] = (uint8_t)1;
      alphanumeric[0x1A] = (uint8_t)1;
      alphanumeric[0x1B] = (uint8_t)1;
      alphanumeric[0x1E] = (uint8_t)1;
      alphanumeric[0x1F] = (uint8_t)1;
      alphanumeric[0x20] = (uint8_t)1;
      alphanumeric[0x21] = (uint8_t)1;
      alphanumeric[0x22] = (uint8_t)1;
      alphanumeric[0x23] = (uint8_t)1;
      alphanumeric[0x24] = (uint8_t)1;
      alphanumeric[0x25] = (uint8_t)1;
      alphanumeric[0x26] = (uint8_t)1;
      alphanumeric[0x27] = (uint8_t)1;
      alphanumeric[0x28] = (uint8_t)1;
      alphanumeric[0x2C] = (uint8_t)1;
      alphanumeric[0x2D] = (uint8_t)1;
      alphanumeric[0x2E] = (uint8_t)1;
      alphanumeric[0x2F] = (uint8_t)1;
      alphanumeric[0x30] = (uint8_t)1;
      alphanumeric[0x31] = (uint8_t)1;
      alphanumeric[0x32] = (uint8_t)1;
      alphanumeric[0x33] = (uint8_t)1;
      alphanumeric[0x34] = (uint8_t)1;
      alphanumeric[0x35] = (uint8_t)1;
      alphanumeric[0x2B] = (uint8_t)1;

      return;

}
