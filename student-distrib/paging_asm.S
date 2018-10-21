.text
.globl init_control_reg
.globl directory_paging, paging_table
pg_cr0_mask:
      .long 0x80000000
pae_cr4_mask:
      .long 0xFFFFFFDF
pse_cr4_mask:
      .long 0x00000010
pge_cr4_mask:
      .long 0x00000040
init_control_reg:
      PUSHL %EAX
      MOVL $directory_paging, %EAX
      MOVL %EAX, %CR3
      MOVL %CR4, %EAX
      ANDL pae_cr4_mask, %EAX
      ORL pse_cr4_mask, %EAX
      ORL pge_cr4_mask, %EAX
      MOVL %EAX, %CR4
      MOVL %CR0, %EAX
      ORL pg_cr0_mask, %EAX
      MOVL %EAX, %CR0
      POPL %EAX
      ret
.align 4096
directory_paging:
      .rept 1024
      .long 0xFFFFFFFE
      .endr
.align 4096
paging_table:
      .rept 1024
      .long 0xFFFFFFFE
      .endr
