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
void fs_init(uint32_t start) {// this will take in mod_start
    boot = (void*) start;
    dentries = (void*) start + 64; // First 64B reserved for boot block statistics
    inodes = (void*) start + BLK_SZ; 
    dataBlocks = (void*) start + BLK_SZ * (boot->inodes + 1); // Our size will be + 1 because we want to account for the bootBlock
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

/*
 * read_dentry_by_name
 *   DESCRIPTION: Given a file name, copies file information to *dentry
 *   INPUTS: fname -- the file name we want to look for
 *           dentry -- pointer to dentry_t struct that we want to copy the information to
 *   OUTPUTS: dentry -- modifies the dentry struct if we find a positive match
 *   RETURN VALUE: int -- 0 for success, -1 on failure
 *   SIDE EFFECTS: none
 */
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry) {
    if (strlen((int8_t*)fname) == 0) return -1;

    uint8_t i;
    // We loop through every directory entry
    for (i = 0; i < MAX_D_ENTRIES; i++) { 
        if (strncmp((int8_t*)fname, (int8_t*)dentries[i].fileName, FILE_NAME_LEN) == 0) { 
            // If they are the same, we fill in the dentry
            memcpy(dentry, &dentries[i], sizeof(dentry_t));
            return 0;
        }
    }
    // Default case: return failure
    return -1;
}

/*
 * read_dentry_by_index
 *   DESCRIPTION: Given an index value, copies file information to *dentry
 *   INPUTS: index -- holds value that is the index of the file struct that we want
 *           dentry -- pointer to dentry_t struct that we want to copy the information to
 *   OUTPUTS: dentry -- modifies the dentry struct if we find a positive match
 *   RETURN VALUE: int -- 0 for success, -1 on failure
 *   SIDE EFFECTS: none
 */
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry) {
    if (index > MAX_D_ENTRIES) return -1;
    
    else if (strlen((int8_t*) dentries[index].fileName) > 0) {
        memcpy(dentry, &dentries[index], sizeof(dentry_t));
        // Ensure that we have a null-terminated string
        dentry->fileName[FILE_NAME_LEN-1] = '\0'; 
        return 0;
    } 
    // Default case: return failure
    return -1; 
}

/*
 * read_data
 *   DESCRIPTION: Reads data from a file, using a given inode.
 *   INPUTS: inode -- the index to the inode we want to read data from
 *           offset -- number of bytes to skip at the beginning of the file
 *           buf -- the target we want to copy our file to 
 *           length -- the number of bytes 
 *   OUTPUTS: buf -- copies data from the file to the buffer
 *   RETURN VALUE: int -- number of bytes copied, or -1 if failure
 *   SIDE EFFECTS: none
 */
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t *buf, uint32_t length) {
    int32_t i, mem_location_off, file_length, blocks_used,
            start_block, end_block, bytes_to_copy, bytes_copied, initial_offset, copy_len;
    
    // TODO: Check for invalid inputs
    // Set the file length
    file_length = inodes[inode].length;
    // Check edge case
    if (offset > file_length || length == 0 || file_length == 0) return -1; 

    // Calculate the block size 
    blocks_used = (file_length % BLK_SZ == 0) ? (file_length)/BLK_SZ : (file_length)/BLK_SZ + 1;
    // Calculate # bytes we need to copy and # of bytes copied so far
    bytes_to_copy = (length < file_length) ? length : file_length ; // Take the smaller of the two 
    bytes_copied = 0;
    
    // Calculate memory location w/ offset
    mem_location_off = offset + length;
    // Update # bytes we need to copy if necessary
    if (mem_location_off > blocks_used * BLK_SZ) bytes_to_copy -= offset;
    
    // Calculate the index of start and end blocks
    start_block = (offset % BLK_SZ == 0) ? offset/BLK_SZ : offset/BLK_SZ +1;
    end_block = blocks_used;

    for (i = start_block-1; i < end_block; i++) {
        // Calculate the initial byte offset + size of bytes we need to copy 
        initial_offset = (i == start_block-1) ? (offset % BLK_SZ) : 0;
        copy_len = (bytes_to_copy >= BLK_SZ) ? BLK_SZ : bytes_to_copy;
        copy_len = ((BLK_SZ-initial_offset) < copy_len) ? (BLK_SZ-initial_offset) : copy_len;
        
        // Perform the copy operations, update the # bytes copied and # remaining to copy
        memcpy(buf + bytes_copied, dataBlocks[inodes[inode].block[i]].contents + initial_offset, copy_len);
        bytes_copied += copy_len;
        bytes_to_copy -= copy_len;
        
        if (bytes_to_copy <= 0) break;
    }
    return bytes_copied;
}
