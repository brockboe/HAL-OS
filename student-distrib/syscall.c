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
#include "term_sched.h"


#define CMD_MAX_LEN 32
#define METADATA_LEN 40
#define X_MAGIC_1 0x7F
#define X_MAGIC_2 0x45
#define X_MAGIC_3 0x4C
#define X_MAGIC_4 0x46
#define VIDMEM 0x000B8000

#define USER_STACK_BEGIN 0x8400000 - 4
#define VID_MEM_PD 33 // (132 MB / 4MB)


page_directory_t task_pd[MAX_CONCURRENT_TASKS] __attribute__((aligned (_4KB)));
PCB_t * task_pcb[MAX_CONCURRENT_TASKS] = {(PCB_t *)(_8MB - 2 * _8KB),
                                          (PCB_t *)(_8MB - 3 * _8KB),
                                          (PCB_t *)(_8MB - 4 * _8KB),
                                          (PCB_t *)(_8MB - 5 * _8KB),
                                          (PCB_t *)(_8MB - 6 * _8KB),
                                          (PCB_t *)(_8MB - 7 * _8KB)};

static uint32_t vidmap_pt[1024] __attribute__((aligned (_4KB)));

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

PCB_t * get_pcb_ptr(){
      PCB_t * pcb;
      asm volatile("                \n\
            MOVL %%ESP, %%EAX       \n\
            ANDL %1, %%EAX          \n\
            MOVL %%EAX, %0          \n\
            "
            :"=r"(pcb)
            :"g"(_8KB_MASK)
            :"%eax"
      );
      return pcb;
}

