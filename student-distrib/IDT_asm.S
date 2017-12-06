.data
    ONE_LEVEL       = 4
    ERROR_CODE_LOC  = 32
    
    SYSCALL_NUM_MIN = 1
    SYSCALL_NUM_MAX = 10
    RETVAL_ERR      = -1

.globl print_error_code
# Exceptions that give error codes are #8, #11, #12, #13, #14, #17, #30
# For error code interpretation, see:
# 1. http://wiki.osdev.org/Exceptions
# 2. https://stackoverflow.com/questions/10360888/identifying-faulting-address-on-general-protection-fault-x86

# ASM code for printing error code:
    # pushl ERROR_CODE_LOC(%esp)
    # call print_error_code
    # addl ONE_LEVEL, %esp

.globl keyboard_handler_asm, kb_int_handler
.globl rtc_handler_asm, rtc_handler
.globl pit_handler_asm, task_switch

.globl handle_e0_asm, handle_e0
.globl handle_e1_asm, handle_e1
.globl handle_e2_asm, handle_e2
.globl handle_e3_asm, handle_e3
.globl handle_e4_asm, handle_e4
.globl handle_e5_asm, handle_e5
.globl handle_e6_asm, handle_e6
.globl handle_e7_asm, handle_e7
.globl handle_e8_asm, handle_e8
.globl handle_e9_asm, handle_e9
.globl handle_e10_asm, handle_e10
.globl handle_e11_asm, handle_e11
.globl handle_e12_asm, handle_e12
.globl handle_e13_asm, handle_e13
.globl handle_e14_asm, handle_e14
.globl handle_e15_asm, handle_e15
.globl handle_e16_asm, handle_e16
.globl handle_e17_asm, handle_e17
.globl handle_e18_asm, handle_e18
.globl handle_e19_asm, handle_e19
.globl handle_default_asm, handle_default
.globl handle_syscall_asm

.globl halt, execute, read, write, open, close, getargs, vidmap, set_handler, sigreturn

syscall_jump_table:
    .long halt, execute, read, write, open, close, getargs, vidmap, set_handler, sigreturn

# handle_syscall_asm
#    DESCRIPTION : Assembly linkage for system calls (0x80)
#    INPUTS : none
#    OUTPUT : none
#    RETURN : none
#    SIDE EFFECTS : Calls the appropriate system call function with number specified by EAX
handle_syscall_asm:
    # Checking argument in EAX
    cmpl $SYSCALL_NUM_MIN, %eax
    jl syscall_negative_ret 
    cmpl $SYSCALL_NUM_MAX, %eax
    jg syscall_negative_ret
    
    # We don't want to clobber the return value of the system call function (EAX) here,
    # so I write out the entire 'pushal' / 'popal routine instead
    # Push (original) ESP, EBP, ESI, EDI, EDX, ECX, EBX
    pushl %esp
    pushl %ebp
    pushl %esi
    pushl %edi
    # Push the arguments
    pushl %edx
    pushl %ecx
    pushl %ebx
    
    decl %eax # We want EAX to have value in the range [0,9]
    call *syscall_jump_table(, %eax, ONE_LEVEL)

    popl %ebx
    popl %ecx
    popl %edx
    popl %edi
    popl %esi
    popl %ebp
    popl %esp
    iret

syscall_negative_ret:
    movl $RETVAL_ERR, %eax
    iret

# keyboard_handler_asm
#    DESCRIPTION : Assembly linkage for keyboard-generated interrupt
#    INPUTS : none
#    OUTPUT : none
#    RETURN : none
#    SIDE EFFECTS : none
keyboard_handler_asm:
    pushal
    call kb_int_handler # Calling interrupt handler defined in C
    popal
    iret # Return control to interrupted program

# rtc_handler_asm
#    DESCRIPTION : Assembly linkage for RTC-generated interrupt
#    INPUTS : none
#    OUTPUT : none
#    RETURN : none
#    SIDE EFFECTS : none
rtc_handler_asm:
    pushal
    call rtc_handler # Calling interrupt handler defined in C
    popal
    iret # Return control to interrupted program

