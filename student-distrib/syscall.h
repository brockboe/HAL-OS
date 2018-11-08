#include "types.h"
#include "filesys.h"
#include "structures.h"
#include "paging.h"

#define PAGE_SIZE 1024
#define PAGING_SHIFT 12

//dispatcher used for interrupt handling
int32_t syscall_dispatcher(uint32_t syscall_num, uint32_t arg1, uint32_t arg2, uint32_t arg3);
