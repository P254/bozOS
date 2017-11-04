# Source:  http://jamesmolloy.co.uk/tutorial_html/10.-User%20Mode.html

# Define constants here
.data
    IF_FLAG = 0x200

.globl load_user_space_asm

# The IRET instruction expects, when executed, the stack to have the following contents (starting from the stack pointer - the lowermost address upwards):
# 1. The instruction to continue execution at - the value of EIP (pointer to our next function).
# 2. The code segment (CS) selector to change to.
# 3. The value of the EFLAGS register to load.
# 4. The stack pointer to load.
# 5. The stack segment (SS) selector to change to.
#
# The stack prior to IRET looks like:
# ----------
#    SS      
# ----------
#    ESP      
# ----------
#   EFLAGS      
# ----------
#    CS      
# ----------
#    EIP      
# ----------

# TODO Sean: Add function definition and comments
# Switch to user space (somehow)
load_user_space_asm:
    cli             
    movl %esp, %eax 
    pushl $USER_DS  
    pushl %eax
    
    pushf               # Push EFLAGS onto the stack         
    popl %eax           # Get EFLAGS back into EAX. The only way to read EFLAGS is to pushf then pop
    orl $IF_FLAG, %eax  # Set the IF flag (same thing as STI)
    pushl %eax          # Push the new EFLAGS value back onto the stack
    
    pushl $USER_CS  
    pushl new_func_call
    iret
