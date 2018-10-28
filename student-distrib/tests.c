#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "keyboard.h"
#include "rtc.h"
#include "filesys.h"
#include "syscall.h"
#include "vc.h"
#include "video.h"

#define PASS 1
#define FAIL 0
#define VIDMEM_ADDR		0xB8000
#define KMEM_ADDR			0x400000

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}


/* Checkpoint 1 tests */

/* IDT Test - Example
 *
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test(){
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < 256; ++i){
		if ((idt[i].offset_15_00 == NULL) &&
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}

/*  rtc_test  - for showing paging structure are populatd
 *
 *  increment rtc_count to prove functionality and proper initialization
 *  Inputs : None
 *  Output : PASS
 *  Side Effects : None
 *  Coverage : proves rtc works
 *  Files : rtc.c/ rtc.h
*/
int rtc_test(){
	TEST_HEADER;
	while(rtc_count < 20);

	return PASS;
}

/*  paging_test  - for showing paging structure are populatd
 *
 *  check entries for Paging Directory and Paging table that are not NULL
 *  Inputs : None
 *  Output : PASS
 *  Side Effects : None
 *  Coverage : Initialize Paging, Enabling Paging, Paging defination
 *  Files : paging.c/ paging.h
*/

int paging_test(){
	TEST_HEADER;

	int i;
	int* j;

	/* First set j to video memory start address */
	j = (int*)(VIDMEM_ADDR);
	i = *j;

	/* Set j to kernel memory start address */
	j = (int*)(KMEM_ADDR);
	i = *j;

	return PASS;
}

/*  exception_test  - checks exceptions in IDT table
 *
 *  Will trigger two different exceptions to prove proper IDT init
 *  Inputs : None
 *  Output : FAIL
 *  Side Effects : None
 *  Coverage : proves IDT is properly initialized
 *  Files : interrupt.c
*/
void exception_test() {
	int var1 = 0;
	int zero = 0;

	/* trigger a divide by zero exception */
	var1 = 5 / zero;

	/* call interrupt 15 */
	assertion_failure();

	return;
}

/*  page_fault_test  - showing page fault on invalid memory de-reference
 *
 *  de-reference an invalid memory address to show we page-fault properly
 *  Inputs : None
 *  Output : FAIL
 *  Side Effects : None
 *  Coverage : Initialize Paging, Enabling Paging, Paging defination
 *  Files : paging.c/ paging.h
*/
void page_fault_test() {
	TEST_HEADER;

	int i;
	int* j;

	/* First set j to video memory start address */
	j = (int*)(VIDMEM_ADDR - 1);
	i = *j;

	/* call interrupt 15 */
	assertion_failure();

	return;
}

// add more tests here

/* Checkpoint 2 tests */

int filesys_test(){

	char charbuffer[127];
	uint32_t temp;

	while(1){
		temp = read(0, charbuffer, 127);

		print_term(charbuffer, 127);
	}

	return PASS;
}

/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	/* idt_test */
	TEST_OUTPUT("idt_test", idt_test());
	/* rtc_test */
	TEST_OUTPUT("rtc_test", rtc_test());
	/* paging_test */
	TEST_OUTPUT("paging_test", paging_test());

	TEST_OUTPUT("filesys_test", filesys_test());

	return;
}

/* Exception test suite entry point */
void launch_divide_by_zero_test(){
	/* Function will give red screen of death */
	exception_test();
}

/* Page fault test suite entry point */
void launch_page_fault_test(){
	/* Function will give red screen of death */
	page_fault_test();
}
