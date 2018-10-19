#include "lib.h"
#include "types.h"
#include "debug.h"
#include "x86_desc.h"

#define VIDEO 0xB8000
#define NUM_COLS 80
#define NUM_ROWS 25
#define RED 0xC

#define NUM_INTERRUPTS 16

extern void (*assembly_linkage[NUM_INTERRUPTS])();

void RSOD(char * error);
void install_interrupt(int vect_num, void handler());

void int_zero_handler(){
      RSOD("EXCEPTION 0: DIVIDE BY ZERO ERROR");
}

void int_one_handler(){
      RSOD("EXCEPTION 1: RESERVED - Intel Use only");
}

void int_two_handler(){
      RSOD("EXCEPTION 2: NMI - non-maskable interrupt occured");
}

void int_three_handler(){
      RSOD("EXCEPTION 3: BREAKPOINT  - INT 3 called");
}

void int_four_handler(){
      RSOD("EXCEPTION 4: OVERFLOW ERROR - INTO instruction called");
}

void int_five_handler(){
      RSOD("EXCEPTION 6: BOUND RANGE EXCEEDED - Check BOUND instructions");
}

void int_six_handler(){
      RSOD("EXCEPTION 6: INVALID OPCODE - Check assembly code");
}

void int_seven_handler(){
      RSOD("EXCEPTION 7: DEVICE NOT AVAILABLE");
}

void int_eight_handler(){
      RSOD("EXCEPTION 8: DOUBLE FAULT");
}

void int_nine_handler(){
      RSOD("EXCEPTION 9: COPROCESSOR SEGMENT OVERRUN");
}

void int_ten_handler(){
      RSOD("EXCEPTION 10: INVALID TSS");
}

void int_eleven_handler(){
      RSOD("EXCEPTION 11: SEGMENT NOT PRESENT");
}

void int_twelve_handler(){
      RSOD("EXCEPTION 12: STACK-SEGMENT FAULT");
}

void int_thirteen_handler(){
      RSOD("EXCEPTION 13: GENERAL PROTECTION FAULT");
}

void int_fourteen_handler(){
      RSOD("EXCEPTION 14: PAGE FAULT");
}

void int_fifteen_handler(){
      RSOD("EXCEPTION 15: assertion_failure() called");
}

void int_sixteen_handler(){
      RSOD("EXCEPTION 16: FPU FLOATING-POINT ERROR");
}

void int_seventeen_handler(){
      RSOD("EXCEPTION 17: ALIGNMENT CHECK");
}

void int_eighteen_handler(){
      RSOD("EXCEPTION 18: MACHINE CHECK");
}

void int_nineteen_handler(){
      RSOD("EXCEPTION 19: SIMD FLOATING-POINT EXCEPTION");
}

void int_setup(){
      int i;
      for(i=0; i<NUM_INTERRUPTS; i++){
            install_interrupt(i, *assembly_linkage[i]);
      }
}

void C_int_dispatcher(unsigned long interrupt){
      switch(interrupt){
            case 1: int_one_handler(); break;
            case 2: int_two_handler(); break;
            case 3: int_three_handler(); break;
            case 15: int_fifteen_handler(); break;
            default: break;
      }
      return;
}

void install_interrupt(int idt_offset, void handler()){
      SET_IDT_ENTRY(idt[idt_offset], handler);
      idt[idt_offset].seg_selector = KERNEL_CS;
      idt[idt_offset].reserved3 = 0;
      idt[idt_offset].reserved2 = 1;
      idt[idt_offset].reserved1 = 1;
      idt[idt_offset].size = 1;
      idt[idt_offset].reserved0 = 0;
      idt[idt_offset].dpl = 0;
      idt[idt_offset].present = 1;
      return;
}

void RSOD(char * error){
      int screen_x, screen_y;
      int i;
      char * preface = "I'm sorry Dave, I'm afraid I can't do that";
      static char* video_mem = (char *)VIDEO;

      clear();

      screen_x = 2;
      screen_y = 1;

      for(i = 0; preface[i] != 0; i++){
            *(uint8_t *)(video_mem + ((NUM_COLS * screen_y + screen_x) << 1)) = preface[i];
            *(uint8_t *)(video_mem + ((NUM_COLS * screen_y + screen_x) << 1) + 1) = RED;
            screen_x++;
      }

      screen_x = 2;
      screen_y = 3;

      for(i = 0; error[i] != 0; i++){
            *(uint8_t *)(video_mem + ((NUM_COLS * screen_y + screen_x) << 1)) = error[i];
            *(uint8_t *)(video_mem + ((NUM_COLS * screen_y + screen_x) << 1) + 1) = RED;
            screen_x++;
      }
      return;
}
