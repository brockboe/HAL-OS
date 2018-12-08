#include "mouse.h"
#include "video.h"

static int poll;

void mouse_init(void){
      int8_t status;
      mouse_x = 0;
      mouse_y = 0;
      mouse_vx = 0;
      mouse_vy = 0;
      enable_irq(12);
      outb(0xA8, 0x64);                 //enable auxillary device (osdev said this couldn't hurt)
      outb(0x20, 0x64);                 //retrieve status byte
      status = inb(0x60);               //read status byte from keyboard
      write_to_mouse(status | 0x02, 0); //update status bit to enable irq_12
      write_to_mouse(0x47, 0);          //enable interrupts on mouse
      write_to_mouse(0xf4, 1);          //tell mouse to start sending data

      return;
}


void mouse_interrupt_handler(void){
      //cli();
      poll = inb(0x64);
      poll = poll & 0x20;
      if(poll != 0x20){
        send_eoi(12);
        return;
      }

      int8_t input = inb(0x60);
      int32_t x = inb(0x60);
      int32_t y = inb(0x60);


      clear_term();
      send_eoi(12);
    // enable_irq(12);
    //  sti();
      return;

}


/*  write_to_mouse(int8_t command, bool m_flag)
    DESCRIPTION: helper function to make writing commands less tedious
    PARAMETERS: int8_t command -- the command to be written to mouse/keyboard port
                bool m_flag    -- flag indicating whether the command is written to keyboard or mouse
    OUTPUTS: none
    INPUTS: none

*/

void write_to_mouse(int8_t command, int m_flag){
     if(!m_flag){
         outb(0x60, 0x64)   ; //tell mouse command to be written
         outb(command, 0x60);//write command to keyboard/mouse port
     }
     else{
        outb(0xD4, 0x64);    //write to mouse
        outb(command, 0x60);
     }
     return;
}
