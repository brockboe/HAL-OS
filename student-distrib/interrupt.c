/*interrupt.c
 *This file contains macros and definitions necessary for setting up the
 *IDT table, and installing and removing interrupt handlers. The functions in
 *this file work in conjunction with the assembly linkage found in int_setup.S
 */

#include "lib.h"
#include "types.h"
#include "debug.h"
#include "x86_desc.h"
#include "int_handlers.h"

/*Total number of Intel-Defined interrupts*/
#define NUM_INTEL_INTERRUPTS 30
/*Total number of possible interrupt vectors (even though most will be unused)*/
#define TOTAL_VECTOR_NUM 256

/*assembly_linkage is an array of functions that are called whenever
 *an intel-defined interrupt occurs. Every entry pushes the vector Number
 *and then calls common_interrupt
 */
extern void * assembly_linkage[NUM_INTEL_INTERRUPTS];

/*These are the linkage files for the keyboard and RTC*/
extern void keyboard();
extern void RTC();

/*defualt_linkage is an external function that pushes 256 and then
 *calls common_interrupt. Because the highest possible vector number
 *is 255, whenever default_linkage is called we can know with certanty
 *that this interrupt as not been set up yet.
 */
extern void default_linkage();

/*An array of pointers towards the functions that handle interrupts.
 *Whenever an interrupt is occured, this array is used to find the
 *handler associated with an interrupt vector.
 */

void (*handler_table[TOTAL_VECTOR_NUM+1])();

void install_idt_entry(int idt_offset, void handler());
void RSOD(char * error);
void install_handler(int vector_num, void handler());

/* int_setup()
 * This function initializes the IDT entries and sets up everything
 * necessary for the first 30 intel-defined exceptions. It takes no
 * arguments and returns nothing. It installs the necessary handlers
 * and IDT linkages.
 */
void int_setup(){
      int i;

      /*Ensure that the handler is filled with the default entry:
       *the default handler does nothing, is simply a "return"
       *statment that gives control back to the program
       */
      for(i = 0; i < TOTAL_VECTOR_NUM+1; i++){
            install_handler(i, default_handler);
      }

      /*Install the default linkage - pushes vector number 256
       *which does not exist. This handler in the handler_table
       *associated with vector 256 is left blank (NULL) so when
       *it is called, nothing happens and the interrupt ends quickly.
       *This value is meant to be overwritten whenever an interrupt
       *is installed on this IDT entry
       */
      for(i = 0; i < TOTAL_VECTOR_NUM; i++){
            install_idt_entry(i, &default_linkage);
      }

      install_idt_entry(0x21, &keyboard);
      install_idt_entry(0x28, &RTC);

      /*Set up the first few interrupt vectors, which are intel
       *defined faults, exceptions, and errors.
       */
      for(i = 0; i < NUM_INTEL_INTERRUPTS; i++){
            install_idt_entry(i, assembly_linkage[i]);
      }

      /*Install the intel handlers into the handler_table*/
      install_handler(0, int_zero_handler);
      install_handler(1, int_one_handler);
      install_handler(2, int_two_handler);
      install_handler(3, int_three_handler);
      install_handler(4, int_four_handler);
      install_handler(5, int_five_handler);
      install_handler(6, int_six_handler);
      install_handler(7, int_seven_handler);
      install_handler(8, int_eight_handler);
      install_handler(9, int_nine_handler);
      install_handler(10, int_ten_handler);
      install_handler(11, int_eleven_handler);
      install_handler(12, int_twelve_handler);
      install_handler(13, int_thirteen_handler);
      install_handler(14, int_fourteen_handler);
      install_handler(15, int_fifteen_handler);
      install_handler(16, int_sixteen_handler);
      install_handler(17, int_seventeen_handler);
      install_handler(18, int_eighteen_handler);
      install_handler(19, int_nineteen_handler);
      install_handler(20, int_twenty_handler);
      install_handler(30, int_thirty_handler);
      install_handler(0x28, RTC_RSOD);
}

/*C_int_Dispatcher is called whenever an interrupt occurs. The
 *linkage gives this function the vector number as argument so
 *this function then then calls the handler associated with this
 *interrupt vector
 */
void C_int_dispatcher(unsigned long interrupt_vector){
      handler_table[interrupt_vector]();
      return;
}

/*install_idt_entry()
 *This takes an idt vector number and a pointer to a handler (which
 * is an assembly linkage in our case) and creates an entry in the IDT
 * table.
 */
void install_idt_entry(int idt_offset, void handler()){
      SET_IDT_ENTRY(idt[idt_offset], handler);
      idt[idt_offset].seg_selector = KERNEL_CS;
      idt[idt_offset].reserved3 = 0;
      idt[idt_offset].reserved2 = 1;
      idt[idt_offset].reserved1 = 1;
      idt[idt_offset].size = 1;
      idt[idt_offset].reserved0 = 0;
      idt[idt_offset].dpl = 0;
      idt[idt_offset].present = 1;
      return;
}

/*install_handler automates the process of putting a function
 *into the handler_table
 */
void install_handler(int vector_num, void handler()){
      handler_table[vector_num] = handler;
      return;
}