# pit_handler_asm
#    DESCRIPTION : Assembly linkage for PIT-generated interrupt
#    INPUTS : none
#    OUTPUT : none
#    RETURN : none
#    SIDE EFFECTS : none
pit_handler_asm:
    pushal
    call task_switch # Calling interrupt handler defined in C
    popal
    iret # Return control to interrupted program

# handle_e0_asm
#	 DESCRIPTION : Assembly linkage for exception #0
#	 INPUTS : none
#	 OUTPUT : none
#	 RETURN : none
#	 SIDE EFFECTS : none
handle_e0_asm:
    pushal
    call handle_e0 # Calling interrupt handler defined in C
    popal
    iret # Return control to interrupted program

# handle_e1_asm
#	 DESCRIPTION : Assembly linkage for exception #1
#	 INPUTS : none
#	 OUTPUT : none
#	 RETURN : none
#	 SIDE EFFECTS : none
handle_e1_asm:
    pushal
    call handle_e1 # Calling interrupt handler defined in C
    popal
    iret # Return control to interrupted program

# handle_e2_asm
#	 DESCRIPTION : Assembly linkage for exception #2
#	 INPUTS : none
#	 OUTPUT : none
#	 RETURN : none
#	 SIDE EFFECTS : none
handle_e2_asm:
    pushal
    call handle_e2 # Calling interrupt handler defined in C
    popal
    iret # Return control to interrupted program

# handle_e3_asm
#	 DESCRIPTION : Assembly linkage for exception #3
#	 INPUTS : none
#	 OUTPUT : none
#	 RETURN : none
#	 SIDE EFFECTS : none
handle_e3_asm:
    pushal
    call handle_e3 # Calling interrupt handler defined in C
    popal
    iret # Return control to interrupted program

# handle_e4_asm
#	 DESCRIPTION : Assembly linkage for exception #4
#	 INPUTS : none
#	 OUTPUT : none
#	 RETURN : none
#	 SIDE EFFECTS : none
handle_e4_asm:
    pushal
    call handle_e4 # Calling interrupt handler defined in C
    popal
    iret # Return control to interrupted program

# handle_e5_asm
#	 DESCRIPTION : Assembly linkage for exception #5
#	 INPUTS : none
#	 OUTPUT : none
#	 RETURN : none
#	 SIDE EFFECTS : none
handle_e5_asm:
    pushal
    call handle_e5 # Calling interrupt handler defined in C
    popal
    iret # Return control to interrupted program

# handle_e6_asm
#	 DESCRIPTION : Assembly linkage for exception #6
#	 INPUTS : none
#	 OUTPUT : none
#	 RETURN : none
#	 SIDE EFFECTS : none
handle_e6_asm:
    pushal
    call handle_e6 # Calling interrupt handler defined in C
    popal
    iret # Return control to interrupted program

# handle_e7_asm
#	 DESCRIPTION : Assembly linkage for exception #7
#	 INPUTS : none
#	 OUTPUT : none
#	 RETURN : none
#	 SIDE EFFECTS : none
handle_e7_asm:
    pushal
    call handle_e7 # Calling interrupt handler defined in C
    popal
    iret # Return control to interrupted program

# handle_e8_asm
#	 DESCRIPTION : Assembly linkage for exception #8
#	 INPUTS : none
#	 OUTPUT : none
#	 RETURN : none
#	 SIDE EFFECTS : none
handle_e8_asm:
    pushal
    pushl ERROR_CODE_LOC(%esp)
    call handle_e8 # Calling interrupt handler defined in C
    addl ONE_LEVEL, %esp
    popal
    iret # Return control to interrupted program

# handle_e9_asm
#	 DESCRIPTION : Assembly linkage for exception #9
#	 INPUTS : none
#	 OUTPUT : none
#	 RETURN : none
#	 SIDE EFFECTS : none
handle_e9_asm:
    pushal
    call handle_e9 # Calling interrupt handler defined in C
    popal
    iret # Return control to interrupted program

# handle_e10_asm
#	 DESCRIPTION : Assembly linkage for exception #10
#	 INPUTS : none
#	 OUTPUT : none
#	 RETURN : none
#	 SIDE EFFECTS : none
handle_e10_asm:
    pushal
    call handle_e10 # Calling interrupt handler defined in C
    popal
    iret # Return control to interrupted program

