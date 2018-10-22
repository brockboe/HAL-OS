/*int_handlers.h
 *This function contains code for the most basic handlers to be used
 *in MP3, almost entirely exception handlers. The exception handlers
 *produce a "Red Screen of Death" which clears the screen and produces
 *a scary error message listing the exception.
 */

#include "lib.h"
#include "i8259.h"
#include "rtc.h"

#define VIDEO 0xB8000
#define NUM_COLS 80
#define NUM_ROWS 25
#define RED 0xC

/*RSOD()
 *Clears the screen and produces a scary error saying what went wrong
 */
void RSOD(char * error){
      int screen_x, screen_y; /*Position on screen*/
      int i;  /*index for loops*/
      char * preface = "I'm sorry Dave, I'm afraid I can't do that"; /*Initial message*/
      static char* video_mem = (char *)VIDEO;   /*Pointer to vidmem*/

      clear();

      screen_x = 2;
      screen_y = 1;

      /*Print the preface*/
      for(i = 0; preface[i] != 0; i++){
            *(uint8_t *)(video_mem + ((NUM_COLS * screen_y + screen_x) << 1)) = preface[i];
            *(uint8_t *)(video_mem + ((NUM_COLS * screen_y + screen_x) << 1) + 1) = RED;
            screen_x++;
      }

      /*Move down a row*/
      screen_x = 2;
      screen_y = 3;

      /*Print the argument*/
      for(i = 0; error[i] != 0; i++){
            *(uint8_t *)(video_mem + ((NUM_COLS * screen_y + screen_x) << 1)) = error[i];
            *(uint8_t *)(video_mem + ((NUM_COLS * screen_y + screen_x) << 1) + 1) = RED;
            screen_x++;
      }
      return;
}


/*
 * The following functions are exception handlers that produce a RSOD when called.
 * They do nothing special and all have essentially the same funciton.
 */

void int_zero_handler(void){
      RSOD("EXCEPTION 0: DIVIDE BY ZERO ERROR");
}

void int_one_handler(void){
      RSOD("EXCEPTION 1: RESERVED - Intel Use only");
}

void int_two_handler(void){
      RSOD("EXCEPTION 2: NMI - non-maskable interrupt occured");
}

void int_three_handler(void){
      RSOD("EXCEPTION 3: BREAKPOINT  - INT 3 called");
}

void int_four_handler(void){
      RSOD("EXCEPTION 4: OVERFLOW ERROR - INTO instruction called");
}

void int_five_handler(void){
      RSOD("EXCEPTION 6: BOUND RANGE EXCEEDED - Check BOUND instructions");
}

void int_six_handler(void){
      RSOD("EXCEPTION 6: INVALID OPCODE - Check assembly code");
}

void int_seven_handler(void){
      RSOD("EXCEPTION 7: DEVICE NOT AVAILABLE");
}

void int_eight_handler(void){
      RSOD("EXCEPTION 8: DOUBLE FAULT");
}

void int_nine_handler(void){
      RSOD("EXCEPTION 9: COPROCESSOR SEGMENT OVERRUN");
}

void int_ten_handler(void){
      RSOD("EXCEPTION 10: INVALID TSS");
}

void int_eleven_handler(void){
      RSOD("EXCEPTION 11: SEGMENT NOT PRESENT");
}

void int_twelve_handler(void){
      RSOD("EXCEPTION 12: STACK-SEGMENT FAULT");
}

void int_thirteen_handler(void){
      RSOD("EXCEPTION 13: GENERAL PROTECTION FAULT");
}

void int_fourteen_handler(void){
      RSOD("EXCEPTION 14: PAGE FAULT");
}

void int_fifteen_handler(void){
      RSOD("EXCEPTION 15: assertion_failure() called");
}

void int_sixteen_handler(void){
      RSOD("EXCEPTION 16: FPU FLOATING-POINT ERROR");
}

void int_seventeen_handler(void){
      RSOD("EXCEPTION 17: ALIGNMENT CHECK");
}

void int_eighteen_handler(void){
      RSOD("EXCEPTION 18: MACHINE CHECK");
}

void int_nineteen_handler(void){
      RSOD("EXCEPTION 19: SIMD FLOATING-POINT EXCEPTION");
}

void int_twenty_handler(void){
      RSOD("EXCEPTION 20: VIRTUALIZATION EXCEPTION");
}

void int_thirty_handler(void){
      RSOD("EXCEPTION 30: SECURITY EXCEPTION");
}

/*default_handler is installed for all handlers and does nothing when called but returns*/

void default_handler(void){
      return;
}
