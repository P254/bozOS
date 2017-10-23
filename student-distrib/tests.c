#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "paging.h"

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

int paging_table_test(){
    int i;
    int counter=0;
    uint32_t* page_table_ptr;
    for(i = 0 ; i<1024 ; i++) {
        if ( (0x1 & page_directory[i]) && (page_directory[i] != 0x0) )
            counter++;
    }
    if(counter>2) return FAIL;
    page_table_ptr = &page_directory[0];

    if(page_table_ptr[0xB8] == 0) return FAIL;
    return PASS;
}

int paging_kernel_test() {
    int result = FAIL;
    int* kernel_mem_ptr = KERN_MEM;

    if(*(kernel_mem_ptr)) {
        result = PASS;
    }
    return result;
}

int paging_video_test() {
    int result = FAIL;
    int* vid_mem_ptr = VID_MEM;

    if(*(vid_mem_ptr)) {
        result = PASS;
    }
    return result;
}

int div0_test() {
	int x = 1/0;
	return 1;
}

int pagefault_test() {
    int* x = NULL;
    int y = *x;
    return 1;
}

int segment_test() {
    asm("int $0x79");
    return 1;
}

int sys_call_test() {
    asm("int $0x80");
    return 1;
}
// add more tests here

/* Checkpoint 2 tests */
/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */

/* Test suite entry point */
void launch_tests(){
	// launch your tests here
    //TEST_OUTPUT("idt_test", idt_test());
	//TEST_OUTPUT("exception_test", div0_test());
    //TEST_OUTPUT("paging_kernel_test", paging_kernel_test());
    //TEST_OUTPUT("paging_video_test", paging_video_test());
	//TEST_OUTPUT("pagefault_test", pagefault_test());
    //TEST_OUTPUT("segment_test", segment_test());
    //TEST_OUTPUT("sys_call_test", sys_call_test());
    //TEST_OUTPUT("paging_table_test", paging_table_test());
}
