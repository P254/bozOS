.globl keyboard_handler_asm, kb_int_handler
.globl rtc_handler_asm, rtc_handler

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


# keyboard_handler_asm
#    DESCRIPTION : Warpper for keyboard-generated interrupt
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
#    DESCRIPTION : Warpper for RTC-generated interrupt
#    INPUTS : none
#    OUTPUT : none
#    RETURN : none
#    SIDE EFFECTS : none
rtc_handler_asm:
    pushal
    call rtc_handler # Calling interrupt handler defined in C
    popal
    iret # Return control to interrupted program

# handle_e0_asm
#	 DESCRIPTION : Wrapper for exception #0
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
#	 DESCRIPTION : Wrapper for exception #1
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
#	 DESCRIPTION : Wrapper for exception #2
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
#	 DESCRIPTION : Wrapper for exception #3
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
#	 DESCRIPTION : Wrapper for exception #4
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
#	 DESCRIPTION : Wrapper for exception #5
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
#	 DESCRIPTION : Wrapper for exception #6
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
#	 DESCRIPTION : Wrapper for exception #7
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
#	 DESCRIPTION : Wrapper for exception #8
#	 INPUTS : none
#	 OUTPUT : none
#	 RETURN : none
#	 SIDE EFFECTS : none
handle_e8_asm:
	 pushal
	 call handle_e8 # Calling interrupt handler defined in C
	 popal
	 iret # Return control to interrupted program

# handle_e9_asm
#	 DESCRIPTION : Wrapper for exception #9
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
#	 DESCRIPTION : Wrapper for exception #10
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
#	 DESCRIPTION : Wrapper for exception #11
#	 INPUTS : none
#	 OUTPUT : none
#	 RETURN : none
#	 SIDE EFFECTS : none
handle_e11_asm:
	 pushal
	 call handle_e11 # Calling interrupt handler defined in C
	 popal
	 iret # Return control to interrupted program

# handle_e12_asm
#	 DESCRIPTION : Wrapper for exception #12
#	 INPUTS : none
#	 OUTPUT : none
#	 RETURN : none
#	 SIDE EFFECTS : none
handle_e12_asm:
	 pushal
	 call handle_e12 # Calling interrupt handler defined in C
	 popal
	 iret # Return control to interrupted program

# handle_e13_asm
#	 DESCRIPTION : Wrapper for exception #13
#	 INPUTS : none
#	 OUTPUT : none
#	 RETURN : none
#	 SIDE EFFECTS : none
handle_e13_asm:
	 pushal
	 call handle_e13 # Calling interrupt handler defined in C
	 popal
	 iret # Return control to interrupted program

# handle_e14_asm
#	 DESCRIPTION : Wrapper for exception #14
#	 INPUTS : none
#	 OUTPUT : none
#	 RETURN : none
#	 SIDE EFFECTS : none
handle_e14_asm:
	 pushal
	 call handle_e14 # Calling interrupt handler defined in C
	 popal
	 iret # Return control to interrupted program

# handle_e15_asm
#	 DESCRIPTION : Wrapper for exception #15
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
#	 DESCRIPTION : Wrapper for exception #16
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
#	 DESCRIPTION : Wrapper for exception #17
#	 INPUTS : none
#	 OUTPUT : none
#	 RETURN : none
#	 SIDE EFFECTS : none
handle_e17_asm:
	 pushal
	 call handle_e17 # Calling interrupt handler defined in C
	 popal
	 iret # Return control to interrupted program

# handle_e18_asm
#	 DESCRIPTION : Wrapper for exception #18
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
#	 DESCRIPTION : Wrapper for exception #19
#	 INPUTS : none
#	 OUTPUT : none
#	 RETURN : none
#	 SIDE EFFECTS : none
handle_e19_asm:
	 pushal
	 call handle_e19 # Calling interrupt handler defined in C
	 popal
	 iret # Return control to interrupted program

