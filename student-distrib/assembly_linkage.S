.globl keyboard_handler, getScanCode


keyboard_handler:
    pushl %eax
    pushl %ecx
    pushl %edx

    call getScanCode

    popl %edx
    popl %ecx
    popl %eax
    iret
