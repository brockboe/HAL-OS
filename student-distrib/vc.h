#ifndef _VC_H
#define _VC_H
#include "types.h"

//
// typedef struct{
//   unsigned long osem;
//   unsigned long isem;
// } sems_t;

#define BUFFER_SIZE 128
#define VGA_WIDTH 80

void init_vc(void);
// void load_vc(uint8_t vc_id);
// void copy_vc(uint8_t vc_id);
// void rstr_vc(uint8_t vc_id);

int32_t vc_open(void * buf, uint32_t bytes);
int32_t vc_close(void * buf, uint32_t bytes);
int32_t vc_read(void * buf, uint32_t bytes);
int32_t vc_write(void * buf, uint32_t bytes);


void clr_buf();
void update_cursor(int x, int y);

volatile char vc_buffer[BUFFER_SIZE];


#endif  /* _VC_H */
