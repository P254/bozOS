#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "paging.h"
#include "filesystem.h"
#include "terminal.h"
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

/* read_dentry_by_index_test
 * Asserts that our read_dentry_by_index function works 
 * Inputs: index - dentry index we want to print out
 * Outputs: 1
 * Side Effects: None
 * Coverage: All currently defined IDT values.
 */

/* Checkpoint 2 tests */
int read_dentry_by_index_test(uint32_t index){
    dentry_t test_dentry;
    printf("running read_dentry_by_index test:\n");
    int status = read_dentry_by_index(index, &test_dentry);

    if (status == 0){
    printf("File name: %s\n", test_dentry.fileName);
    } else {
        printf("Invalid index \n");
    }
    
    return 1;
}
/* read_dentry_by_name_test
 * Asserts that our read_dentry_by_index function works 
 * Inputs: fname - file name of the dentry
 * Outputs: 1
 * Side Effects: None
 * Coverage: All currently defined IDT values.
 */
int read_dentry_by_name_test(int8_t * fname) {
    dentry_t test_dentry;
    printf("running read_dentry_by_name test:\n");
    int status = read_dentry_by_name((uint8_t*)fname, &test_dentry);

    if (status == 0) {
        printf("Found file %s\n", fname);
    } else {
        printf("File not found\n");
    }
    return 1;
}

/* read_data_test
 * Asserts that our read_data_test function works 
 * Inputs: index - dentry index we want to print out the data of
 * Outputs: 1
 * Side Effects: None
 * Coverage: All currently defined IDT values.
 */

int read_data_test(int8_t * fname, int32_t size_to_copy, uint32_t offset, int type){
    dentry_t test_dentry;
    if (read_dentry_by_name((uint8_t*)fname, &test_dentry) == -1) {
        printf("File not found\n");
    }

    uint32_t buf_length = inodes[test_dentry.inode].length;
    size_to_copy = (size_to_copy < 0) ? buf_length : size_to_copy;
    uint8_t copy_buf[size_to_copy];
    
    int status;
    status = read_data(test_dentry.inode, offset, copy_buf, size_to_copy);
    printf("Copy status: %d\n", status);

    int i;
    printf("Copied contents to buf:\n");
    for (i = 0; i < status; i++) {
        if (type == TEXT) putc(copy_buf[i]);
        else if (type == NONTEXT) printf("%#x \n", copy_buf[i] );
    }
    return 0;
}

/* print_all_directories_test()
 * Prints out all the dentries
 * Inputs: None
 * Outputs: 1
 * Side Effects: None
 * Coverage:
 */
int print_all_directories_test()
{
    int i;
    int num_directories = boot->dirEntries;
    dentry_t temp_dentry;
    for ( i = 0 ; i < num_directories ; i++)
    {
        dread(i,&temp_dentry);
        printf("dentryIndex: %d fileName: %s size: %d \n", i , temp_dentry.fileName, inodes[temp_dentry.inode].length );
    }
    return 0;
}

int test_terminal_write_overload(){
    unsigned char test_string_overload[500];
    int i;
    for (i = 0 ; i<498 ; i++){
        test_string_overload[i] = 'a';
    }
    test_string_overload[499] = '\n';
    terminal_write(0,test_string_overload,128);
    putc('\n');
    return 1;
}

int test_terminal_write_underload(){
    unsigned char test_string_overload[5];
    int i;
    for (i = 0 ; i<4 ; i++){
      test_string_overload[i] = 'a';
    }   
    test_string_overload[4] = '\n';
    terminal_write(0,test_string_overload,128);
    putc('\n');
    return 1;
}

int test_terminal_read(){
    unsigned char sys_buf[128];
    terminal_read(0,sys_buf,128);
    printf("%s\n", sys_buf);
    return 1;
}

// add more tests here

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
    clear();
    printf("\n");
    //read_data_test("frame0.txt",-1,0,TEXT);
    //read_data_test("frame0.txt",24,0,TEXT);
    //read_data_test("frame0.txt",450,0,TEXT);
    //read_data_test("verylargetextwithverylongname.txt",3,4095,TEXT); 
    //read_data_test("fish",10,0,NONTEXT);
    //read_dentry_by_name_test("verylargetextwithverylongname.txt");
    //read_dentry_by_name_test("wtf name");
    //print_all_directories_test();
    // read_dentry_by_index_test(5);
    // read_dentry_by_index_test(500);

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

    //TEST_OUTPUT("test_terminal_write_overload", test_terminal_write_overload());
    //TEST_OUTPUT("test_terminal_write_underload", test_terminal_write_underload());
    //TEST_OUTPUT("test_terminal_read", test_terminal_read());



}
