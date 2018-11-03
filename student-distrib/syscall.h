#include "types.h"
#include "filesys.h"
#include "structures.h"

//execute a program file
int32_t execute(const uint8_t * command);

//read data from a file
int32_t read(int32_t fd, void* buf, int32_t n_bytes);

//write data into a file
int32_t write(int32_t fd, const void * buf, int32_t n_bytes);

//open a file
int32_t open(const uint8_t * filename);

//close a file
int32_t close(int32_t fd);

//dispatcher used for interrupt handling
int32_t syscall_dispatcher(uint32_t syscall_num, uint32_t arg1, uint32_t arg2, uint32_t arg3);
