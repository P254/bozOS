#include "filesystem.h"
#include "lib.h"

// TODO: GLOBAL VARS

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
    int dataEntries = inodes[inode].length / 4;                            //because the length in inodes is defined as B whereas each block is 4B
    int bytesRetured = 0;
    int j;
    unsigned char temp;
    uint32_t mem_location_off;
    uint32_t blocks_used;
    blocks_used = inodes[inode].length % 4096 == 0 ? (inodes[inode].length)/4096 : (inodes[inode].length)/4096 + 1;
    data_block_t *cur_block;
    int bytes_to_copy = length; 
    int file_length = inodes[inode].length;
    if (offset > file_length)
        return -1;
    if (length == 0)
        return -1;
    if (length > inodes[inode].length)
        bytes_to_copy = inodes[inode].length;

    mem_location_off = offset + length;
    if (mem_location_off > blocks_used * 4096)
        bytes_to_copy -= offset;
    
    int start_block = offset % 4096 == 0 ? offset / 4096 : offset/4096 +1;
    int end_block = blocks_used;

    printf("inode: %d \n", inode);
    printf("n_blocks %d \n", end_block);
    printf("file length %d \n", inodes[inode].length);
    printf("len of datablocks %d \n", boot->datablocks);
    printf("copying: %d to the buffer\n", bytes_to_copy);
    printf("start block %d \n ", start_block);
    printf("memory address of start position  %d", &dataBlocks[inodes[inode].block[0]].contents);
    uint32_t start_mem = &dataBlocks[inodes[inode].block[0]].contents;
    
    
    int byte_offset = offset % 4096;
    int bytes_copied = 0;
    int initial_offset, copy_len;

    for (i = start_block-1; i < end_block; i++) {
        initial_offset = (i == start_block-1) ? byte_offset : 0;
        copy_len = (bytes_to_copy >= 4096) ? 4096 : bytes_to_copy;
        copy_len = ((4096-initial_offset) < copy_len) ? (4096-initial_offset) : copy_len;
        printf("\n printing %d in block %d with initial offset %d \n" ,copy_len,i,initial_offset) ;
        memcpy(buf + bytes_copied /*TODO check this*/, dataBlocks[inodes[inode].block[i]].contents + initial_offset, copy_len);
        bytes_copied += copy_len;
        bytes_to_copy -= copy_len;
        if (bytes_to_copy <= 0) {
            break;
        }
    }

    // should do some math to first determine what can be placed into this buffer.
    // this will have to return number of bytes that are entered into this buffer
    // we will find the inodes[]. Then we will loop through the blocks and put the block[i] into buf[i]
    // we have data blocks of size 4kB as well. How do we access these data blocks? must have some sort of global variable

    return bytes_copied;
}
