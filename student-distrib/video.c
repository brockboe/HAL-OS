#include "video.h"
#include "lib.h"
#include "vc.h"

#define CYAN 0xB
#define RED 0xC

vid_data_t * display;
terminal_info_t tinfo;

/* vid_init
 * Description : initialize the terminal display  -- red as default
 * Input : none
 * Output : none
 * Side effects: none
 * Return : none
 */
void vid_init(){
      int i;
      display = (vid_data_t *)VIDMEM;
      tinfo.offset = 0;
      //Write the color information into the video memory.
      for(i = 0; i < MAXCHAR; i++){
            display[i].highbits = RED;
      }

      return;
}

/* fill_color
 * Description : Helper function to fill display with red as font color
 * Input : none
 * Output : none
 * Side effects: none
 * Return : none
 */

void fill_color(){
      int i;
      //write the color information into the video memory
      for(i = 0; i < MAXCHAR; i++){
            display[i].highbits = RED;
      }
      return;
}

/* clear_term
 * Description : Clears terminal, can be called with ctrl + l
 * Input : none
 * Output : none
 * Side effects: none
 * Return :none
 */
void clear_term(){
      int i;
      //empty the character information in the video memory
      for(i = 0; i < MAXCHAR; i++){
            display[i].character = 0;
      }
      //set the cursor back to 0.
      tinfo.offset = 0;
      return;
}

/* scroll_term
 * Description : shifts characters on display when last line is reached
 * Input : none
 * Output : none
 * Side effects: shifts location of characters on display, clears last line
 * Return :none
 */
void scroll_term(){
      int i;
      //shift all the vidmem left by 80
      for(i = TERMWIDTH; i < MAXCHAR; i++){
            display[i - TERMWIDTH].character = display[i].character;
      }
      //set the cursor to the bottom row
      tinfo.offset = TERMWIDTH * (TERMHEIGHT - 1);

      //clear the bottom row
      for(i = TERMWIDTH * (TERMHEIGHT - 1); i < MAXCHAR; i++){
            display[i].character = 0;
      }
}

/* print_term
 * Description : behaves as a printf function printing the string of given length to the terminal
 * Input : string - character string to be printed to screen
 *         length - the length of the string
 * Output : none
 * Side effects: edits display and calls scroll_term when bottom of terminal is reached
 * RETURN : none
 */
void print_term(uint8_t * string, int length){
      int i;
      for(i = 0; i < length; i++){
            //check if we've reached the null-termination
            if(string[i] == 0){
                  continue;
            }
            //check if we need to scroll
            if(tinfo.offset >= MAXCHAR){
                  scroll_term();
            }
            //check if we have a new line
            if(string[i] == '\n'){
                  tinfo.offset -= tinfo.offset % TERMWIDTH;
                  tinfo.offset += TERMWIDTH;
            }
            //otherwise simply print the character
            else{
                  display[tinfo.offset].character = string[i];
                  tinfo.offset++;
            }
      }
}

/* printchar_term
 * Description : behaves as putc function by printing character to terminal
 * Input : char a - the character to be printed to terminal
 * Output : none
 * Side effects: edits display and calls scroll_term when bottom of temrinal is reached
 * RETURN : none
 */
void printchar_term(char a){
      //check for null character
      if(a == '\0'){
            return;
      }
      //check if we need to scroll
      if(tinfo.offset >= MAXCHAR){
            scroll_term();
      }
      //check for nl character
      if(a == '\n'){
            tinfo.offset -= tinfo.offset % TERMWIDTH;
            tinfo.offset += TERMWIDTH;
      }
      //otherwise print the character
      else{
            display[tinfo.offset].character = a;
            tinfo.offset++;
            return;
      }
}

/* backspace
 * Description : Removes last character from terminal
 * Input : none
 * Output : none
 * Side effects: wipes character from display, moves cursor back one
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
 * Description : moves cursor 10 spaces forward
 * Input : none
 * Output : none
 * Side effects: none
 * RETURN : none
 */
void tab(){
      if(tinfo.offset == MAXCHAR){
            scroll_term();
      }
      tinfo.offset = tinfo.offset + 10;
      if(tinfo.offset > MAXCHAR){
            tinfo.offset = MAXCHAR;
      }
      return;
}

/* set_term_x
 * Description : moves cursor by x input
 * Input : x - how far along cursor is to be moved
 * Output : none
 * Side effects: none
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
 * Description : Helper function for printing large integer numbers to terminal
 * Input : none
 * OUTPUT : none
 * Side effects: none
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
