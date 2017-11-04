#include "syscalls.h"
#include "lib.h"
#include "x86_desc.h"
#include "filesystem.h"
#include "paging.h"

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
    printf("Executing system call #%d.\n", call_num);

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

    /*********** Step 1: Parse arguments ***********/
    // TODO: Need to perform appropriate checking of command string
    // Command is a space-separated sequence of words
    // For now, I hardcode cmd so that we execute "shell"
    uint8_t* cmd = "shell";

    
    /*********** Step 2: Check file validity ***********/
    // Check if the file can be read or not
    dentry_t cmd_dentry;
    if (read_dentry_by_name(cmd, &cmd_dentry) == -1) return -1;

    // Check if the file can be executed or not
    uint8_t exe_buf[BYTES_4];
    if (read_data(cmd_dentry.inode, 0, exe_buf, BYTES_4) == -1) return -1;
    if (exe_buf[0] != EXE_BYTE0) return -1;
    if (exe_buf[1] != EXE_BYTE1) return -1;
    if (exe_buf[2] != EXE_BYTE2) return -1;
    if (exe_buf[3] != EXE_BYTE3) return -1;
    
    // Get the entry point from bytes 24-27 of the executable 
    uint32_t entry_point_addr;
    if (read_data(cmd_dentry.inode, ENTRY_PT_OFFSET, entry_point_addr, BYTES_4) == -1) return -1;

    
    /*********** Step 3: Set up paging ***********/
    // 'page_directory' is defined in paging.h
    // We map virtual address USER_MEM_V (128 MiB) to physical address USER_MEM_P 8 MiB
    page_directory[(USER_MEM_V >> ALIGN_4MB)] = USER_MEM_P | 0x87; // 4 MiB page, user & supervisor-access, r/w access, present
    
    // We don't need to reload page_directory into CR3 
    // Flush the TLB (flushing happens whenever we reload CR3)
    asm volatile(
        "movl %%cr3, %%eax"
        "movl %%eax, %%cr3"
        : /* no outputs */
        : /* no inputs */
        : "eax"
    );

    /*********** Step 4: Load file into memory ***********/
    // The program image must be copied to the correct offset (0x48000) within that page


    /*********** Step 5: Create PCB / open FDs ***********/



    /*********** Step 6: Set up IRET context ***********/
    // The only things that really change here upon each syscall are: 1) tss.esp0, 2) ESP-on-stack (in IRET context), 3) page table

    // TODO: Check SS0 and ESP0 again
    tss.ss0 = KERNEL_DS; // Segment selector
    tss.esp0 = ??; // New kernel stack. Check the slides on PCB address.  

    /* The IRET instruction expects, when executed, the stack to have the following contents 
     * (starting from the stack pointer - lowermost address upwards):
     * 1. The instruction to continue execution at - the value of EIP (pointer to our function entry point).
     * 2. The code segment (CS) selector to change to.
     * 3. The value of the EFLAGS register to load.
     * 4. The stack pointer to load.
     * 5. The stack segment (SS) selector to change to.
     *
     * The stack prior to IRET looks like:
     * ----------
     *    EIP      <-- Bytes 24-27 of the executable we load
     * ----------
     *     CS      <-- User-mode Code Segment
     * ----------
     *   EFLAGS    
     * ----------
     *    ESP      <-- User-mode ESP
     * ----------
     *     SS      <-- User-mode Stack Segment
     * ----------
     */
    // Push IRET context to stack
    asm volatile(
        "cli"                   /* Clear interrupts */
        
        "movw $USER_DS, %%cx"
        "movw %%cx, %%ss"       /* Code-segment */
        "movw %%cx, %%ds"       /* Data-segment */
        "movw %%cx, %%es"       /* Additional data-segment register */
        "movw %%cx, %%fs"       /* Additional data-segment register */
        "movw %%cx, %%gs"       /* Additional data-segment register */

        "pushl $USER_DS"
        "pushl $(132Mib - 4) ??"        /*TODO: Figure out what to set the ESP-on-stack*/

        "pushf"                 /* Push EFLAGS onto the stack */
        "popl %%eax"            /* Get EFLAGS back into EAX. The only way to read EFLAGS is to pushf then pop */
        "orl $INT_FLAG, %%eax"   /* Set the IF flag (same thing as STI; we use this because calling STI will cause a pagefault) */
        "pushl %%eax"           /* Push the new EFLAGS value back onto the stack */

        "pushl $USER_CS"
        "pushl %0"              /* Function entry point */
        "iret"
        : /*no outputs*/ 
        : "r" (entry_point_addr)
    );
    
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
int32_t ece391_read(int32_t fd, void* buf, int32_t nbytes) {
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