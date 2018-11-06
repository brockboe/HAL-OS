#include "x86_desc.h"
#include "filesys.h"
#include "types.h"
#include "lib.h"
#include "vc.h"
#include "rtc.h"
#include "structures.h"
#include "paging.h"
#include "syscall.h"
#include "video.h"


#define CMD_MAX_LEN 32
#define METADATA_LEN 40
#define X_MAGIC_1 0x7F
#define X_MAGIC_2 0x45
#define X_MAGIC_3 0x4C
#define X_MAGIC_4 0x46
#define MAX_CONCURRENT_TASKS 2
#define MAX_FS 1023*4096
#define VIDMEM 0x000B8000
#define PROG_OFFSET 0x48000
#define _4MB 0x400000
#define _8MB 0x800000
#define _128MB 0x8000000
#define _4KB 0x1000
#define _8KB 0x2000

#define USER_STACK_BEGIN 0x8400000 - 4

static page_directory_t task_pd[MAX_CONCURRENT_TASKS] __attribute__((aligned (_4KB)));
static PCB_t * task_pcb[6] = {(PCB_t *)(_8MB - 1 * _8KB),
                              (PCB_t *)(_8MB - 2 * _8KB)};

PCB_t test_pcb;

extern void context_switch(long user_ds, long user_sp, long user_cs, long entry_addr);

//general format for device-specific io:
//open(const uint8_t * filename)
//read(uint32_t inode_index, uint32_t offset, uint8_t * buf, uint32_t nbytes)
//write(int32_t fd, const void * buf, int32_t n_bytes)
//close(int32_t fd)

//Syscall enumeration:
// 1. Halt
// 2. execute
// 3. read
// 4. write
// 5. open
// 6. close
// 7. getargs
// 8. vidmap
// 9. set_handler
// 10. sigreturn

//file operations jump table
op_jmp_table_t file_op_table = { &file_open, &file_read, &file_write, &file_close };
//directory operations jump table
op_jmp_table_t dir_op_table = { &dir_open, &dir_read, &dir_write, &dir_close };
//rtc operations jump table
op_jmp_table_t rtc_op_table = { &rtc_open, &rtc_read, &rtc_write, &rtc_close };
//virtual console jump table
op_jmp_table_t vc_op_table = { &vc_open, &vc_read, &vc_write, &vc_close};


int32_t execute(const uint8_t * command){
      int32_t retval;
      asm volatile("          \n\
            PUSHL %%EBX       \n\
            PUSHL %%ECX       \n\
            PUSHL %%EDX       \n\
            MOVL 8(%%EBP), %%EBX     \n\
            MOVL $2, %%EAX    \n\
            INT $0x80         \n\
            POPL %%EDX        \n\
            POPL %%ECX        \n\
            POPL %%EBX        \n\
            "
            :"=r" (retval)
      );
      return retval;
}

int32_t read(int32_t fd, void * buf, int32_t n_bytes){
      int32_t retval;
      asm volatile("          \n\
            PUSHL %%EBX       \n\
            PUSHL %%ECX       \n\
            PUSHL %%EDX       \n\
            MOVL 8(%%EBP), %%EBX     \n\
            MOVL 12(%%EBP), %%ECX    \n\
            MOVL 16(%%EBP), %%EDX    \n\
            MOVL $3, %%EAX    \n\
            INT $0x80         \n\
            POPL %%EDX        \n\
            POPL %%ECX        \n\
            POPL %%EBX        \n\
            "
            :"=r" (retval)
      );
      return retval;
}

int32_t write(int32_t fd, const void * buf, int32_t n_bytes){
      int32_t retval;
      asm volatile("          \n\
            PUSHL %%EBX       \n\
            PUSHL %%ECX       \n\
            PUSHL %%EDX       \n\
            MOVL 8(%%EBP), %%EBX     \n\
            MOVL 12(%%EBP), %%ECX    \n\
            MOVL 16(%%EBP), %%EDX    \n\
            MOVL $4, %%EAX    \n\
            INT $0x80         \n\
            POPL %%EDX        \n\
            POPL %%ECX        \n\
            POPL %%EBX        \n\
            "
            :"=r" (retval)
            );
      return retval;
}

int32_t open(const uint8_t * filename){
      int32_t retval;
      asm volatile("          \n\
            PUSHL %%EBX       \n\
            PUSHL %%ECX       \n\
            PUSHL %%EDX       \n\
            MOVL 8(%%EBP), %%EBX     \n\
            MOVL $5, %%EAX    \n\
            INT $0x80         \n\
            POPL %%EDX        \n\
            POPL %%ECX        \n\
            POPL %%EBX        \n\
            "
            :"=r" (retval)
      );
      return retval;
}

int32_t close(int32_t fd){
      int32_t retval;
      asm volatile("          \n\
            PUSHL %%EBX       \n\
            PUSHL %%ECX       \n\
            PUSHL %%EDX       \n\
            MOVL 8(%%EBP), %%EBX     \n\
            MOVL $6, %%EAX    \n\
            INT $0x80         \n\
            POPL %%EDX        \n\
            POPL %%ECX        \n\
            POPL %%EBX        \n\
            "
            :"=r" (retval)
      );
      return retval;
}

