#include "paging.h"
#include "x86_desc.h"

// Source: http://wiki.osdev.org/Setting_Up_Paging
/* Instantiation of our page directory and page table */
uint32_t page_directory[PAGE_SIZE] __attribute__((aligned(ALIGN_4KB)));
uint32_t page_table[PAGE_SIZE] __attribute__((aligned(ALIGN_4KB)));

/* The PDE look like this: 
 * 
 *             31:12               11:9    8   7   6   5   4   3   2   1   0 
 * [ Page table 4KB aligned addr | Avail | G | S | 0 | A | D | W | U | R | P ]
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

/*
 * load_page_directory
 *   DESCRIPTION: Loads the page directory by modifying control register 3.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: void
 *   SIDE EFFECTS: modifies CR3
 */
void load_page_directory() {
    asm volatile(
        "mov %0, %%cr3"
        : /* no outputs */
        : "r" (page_directory)
        : "eax"
    );
}


/*
 * enable_paging
 *   DESCRIPTION: Enables paging on our operating system,
 *                modifies control register 0 and 4 in that order.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: void
 *   SIDE EFFECTS: modifies CR4 and CR0
 */
void enable_paging() {
    asm volatile(
        "mov %%cr4, %%eax       \n\
        or $0x00000010, %%eax   \n\
        mov %%eax, %%cr4        \n\
        mov %%cr0, %%eax        \n\
        or $0x80000001, %%eax   \n\
        mov %%eax, %%cr0"
        : /* no outputs */
        : /* no inputs */
        : "eax"
    );
}

/*
 * paging_init
 *   DESCRIPTION: Initializes the process that enables paging on our OS 
 *                by building a table of PDEs and PTEs.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: void
 *   SIDE EFFECTS: Enables paging on our OS. 
 */
void paging_init() {
    unsigned int i;
    // Step 1: Page directory
    for(i = 0; i < PAGE_SIZE; i++) {
        page_directory[i] = 0x2; // supervisor-only, r/w access, not present
    }

    // The kernel starts at 4MB, so we want to the corresponding entry in the PDE as present
    page_directory[1] = KERNEL_MEM | 0x83; // 4 MiB page, supervisor-only, r/w access, present

    // Step 2: Page table for the video memory
    // Holds the physical address where we want to start mapping these pages to.
    page_directory[0] = ((uint32_t) page_table) | 0x3; // assign table to [0], give r/w access, mark as present

    for (i = 0; i < PAGE_SIZE; i++) {
        page_table[i] = 0x2; // 4 KiB page, supervisor-only, r/w access, not present
    }
    // We want to allocate a 4KiB page for VIDEO MEMORY
    page_table[(VIDEO_MEM >> ALIGN_4KB_SHIFT)] = VIDEO_MEM | 0x3; // give page r/w access, mark as present

    // Last step: See http://wiki.osdev.org/Setting_Up_Paging#Enable_Paging
    load_page_directory();
    enable_paging();   
}
