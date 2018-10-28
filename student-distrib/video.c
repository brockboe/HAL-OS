#include "video.h"
#include "lib.h"

#define CYAN 0xB

vid_data_t * display;
terminal_info_t tinfo;

void vid_init(){
      int i;
      display = (vid_data_t *)VIDMEM;
      tinfo.offset = 0;
      for(i = 0; i < MAXCHAR; i++){
            display[i].highbits = CYAN;
      }

      return;
}

void fill_color(){
      int i;
      for(i = 0; i < MAXCHAR; i++){
            display[i].highbits = CYAN;
      }
      return;
}

void clear_term(){
      int i;
      for(i = 0; i < MAXCHAR; i++){
            display[i].character = 0;
      }
      tinfo.offset = 0;
      return;
}

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

void backspace(){
      tinfo.offset--;
      if(tinfo.offset > MAXCHAR){
            tinfo.offset = 0;
      }
      display[tinfo.offset].character = 0;
      return;
}

void tab(){
      tinfo.offset = (tinfo.offset / 10) + 10;
      if(tinfo.offset > MAXCHAR){
            scroll_term();
      }
      return;
}

void set_term_x(uint32_t x){
      if(x > TERMWIDTH){
            return;
      }
      tinfo.offset -= tinfo.offset % TERMWIDTH;
      tinfo.offset += x;
      return;
}

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
