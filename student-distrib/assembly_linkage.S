.globl keyboard_handler, get_char


keyboard_handler:
    pushl %eax
    pushl %ecx
    pushl %edx

    call get_char

    popl %edx
    popl %ecx
    popl %eax
    iret
