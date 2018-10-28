#include "filesys.h"
#include "types.h"
#include "lib.h"
#include "syscall.h"
#include "vc.h"
#include "rtc.h"

PCB_t test_pcb;

int32_t read(int32_t fd, void* buf, int32_t n_bytes){
      /* TODO:
       * 1. Find the fd in the PCB
       * 2. Call the function in the file descriptor
       * 3. Return the number of bytes read
       */
       switch(fd){
             case 0:
                  return vc_read(buf, n_bytes);
             case 1:
                  return -1;
             default:{
                   int32_t retval;
                   retval = (test_pcb.fd[fd].operations_pointer)(0, test_pcb.fd[fd].inode, test_pcb.fd[fd].file_pos, (uint8_t *)buf, n_bytes);
                   test_pcb.fd[fd].file_pos += retval;
                   return retval;
             }
       }
}

int32_t write(int32_t fd, const void * buf, int32_t n_bytes){
      /* TODO:
       * 1. Find the fd in the PCB
       * 2. Call the function in the file descriptor
       * 3. Return the value of the associated function
       */
       switch(fd){
             case 0:
                  return -1;
             case 1:
                  return vc_write((uint8_t *)buf, n_bytes);
             default:
                  return (test_pcb.fd[fd].operations_pointer)(1, 0, 0, (uint8_t *)buf, n_bytes);
       }
}

int32_t open(const uint8_t * filename){
      /*TODO:
       * 1. Determine if file is RTC, Keyboard, Terminal, Directory, or File
       * 2. Find the first available space in the PCB to put the file descriptor
       * 3. Fill out the file descriptor, depending on the file type (see filesys.h for more info)
       * 4. Return the index of the file descriptor in the PCB
       */
       dentry_t temp_dentry;
       if(read_dentry_by_name(filename, &temp_dentry)){
             return -1;
       }

       test_pcb.fd[2].inode = temp_dentry.inode_num;
       test_pcb.fd[2].file_pos = 0;
       test_pcb.fd[2].flags = 0;

       switch(temp_dentry.file_type){
             case 0:
                  //RTC
                  (test_pcb.fd[2].operations_pointer) = rtc_io;
                  return 2;
             case 1:
                  //Directory
                  (test_pcb.fd[2].operations_pointer) = dir_io;
                  return 2;
             case 2:
                  //Regular File
                  (test_pcb.fd[2].operations_pointer) = file_io;
                  return 2;
             default:
                  return -2;
       }

       return 2;
}

int32_t close(int32_t fd){
      /* TODO:
       * 1. Fill the associated fd entry in the PCB with an "empty" value;
       */
       return 0;
}
