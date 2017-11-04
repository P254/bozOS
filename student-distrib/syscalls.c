#include "syscalls.h"
#include "lib.h"
#include "x86_desc.h"

/* Sources:
 * https://stackoverflow.com/questions/6892421/switching-to-user-mode-using-iret
 * http://jamesmolloy.co.uk/tutorial_html/10.-User%20Mode.html
 * http://x86.renejeschke.de/html/file_module_x86_id_145.html
 * http://www.felixcloutier.com/x86/IRET:IRETD.html
 * http://wiki.osdev.org/Getting_to_Ring_3
 * http://wiki.osdev.org/System_Calls
 * http://wiki.osdev.org/Task_State_Segment
 * http://wiki.osdev.org/Context_Switching
 */

/* Function pointer array for system calls */
void (*handle_syscalls_arr[N_SYSCALLS])() = {};

/*
 * handle_syscall
 *   DESCRIPTION: Handler for system calls.
 *                Placeholder for now, will be filled in as part of future checkpoints.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: int32_t -- 0 on success, -1 on failure
 *   SIDE EFFECTS: none
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

    if (call_num < SYS_HALT || call_num > SYS_SIGRETURN) return -1;
    printf("System call #%d.\n", call_num);

    cli();
    while(1);
    return 0;
}

/*
 * ece391_halt
 *   DESCRIPTION: Handler for 'halt' system call. 
 *   INPUTS: status -- ??? 
 *   OUTPUTS: none
 *   RETURN VALUE: int32_t -- 0 on success, -1 on failure
 *   SIDE EFFECTS: none
 */
int32_t ece391_halt(uint8_t status) {
    printf("System call HALT.\n");

    cli();
    while(1);
    return 0;
}

/*
 * ece391_execute
 *   DESCRIPTION: Handler for 'execute' system call. 
 *   INPUTS: command -- space-separated sequence of words
 *   OUTPUTS: none
 *   RETURN VALUE: int32_t -- 0 on success, -1 on failure
 *   SIDE EFFECTS: none
 */
int32_t ece391_execute(const uint8_t* command) {
    printf("System call EXECUTE.\n");

    // Context-switching happens here

    cli();
    while(1);
    return 0;
}

/*
 * ece391_read
 *   DESCRIPTION: Handler for 'read' system call. 
 *   INPUTS: fd -- ??? 
 *           buf -- ???
 *           nbytes -- ???
 *   OUTPUTS: none
 *   RETURN VALUE: int32_t -- 0 on success, -1 on failure
 *   SIDE EFFECTS: none
 */
int32_t ece391_execute(int32_t fd, void* buf, int32_t nbytes) {
    printf("System call READ.\n");

    cli();
    while(1);
    return 0;
}

/*
 * ece391_write
 *   DESCRIPTION: Handler for 'write' system call. 
 *   INPUTS: fd -- ??? 
 *           buf -- ???
 *           nbytes -- ???
 *   OUTPUTS: none
 *   RETURN VALUE: int32_t -- 0 on success, -1 on failure
 *   SIDE EFFECTS: none
 */
int32_t ece391_write(int32_t fd, void* buf, int32_t nbytes) {
    printf("System call WRITE.\n");

    cli();
    while(1);
    return 0;
}


/*
 * ece391_open
 *   DESCRIPTION: Handler for 'open' system call. 
 *   INPUTS: filename -- ???
 *   OUTPUTS: none
 *   RETURN VALUE: int32_t -- 0 on success, -1 on failure
 *   SIDE EFFECTS: none
 */
int32_t ece391_open(const uint8_t* filename) {
    printf("System call OPEN.\n");

    cli();
    while(1);
    return 0;
}

/*
 * ece391_close
 *   DESCRIPTION: Handler for 'close' system call. 
 *   INPUTS: fd -- ???
 *   OUTPUTS: none
 *   RETURN VALUE: int32_t -- 0 on success, -1 on failure
 *   SIDE EFFECTS: none
 */
int32_t ece391_close(int32_t fd) {
    printf("System call CLOSE.\n");

    cli();
    while(1);
    return 0;

}

/*
 * ece391_getargs
 *   DESCRIPTION: Handler for 'getargs' system call. 
 *   INPUTS: buf -- ???
 *           nbytes -- ???
 *   OUTPUTS: none
 *   RETURN VALUE: int32_t -- 0 on success, -1 on failure
 *   SIDE EFFECTS: none
 */
int32_t ece391_close(uint8_t* buf, int32_t nbytes) {
    printf("System call GETARGS.\n");

    cli();
    while(1);
    return 0;
}

/*
 * ece391_vidmap
 *   DESCRIPTION: Handler for 'vidmap' system call. 
 *   INPUTS: screen_start -- ???
 *   OUTPUTS: none
 *   RETURN VALUE: int32_t -- 0 on success, -1 on failure
 *   SIDE EFFECTS: none
 */
int32_t ece391_vidmap (uint8_t** screen_start) {
    printf("System call VIDMAP.\n");
    
    cli();
    while(1);
    return 0;
}
/*
 * ece391_set_handler
 *   DESCRIPTION: Handler for 'set_handler' system call. 
 *   INPUTS: signum -- ???
 *           handler -- ???
 *   OUTPUTS: none
 *   RETURN VALUE: int32_t -- 0 on success, -1 on failure
 *   SIDE EFFECTS: none
 */
int32_t ece391_set_handler (int32_t signum, void* handler) {
    
    printf("System call SET_HANDLER.\n");
    
    cli();
    while(1);
    return 0;
}

/*
 * ece391_sigreturn
 *   DESCRIPTION: Handler for 'sigreturn' system call. 
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: int32_t -- 0 on success, -1 on failure
 *   SIDE EFFECTS: none
 */
int32_t ece391_sigreturn (void) {
    printf("System call SIGRETURN.\n");
    
    cli();
    while(1);
    return 0;
}