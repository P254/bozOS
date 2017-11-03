#include "syscalls.h"
#include "lib.h"
#include "x86_desc.h"

/* Function pointer array for system calls */
void (*handle_syscalls_arr[N_SYSCALLS])() = {};

/*
 * handle_syscall
 *   DESCRIPTION: Handler for system calls.
 *                Placeholder for now, will be filled in as part of future checkpoints.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: int32_t -- 0 on success, -1 on failure
 *   SIDE EFFECTS: masks interrupts, halts system
 */
int32_t handle_syscall() {
    int32_t call_num, arg1, arg2, arg3;

    // Get the call number
    asm volatile("         \n\
        movl %%eax, %0     \n\
        movl %%ebx, %1     \n\
        movl %%ecx, %2     \n\
        movl %%edx, %3"
        : "=r" (call_num), "=r" (arg1), "=r" (arg2), "=r" (arg3)
        : /* no inputs */
    );

    if (call_num < 1 || call_num > 10) return -1;
    printf("System call #%d.\n", call_num);



    cli();
    while(1);
    return 0;
}

/*
 * syscall_halt
 *   DESCRIPTION: Handler for 'halt' system call. 
 *   INPUTS: status -- ??? 
 *   OUTPUTS: none
 *   RETURN VALUE: int32_t -- 0 on success, -1 on failure
 *   SIDE EFFECTS: masks interrupts, halts system
 */
int32_t syscall_halt(uint8_t status) {
    printf("Executing system call HALT.\n");

    cli();
    while(1);
    return 0;
}
