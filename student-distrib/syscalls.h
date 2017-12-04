#ifndef SYS_CALL_H
#define SYS_CALL_H

#include "types.h"
#include "paging.h"

#define KB_BUF_SIZE 128
#define BUF_SIZE 1024
#define MIN_NAME_TEXT 4
#define TXT ".txt"

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
#define FILE_IN_USE 1
#define FILE_NOT_IN_USE 0
#define MAX_FILES 8
#define PROG_DIED_BY_EXCEPTION 255
#define MAX_FILE_POS 5

#define KERNEL_BASE (8 << ALIGN_1MB)
#define PCB_OFFSET  (8 << ALIGN_1KB)

// User memory paging
#define USER_PROG_LOC 0x08048000
#define USER_PROG_SIZE (4 << ALIGN_1MB)
#define USER_MEM_V (128 << ALIGN_1MB)
#define USER_MEM_P (8 << ALIGN_1MB)
#define USER_STACK ((132 << ALIGN_1MB) - 4)

#define TASK_RUNNING 1
#define MAX_PROCESSES 6

// 4 MiB page, user & supervisor-access, r/w access, present
#define USER_PAGE_SET_BITS  0x87

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

#define FOTP_OPEN       0
#define FOTP_READ       1
#define FOTP_WRITE      2
#define FOTP_CLOSE      3

#define _RTC_        0
#define _DIR_        1
#define _FILE_       2

// Stuff for the vidmap
#define KERNEL_TOP          (4 << ALIGN_1MB)
#define USER_VIDEO_MEM      (136 << ALIGN_1MB)
#define SYS_VIDEO_MEM       0xB8000
uint32_t vidmap_ptable[PAGE_SIZE] __attribute__((aligned(1 << ALIGN_4KB)));

/* Declaring Global Variables and arrays */
typedef int (*generic_fp)();

typedef struct fd {
    generic_fp* fotp; //file operations table Pointer
    uint8_t inode_number;
    uint32_t file_position; //FP
    uint8_t in_use_flag;
    uint8_t arg[KB_BUF_SIZE];
    uint8_t text_file_flag;
} fd_t;

typedef struct pcb {
    uint8_t status;         // Holds the status of the current process
    uint8_t pid;            // Process ID
    fd_t fd_arr[8];         // File descriptor array
    uint32_t self_esp;      // Pointer to own ESP (will be used by child process later)
    uint32_t self_ebp;      // Pointer to own EBP (will be used by child process later)
    uint32_t self_k_stack;  // Pointer to own kernel stack
    uint32_t self_page;     // Pointer for own paging information
    uint32_t esp_switch;    // Pointer to own ESP (for use by task-switching)
    uint32_t ebp_switch;    // Pointer to own EBP (for use by task-switching)
    struct pcb* child_pcb;       // Pointer to child process
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
