#include "types.h"

uint32_t read(int32_t fd, void* buf, int32_t n_bytes);

uint32_t write(int32_t fd, const void * buf, in32_t n_bytes);

uint32_t open(const uint8_t * filename);

uint32_t close(int32_t fd);
