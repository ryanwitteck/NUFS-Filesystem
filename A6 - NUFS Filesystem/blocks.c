#define _GNU_SOURCE
#include <string.h>

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "blocks.h"

#include "bitmap.h"
#include "blocks.h"

const int BLOCK_COUNT = 256; // we split the "disk" into 256 blocks
const int BLOCK_SIZE = 4096; // = 4K
const int NUFS_SIZE = 4096*256; // = 1MB

const int BLOCK_BITMAP_SIZE = 256 / 8;
// Note: assumes block count is divisible by 8

static int   blocks_fd   = -1;
static void* blocks_base =  0;

static char* bmap;
static void* blocks;
static int max_nodes = 32;
static char* nmap;
static inode* nodes;
static int next_node = 2;
//done?
void blocks_init(const char* image_path)
{ 
  blocks_fd = open(image_path, O_CREAT | O_RDWR, 0644);
  assert(blocks_fd != -1);
  
  int rv = ftruncate(blocks_fd, NUFS_SIZE);
  assert(rv == 0);
  
  blocks_base = mmap(0, NUFS_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, blocks_fd, 0);
  assert(blocks_base != MAP_FAILED);
  //end of starter code, removed some code 
  //helps us establish a base
  void* here = blocks_base;
  nmap = here; //global positioning in the nodemap
  here += max_nodes * sizeof(char); //offset for data, allocating max space
  bmap = here; //space for the data
  here += BLOCK_COUNT * sizeof(char); //accounting for the space we just made
  nodes = here;
  int i; 
  for (i = 0; i < max_nodes; i++) {
        here += sizeof(inode);
  } //accounting for the space of each node
  blocks = blocks_base + (5 * BLOCK_SIZE);
}

void blocks_free()
{   //starter code

    int rv = munmap(blocks_base, NUFS_SIZE);
    assert(rv == 0);
}

void* blocks_get_block(int bnum)
{   //starter code
    return blocks + 4096 * bnum;
}

inode* blocks_get_node(int node_id)
{   //simply returns the inode at the given id
    return &(nodes[node_id]);
}

int blocks_find_empty_block()
{ 
  int i;
  int bnum = -1;
  for (i = 0; i < BLOCK_COUNT; i++) {
    if (bmap[i] == 0) { // this block is empty
        bnum = i; 
        bmap[i] = 1;
        break;
    }
  }//find the first empty block in the map
  return bnum;
}

int find_empty_inode()
{ 
  int i;
  int inum = -1;
  for (i = 2; i < BLOCK_COUNT; i++) {
    if (nmap[i] == 0) { // if block is empty
         inum = i; 
         nmap[i] = 1;
         break;
    }
  } //same as previous function, we are finding an empty node in the map
    return inum;
}

inode* get_node_at_block(const char *path)
{ 
  int i; 
  for (i = 0; i < max_nodes; i++) {
    if (nmap[i] == 1 && strcmp(nodes[i].path, path) == 0) {
        return &(nodes[i]);
    }
  }//waits for the given path to show up and returns the node at that path
  return NULL;
}

void print_node(inode* node)
{ 
  if (node) { //print statement for reference, of no real use
        printf("node{refs: %d, mode: %04o, size: %d, path: %s}\n", node->refs, node->mode, node->size, node->path);
  } //if not found
  else {
        printf("Node not found");
  }
}

int read_used_inodes(const char* path, void* buf, fuse_fill_dir_t filler)
{ 
  inode* node = get_node_at_block(path); //get our path
  int* data = blocks_get_block(node->direct[0]); //get our data from pointer
  int num_ents = data[0];
  int i;
  //TODO double check assignments
  for (i = 0; i < num_ents; i++) {
    struct stat st;
    int current = data[i + 1]; //where we are
    memset(&st, 0, sizeof(struct stat));
    st.st_uid  = getuid();
    st.st_ino = current;
    st.st_mode = nodes[current].mode; //pairing our attributes to st and pushing it to filler later
    st.st_size = nodes[current].size;
    filler(buf, ((void*)&(nodes[current].path)) + 1, &st, 0);
  }
  return 0;
}

int give_inode_block(inode* node)
{
  //gives the node another empty block
  int bnum = blocks_find_empty_block();
  node->direct[node->num_blocks] = bnum;
  node->num_blocks++;
  return bnum;
}

//TODO in progress need to test
void add_file_to_dir(const char* dir, const char* file)
{
  inode* dir_node = get_node_at_block(dir); //gets directory node
  inode* file_node = get_node_at_block(file); //gets file node
  int* data = blocks_get_block(dir_node->direct[0]); //gets the block map aligned
  int dir_size = data[0];
  data[dir_size + 1] = file_node->inode_num; //adds the size and the file to the given directory
  data[0]++; //size
}

//free from memory? TODO maybe deref it from other nodes
void free_inode(inode* node)
{
  //deallocates references, essentially freeing it
  nmap[node->inode_num] = 0;
  if (node->refs > 0) {
    int i;
    for (i = 0; i < node->num_blocks; i++) {
         bmap[node->direct[i]] = 0;
    }
   }
}

void remove_inode_from_directory(inode* dir, int node_num)
{
  //gets the block of the directory given 
  int* entry = blocks_get_block(dir->direct[0]);
  int dir_size = entry[0];
  if (entry[dir_size] == node_num) {
        entry[0]--; //if we found it just dereference it
        return;
  }
  int i; //if not go through the directory
  for (i = 1; i < dir_size; i++) {
    if (entry[i] == node_num) { //if what we are looking at is the node, dereference it and shrink the size
            entry[i] = entry[dir_size];
            entry[0]--;
            return;
    }
  }
}
