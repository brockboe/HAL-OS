#include "video.h"
#include "lib.h"

#define CYAN 0xB
#define RED 0xC

vid_data_t * display;
terminal_info_t tinfo;

/* video_initialization 
 * INPUT : none 
 * OUTPUT :none 
 * EFFECT : initialze the terminal display to cyan as default 
 * RETURN : none 
 */
void vid_init(){
      int i;
      display = (vid_data_t *)VIDMEM;
      tinfo.offset = 0;
      for(i = 0; i < MAXCHAR; i++){
            display[i].highbits = CYAN;
      }

      return;
}

/* fill_color 
 * INPUT : none 
 * OUTPUT : none 
 * EFFECT : fill the terminal by color red after first initialilzation 
 * RETURN : none 
 */

void fill_color(){
      int i;
      for(i = 0; i < MAXCHAR; i++){
            display[i].highbits = RED;
      }
      return;
}

/* clear_term 
 * INPUT : none 
 * OUTPUT : none 
 * EFFECT :called with ctrl+l is hit, wipe the terminal's characters 
 *         reposition the cursor's place at (0,0)
 * RETURN :none 
 */
void clear_term(){
      int i;
      for(i = 0; i < MAXCHAR; i++){
            display[i].character = 0;
      }
      tinfo.offset = 0;
      return;
}

/* scroll_term 
 * DESCRIPTION : called when the terminal is fully written and need to move upwards for one empty line at bottom 
 * INPUT : none 
 * OUTPUT : none 
 * EFFECT : shift the display screen up with one empty line at the bottom 
 * RETURN :none 
 */
void scroll_term(){
      int i;
      for(i = TERMWIDTH; i < MAXCHAR; i++){
            display[i - TERMWIDTH].character = display[i].character;
      }
      tinfo.offset = TERMWIDTH * (TERMHEIGHT - 1);

      for(i = TERMWIDTH * (TERMHEIGHT - 1); i < MAXCHAR; i++){
            display[i].character = 0;
      }
}

/* print_term 
 * DESCRIPTION : called when 'ENTER' is hit and should display chars typed by user
 * INPUT : string - character string that needed to be printed 
 *         length - the length of the string 
 * OUTPUT : none 
 * EFFECT : display the characters on the terminal by modifying the display screen 
 *          check for if needed to scroll if the terminal display is fully written 
 * RETURN : none 
 */
void print_term(uint8_t * string, int length){
      int i;
      for(i = 0; i < length; i++){
            if(string[i] == 0){
                  continue;
            }
            if(tinfo.offset >= MAXCHAR){
                  scroll_term();
            }
            if(string[i] == '\n'){
                  tinfo.offset -= tinfo.offset % TERMWIDTH;
                  tinfo.offset += TERMWIDTH;
            }
            else{
                  display[tinfo.offset].character = string[i];
                  tinfo.offset++;
            }
      }
}

/* print_term 
 * DESCRIPTION : called whenever user typed in a character 
 * INPUT : a - the character that needed to be printed 
 * OUTPUT : none 
 * EFFECT : display the character just typed in by the user 
 *          check for if needed to scroll if the terminal is fully written 
 * RETURN : none 
 */
void printchar_term(char a){

      if(a == '\0'){
            return;
      }

      if(tinfo.offset >= MAXCHAR){
            scroll_term();
      }
      if(a == '\n'){
            tinfo.offset -= tinfo.offset % TERMWIDTH;
            tinfo.offset += TERMWIDTH;
      }
      else{
            display[tinfo.offset].character = a;
            tinfo.offset++;
            return;
      }
}

/* backspace 
 * DESCRIPTION : called when user hit 'BACKSPACE' to delete a character 
 * INPUT : none 
 * OUTPUT : none 
 * EFFECT : move cursor backward one position , wipe out the last character displated on the terminal 
 * RETURN : none 
 */
void backspace(){
      tinfo.offset--;
      if(tinfo.offset > MAXCHAR){
            tinfo.offset = 0;
      }
      display[tinfo.offset].character = 0;
      return;
}

/* tab 
 * DESCRIPTION : move the cursor to the corresponding position after hitting tab 
 * INPUT : none 
 * OUTPUT : none 
 * EFFECT : move the cursor to the corresponding position after hitting tab 
 * RETURN : none 
 */
void tab(){
      tinfo.offset = (tinfo.offset / 10) + 10;
      if(tinfo.offset > MAXCHAR){
            scroll_term();
      }
      return;
}

/* set_term_x 
 * DESCRIPTION : move the cursor to the correct character 
 * INPUT : x - the term where cursor should be at 
 * OUTPUT : none 
 * EFFECT : move the cursor to the correct character 
 * RETURN : none 
 */
void set_term_x(uint32_t x){
      if(x > TERMWIDTH){
            return;
      }
      tinfo.offset -= tinfo.offset % TERMWIDTH;
      tinfo.offset += x;
      return;
}
/* print_num 
 * DESCRIPTION : print the number of its correct order of characters digits 
 * INPUT : none 
 * OUTPUT : none 
 * EFFECT : print the number of its correct order of characters digits 
 * RETURN : none 
 */
void print_num(int x){
      char num_ascii[100];
      int count = -1;
      int low_place;
      int remaining = x;

      if(x == 0){
            printchar_term('0');
            return;
      }

      while(remaining > 0){
            count++;
            low_place = remaining % 10;
            remaining /= 10;
            num_ascii[count] = (char)(low_place + '0');
      }

      while(count >= 0){
            printchar_term(num_ascii[count]);
            count--;
      }

      return;
}