int32_t halt_handler(uint8_t status){
      PCB_t * current_pcb;
      cli();
      current_pcb = get_pcb_ptr();

      //ensure we don't close the base shells
      if(current_pcb->PID < 3){
            return 0;
      }

      current_pid[current_display] = current_pcb->parent_pcb->PID;

      //Set all the file descriptors to open
      task_pcb[current_pcb->PID]->fd[0].flags.in_use = 0;
      task_pcb[current_pcb->PID]->fd[1].flags.in_use = 0;
      task_pcb[current_pcb->PID]->fd[2].flags.in_use = 0;
      task_pcb[current_pcb->PID]->fd[3].flags.in_use = 0;
      task_pcb[current_pcb->PID]->fd[4].flags.in_use = 0;
      task_pcb[current_pcb->PID]->fd[5].flags.in_use = 0;
      task_pcb[current_pcb->PID]->fd[6].flags.in_use = 0;
      task_pcb[current_pcb->PID]->fd[7].flags.in_use = 0;

      //restore the TSS
      tss.ss0 = current_pcb->parent_pcb->ss0;
      tss.esp0 = current_pcb->parent_pcb->esp0;

      //set the process as inactive
      current_pcb->is_active = 0;

      //Reset the paging to the parent's page
      init_control_reg(&(task_pd[current_pcb->parent_pcb->PID].PDE[0]));

      sti();

      //jump to the end of the execute function and return our value
      asm volatile("                \n\
            MOVL %1, %%EBP          \n\
            XORL %%EAX, %%EAX       \n\
            MOVB %0, %%AL          \n\
            JMP execute_return      \n\
            "
            :
            :"r"(status), "g"(current_pcb->parent_pcb->EBP)
            :"%eax"
      );

      //we shouldn't get here because of the JMP instruction
      return -1;

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
      int cmd_len = 0;

      //vars for executable check
      uint8_t exe_dat[40];
      uint8_t arg_dat[128];
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
            cmd_len++;
      }

      //clear the argument string
      for(i = 0; i < 128; i++){
            arg_dat[i] = 0;
      }
      //check if arguments are present
      //if arguments are present, grab them
      if(command[cmd_len] == ' '){
            //increment cmd_len so we don't grab the space
            cmd_len++;
            //grab the arguments
            for(i = 0; command[i + cmd_len] != '\n' && command[i + cmd_len] != '\0'; i++){
                  arg_dat[i] = command[i + cmd_len];
            }
      }

      //check if the command was simply an enter press
      if(cmd_name[0] == 0){
            return 0;
      }

      //Check to see if we need to kill the terminal (quit command = kill term)
      if(!stringcompare((uint8_t *)cmd_name, (uint8_t *)"quit", 4)){
            (void)halt(0);
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
                  return -1;
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

      //load that PID into the current_PID
      current_pid[current_display] = PID;

      // Store arg_data into pcb argbuf variable
      strcpy((int8_t*)task_pcb[PID]->argbuf, (const int8_t*)arg_dat);

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
      task_pcb[PID]->parent_pcb = get_pcb_ptr();

      //set the fd's as empty
      task_pcb[PID]->fd[0].flags.in_use = 1;
      task_pcb[PID]->fd[0].actions = &vc_op_table;
      task_pcb[PID]->fd[1].flags.in_use = 1;
      task_pcb[PID]->fd[1].actions = &vc_op_table;
      task_pcb[PID]->fd[2].flags.in_use = 0;
      task_pcb[PID]->fd[3].flags.in_use = 0;
      task_pcb[PID]->fd[4].flags.in_use = 0;
      task_pcb[PID]->fd[5].flags.in_use = 0;
      task_pcb[PID]->fd[6].flags.in_use = 0;
      task_pcb[PID]->fd[7].flags.in_use = 0;

      //set the parent EBP
      asm volatile("                \n\
            MOVL %%EBP, %0          \n\
            "
            : "=r"(task_pcb[PID]->parent_pcb->EBP)
      );

      task_pcb[PID]->parent_pcb->ss0 = tss.ss0;
      task_pcb[PID]->parent_pcb->esp0 = tss.esp0;

      //
      //Step Six : Context Switch
      //

      user_sp = (void *)(_128MB + _4MB - 4);

      //set up the TSS

      tss.ss0 =  KERNEL_DS;
      tss.esp0 = _8MB - ((PID+1) * _8KB) - 4;

            sti();

                  //lower the privilege level using IRET
                  asm volatile("                \n\
                        MOVW %2, %%AX           \n\
                        MOVW %%AX, %%DS         \n\
                        PUSHL %0                \n\
                        PUSHL %1                \n\
                        PUSHFL                  \n\
                        PUSHL %2                \n\
                        PUSHL %3                \n\
                        IRET                    \n\
                        execute_return:         \n\
                        LEAVE                   \n\
                        RET                     \n\
                        "
                        :
                        :"g"(USER_DS), "g"(user_sp), "g"(USER_CS), "g"(entry_address)
                        :"%eax"
                  );

      return -1;
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

       //check to ensure the current fd is in use
       PCB_t * curr_pcb = get_pcb_ptr();

       if(curr_pcb->fd[fd].flags.in_use == 0){
             return -1;
       }

       if(fd < 0 || fd > 7){
             return -1;
       }

       switch(fd){
             case 0:
                  //read from stdin
                  return vc_read(curr_pcb->fd[fd].inode, curr_pcb->fd[fd].file_pos, (uint8_t *)buf, n_bytes);
             case 1:
                  // "read" from standard out, ie, produce an error
                  return -1;
             default:{
                   //otherwise use the associated file handler in the file descriptor.
                   int32_t retval;
                   retval = (curr_pcb->fd[fd].actions->dev_read)(curr_pcb->fd[fd].inode, curr_pcb->fd[fd].file_pos, (uint8_t *)buf, n_bytes);
                   curr_pcb->fd[fd].file_pos += retval;
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

       //check to ensure the current fd is in use
       PCB_t * curr_pcb = get_pcb_ptr();

       if(curr_pcb->fd[fd].flags.in_use == 0){
             return -1;
       }

       if(fd < 0 || fd > 7){
             return -1;
       }

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
                  return (curr_pcb->fd[fd].actions->dev_write)(fd, (const void *)buf, n_bytes);
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
       int i;
       int fd_index = -1;

       //get the pcb pointer
       PCB_t * pcb;
       pcb = get_pcb_ptr();

       //check for null pointer
       if(filename == NULL || *filename == '\0'){
             return -1;
       }

       dentry_t temp_dentry;
       if(read_dentry_by_name(filename, &temp_dentry)){
             return -1;
       }

       //grab the first available file descriptor
       for(i = 2; i < 8; i++){
             if(pcb->fd[i].flags.in_use == 0){
                   fd_index = i;
                   break;
             }
       }

       //check if there was an empty spot
       if(fd_index == -1){
             return -1;
       }

       pcb->fd[fd_index].inode = temp_dentry.inode_num;
       pcb->fd[fd_index].file_pos = 0;
       pcb->fd[fd_index].flags.in_use = 1;

       switch(temp_dentry.file_type){
             case 0:
                  //RTC
                  (pcb->fd[fd_index].actions) = &rtc_op_table;
                  return fd_index;
             case 1:
                  //Directory
                  (pcb->fd[fd_index].actions) = &dir_op_table;
                  return fd_index;
             case 2:
                  //Regular File
                  (pcb->fd[fd_index].actions) = &file_op_table;
                  return fd_index;
             default:
                  return -2;
       }

       return fd_index;
}

/*close_handler
 *Should remove the associated file descriptor from the fd array in the PCB
 *return 0;
 */
int32_t close_handler(int32_t fd){
      /* TODO:
      * 1. Fill the associated fd entry in the PCB with an "empty" value;
      */
      PCB_t * pcb;
      pcb = get_pcb_ptr();
      if(fd < 0 || fd > 7){
            return -1;
      }
      else if(fd == 0 || fd == 1){
            return -1;
      }
      else if(pcb->fd[fd].flags.in_use != 0){
            pcb->fd[fd].flags.in_use = 0;
            return 0;
      }
      return -1;
}

/* getargs handler
 * description: puts arguments into the user level buffer
 * input: user level buffer and nbytes
 * output: 0 if successful, -1 if unsuccessful
 *
 */
int32_t getargs_handler(void * buf, int32_t n_bytes) {
    // Check invalid inputs
    if(n_bytes <= 0 || buf == NULL)
    {
        return -1;
    }

    // Get current pcb pointer to modify argbuf
    PCB_t * curr_pcb = get_pcb_ptr();

    //check if there are no arguments present
    if(*curr_pcb->argbuf == '\0'){
          return -1;
    }

    // Check to see if argbuf size matches n_bytes to be copied
    if(n_bytes < strlen((int8_t *) curr_pcb->argbuf) + 1)
    {
        return -1;
    }

    // Copy the argbuf in the pcb to the buf
    memcpy((void*) buf, (void *)curr_pcb->argbuf, n_bytes);

    // Successfully got args, so return 0
    return 0;


}

/* vidmap_handler
 * description: maps text-mode video memory into user space at _132MB
 * input: screen_start is memory location provided by caller
 * output: none
 * side effects: adds 4kb entry to page directory
 * return: _132MB is returned on success; -1 on failure
 */
int32_t vidmap_handler(uint8_t ** screen_start){
      // if(screen_start == NULL)
      //     return -1; moved NULL check to syscall_dispatcher - presumably fine to do (delete this after syserr check)

      PCB_t * curr_pcb = get_pcb_ptr();
      uint32_t pid = curr_pcb->PID;

      //Ensure the pointer is not NULL and is in bounds
      if(screen_start == NULL){
            return -1;
      }

      //check that pointer is within bounds
      if((int)screen_start < _128MB || (int)screen_start >= _128MB + _4MB){
            return -1;
      }

      //set up the page table
      page_directory_entry_4kb_t temp;
      temp.table_base_addr = ((uint32_t)vidmap_pt) >> 12;
      temp.available = 0;
      temp.g = 0;
      temp.page_size = 0;
      temp.accessed = 0;
      temp.cached = 0;
      temp.write_through = 0;
      temp.us = 1;
      temp.wr = 1;
      temp.present = 1;

      //add the page table to the page directory
      task_pd[pid].PDE[_132MB >> 22] = temp.val;

      //add an entry to the page table
      page_table_entry_t temp_pte;
      temp_pte.physical_page_addr = VIDMEM >> 12;
      temp_pte.available = 0;
      temp_pte.global = 0;
      temp_pte.cached = 0;
      temp_pte.us = 1;
      temp_pte.wr = 1;
      temp_pte.present = 1;

      //insert the entry into the page table
      vidmap_pt[((_132MB >> 12) & 0x03FF)]  = temp_pte.val;

      *screen_start = (uint8_t *)_132MB;

      return 0;
}

/*set_handler
 * 0 on success, -1 if fails
 */
int32_t set_handler() {
      return -1;
 }

 /*sigreturn_handler
  * 0 on success, -1 if fails
  */
int32_t sigreturn_handler() {
      return -1;
}

int32_t syscall_dispatcher(uint32_t syscall_num, uint32_t arg1, uint32_t arg2, uint32_t arg3){
      switch(syscall_num){
            case 1:
                  //system halt
                  return halt_handler((uint8_t)arg1);
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
            case 7:
                  //system getargs
                  return getargs_handler((void *)arg1, (int32_t)arg2);
            case 8:
                  //vidmap
                  if(arg1 == NULL)
                        return -1;
                  return vidmap_handler((uint8_t **) arg1);
            case 9:
                  // system set_handler
                  return set_handler();
            case 10:
                  //system sigreturn
                  return sigreturn_handler();
            default:
                  return -1;
      }
}
