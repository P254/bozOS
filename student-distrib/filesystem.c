#include "filesystem.h"
#include "lib.h"

// TODO: GLOBAL VARS
dentry_t *dentries; // is this right?
inode_t *inodes;    // is this right?
bootBlock_t *boot;
data_block_t *dataBlocks;
// uint32_t * dataBlocks; // Memory location of the data Block
// datablocks array? what type is this? 4kb Blocks?
// where do they get read from?
// can we just look at memory locations and increment by 4kB each time?
// CP 2: No fopen/read dopen/read
// ls in system calls will help us understand how directory read will be called

// this just makes it a bit eaiser for us to setup variables for the other functions. not really needed
void fs_init(uint32_t start) // this will take in mod_start
{
    // which syntax here is right lol
    boot = (void *)start;
    dentries = (void *)start + 64;                          /*+ 64B?*/
                                                            // do we need some sort of cast to a dentry_t ?
    inodes = (void *)start + 4096;                          /* + next Block. 4kB */
                                                            // do we need some soft of cast to inode_t?
    dataBlocks = (void *)start + 4096 * (boot->inodes + 1); // our size will be + 1 because we want to account for the bootBlock

    // pointer to head of dentries?
    //  pointer to head of boot block
    // pointer to head of inodes
    // need to init some global variables and set the pointer to
}

int32_t fopen(uint8_t *fname) {
    // what am i initializing?
    return -1;
}
int32_t fclose(uint8_t *fname) {
    return -1;
}
/*int32_t fread(uint8_t* fname, uint32_t offset, uint8_t* buf, uint32_t length)
{
  return -1;
}*/
int32_t fwrite(void) {
    return -1;
}

int32_t dopen(uint32_t fname, dentry_t *dentry) {
    return read_dentry_by_name(fname, dentry);
    //return -1;
}
int32_t dread(uint32_t index, dentry_t *dentry) {
    return read_dentry_by_index(index, dentry);
}
int32_t dclose() {
    return -1;
}
int32_t dwrite(void) {
    return -1;
}

int32_t read_dentry_by_name(const uint8_t *fname, dentry_t *dentry) {
    int i;
    for (i = 0; i < 62; i++) // loop through every directory entry
    {
        printf(dentries[i].fileName);
        putc('\n');
        // if fname = directory entries string. then call it a day

        if (strncmp(fname, dentries[i].fileName, 32) == 0) // if they are the same we will fill in the dentry with something?
        {
            clear();
            memcpy(dentry, &dentries[i], 64);
            return 0;
        }
    }
    printf("didnt find shit");
    // nothing
    return -1;
}
int32_t read_dentry_by_index(uint32_t index, dentry_t *dentry) {
    int i;
    clear();
    if (dentries[index].fileName != NULL)
    {
        memcpy(dentry, &dentries[index], 64);
        return 0;
    }
    // cant find.
    return -1;
}

// something like ece391emulate.c/ece391_read sys call??
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t *buf, uint32_t length) {
    int i;
    int n_blocks = length % 4096 == 0 ? length / 4096 : length / 4096 + 1; // number of blocks we need to look at!!
    int dataEntries = inodes[inode].length / 4;                            //because the length in inodes is defined as B whereas each block is 4B
    int bytesRetured = 0;

    printf("inode: %d \n", inode);
    printf("n_blocks %d \n", n_blocks);
    int j;
    unsigned char temp;
    data_block_t *cur_block;
    printf("file length %d \n", inodes[inode].length);
    printf("len of datablocks %d \n", boot->datablocks);
    for (i = 0; i < 1; i++) {
        // need to do some math here to determine how many blocks we want. We also need to implement offset crap. Very easy stuff
        // memcpy(cur_block ,&dataBlocks[inodes[inode].block[i]],4096);

        // instead of this we need to add to the buffer. that is essentially the same shit.  we can probably do some sort of memcpy?

        /* TODO: SEAN. change this from printing to the screen to adding to the buffer. I can do this character by character, but I think
        we can use memcpy */

        // clear();
        memcpy(buf, dataBlocks[inodes[inode].block[i]].contents + 80, 4016);
        // for (j = 0; j < inodes[inode].length; j++)
        // {
        //     putc(dataBlocks[inodes[inode].block[i]].contents[j]);
        // }
    }
    // for (i = 0; i < n_blocks ; i++)
    // {
    //     cur_block = dataBlocks + (4096*(inodes[inode].block[i])); // start + 4096 + (N+D-1)
    //     printf("%d ",*cur_block);
    //   // some sort of valid check?
    //   // how to add to buffer with offset?
    //
    // }

    // should do some math to first determine what can be placed into this buffer.
    // this will have to return number of bytes that are entered into this buffer
    // we will find the inodes[]. Then we will loop through the blocks and put the block[i] into buf[i]
    // we have data blocks of size 4kB as well. How do we access these data blocks? must have some sort of global variable

    return -1;
}
