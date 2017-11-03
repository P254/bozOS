#ifndef SYS_CALL_H
#define SYS_CALL_H

#include "types.h"

#define N_SYSCALLS 10
#define SYS_CALL_ADDR 0x80

/* Forward declarations */
int32_t handle_syscall(); 
int32_t syscall_halt(uint8_t status);

#endif /* SYS_CALL_H */
