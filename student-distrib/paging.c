#include "paging.h"
#include "x86_desc.h"

// Source: http://wiki.osdev.org/Setting_Up_Paging
unsigned int page_directory[1024] __attribute__((aligned(ALIGN_4KB)));
unsigned int page_table[1024] __attribute__((aligned(ALIGN_4KB)));

// TODO Sean: Complete function prototype and definition
void loadPageDirectory() {
    asm volatile(
        "mov %0, %%cr3"
        : /* no outputs */
        : "r" (page_directory)
        : "eax"
    );
}

// TODO Sean: Complete function prototype and definition
void enablePaging() {
    asm volatile(
        "mov %%cr0, %%eax       \n\
        or $0x80000000, %%eax   \n\
        mov %%eax, %%cr0        \n\
        mov %%cr4, %%eax        \n\
        or $0x00000010, %%eax   \n\
        mov %%eax, %%cr4"
        : /* no outputs */
        : /* no inputs */
        : "eax"
    );
}

/* The PDE look like this: 
 * 
 *             31:12               11:9    8   7   6   5   4   3   2   1   0 
 * [ Page table 4KB aligned addr | Avail | G | S | 0 | A | D | W | U | R | P ]
 * 
 *  
 * Avail: Used by the OS to store accounting information
 * 8 - G: Global page (ignored)
 * 7 - S: Page Size (1: 4 MiB, 0: 4 KiB)
 * 6 - 0: Ignored
 * 5 - A: Accessed (has this page been written to? 1: Yes, 0: No)
 * 4 - D: Cache Disabled (1: disable cache, 0: enable cache)
 * 3 - W: Write-through (controls the write-through policy. 1: Enabled, 0: Disabled)
 * 2 - U: User/supervisor access (1: accessed by all, 0: supervisor access only)
 * 1 - R: Read/write access (1: read & write allowed, 0: read-only)
 * 0 - P: Presence of the page (1: Present, 0: Page is swapped out)
 */


/*TODO Sean: Clean up this code and add comments*/
void paging_init() {
    unsigned int i;
    // Step 1: Page directory
    for(i = 0; i < N_PDE; i++) {
        /* Sets every PDE to be:
         * P - Marked as 'not present'
         * R - Allows for both read & write
         * U - Supervisor access only
         */
        page_directory[i] = 0x2;
    }

    // Step 2: Page table for the video memory
    // holds the physical address where we want to start mapping these pages to.
    // in this case, we want to map these pages to the very beginning of memory.

    // The kernel starts at 4MB, so we want to mark entry 0x400000 (the second entry) in the PDE as present
    page_directory[2] = KERNEL_MEM | 0x3;

    for (i = 0; i < N_PTE; i++) {
        // Attributes: supervisor level, r/w, present    
        page_table[i] = (i * 0x1000) | 0x3;
    }
    // Attributes: supervisor level, r/w, present
    page_directory[1] = ((unsigned int) page_table) | 0x3;

    // Last step: See http://wiki.osdev.org/Setting_Up_Paging#Enable_Paging
    loadPageDirectory();
    enablePaging();   
}
