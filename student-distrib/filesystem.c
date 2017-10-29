#include "filesystem.h"
#include "lib.h"

// TODO: GLOBAL VARS
dentry_t *dentries ; // is this right?
inode_t *inodes; // is this right?
bootBlock_t *boot;
uint32_t * dataBlocks; // Memory location of the data Block 
// datablocks array? what type is this? 4kb Blocks? 
// where do they get read from?
// can we just look at memory locations and increment by 4kB each time?
// CP 2: No fopen/read dopen/read
// ls in system calls will help us understand how directory read will be called

// TODO: Figure out how this shit is written in memory
void fs_init(uint32_t start) // this will take in mod_start
{
  // which syntax here is right lol
  boot = (void *)start;
  dentries = (void*)start + 64;  /*+ 64B?*/ ; // do we need some sort of cast to a dentry_t ?
  inodes =  (void*)start + 4096; /* + next Block. 4kB */ ;  // do we need some soft of cast to inode_t? 
  uint32_t dataBlockOffset = (start + 4);
  dataBlocks = (void*)start + 4096* dataBlockOffset ; 
  // pointer to head of dentries?
  //  pointer to head of boot block
  // pointer to head of inodes 
  // need to init some global variables and set the pointer to

}

/*
int32_t fopen(uint8_t* fname)
{
  return -1;
}
int32_t fclose(uint8_t* fname)
{
  return -1;
}
int32_t fread(uint8_t* fname, uint32_t offset, uint8_t* buf, uint32_t length)
{
  return -1;
}
int32_t fwrite(void)
{
  return -1;
}


int32_t dopen(uint32_t fname, dentry_t* dentry)
{
  return -1;
}
int32_t dread(uint32_t index, dentry_t* dentry)
{
  return -1;
}
int32_t dclose()
{
  return -1;
}
int32_t dwrite(void)
{
  return -1;
}



int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry)
{
  int i;
  for ( i = 0 ; i < 62; i++) // loop through every directory entry
  {
    // if fname = directory entries string. then call it a day
    if (strncmp(fname,dentries[i].fileName,32)) // if they are the same we will fill in the dentry with something?
    {
      dentry->fileName = dentries[i]->fileName;
      dentry->inode = dentries[i]->inode;
      return 0;
    }
  }
  // nothing
  return -1;
}
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry)
{
  if (dentries[index].fileName != NULL)
  {
    dentry->fileName = dentries[index]->fileName;
    dentry->inode = dentries[index]->inode;
    return 0;
  }
  // cant find.
  return -1;
}

// something like ece391emulate.c/ece391_read sys call??
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length )
{
  int i;
  int dataEntries = inodes[inode].length /4 ; //because the length in inodes is defined as B whereas each block is 4B
  int bytesRetured = 0;
  uint8_t * to,from;
  to = buf;
  from = inode
  for (i = 0; i < dataEntries ; i++)
  {
    to=to;
    // some sort of valid check? 
    // how to add to buffer with offset?

  }

  // should do some math to first determine what can be placed into this buffer.
  // this will have to return number of bytes that are entered into this buffer
  // we will find the inodes[]. Then we will loop through the blocks and put the block[i] into buf[i]
  // we have data blocks of size 4kB as well. How do we access these data blocks? must have some sort of global variable 

  return -1;
}

*/
