#include "types.h"

#ifndef _FILE_SYSTEM_H
#define _FILE_SYSTEM_H

/* Magic numbers */
#define MAX_D_ENTRIES 62
#define FILE_NAME_LEN 32
#define BLK_SZ 4096
#define BOOT_BLOCK_SIZE 64

/* Create three structures:
 * bootblock
 * entry
 * inode
 */
typedef struct bootBlock {
    uint32_t dirEntries;
    uint32_t inodes;
    uint32_t datablocks;
    uint8_t reserved[52]; // So that bootBlock size = 64B
} bootBlock_t;

typedef struct dentry {
    uint8_t fileName[32]; // 32B
    uint32_t fileType;    // 4B
    uint32_t inode;       // 4B
    uint8_t reserved[24]; // 24B RESERVED
} dentry_t;

typedef struct inode {
    uint32_t length;      // 4B
    uint32_t block[1023]; // 4096 B
} inode_t;

typedef struct dataBlock {
    uint8_t contents[4096];
} data_block_t;

dentry_t *dentries;
inode_t *inodes;
bootBlock_t *boot;
data_block_t *dataBlocks;
int loc;
// File system init functions. Similar to RTC, keyboard and others.
void fs_init(uint32_t start);

int32_t read_dentry_by_name(const uint8_t *fname, dentry_t *dentry);
int32_t read_dentry_by_index(uint32_t index, dentry_t *dentry);
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t *buf, uint32_t length);

int32_t fopen(const uint8_t *fname);
int32_t fclose(uint8_t *fname);
int32_t fread(uint8_t *fname, uint8_t *buf, int32_t nbytes);
int32_t fwrite(void);

int32_t dopen(const uint8_t* fname, dentry_t *dentry);
int32_t dread(uint8_t fd, uint8_t *buf, int32_t nbytes);

int32_t dclose();
int32_t dwrite(void);

#endif
