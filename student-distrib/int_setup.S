#int_setup.S
#This file provides an assembly linkage between the interrupt vector
#table and the C files found in interrupt.C. It contains common_interrupt,
#Which simply provides a link to the C functions, and a collection of
#small, 2-line functions that save the vector number (for later use) and
#then jump to common_interrupt.

.extern C_int_dispatcher

.text

.globl common_interrupt
.globl assembly_linkage
.globl default_linkage
.globl RTC, keyboard, SYSC, PIT, MOUSE

#This is where the interrupt number is saved so it can be pushed later
interrupt_num:
      .long 0

common_interrupt:
      #Save all the interrupts
      #PUSHL return address
      #PUSHL IRQ number

      #Save all the Registers
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

      #Switch to the kernel DS
      PUSHL %EAX
      MOVW $0x18, %AX
      MOVW %AX, %DS
      POPL %EAX

      #Call the dispatcher so we're now working in C
      CALL C_int_dispatcher

      #Restore all the registers
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
      #Pop off the argument
      ADDL $4, %ESP

      IRET

#The following functions are linkage functions that are jumped to
#whenever the associated IDT vector number is called. They push
#The associated vector as argument and then call common_interrupt
#which then further acts upon the interrupt.

_0:
      PUSHL $0
      JMP common_interrupt

_1:
      PUSHL $1
      JMP common_interrupt

_2:
      PUSHL $2
      JMP common_interrupt

_3:
      PUSHL $3
      JMP common_interrupt

_4:
      PUSHL $4
      JMP common_interrupt

_5:
      PUSHL $5
      JMP common_interrupt

_6:
      PUSHL $6
      JMP common_interrupt

_7:
      PUSHL $7
      JMP common_interrupt

_8:
      PUSHL $8
      JMP common_interrupt

_9:
      PUSHL $9
      JMP common_interrupt

_10:
      PUSHL $10
      JMP common_interrupt

_11:
      PUSHL $11
      JMP common_interrupt

_12:
      PUSHL $12
      JMP common_interrupt

_13:
      PUSHL $13
      JMP common_interrupt

_14:
      PUSHL $14
      JMP common_interrupt

_15:
      PUSHL $15
      JMP common_interrupt

_16:
      PUSHL $16
      JMP common_interrupt

_17:
      PUSHL $17
      JMP common_interrupt

_18:
      PUSHL $18
      JMP common_interrupt

_19:
      PUSHL $19
      JMP common_interrupt

_20:
      PUSHL $20
      JMP common_interrupt

_21:
      PUSHL $21
      JMP common_interrupt

_22:
      PUSHL $22
      JMP common_interrupt

_23:
      PUSHL $23
      JMP common_interrupt

_24:
      PUSHL $24
      JMP common_interrupt

_25:
      PUSHL $25
      JMP common_interrupt

_26:
      PUSHL $26
      JMP common_interrupt

_27:
      PUSHL $27
      JMP common_interrupt

_28:
      PUSHL $28
      JMP common_interrupt

_29:
      PUSHL $29
      JMP common_interrupt

_30:
      PUSHL $30
      JMP common_interrupt

default_linkage:
      PUSHL $256
      JMP common_interrupt

keyboard:
      PUSHL $0x21
      JMP common_interrupt

RTC:
      PUSHL $0x28
      JMP common_interrupt

SYSC:
      PUSHL $0x80
      JMP common_interrupt

PIT:
      PUSHL $0x20
      JMP common_interrupt

MOUSE:
      PUSHL $0x2C
      JMP common_interrupt

#This is a table containing pointers to the linkage functions
#that push the interrupt vector number and then jump to the
#common_interupt function.
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
