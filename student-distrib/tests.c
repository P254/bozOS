#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "paging.h"
#include "filesystem.h"
#define PASS 1
#define FAIL 0

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
/*int idt_test(){
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < IDT_SIZE; ++i){ // loops through the first 10 IDT entries
		if ((idt[i].offset_15_00 == NULL) &&
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}*/

/* Paging table test
 * Asserts that page directory and page table are not empty.
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Checks elements for page directory and page table.
 */
/*int paging_table_test(){
    int i;
    int counter=0;
    uint32_t* page_table_ptr;
    for(i = 0 ; i<SIZE_TABLE ; i++) { //Check our entire directory
        if ( (0x1 & page_directory[i]) && (page_directory[i] != 0x0) )
            counter++; //if that element is present and not empty, increment our counter
    }
    if(counter>FILLED_LOC) return FAIL; // if there are more than 2 present entries, the paging isnt set up right.

    page_table_ptr = &page_directory[0]; //our paging table contains the first element of the page directory.

    if(page_table_ptr[VID_MEM_LOC] == 0) return FAIL; //if our video memory is empty, we have set up paging wrong.
    return PASS;
}*/

/* Paging kernel test
 * Asserts that kernel memory does not page fault
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Covers page faults
 */
/*int paging_kernel_test() {
    int result = FAIL;
    int* kernel_mem_ptr = (int*) KERN_MEM; //set pointer to kernel memory

    if(*(kernel_mem_ptr)) {  // if we can dereference then paging works.
        result = PASS;
    }
    return result;
}*/

/* Paging video test
 * Asserts that video memory does not page fault
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Covers page faults
 */
/*int paging_video_test() {
    int result = FAIL;
    int* vid_mem_ptr = (int*) VID_MEM; //set pointer to video memory
    if(*(vid_mem_ptr)) {
        result = PASS;  // if we can dereference then paging works.
    }
    return result;
}*/

/* div0 test
 * Asserts that division by 0 causes exception
 * Inputs: None
 * Outputs: 1
 * Side Effects: None
 * Coverage: Covers divide by 0 error
 */
/*int div0_test() {
	int x = 1/0; //divide by 0, will call exception.
    x = x+1 ; //bypass warning
	return 1;
}*/

/* Page fault test
 * Asserts that dereferencing a null pointer causes page fault.
 * Inputs: None
 * Outputs: 1
 * Side Effects: None
 * Coverage: Covers page faults
 */
/*int pagefault_test() {
    int* x = NULL;
    int y = *x;  //try to dereference NULL memory.
    y = y+1; //bypass warning
    return 1;
}*/

/* Paging segment test
 * Asserts that accessing a undefined IDT value causes an exception
 * Inputs: None
 * Outputs: 1
 * Side Effects: None
 * Coverage: Covers segment faults
 */
/*int segment_test() {
    asm("int $0x79"); //random interrupt call
    return 1;
}*/

/* System Calls
 * Asserts that we can access system call memory area
 * Inputs: None
 * Outputs: 1
 * Side Effects: None
 * Coverage: Covers system call basics
 */
/*int sys_call_test() {
    asm("int $0x80"); //system call
    return 1;
}*/

/* test exceptions
 * Asserts that our IDT calls exceptions properly
 * Inputs: None
 * Outputs: 1
 * Side Effects: None
 * Coverage: All currently defined IDT values.
 */
/*int test_exceptions() {
    asm("int $0x1"); //put in any random exception
    return 1;
}*/

int dentry_by_index_test(){
    dentry_t test_dentry;
    printf("running test");
    read_dentry_by_index(6,&test_dentry);
    return 1;
}
int dentry_by_name_test(){
    dentry_t test_dentry;
    printf("running by name test");
    read_dentry_by_name("fish",&test_dentry);
}
int read_data_test(){
    dentry_t test_dentry;
    read_dentry_by_name("verylargetextwithverylongname.tx",&test_dentry);
    int buf_length;
    buf_length = inodes[test_dentry.inode].length;
    uint8_t buf[buf_length];
    int i;
    for (i = 0 ; i < buf_length; i++)
        buf[i] = '\0';
    read_data(test_dentry.inode,4095,buf,3);

    uint32_t j;
    printf("Copied contents to buf:\n");
    for (j = 0; j < 3; j++) {
        putc(buf[j]);
    }
}

// add more tests here

/* Checkpoint 2 tests */
/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */

/* Test suite entry point */
/* launch tests
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 * Coverage: Launches the tests that we wrote before.
 */
void launch_tests(){
    //dentry_by_index_test();
    //dentry_by_name_test();
    read_data_test();
	// launch your tests here
    //TEST_OUTPUT("idt_test", idt_test());
	//TEST_OUTPUT("divisionby0_test", div0_test());
    //TEST_OUTPUT("paging_kernel_test", paging_kernel_test());
    //TEST_OUTPUT("paging_video_test", paging_video_test());
	//TEST_OUTPUT("pagefault_test", pagefault_test());
    //TEST_OUTPUT("segment_test", segment_test());
    //TEST_OUTPUT("sys_call_test", sys_call_test());
    //TEST_OUTPUT("paging_table_test", paging_table_test());
    //TEST_OUTPUT("test_exceptions", test_exceptions());
}
