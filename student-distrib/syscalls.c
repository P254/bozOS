#include "syscalls.h"
#include "lib.h"
#include "x86_desc.h"
#include "filesystem.h"
#include "paging.h"
static int process_number=-1;

/*
 * ----------- Notes for everyone: -----------
 * If you haven't already read through the relevant material, I suggest you do so.
 * Look at the slides from last week's discussion, plus Appendices A, C, D, E and the flowchart on Piazza.
 * So the bulk of the stuff is here. I wrote bits and pieces of the ece391_execute function.
 *
 * There's a couple things that need to be done, in order of priority:
 * 1) Figure out what needs to go into the structure for the PCB (and why)
 * 2) Complete steps 4 and 5
 * 3) Complete the ece391_halt function, which sort of does the opposite thing that ece391_execute does.
 * 4) Complete the ece391_read, ece391_write, ece391_open and ece391_close system calls.
 * 5) Finish step 1 of ece391_execute.
 *
 * I think it's best to work together on how to complete steps 4 and 5, as well as the PCB structure.
 * - Sean 11/8/17
 * --------------------------------------------
 */

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

    // Store ESP and EBP of the parent process, we can call a normal ret
    // Then we can resume at the parent program where we left off
    // Also check the diagram for the other things that need to be done (e.g. change paging)

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
    uint8_t i;

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
    uint8_t entry_pt_buf[BYTES_4];
    if (read_data(cmd_dentry.inode, ENTRY_PT_OFFSET, entry_pt_buf, BYTES_4) == -1) return -1;

    uint32_t entry_pt_addr = 0;
    for (i = 0; i < BYTES_4; i++) {
        // Sanity check: The entry point address should be somewhere near 0x08048000 (see Appendix C)
        // TODO: Check if the order of bits in entry_pt_addr is [24-25-26-27] or [27-26-25-24]
        entry_pt_addr = (entry_pt_addr << SHIFT_8) | entry_pt_buf[i];
    }

    /*********** Step 3: Set up paging ***********/
    // 'page_directory' is defined in paging.h
    // We map virtual address USER_MEM_V (128 MiB) to physical address USER_MEM_P + (process #) * 4 MiB
    page_directory[(USER_MEM_V >> ALIGN_4MB)] = USER_MEM_P | 0x87; // 4 MiB page, user & supervisor-access, r/w access, present

    // Tadas pointed out that we don't need to reload page_directory into CR3
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
    // TODO: This needs to be completed
    uint8_t data_buf[0x400000];
    if (read_data(cmd_dentry.inode, 0, data_buf, 0x400000) == -1) return -1;
    memcpy(USER_PROG,data_buf,0x400000);


    /*********** Step 5: Create PCB / open FDs ***********/
    // TODO: This needs to be completed
    // We can simply cast the address of the program's kernel stack to be a pcb_t pointer. No need to use memcpy.
     process_number++;
     kernel_base = 0x400000; //4MP is base of kernel
     PCB_offset = (process_number+1)*0x8000;
     program_kernel_base= kernel_base - PCB_offset; //find where program stack starts
     pcb_t* PCB_base = (pcb_t*) program_kernel_base; //cast it to PCB so start of program stack contains PCB.
     PCB_base.status = TASK_RUNNING;
     PCB_base.pid = process_number            // Process ID
    // PCB_base->pcb_loc = ??;      // PCB location in the 8 KB block within the kernel, ISNT IT ALWAYS AT BASE?
     PCB_base->user_loc = (0x800000 + process_number* 0x400000);     // Location of program in physical memory
     PCB_base->fd_arr = ??;   //TODO: PRANAV Figure out what to do with this
     if(process_number==0)  PCB_base->parent = NULL;       // Pointer to parent task
     else {
       //use in line assembly to acquire parent ESP and store in PCB_base->parent;
     }
    /*********** Step 6: Set up IRET context ***********/
    // The only things that really change here upon each syscall are: 1) tss.esp0, 2) ESP-on-stack (in IRET context), 3) page table

    // TODO: Check SS0 and ESP0 again
    tss.ss0 = KERNEL_DS; // Segment selector
    tss.esp0 = program_kernel_base; // New user program's kernel stack. Starts at (8MB - 8KB) for process #0, (8MB - 8KB - 8KB) for process #1, etc...

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
     *
     * Sources:
     * https://stackoverflow.com/questions/6892421/switching-to-user-mode-using-iret
     * http://jamesmolloy.co.uk/tutorial_html/10.-User%20Mode.html
     * http://x86.renejeschke.de/html/file_module_x86_id_145.html
     * http://www.felixcloutier.com/x86/IRET:IRETD.html
     * http://wiki.osdev.org/Getting_to_Ring_3
     * http://wiki.osdev.org/System_Calls
     * http://wiki.osdev.org/Task_State_Segment
     * http://wiki.osdev.org/Context_Switching
     */
    // Push IRET context to stack
    asm volatile(
        "cli"                   /* Context-switch is critical, so we suppress interrupts */

        "movw $USER_DS, %%cx"
        "movw %%cx, %%ss"       /* Code-segment */
        "movw %%cx, %%ds"       /* Data-segment */
        "movw %%cx, %%es"       /* Additional data-segment register */
        "movw %%cx, %%fs"       /* Additional data-segment register */
        "movw %%cx, %%gs"       /* Additional data-segment register */

        "pushl $USER_DS"
        "pushl $USER_STACK"     /* TODO: Figure out why we set the virutal ESP-on-stack memory to only be 4 MiB */

        "pushf"                 /* Push EFLAGS onto the stack */
        "popl %%eax"            /* Get EFLAGS back into EAX. The only way to read EFLAGS is to pushf then pop */
        "orl $INT_FLAG, %%eax"   /* Set the IF flag (same thing as STI; we use this because calling STI will cause a pagefault) */
        "pushl %%eax"           /* Push the new EFLAGS value back onto the stack */

        "pushl $USER_CS"
        "pushl %0"              /* User program/function entry point */
        "iret"
        : /*no outputs*/
        : "r" (entry_pt_addr)
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
    // This function is called within a given user program.
    // Based on the file descriptor #, we index into the PCB's FD array and find the relevant 'file operations table pointer'

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
    // This function is called within a given user program.
    // Based on the file descriptor #, we index into the PCB's FD array and find the relevant 'file operations table pointer'

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
    // This function is called within a given user program.
    // Based on the file descriptor #, we index into the PCB's FD array and find the relevant 'file operations table pointer'

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
    // This function is called within a given user program.
    // Based on the file descriptor #, we index into the PCB's FD array and find the relevant 'file operations table pointer'

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
    /******************* EXTRA CREDIT *************************/

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
    /******************* EXTRA CREDIT *************************/

    return 0;
}
