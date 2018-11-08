#ifndef _SYSCALL_LIB_H
#define _SYSCALL_LIB_H

#include "syscall.h"

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

#endif /*_SYSCALL_LIB_h*/
