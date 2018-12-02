#ifndef _SYSCALL_H
#define _SYSCALL_H

#include "types.h"
#include "filesys.h"
#include "structures.h"
#include "paging.h"

#define PAGE_SIZE 1024
#define PAGING_SHIFT 12
#define MAX_CONCURRENT_TASKS 6
#define PROG_OFFSET 0x48000
#define _4MB 0x400000
#define _8MB 0x800000
#define _128MB 0x8000000
#define _132MB 0x8400000
#define _4KB 0x1000
#define _8KB 0x2000
#define _8KB_MASK 0xFFFFE000
#define MAX_FS 1023*4096


//dispatcher used for interrupt handling
int32_t syscall_dispatcher(uint32_t syscall_num, uint32_t arg1, uint32_t arg2, uint32_t arg3);

extern page_directory_t task_pd[MAX_CONCURRENT_TASKS];
extern PCB_t * task_pcb[MAX_CONCURRENT_TASKS];

extern op_jmp_table_t vc_op_table;


//returns the currently running process' PCB
PCB_t * get_pcb_ptr();

static inline int32_t execute(const uint8_t * command){
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

static inline int32_t read(int32_t fd, void * buf, int32_t n_bytes){
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

static inline int32_t write(int32_t fd, const void * buf, int32_t n_bytes){
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

static inline int32_t open(const uint8_t * filename){
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

static inline int32_t close(int32_t fd){
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

static inline int32_t halt(uint8_t status){
      int32_t retval;
      asm volatile("          \n\
            PUSHL %%EBX       \n\
            PUSHL %%ECX       \n\
            PUSHL %%EDX       \n\
            MOVL 8(%%EBP), %%EBX     \n\
            MOVL $1, %%EAX    \n\
            INT $0x80         \n\
            POPL %%EDX        \n\
            POPL %%ECX        \n\
            POPL %%EBX        \n\
            "
            :"=r" (retval)
      );
      return retval;
}

#endif
