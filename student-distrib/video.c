#include "video.h"
#include "lib.h"

vid_data_t * display;
terminal_info_t tinfo;

void vid_init(){
      int i;
      display = (vid_data_t *)VIDMEM;
      tinfo.offset = 0;

      for(i = 0; i < MAXCHAR; i++){
            display[i].highbits = 7;
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
      if(tinfo.offset >= MAXCHAR){
            scroll_term();
      }
      if(a == '\n'){
            tinfo.offset -= tinfo.offset % TERMWIDTH;
            tinfo.offset += TERMWIDTH;
      }
      display[tinfo.offset].character = a;
      tinfo.offset++;
}

void backspace(){
      tinfo.offset--;
      display[tinfo.offset].character = 0;
      return;
}
