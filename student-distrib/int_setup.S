.extern C_int_dispatcher

.text

.globl common_interrupt
.globl assembly_linkage

interrupt_num:
      .long 0

common_interrupt:
      #Save all the interrupts
      #PUSHL return address
      #PUSHL IRQ number
      PUSHW %FS
      PUSHW $0
      PUSHW %ES
      PUSHW $0
      PUSHW %DS
      PUSHW $0
      PUSHL %EAX
      PUSHL %EBP
      PUSHL %EDI
      PUSHL %ESI
      PUSHL %EDX
      PUSHL %ECX
      PUSHL %EBX
      PUSHL interrupt_num
      #PUSHL linkage return address

      CALL C_int_dispatcher

      ADDL $4, %ESP
      POPL %EBX
      POPL %ECX
      POPL %EDX
      POPL %ESI
      POPL %EDI
      POPL %EBP
      POPL %EAX
      ADDL $2, %ESP
      POPW %DS
      ADDL $2, %ESP
      POPW %ES
      ADDL $2, %ESP
      POPW %FS

      IRET

_0:
      MOVL $0, interrupt_num
      JMP common_interrupt

_1:
      MOVL $1, interrupt_num
      JMP common_interrupt

_2:
      MOVL $2, interrupt_num
      JMP common_interrupt

_3:
      MOVL $3, interrupt_num
      JMP common_interrupt

_4:
      MOVL $4, interrupt_num
      JMP common_interrupt

_5:
      MOVL $5, interrupt_num
      JMP common_interrupt

_6:
      MOVL $6, interrupt_num
      JMP common_interrupt

_7:
      MOVL $7, interrupt_num
      JMP common_interrupt

_8:
      MOVL $8, interrupt_num
      JMP common_interrupt

_9:
      MOVL $9, interrupt_num
      JMP common_interrupt

_10:
      MOVL $10, interrupt_num
      JMP common_interrupt

_11:
      MOVL $11, interrupt_num
      JMP common_interrupt

_12:
      MOVL $12, interrupt_num
      JMP common_interrupt

_13:
      MOVL $13, interrupt_num
      JMP common_interrupt

_14:
      MOVL $14, interrupt_num
      JMP common_interrupt

_15:
      MOVL $15, interrupt_num
      JMP common_interrupt

assembly_linkage:
      .long _0
      .long _1
      .long _2
      .long _3
      .long _4
      .long _5
      .long _6
      .long _7
      .long _8
      .long _9
      .long _10
      .long _11
      .long _12
      .long _13
      .long _14
      .long _15
