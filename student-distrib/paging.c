#include "paging.h"

 //Source: http://wiki.osdev.org/Setting_Up_Paging
 uint32_t page_directory[1024] __attribute__((aligned(ALIGN_4KB)));
 uint32_t first_page_table[1024] __attribute__((aligned(ALIGN_4KB)));

/*The PDE look like this:
 *             31:12               11:9    8   7   6   5   4   3   2   1   0
 * [ Page table 4KB aligned addr | Avail | G | S | 0 | A | D | W | U | R | P ]
 *
 * TODO Sean: Fill in the comments*/


/*TODO Sean: Clean up this code and add comments*/
void paging_init() {
    unsigned int i;
    for(i = 0; i < N_PDE; i++) {
        /* Sets every PDE to be:
         * P - Marked as 'not present'
         * R - Allows for both read & write
         * U - Supervisor access only*/

        page_directory[i] = 0x2;
    }
}
