.text
.globl init_control_reg
.globl directory_paging, paging_table

//cr0 mask: the bit that enabled paging from cr0 out
pg_cr0_mask:
      .long 0x80000000

//cr4 mask for pae : pae paging mechanism
pae_cr4_mask:
      .long 0xFFFFFFDF

//cr4 mask for pse : the bit that enable 4MB pages
pse_cr4_mask:
      .long 0x00000010

//cr4 mask for pge : the bit that enable global paging
pge_cr4_mask:
      .long 0x00000080

init_control_reg:
      //get the page directory location
      PUSHL %EAX
      MOVL 8(%ESP), %EAX
      MOVL %EAX, %CR3

      //set up correspodning bits in cr4 to enable 4MB size page, disable extension, and enable global paging setting
      MOVL %CR4, %EAX
      ANDL pae_cr4_mask, %EAX
      ORL pse_cr4_mask, %EAX
      ORL pge_cr4_mask, %EAX
      MOVL %EAX, %CR4

      //set the paging enbale bit in cr0 to 1 to enable paging
      MOVL %CR0, %EAX
      ORL pg_cr0_mask, %EAX
      MOVL %EAX, %CR0
      POPL %EAX
      ret


.align 4096
//allocate page directory with 1024 entries, each filled with everybit set except present bit
directory_paging:
      .rept 1024
      .long 0xFFFFFFFE
      .endr

.align 4096
//allocate page table with 1024 entries, each filled with everybit set except present bit
paging_table:
      .rept 1024
      .long 0xFFFFFFFE
      .endr
