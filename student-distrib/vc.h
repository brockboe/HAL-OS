#ifndef _VC_H
#define _VC_H
#include "types.h"

#define BUFFER_SIZE 128
#define VGA_WIDTH 80

void init_vc(void);
// void load_vc(uint8_t vc_id);
// void copy_vc(uint8_t vc_id);
// void rstr_vc(uint8_t vc_id);

int32_t vc_open(const uint8_t * filename);
int32_t vc_close(int32_t fd);
int32_t vc_read(uint32_t inode_index, uint32_t offset, uint8_t * buf, uint32_t nbytes);
int32_t vc_write(int32_t fd, const void * buf, int32_t n_bytes);

char * get_buffer();

void clr_buf();
void update_cursor(int x, int y);

<<<<<<< HEAD
int vc_active;
char terminals[3][BUFFER_SIZE];
=======
char vc_buffer[3][BUFFER_SIZE];


>>>>>>> checkpoint5/brock
#endif  /* _VC_H */
