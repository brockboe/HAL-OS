#include "filesys.h"
#include "types.h"
#include "lib.h"

uint32_t read(int32_t fd, void* buf, int32_t n_bytes){
      /* TODO:
       * 1. Find the fd in the PCB
       * 2. Call the function in the file descriptor
       * 3. Return the number of bytes read
       */
}

uint32_t write(int32_t fd, const void * buf, in32_t n_bytes){
      /* TODO:
       * 1. Find the fd in the PCB
       * 2. Call the function in the file descriptor
       * 3. Return the value of the associated function
       */
}

uint32_t open(const uint8_t * filename){
      /*TODO:
       * 1. Determine if file is RTC, Keyboard, Terminal, Directory, or File
       * 2. Find the first available space in the PCB to put the file descriptor
       * 3. Fill out the file descriptor, depending on the file type (see filesys.h for more info)
       * 4. Return the index of the file descriptor in the PCB
       */
}

uint32_t close(int32_t fd){
      /* TODO:
       * 1. Fill the associated fd entry in the PCB with an "empty" value;
       */
}
