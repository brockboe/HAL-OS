.extern C_int_dispatcher

.text

.globl common_interrupt
.globl assembly_linkage
.globl default_linkage

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

_16:
      MOVL $16, interrupt_num
      JMP common_interrupt

_17:
      MOVL $17, interrupt_num
      JMP common_interrupt

_18:
      MOVL $18, interrupt_num
      JMP common_interrupt

_19:
      MOVL $19, interrupt_num
      JMP common_interrupt

_20:
      MOVL $20, interrupt_num
      JMP common_interrupt

_21:
      MOVL $21, interrupt_num
      JMP common_interrupt

_22:
      MOVL $22, interrupt_num
      JMP common_interrupt

_23:
      MOVL $23, interrupt_num
      JMP common_interrupt

_24:
      MOVL $24, interrupt_num
      JMP common_interrupt

_25:
      MOVL $25, interrupt_num
      JMP common_interrupt

_26:
      MOVL $26, interrupt_num
      JMP common_interrupt

_27:
      MOVL $27, interrupt_num
      JMP common_interrupt

_28:
      MOVL $28, interrupt_num
      JMP common_interrupt

_29:
      MOVL $29, interrupt_num
      JMP common_interrupt

_30:
      MOVL $30, interrupt_num
      JMP common_interrupt

default_linkage:
      MOVL $256, interrupt_num
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
      .long _16
      .long _17
      .long _18
      .long _19
      .long _20
      .long _21
      .long _22
      .long _23
      .long _24
      .long _25
      .long _26
      .long _27
      .long _28
      .long _29
      .long _30
