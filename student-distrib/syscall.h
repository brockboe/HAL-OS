#include "types.h"
#include "filesys.h"

typedef struct PCB {
      file_descriptor_t fd[6];
} PCB_t;

int32_t read(int32_t fd, void* buf, int32_t n_bytes);

int32_t write(int32_t fd, const void * buf, int32_t n_bytes);

int32_t open(const uint8_t * filename);

int32_t close(int32_t fd);