int32_t execute_handler(const uint8_t * command){

      cli();

      //Seven Steps:
      // 1. Parse
      // 2. Executable check
      // 3. Paging
      // 4. User - level program loader
      // 5. Create PCB
      // 6. Context Switch

      //Vars for Parsing
      uint8_t cmd_name[CMD_MAX_LEN];     //Name of the command
      dentry_t cmd_dentry;
      int32_t cmd_inode;
      int i;
      int cmd_len;

      //vars for executable check
      uint8_t exe_dat[40];
      uint8_t x_magic[4] = {X_MAGIC_1, X_MAGIC_2, X_MAGIC_3, X_MAGIC_4};
      void * entry_address;

      //vars for paging setup
      int PID = -1;
      int PDE_index;

      //vars for context switch
      void * user_sp;

      //
      //Step One : Parse
      //

      //clear the cmd_len array
      for(i = 0; i < CMD_MAX_LEN; i++){
            cmd_name[i] = 0;
      }

      //grab the command
      for(i = 0; (i < CMD_MAX_LEN) && (command[i] != ' ') && (command[i] != '\n'); i++){
            cmd_name[i] = command[i];
      }

      //Begin searching for the file
      //First get the dentry
      if(read_dentry_by_name(cmd_name, &cmd_dentry)){
            return -1;
      }

      //ensure what we're executing is a file
      if(cmd_dentry.file_type != 2){
            return -1;
      }

      //then get the inode number
      cmd_inode = cmd_dentry.inode_num;

      //
      //Step Two : Executable Check
      //

      //Grab the first 40 bytes (executable metadata)
      file_read(cmd_inode, 0, exe_dat, 40);

      //Check the 4 magic numbers
      for(i = 0; i < 4; i++){
            if(exe_dat[i] != x_magic[i]){
                  return -2;
            }
      }

      //grab the entry address of the program
      entry_address = (void *)((exe_dat[27] << 24)|(exe_dat[26] << 16)|(exe_dat[25] << 8)|(exe_dat[24]));

      //
      //Step Three : Paging
      //

      //find the first available PCB
      for(i = 0; i < MAX_CONCURRENT_TASKS; i++){
            if(!task_pcb[i]->is_active){
                  PID = i;
                  break;
            }
      }

      //Check that there was room for that program
      if(PID == -1){
            return -1;
      }

      //set up the paging
      //set up the vid mem
      task_pd[PID].PDE[0] = directory_paging[0];
      //set up the kernel mem
      task_pd[PID].PDE[1] = directory_paging[1];
      //set up the program's 4mb page
      PDE_index = (int)(_128MB >> 22);
      page_directory_entry_4mb_t temp_pde;
      temp_pde.page_base_addr = (uint32_t)((_8MB + PID * _4MB) >> 22);
      temp_pde.present = 1;
      temp_pde.wr = 1;
      temp_pde.us = 1;
      temp_pde.write_through = 1;
      temp_pde.cached = 1;
      temp_pde.accessed = 0;
      temp_pde.paddling = 0;
      temp_pde.page_size = 1;
      temp_pde.g = 0;
      temp_pde.available = 0;
      temp_pde.pat = 0;
      temp_pde.reserved = 0;

      task_pd[PID].PDE[PDE_index] = (uint32_t)temp_pde.val;
      //set the CR3 register to match the new setup
      init_control_reg(&(task_pd[PID].PDE[0]));

      //
      //Step Four : User level program loader
      //

      read_data(cmd_inode, 0, (uint8_t *)(_128MB + PROG_OFFSET), MAX_FS);

      //
      //Step Five : Create PCB
      //

      task_pcb[PID]->PID = PID;
      task_pcb[PID]->is_active = 1;
      //set the fd's as empty
      task_pcb[PID]->fd[0].flags.in_use = 1;
      task_pcb[PID]->fd[0].actions = &vc_op_table;
      task_pcb[PID]->fd[1].flags.in_use = 1;
      task_pcb[PID]->fd[1].actions = &vc_op_table;
      task_pcb[PID]->fd[2].flags.in_use = 0;
      task_pcb[PID]->fd[3].flags.in_use = 0;
      task_pcb[PID]->fd[4].flags.in_use = 0;
      task_pcb[PID]->fd[5].flags.in_use = 0;


      //
      //Step Six : Context Switch
      //

      user_sp = (void *)(_128MB + _4MB - 4);

      //set up the TSS
      tss.ss0 =  KERNEL_DS;
      tss.esp0 = _8MB - (PID * _8KB) - 4;

      sti();

            //lower the privilege level using IRET
            asm volatile("                \n\
                  MOVW %0, %%AX           \n\
                  MOVW %%AX, %%DS         \n\
                  PUSHL %0                \n\
                  PUSHL %1                \n\
                  PUSHFL                  \n\
                  POPL %%EAX              \n\
                  ORL $0x200, %%EAX       \n\
                  PUSHL %%EAX             \n\
                  PUSHL %2                \n\
                  PUSHL %3                \n\
                  IRET                    \n\
                  "
                  :
                  :"g"(USER_DS), "g"(user_sp), "g"(USER_CS), "g"(entry_address)
                  :"%eax"
            );

      return 0;
}

