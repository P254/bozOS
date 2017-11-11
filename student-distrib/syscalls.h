#ifndef SYS_CALL_H
#define SYS_CALL_H

#include "types.h"

#define N_SYSCALLS 10
#define SYS_CALL_ADDR 0x80
#define INT_FLAG 0x200

// Used to check if the file is executable or not
// EXE_BYTE* taken from Appendix C of MP3 spec
#define EXE_BYTE0 0x7f
#define EXE_BYTE1 0x45
#define EXE_BYTE2 0x4c
#define EXE_BYTE3 0x46
#define BYTES_4 4
#define SHIFT_8 8
#define ENTRY_PT_OFFSET 24

// User memory paging
#define USER_PROG_LOC 0x08048000
#define USER_PROG_SIZE (4 << ALIGN_1MB)
#define USER_MEM_V (128 << ALIGN_1MB)
#define USER_MEM_P (8 << ALIGN_1MB)
#define USER_STACK ((132 << ALIGN_1MB) - 4)

#define TASK_RUNNING 1

// Taken from "../syscalls/ece391sysnum.h"
#define SYS_HALT        1
#define SYS_EXECUTE     2
#define SYS_READ        3
#define SYS_WRITE       4
#define SYS_OPEN        5
#define SYS_CLOSE       6
#define SYS_GETARGS     7
#define SYS_VIDMAP      8
#define SYS_SET_HANDLER 9
#define SYS_SIGRETURN   10

typedef struct fd {
    uint8_t* fotp; //file operations table Pointer
    uint8_t inode_number; //inode, only for text files
    uint8_t file_position; //FP
    uint8_t in_use_flag;
} fd_t;

typedef struct pcb {
    uint8_t status;         // Holds the status of the current process
    uint8_t pid;            // Process ID
    uint32_t* user_loc;     // Location of program in physical memory
    fd_t* fd_arr;       // File descriptor array -- TODO: Figure out what to do with this
    uint32_t* parent;       // Pointer to parent task

    /* TODO: Also store parent's kernel stack and user stack and return address */
} pcb_t;

/* Forward declarations */
int32_t halt(uint8_t status);
int32_t execute (const uint8_t* command);
int32_t read (int32_t fd, void* buf, int32_t nbytes);
int32_t write (int32_t fd, const void* buf, int32_t nbytes);
int32_t open (const uint8_t* filename);
int32_t close (int32_t fd);
int32_t getargs (uint8_t* buf, int32_t nbytes);
int32_t vidmap (uint8_t** screen_start);
int32_t set_handler (int32_t signum, void* handler);
int32_t sigreturn (void);

#endif /* SYS_CALL_H */
