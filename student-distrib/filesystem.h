
/* Create three structures:
* bootblock
* entry
* inode
*/
#ifndef _FILE_SYSTEM_H
#define _FILE_SYSTEM_H
#include "types.h"

typedef struct bootBlock {
  uint32_t dirEntries;
  uint32_t inodes;
  uint32_t datablocks;
  uint8_t reserved[52];
} bootBlock_t;
typedef struct dentry {
  uint8_t fileName [32]; // 32B
  uint32_t fileType; // 4B
  uint32_t inode; // 4B
  uint8_t reserved[24]; // 24B RESERVED
} dentry_t;
typedef struct inode {
  uint32_t length;  // 4B
  uint32_t block[1023]; // 4096 B
} inode_t;

// NEED to have memory locations of Boot Block, Inode Begin AND DataBlocks Begin
// N = ?
// D = ?

//file system init functions. gonna be similar to RTC and Keyboard and others.

void fs_init(uint32_t start);



int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry);
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry);
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length );


//uses read_dentry_by_name so we need to read in ATLEAST the same arguments
int32_t fopen(uint8_t* fname); // need to create a file structure? or is the uint8_t enough as a File do we need this to take in anything else?
int32_t fclose(uint8_t* fname);
int32_t fread(uint8_t* fname, uint32_t offset, uint8_t* buf, uint32_t length);
// read data
int32_t fwrite(void); //doesn't matter for now
/* Directory: open() , close() , write() , read()
* write will do nothing
*
*
*
 */
//uses read_dentry_by_name
int32_t dopen(uint32_t fname, dentry_t* dentry); // open directory file. So does this mean we need to take in a file instead of an entry
//read_dentry_by_index of inodes?
int32_t dread(uint32_t index, dentry_t* dentry);

//probably does nothing
int32_t dclose(); //doesn't matter for now
//probably does nothing
int32_t dwrite(void); // doesn't matter for now

#endif
