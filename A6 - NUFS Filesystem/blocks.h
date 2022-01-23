// based on cs3650 starter code
// Changed to include both inode header and block header into one.
#ifndef BLOCKS_H
#define BLOCKS_H

#include <stdio.h>
#include <fuse.h>

#include "hub.h"
const int BLOCK_COUNT; // we split the "disk" into blocks (default = 256)
const int BLOCK_SIZE;  // default = 4K
const int NUFS_SIZE;   // default = 1MB

const int BLOCK_BITMAP_SIZE; // default = 256 / 8 = 32

typedef struct inode {
    int refs;       // reference count
    int mode;       // permission & type
    int size;       // bytes for file
    int num_blocks; // number of blocks used for file
    int direct[10];  // array of direct block numbers
    int indirect;   // number of block that contains more blocks
    int inode_num;
    char path[64];   // full path name
} inode;

// Load and initialize the given disk image.
void blocks_init(const char* image_path);

// Close the disk image.
void blocks_free();

// Get the block with the given index, returning a pointer to its start.
void* blocks_get_block(int bnum);

// Get the block at the given node and return it
inode* blocks_get_node(int node_id);

// Find the next empty block
int  blockss_find_empty();

// Prints the node
void print_node(inode* node);

// Find the next empty inode
int  find_empty_inode();

// Find the inode at the given path
inode* get_node_at_block(const char *path);

// Reads the inodes that have been established
int  read_used_inodes(const char* path, void* buf, fuse_fill_dir_t filler);

// Gets the block of the given inode
int  give_inode_block(inode* node);

// Adds file to the directory
void add_file_to_dir(const char* dir, const char* file);

// Frees the given inode
void free_inode(inode* node);

// Deletes the given file from the given directory
void remove_inode_from_directory(inode* dir, int node_num);

#endif
