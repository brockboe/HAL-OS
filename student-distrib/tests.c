#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "keyboard.h"
#include "rtc.h"
#include "filesys.h"
#include "vc.h"
#include "video.h"
#include "syscall.h"
#include "sound.h"

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

/* list_files
 * Prints a continuous list of all the files inside the file system, including
 * their name, size, and file type.
 */
void list_files(){
	int32_t temp;
	int32_t fd;
	uint8_t fname[32];
	dentry_t file_dentry;
	inode_t * file_inode;
	int file_count;
	int i;

	clear_term();

	//Grab the number of files.
	file_count = (int)filesys_begin->num_dir_entries;
	fd = open((uint8_t *)".");

	for(i = 0; i < file_count; i++){
		set_term_x(1);
		//get the file name
		temp = read(fd, fname, FNAME_MAX_LEN);
		temp = write(1, fname, FNAME_MAX_LEN);
		temp = read_dentry_by_name(fname, &file_dentry);

		file_inode = (inode_t *)(filesys_begin->directory_entries[i].inode_num + inodes_begin);

		//print the file type
		set_term_x(40);
		print_term((uint8_t *)"filetype: ", 10);
		print_num((int)file_dentry.file_type);
		//print the size
		set_term_x(60);
		print_term((uint8_t *)"size: ", 6);
		print_num((int)file_inode->length);
		printchar_term('\n');
	}

}

/* print_file
 * Prints the contens of any file when given the name of the desired file as
 * a character string
 */
void print_file(uint8_t * fname){
	int temp;
	int32_t fd;
	int i;

	uint8_t charbuffer[6000];

	//clear the buffer
	for(i = 0; i < 6000; i++){
		charbuffer[i] = 0;
	}

	clear_term();

	//open, read, and print the file
	fd = open(fname);
	temp = read(fd, charbuffer, 6000);
	temp = write(1, charbuffer, 6000);
}

/* print_file_by_index
 * This function does the exact same thing as print_file, but instead of
 * grabbing and printing the file when indexed by the file name, this time
 * the file is indexed by it's index into the directory entries.
 */
void print_file_by_index(uint32_t findex){
	int32_t temp;
	uint8_t charbuffer[1000];
	dentry_t dentry;
	int i;

	for(i = 0; i < 1000; i++){
		charbuffer[i] = 0;
	}

	clear_term();

	temp = read_dentry_by_index(findex, &dentry);
	temp = read_data(dentry.inode_num, 0, charbuffer, 1000);
	temp = write(1, charbuffer, 1000);

	return;
}

/* RTC_IO
 * Opens the RTC and prints a '.' character at a specific frequency. The
 * frequency is increased as the test goes on.
 */
void RTC_IO(){
	int32_t fd;
	int32_t temp;
	int i;
	uint32_t buf[4];

	//open the RTC
	fd = open((uint8_t *)"rtc");

	//set the frequency to 32 and print at that rate
	clear_term();
	*buf = 32;
	temp = write(fd, buf, 4);
	for(i = 0; i < MAXCHAR/4; i++){
		printchar_term('.');
		temp = read(fd, NULL, 0);
	}

	//set the frequency to 64 and print at that rate
	clear_term();
	*buf = 64;
	temp = write(fd, buf, 4);
	for(i = 0; i < MAXCHAR/4; i++){
		printchar_term('.');
		temp = read(fd, NULL, 0);
	}

	//set the frequency to 128 and print at that rate
	clear_term();
	*buf = 128;
	temp = write(fd, buf, 4);
	for(i = 0; i < MAXCHAR/4; i++){
		printchar_term('.');
		temp = read(fd, NULL, 0);
	}

	return;
}

/* free_typing
 * This allows the user to test out the keyboard driver. The screen is cleared,
 * and whatever the user types is echoed to the screen. whenever enter is pressed,
 * the 128 length character buffer is printed to the screen.
 */
void free_typing(){
	uint8_t charbuffer[128];
	uint32_t retval;

	clear_term();

	while(1){
		retval = read(0, charbuffer, 128);
		retval = write(1, charbuffer, 128);
	}
	return;
}

/* filesys_test
 * Runs all the previous functions relating to drivers in succession. Whenever
 * the user hits enter, the function moves onto the next test.
 */
int driver_test(){
	fill_color();

	int retval;
	uint8_t temp[10];

	//begin by listing all the files
	list_files();

	retval = read(0, (void *)temp, 1);

	//print the contents of frame0.txt
	print_file((uint8_t *)"frame0.txt");

	retval = read(0, (void *)temp, 1);

	//print the contents of the large file
	print_file((uint8_t *)"verylargetextwithverylongname.tx");

	retval = read(0, (void *)temp, 1);

	//print the contents of sigtest
	print_file((uint8_t *)"sigtest");

	retval = read(0, (void *)temp, 1);

	retval = read(0, (void *)temp, 1);

	//print the contents of the 10th file in the directory index
	print_file_by_index(10);

	retval = read(0, (void *)temp, 1);

	//print the contents of the 11th file in the directory index
	print_file_by_index(11);

	retval = read(0, (void *)temp, 1);

	//Show off the typing driver
	free_typing();

	return PASS;
}

/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){

	uint8_t terminal_message[] = "\n Please do not hurt the terminal. \n Everytime you kill it, \n it only comes back stronger\n\n";
	int32_t msg_len = stringlength(terminal_message);

	/* idt_test */
	TEST_OUTPUT("idt_test", idt_test());
	/* paging_test */
	TEST_OUTPUT("paging_test", paging_test());

	clear_term();

	/*Tun all the driver tests*/
	//driver_test();

	fill_color();

	while(1){
		execute((uint8_t *)"shell");
		write(1, (void *)terminal_message, msg_len);
	}

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