# handle_e11_asm
#	 DESCRIPTION : Assembly linkage for exception #11
#	 INPUTS : none
#	 OUTPUT : none
#	 RETURN : none
#	 SIDE EFFECTS : none
handle_e11_asm:
    pushal
    pushl ERROR_CODE_LOC(%esp)
    call handle_e11 # Calling interrupt handler defined in C
    addl ONE_LEVEL, %esp
    popal
    iret # Return control to interrupted program

# handle_e12_asm
#	 DESCRIPTION : Assembly linkage for exception #12
#	 INPUTS : none
#	 OUTPUT : none
#	 RETURN : none
#	 SIDE EFFECTS : none
handle_e12_asm:
    pushal
    pushl ERROR_CODE_LOC(%esp)
    call handle_e12 # Calling interrupt handler defined in C
    addl ONE_LEVEL, %esp
    popal
    iret # Return control to interrupted program

# handle_e13_asm
#	 DESCRIPTION : Assembly linkage for exception #13
#	 INPUTS : none
#	 OUTPUT : none
#	 RETURN : none
#	 SIDE EFFECTS : none
handle_e13_asm:
    pushal
    pushl ERROR_CODE_LOC(%esp)
    call handle_e13 # Calling interrupt handler defined in C
    addl ONE_LEVEL, %esp
    popal
    iret # Return control to interrupted program

# handle_e14_asm
#	 DESCRIPTION : Assembly linkage for exception #14
#	 INPUTS : none
#	 OUTPUT : none
#	 RETURN : none
#	 SIDE EFFECTS : none
handle_e14_asm:
    pushal
    pushl ERROR_CODE_LOC(%esp)
	call handle_e14 # Calling interrupt handler defined in C
    addl ONE_LEVEL, %esp
	popal
	iret # Return control to interrupted program

# handle_e15_asm
#	 DESCRIPTION : Assembly linkage for exception #15
#	 INPUTS : none
#	 OUTPUT : none
#	 RETURN : none
#	 SIDE EFFECTS : none
handle_e15_asm:
    pushal
    call handle_e15 # Calling interrupt handler defined in C
    popal
    iret # Return control to interrupted program

# handle_e16_asm
#	 DESCRIPTION : Assembly linkage for exception #16
#	 INPUTS : none
#	 OUTPUT : none
#	 RETURN : none
#	 SIDE EFFECTS : none
handle_e16_asm:
    pushal
    call handle_e16 # Calling interrupt handler defined in C
    popal
    iret # Return control to interrupted program

# handle_e17_asm
#	 DESCRIPTION : Assembly linkage for exception #17
#	 INPUTS : none
#	 OUTPUT : none
#	 RETURN : none
#	 SIDE EFFECTS : none
handle_e17_asm:
    pushal
    pushl ERROR_CODE_LOC(%esp)
    call handle_e17 # Calling interrupt handler defined in C
    addl ONE_LEVEL, %esp
    popal
    iret # Return control to interrupted program

# handle_e18_asm
#	 DESCRIPTION : Assembly linkage for exception #18
#	 INPUTS : none
#	 OUTPUT : none
#	 RETURN : none
#	 SIDE EFFECTS : none
handle_e18_asm:
    pushal
    call handle_e18 # Calling interrupt handler defined in C
    popal
    iret # Return control to interrupted program

# handle_e19_asm
#	 DESCRIPTION : Assembly linkage for exception #19
#	 INPUTS : none
#	 OUTPUT : none
#	 RETURN : none
#	 SIDE EFFECTS : none
handle_e19_asm:
    pushal
    call handle_e19 # Calling interrupt handler defined in C
    popal
    iret # Return control to interrupted program

# handle_default_asm
#    DESCRIPTION : Assembly linkage for any other exception
#    INPUTS : none
#    OUTPUT : none
#    RETURN : none
#    SIDE EFFECTS : none
handle_default_asm:
    pushal
    call handle_default # Calling interrupt handler defined in C
    popal
    iret # Return control to interrupted program
