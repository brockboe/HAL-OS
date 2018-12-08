#include "mouse.h"
#include "video.h"

static int poll;
/*
 *  mouse_init(void)
 *  DESCRIPTION: initialize the mouse driver by setting the status bit,
 *  ensuring the enabling of aux devices and enabling the transfer of data.
 *  No inputs or outputs
 *
 */
void mouse_init(void){
      int8_t status;
      mouse_x = 0;
      mouse_y = 0;
      mouse_vx = 0;
      mouse_vy = 0;
      enable_irq(MOUSE_IRQ);
      outb(AUX_DEVICE_CMD, MK_COMMAND_PORT);                 //enable auxillary device (osdev said this couldn't hurt)
      outb(KEYBOARD_PORT, MK_COMMAND_PORT)   ;       //write to keyboard to set mouse status
      outb(SET_STATUS_INIT, KEYBOARD_PORT);          //write mouse status byte 0 | 0 | 1 | 0 | 0 | 1 | 1 | 1
      // ||Always 0 ||	mode ||	enable||	scaling ||	Always 0	|| left btn ||	middle ||	right btn||//

      outb(SEND_MOUSE_CMD, MK_COMMAND_PORT);         //write to mouse
      outb(ENABLE_SGN_CMD, KEYBOARD_PORT);           //enable mouse to send data

      return;
}
/*
 *  mouse_interrupt_handler(void)
 *  DESCRIPTION: Mouse interrupt handler
 *  TODO
 *
 */

void mouse_interrupt_handler(void){
      //cli();
      poll = inb(MK_COMMAND_PORT);
      poll = poll & MOUSE_CMD_CHECK;
      if(poll != MOUSE_CMD_CHECK){
        send_eoi(MOUSE_IRQ);
        return;
      }

      int8_t input = inb(KEYBOARD_PORT);
      int32_t x = inb(KEYBOARD_PORT);
      int32_t y = inb(KEYBOARD_PORT);


      clear_term();
      send_eoi(MOUSE_IRQ);
    // enable_irq(12);
    //  sti();
      return;

}
