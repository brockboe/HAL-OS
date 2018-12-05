#include "video.h"
#include "lib.h"
#include "vc.h"
#include "term_sched.h"

#define GREEN 0xA
#define CYAN 0xB
#define RED 0xC

vid_data_t * display;
terminal_info_t tinfo[3];

void move_cursor();
void enable_cursor();

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
      for(i = 0; i < 3; i++){
            tinfo[i].offset = 0;
            tinfo[i].cursor_start = 0;
            tinfo[i].cursor_end = 15;
            //Write the color information into the video memory.
      }

      fill_color(0, RED);
      fill_color(1, RED);
      fill_color(2, CYAN);
      fill_color(3, GREEN);
      enable_cursor();
      move_cursor();
      return;
}

/* fill_color
 * Description : Helper function to fill display with red as font color
 * Input : none
 * Output : none
 * Side effects: none
 * Return : none
 */

void fill_color(int vid_page, uint8_t color){
      vid_data_t * temp_display = (vid_data_t *)(VIDMEM + vid_page * 0x1000);

      int i;
      //write the color information into the video memory
      for(i = 0; i < MAXCHAR; i++){
            temp_display[i].highbits = color;
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
      tinfo[running_display].offset = 0;
      move_cursor(); // FIXME: Clear shouldn't remove 391OS> and move_cursor needs to be fixed as well as a result
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
      tinfo[running_display].offset = TERMWIDTH * (TERMHEIGHT - 1);

      //clear the bottom row
      for(i = TERMWIDTH * (TERMHEIGHT - 1); i < MAXCHAR; i++){
            display[i].character = 0;
      }
      move_cursor();
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
            if(tinfo[running_display].offset >= MAXCHAR){
                  scroll_term();
            }
            //check if we have a new line
            if(string[i] == '\n'){
                  tinfo[running_display].offset -= tinfo[running_display].offset % TERMWIDTH;
                  tinfo[running_display].offset += TERMWIDTH;
            }
            //otherwise simply print the character
            else{
                  display[tinfo[running_display].offset].character = string[i];
                  tinfo[running_display].offset++;
            }
      }
      move_cursor();
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
      if(tinfo[running_display].offset >= MAXCHAR){
            scroll_term();
      }
      //check for nl character
      if(a == '\n'){
            tinfo[running_display].offset -= tinfo[running_display].offset % TERMWIDTH;
            tinfo[running_display].offset += TERMWIDTH;
            move_cursor();
      }
      //otherwise print the character
      else{
            display[tinfo[running_display].offset].character = a;
            tinfo[running_display].offset++;
      }
      move_cursor();
}

/* backspace
 * Description : Removes last character from terminal
 * Input : none
 * Output : none
 * Side effects: wipes character from display, moves cursor back one
 * RETURN : none
 */
void backspace(){
      tinfo[running_display].offset--;
      if(tinfo[running_display].offset > MAXCHAR){
            tinfo[running_display].offset = 0;
      }
      display[tinfo[running_display].offset].character = 0;
      move_cursor();
      return;
}

/* tab
 * Description : moves cursor 10 spaces forward, if cursor would overshoot to next line, stops that from occuring
 * Input : none
 * Output : none
 * Side effects: calls scroll_term if MAXCHAR reached
 * RETURN : none
 */
void tab(){
      if(tinfo[running_display].offset == MAXCHAR){
            scroll_term();
      }
      if((tinfo[running_display].offset % TERMWIDTH + 10) > TERMWIDTH){
            tinfo[running_display].offset += (TERMWIDTH - tinfo[running_display].offset % TERMWIDTH);
      }
      else{
            tinfo[running_display].offset = tinfo[running_display].offset + 10;
      }
      move_cursor();
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
      tinfo[running_display].offset -= tinfo[running_display].offset % TERMWIDTH;
      tinfo[running_display].offset += x;
      move_cursor();
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

/*
 move_cursor
 This program was inspired by the resources available
 on the osdev.org wiki
 */
 void move_cursor(){

       if(running_display != running_display){
             return;
       }

       uint32_t pos = tinfo[running_display].offset;
       outb(0x0F, 0x3D4);
       outb((uint8_t)(pos & 0xFF), 0x3D5);
       outb(0x0E, 0x3D4);
       outb((uint8_t)((pos >> 8)&0xFF), 0x3D5);
 }

 void enable_cursor(){
       outb(0x0A, 0x3D4);
       outb((inb(0x3D5) & 0xC0) | tinfo[running_display].cursor_start, 0x3D5);

      outb(0x3D4, 0x0B);
      outb((inb(0x3D5) & 0xE0) | tinfo[running_display].cursor_end, 0x3D5);
 }
