#include "lib.h"
#include "types.h"
#include "structures.h"
#include "paging.h"
#include "video.h"
#include "x86_desc.h"
#include "syscall.h"
#include "filesys.h"
#include "i8259.h"

volatile int current_display;
volatile int current_pid[3];
volatile int running_display;
volatile int flag_for_term_change = -1;

void init_terms(){
      current_display = 0;
      running_display = -1;

      current_pid[0] = 0;
      current_pid[1] = 1;
      current_pid[2] = 2;
      return;
}

void task_switch(int PID){
      cli();

      //Task switching is comprised of the following steps:
      // 1. Save old process' EPB, ESP, and TSS
      // 2. Switch the paging for the new process
      // 3. Switch the video memory
      // 4. Load the new process' TSS, EBP, and ESP
      // 5. Restore these variables
      // 6. LEAVE and RET to start running the new process

      //
      // 1. SAVE THE OLD PROCESS' EBP, ESP, AND TSS
      //

      //get a pointer to the old PCB
      PCB_t * old_pcb = get_pcb_ptr();

      //save the old EBP, SS0, and ESP0

      asm volatile("                \n\
            MOVL %%EBP, %0          \n\
            "
            : "=r"(old_pcb->EBP)
      );
      old_pcb->ss0 = tss.ss0;
      old_pcb->esp0 = tss.esp0;

      //
      // 2. Switch paging for the new process
      //

      init_control_reg(&(task_pd[PID].PDE[0]));

      //
      // 3. Switch video memory
      //

      page_table_entry_t temp_pte;
      int pte_idx;

      pte_idx = (VIDMEM >> 12) & 0x03FF;
      temp_pte.val = paging_table[pte_idx];

      if(running_display == current_display){
            temp_pte.physical_page_addr = VIDMEM >> 12;
      }
      else{
            temp_pte.physical_page_addr = (_3MB + _4KB*(running_display)) >> 12;
      }

      paging_table[pte_idx] = temp_pte.val;

      flush_tlb();

      //
      // 4. Load the new process' TSS, EBP, and ESP
      //

      //get a pointer to the new process' PCB
      PCB_t * new_pcb = task_pcb[PID];

      //
      // 5. Store the new process' TSS, EBP, and ESP
      //

      tss.ss0 = new_pcb->ss0;
      tss.esp0 = new_pcb->esp0;
      asm volatile("          \n\
            MOVL %0, %%EBP    \n\
            "
            :
            : "g"(new_pcb->EBP)
      );

      //
      // 7. LEAVE & RET
      //

      asm volatile("          \n\
            STI               \n\
            LEAVE             \n\
            RET               \n\
            "
      );

      //We should never get here because of LEAVE & RET, but we'll put a
      //return here, just to be safe!
      return;
}

// #define VIDMEM 0x000B8000

// for pre :
// get the pointer to the current video memory address  [x]
// copy the content to the correct offset place, using write or copy string [x]
// change the pt entry to the corresponding locations
// update the cursor location

// for next :
// get the pointer to the next virtual vid mem
// copy the content of that vid mem to the real vid mem
// change the pt entry to the real one
//update the cursor location

void vidchange(int from, int to){
      cli();
      page_table_entry_t temp_pte;
      page_table_entry_t backup;
      int pte_idx = (VIDMEM >> 12) & 0x03FF;
      temp_pte.val = paging_table[pte_idx];
      backup.val = paging_table[pte_idx];

      //restore the paging structures to their original values
      temp_pte.physical_page_addr = (VIDMEM >> 12);
      paging_table[pte_idx] = temp_pte.val;

      flush_tlb();

      // 1. Save the current video memory in the correct location
      (void)memcpy((void *)(_3MB + (from)*_4KB), (void *)VIDMEM, _4KB);

      // 2. Load the new terminals saved video memory into the real video memory
      (void)memcpy((void *)VIDMEM, (void *)(_3MB + (to)*_4KB), _4KB);

      // 3. Update the cursor position
      move_cursor();

      flush_tlb();
      sti();

     return;
}