/* read_handler
 * DESCRIPTION:   read takes a file descriptor as argument and reads a specific
 *                number of bytes from the associated file and places them into
 *                a buffer. Read virtualizes all the individual device drivers
 *                and allows a common interface
 * INPUTS:        fd - index into the file descriptor array in the PCB
 *                buf - the buffer to where the data is to be written
 *                n_bytes - the number of bytes to be written from the file
 * OUTPUTS:       returns the number of bytes written
 * SIDE EFFECTS:  copies n_bytes bytes from a file and places them into buf
 */
int32_t read_handler(int32_t fd, void* buf, int32_t n_bytes){
      /* TODO:
       * 1. Find the fd in the PCB
       * 2. Call the function in the file descriptor
       * 3. Return the number of bytes read
       */
       switch(fd){
             case 0:
                  //read from stdin
                  return vc_read(test_pcb.fd[fd].inode, test_pcb.fd[fd].file_pos, (uint8_t *)buf, n_bytes);
             case 1:
                  // "read" from standard out, ie, produce an error
                  return -1;
             default:{
                   //otherwise use the associated file handler in the file descriptor.
                   int32_t retval;
                   retval = (test_pcb.fd[fd].actions->dev_read)(test_pcb.fd[fd].inode, test_pcb.fd[fd].file_pos, (uint8_t *)buf, n_bytes);
                   test_pcb.fd[fd].file_pos += retval;
                   return retval;
             }
       }
}

/* write_handler
 * DESCRIPTION:   Writes n_bytes bytes of data from buf into location pointed
 *                to by the file descriptor.
 * INPUTS:        fd - index into the file descriptor array from the PCB
 *                buf - the source of the data
 *                n_bytes - the number of bytes to be written
 * OUTPUTS:       returns 0 on success, returns some other number on an error.
 * SIDE EFFECTS:  copies n_bytes bytes of data from buf into the source pointed
 *                to by fd.
 */
int32_t write_handler(int32_t fd, const void * buf, int32_t n_bytes){
      /* TODO:
       * 1. Find the fd in the PCB
       * 2. Call the function in the file descriptor
       * 3. Return the value of the associated function
       */
       switch(fd){
             case 0:
                  //"write" to standard intput, ie, produce an error
                  return -1;
             case 1:
                  //write to standard output, the console
                  return vc_write(fd, (const void *)buf, n_bytes);
             default:
                  //otherwise the associated file handler in the file descriptor
                  //(should return an error)
                  return (test_pcb.fd[fd].actions->dev_write)(fd, (const void *)buf, n_bytes);
       }
}

/* open_handler
 * DESCRIPTION:   Sets up a new file descriptor entry into the PCB when indexed
 *                by the name of the file.
 * INPUTS:        filename - character array of the name of the file
 * OUTPUTS:       returns the index into the PCB where the file descriptor lies
 * SIE EFFECTS:   Creats a new file descriptor and stores it inside the fd
 *                array of the PCB.
 */
int32_t open_handler(const uint8_t * filename){
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
       test_pcb.fd[2].flags.in_use = 1;

       switch(temp_dentry.file_type){
             case 0:
                  //RTC
                  (test_pcb.fd[2].actions) = &rtc_op_table;
                  return 2;
             case 1:
                  //Directory
                  (test_pcb.fd[2].actions) = &dir_op_table;
                  return 2;
             case 2:
                  //Regular File
                  (test_pcb.fd[2].actions) = &file_op_table;
                  return 2;
             default:
                  return -2;
       }

       return 2;
}

/*close_handler
 *Should remove the associated file descriptor from the fd array in the PCB
 *return 0;
 */
int32_t close_handler(int32_t fd){
      /* TODO:
       * 1. Fill the associated fd entry in the PCB with an "empty" value;
       */
       return 0;
}

int32_t syscall_dispatcher(uint32_t syscall_num, uint32_t arg1, uint32_t arg2, uint32_t arg3){
      switch(syscall_num){
            case 1:
                  asm volatile (".1: hlt; jmp .1;");
            case 2:
                  //system execute
                  return execute_handler((const uint8_t *)arg1);
            case 3:
                  //system read
                  return read_handler((int32_t)arg1, (void *)arg2, (int32_t)arg3);
            case 4:
                  //system write
                  return write_handler((int32_t)arg1, (const void *)arg2, (int32_t)arg3);
                  fill_color();
            case 5:
                  //system open
                  return open_handler((const uint8_t *)arg1);
            case 6:
                  //system close
                  return close_handler((int32_t)arg1);
            default:
                  return -2;
      }
}
