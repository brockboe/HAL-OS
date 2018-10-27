#ifndef _VC_H
#define _VC_H
#include "types.h"

//
// typedef struct{
//   unsigned long osem;
//   unsigned long isem;
// } sems_t;



void init_vc(void);
// void load_vc(uint8_t vc_id);
// void copy_vc(uint8_t vc_id);
// void rstr_vc(uint8_t vc_id);

int32_t vc_open(uint32_t fd, void * buf, uint32_t bytes);
int32_t vc_close(uint32_t fd, void * buf, uint32_t bytes);
int32_t vc_read(uint32_t fd, void * buf, uint32_t bytes);
int32_t vc_write(uint32_t fd, void * buf, uint32_t bytes);


void clr_buf();
void update_cursor(int x, int y);







#endif  /* _VC_H */
