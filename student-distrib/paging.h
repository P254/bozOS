#ifndef PAGING_H
#define PAGING_H

#define ALIGN_4KB 4096
#define N_PDE 1024 // A 4GB page directory supports 1024 entries of 4KB page table entries

void paging_init();

#endif /* PAGING_H */