void setup_shells(){
      dentry_t temp_dentry;
      uint8_t entry_bytes[4];
      void * entry_point;
      int PID;
      (void)read_dentry_by_name((uint8_t *)"shell", &temp_dentry);
      (void)file_read(temp_dentry.inode_num, 24, entry_bytes, 4);
      entry_point = (void *)((entry_bytes[3] << 24)|(entry_bytes[2] << 16)|(entry_bytes[1] << 8)|(entry_bytes[0]));

      init_terms();

      for(PID = 0; PID <= 3; PID++){
            //set up the paging
            //set up the vid mem
            task_pd[PID].PDE[0] = directory_paging[0];
            //set up the kernel mem
            task_pd[PID].PDE[1] = directory_paging[1];
            //set up the program's 4mb page
            int PDE_index = (int)(_128MB >> 22);
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

            read_data(temp_dentry.inode_num, 0, (uint8_t *)(_128MB + PROG_OFFSET), MAX_FS);

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

            task_pcb[PID]->ss0 = KERNEL_DS;
            task_pcb[PID]->esp0 = _8MB - ((PID+1) * _8KB) - 4;
            void * user_sp = (void *)(_128MB + _4MB - 4);

            asm volatile("                      \n\
                  MOVL %%ESP, %%EAX             \n\
                  MOVL %%EBP, %%EBX             \n\
                  MOVL %5, %%ESP                \n\
                  PUSHL %1                      \n\
                  PUSHL %2                      \n\
                  PUSHFL                        \n\
                  PUSHL %3                      \n\
                  PUSHL %4                      \n\
                  PUSHL $run_iret               \n\
                  PUSHL %4                      \n\
                  MOVL %%ESP, %0                \n\
                  MOVL %%EAX, %%ESP             \n\
                  MOVL %%EBX, %%EBP             \n\
                  JMP skip_iret                 \n\
                  run_iret:                     \n\
                  IRET                          \n\
                  skip_iret:                    \n\
                  "
                  : "=g"(task_pcb[PID]->EBP)
                  : "g"(USER_DS), "g"(user_sp), "g"(USER_CS), "g"(entry_point), "g"(task_pcb[PID]->esp0)
                  : "eax", "ebx"
            );

      }

      return;
}

void prep_term_with_command(uint8_t * command, int term_number){
      cli();
      dentry_t temp_dentry;
      uint8_t entry_bytes[4];
      void * entry_point;
      int PID = term_number;
      (void)read_dentry_by_name(command, &temp_dentry);
      (void)file_read(temp_dentry.inode_num, 24, entry_bytes, 4);
      entry_point = (void *)((entry_bytes[3] << 24)|(entry_bytes[2] << 16)|(entry_bytes[1] << 8)|(entry_bytes[0]));

      //set up the paging
      //set up the vid mem
      task_pd[PID].PDE[0] = directory_paging[0];
      //set up the kernel mem
      task_pd[PID].PDE[1] = directory_paging[1];
      //set up the program's 4mb page
      int PDE_index = (int)(_128MB >> 22);
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

      read_data(temp_dentry.inode_num, 0, (uint8_t *)(_128MB + PROG_OFFSET), MAX_FS);

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

      task_pcb[PID]->ss0 = KERNEL_DS;
      task_pcb[PID]->esp0 = _8MB - ((PID+1) * _8KB) - 4;
      void * user_sp = (void *)(_128MB + _4MB - 4);

      asm volatile("                      \n\
            STI                           \n\
            MOVL %%ESP, %%EAX             \n\
            MOVL %%EBP, %%EBX             \n\
            MOVL %5, %%ESP                \n\
            PUSHL %1                      \n\
            PUSHL %2                      \n\
            PUSHFL                        \n\
            PUSHL %3                      \n\
            PUSHL %4                      \n\
            PUSHL $run_iret_test          \n\
            PUSHL %4                      \n\
            MOVL %%ESP, %0                \n\
            MOVL %%EAX, %%ESP             \n\
            MOVL %%EBX, %%EBP             \n\
            JMP skip_iret_test            \n\
            run_iret_test:                \n\
            IRET                          \n\
            skip_iret_test:               \n\
            "
            : "=g"(task_pcb[PID]->EBP)
            : "g"(USER_DS), "g"(user_sp), "g"(USER_CS), "g"(entry_point), "g"(task_pcb[PID]->esp0)
            : "eax", "ebx"
      );

      return;
}
