#include "x86_desc.h"

#ifndef PAGING_H
#define PAGING_H

#define ALIGN_4MB 22
#define ALIGN_4KB 12
#define PAGE_SIZE 1024

#define PROG_MEM_SIZE 4 // We haev 4 such 4 MiB blocks that makes 16 MiB
#define ALIGN_1MB 20
#define ALIGN_1KB 10

uint32_t page_directory[PAGE_SIZE] __attribute__((aligned(1 << ALIGN_4KB))); //directory table
uint32_t page_table[PAGE_SIZE] __attribute__((aligned(1 << ALIGN_4KB))); // pages table

#define VIDEO_MEM 0xB8000 // Taken from lib.c
#define KERNEL_MEM 0x400000 // Corresponds to 4 MiB = 0x400000

#define TERM_1_VIDEO    (VIDEO_MEM + 1*(1 << 12))   // + 4KB
#define TERM_2_VIDEO    (VIDEO_MEM + 2*(1 << 12))   // + 8KB
#define TERM_3_VIDEO    (VIDEO_MEM + 3*(1 << 12))   // + 12KB

void paging_init();

#endif /*PAGING_H*/
