#include "vc.h"
#include "lib.h"
#include "keyboard.h"

#define BUFFER_SIZE 128
#define VGA_WIDTH 80



uint8_t curr_vc_id; /*id of the current vc */
char vc_buffer[BUFFER_SIZE];


struct flags {
    unsigned int CLF;
    unsigned int ALTF;
    unsigned int SHIFTF;
};

typedef struct flags flags;

struct vc_info {
    flags vc_flags;
    unsigned int buf_idx;
    unsigned int x_pos;
    unsigned int y_pos;
};


typedef struct vc_info vc_info;



/*
 * init_vc
 * Description:
 * Input:
 * Output:
 * Side effects:
 * Return:
 */

void init_vc(void){
    cli();
    clear();
    update_cursor(0,0);
    vc_info new_vc;
    int i;

    // if(curr_vc_id < 8)
    //     curr_vc_id++;

    for(i = 0; i < BUFFER_SIZE; i++){
      vc_buffer[i] = '\0';
    }
    new_vc.buf_idx = 0;
    new_vc.x_pos = 0;
    new_vc.y_pos = 0;

    new_vc.vc_flags.CLF = 0;
    new_vc.vc_flags.ALTF = 0;
    new_vc.vc_flags.SHIFTF = 0;

    sti();

}
/*
 * vc_open
 * Description:
 * Input:
 * Output:
 * Side effects:
 * Return:
 */
int32_t vc_open(uint32_t fd, void * buf, uint32_t bytes){
  init_vc();
  return 0;
}
/*
 * vc_close
 * Description:
 * Input:
 * Output:
 * Side effects:
 * Return:
 */
int32_t vc_close(uint32_t fd, void * buf, uint32_t bytes){

  // if(curr_vc_id == 0)
  //     return -1;
  // curr_vc_id--;
  // if(curr_vc_id != 0)
  //     vc_load(curr_vc_id);
  return 0;

}
/*
 * vc_write
 * Description:
 * Input:
 * Output:
 * Side effects:
 * Return:
 */

int32_t vc_write(uint32_t fd, void * buf, uint32_t bytes){
  if(buf == NULL)
      return -1;
  int i;
  if (bytes > BUFFER_SIZE)
      bytes = BUFFER_SIZE; /* maximum nmber of bytes we can print to the screen */

  char * buffer = (char * )buf;


  cli();
  for(i = 0; i < bytes; i++){
      putc(buffer[i]);
  }
  sti();

  return 0;
}

/*
 * vc_read
 * Description:
 * Input:
 * Output:
 * Side effects:
 * Return:
 */

int32_t vc_read(uint32_t fd, void * buf, uint32_t bytes){

    if(buf == NULL)
        return -1;

    int i;
    if(bytes > BUFFER_SIZE)
        bytes = BUFFER_SIZE; /* maximum nmber of bytes we can read */

    char* buffer =  (char *)buf;

    for(i = 0; i < bytes; i++){
        buffer[i] = vc_buffer[i];
    }

    clr_buf();
    return 0;

}
/*
 * clr_buf
 * Description:
 * Input:
 * Output:
 * Side effects:
 * Return:
 */

void clr_buf(){
    int i;
    for(i = 0; i < BUFFER_SIZE; i++){
        vc_buffer[i] = '\0';
    }


}


// void load_vc(uint8_t vc_id){
//
// }
//
// void copy_vc(uint8_t vc_id){
//
// }
//
// void rstr_vc(uint8_t vc_id){
//
// }




/* REFERENCE: https://wiki.osdev.org/Text_Mode_Cursor */
void update_cursor(int x, int y)
{
	uint16_t pos = y * VGA_WIDTH + x;

	outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8_t) (pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));

}
