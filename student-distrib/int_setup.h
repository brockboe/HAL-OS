/* int_setup.h
 * This header function provides definitions for interrupt-
 * related functions for use in kernel and other programs.
 */

 /*int_setup() initializes the IDT and sets up the exception handlers*/
void int_setup();

/*C_int_dispatcher(UL) is the file that calls the interrupt handlers*/
void C_int_dispatcher(unsigned long interrupt);

/*install_handler() installs a handler when given a pointer to a function*/
void install_handler(int vector_number, void handler());
