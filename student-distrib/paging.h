#ifndef PAGING_H
#define PAGING_H

#define ALIGN_4KB 4096
#define N_PDE 1024 // A 4GB page directory supports 1024 entries of 4KB page table entries
#define N_PTE 1024 // A 4 MiB page table has 1024 entries

#define VIDEO_MEM 0xB8000 // Taken from lib.c
#define KERNEL_MEM 0x400000

void paging_init();

#endif /* PAGING_H */
