#ifndef PAGING_H
#define PAGING_H

#define ALIGN_4KB 4096
#define PAGE_SIZE 1024 

#define VIDEO_MEM 0xB8000 // Taken from lib.c
#define KERNEL_MEM 0x400000 // Corresponds to 4MB
#define ALIGN_4KB_SHIFT 12 // We need to shift right by 12 to align to 4KB

void paging_init();

#endif /*PAGING_H*/
