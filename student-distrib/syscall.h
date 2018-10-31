#include "types.h"
#include "filesys.h"
#include "structures.h"

//read data from a file
int32_t read(int32_t fd, void* buf, int32_t n_bytes);

//write data into a file
int32_t write(int32_t fd, const void * buf, int32_t n_bytes);

//open a file
int32_t open(const uint8_t * filename);

//close a file
int32_t close(int32_t fd);
