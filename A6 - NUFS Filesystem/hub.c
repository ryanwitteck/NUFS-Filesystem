#include <stdio.h>
#include <string.h>

#include "hub.h"
#include "blocks.h"

//TODO: Shrink, test grow, make sure grow works, esp in other methods, del should work, creating dirs shoould work, 
// EC should be good, need to do shrink, might run out of time. LEAVE SHRINK BLANK IF UNABLE TO VERIFY IT WORKS
// small bug in code that wont let us unmount or clean, cannot find the segfault --- FIXED

//inits the program
void hub_init(const char* path)
{  
   blocks_init(path); //make our block
   hub_mkdir("/", 040777); //create dir
}

//gets the status of the path, linked to get_attr
int hub_stat(const char* path, struct stat* st){
  inode* node = get_node_at_block(path);
  if (node == NULL) {
      return -1;
  }
  //same as the one in nufs, we are setting the status of the path, as well as making sure it exists as showed above
  memset((void*)st, 0, sizeof(struct stat));
  st->st_mode = node->mode;
  st->st_size = node->size;
  st->st_ino = node->inode_num;
  st->st_uid = getuid();
  st->st_nlink = 1;
  return 0;
}

//reads
int hub_read(const char *path, char *buf, size_t size, off_t offset){
  inode* node = get_node_at_block(path);
  if (node == NULL) {
      return -1;
  }//existance check
  if (size == 0 || node->num_blocks == 0) {
      return 0;
  } //make sure we are acutally reading something
  int rsize = size;
  void* block = blocks_get_block(node->direct[0]);
  if (offset > 0) {
      block += offset;
  }//add the given offset so we are in the correct place
  memcpy(buf, block, rsize);//read
  return rsize;
}
//writes
int hub_write(const char *path, const void *buf, size_t size, off_t offset){
  inode* node = get_node_at_block(path);
  if (node == NULL) {
      return -1;
  }//existance check
  if (size == 0) {
      return 0;
  }//nothing to write
  if (node->num_blocks == 0) {
      give_inode_block(node);
  }
  int wsize = size;
  void* chunk = blocks_get_block(node->direct[0]);
  if (offset > 0) {
      chunk += offset;
  }//adds offset so we are in the desired writing place
  memcpy(chunk, buf, wsize); //same as read, but copying to the chunk instead of the buffer.
  if (size + offset > node->size) {
      node->size = size + offset;
  }//adjusting the offset for overflow
  return wsize;
}

//read the directory, corresponds to readdir
int hub_readdir(const char* path, void* buf, fuse_fill_dir_t filler){\
  return read_used_inodes(path, buf, filler); //goes through our inodes to check which ones are not empty and returns their name
}

//creates a file at the given path with the given mode
int hub_mknod(const char *path, mode_t mode){
  //finds node at path
  inode* node = get_node_at_block(path);
  if (node != NULL) {
      return node->inode_num;
  }//reverse existance check
  //gets next available node
  int node_num = find_empty_inode();
  //assigns new node to the empty spot
  node = blocks_get_node(node_num);
  node->mode = mode;
  node->inode_num = node_num;
  if (mode != 040777) {
      node->mode = 0100666;
  } //assigns mode to this, for some reason it doesnt work with 040777 and 0100644, so I used these instead
  strcpy(node->path, path); //assign path to the node path attribute
  if (strcmp(path, "/") != 0) {
      add_file_to_dir("/", path);
  } //as long as its not a directory, add the file to the directory
    return node_num;
}

//creates a dir at the given path with the given mode
int hub_mkdir(const char *path, mode_t mode){
  //same as mknod, just make a file but with a different mode   
  int node_num = hub_mknod(path, 040777);
  inode* node = blocks_get_node(node_num); //node from the reference number from create file
  if (node->num_blocks == 0) {
      int ents = 0; //strange behavior here, ask in OH?
      hub_write(path, &ents, sizeof(int), 0);
  }//return reference number
  return node_num;
}

int hub_link(const char* from, const char* to)
{ //links our node fields, helpful for various operations: could be useful in EC
  int new = find_empty_inode();
  inode* nto = blocks_get_node(new); //get new node from empty pointer
  inode* from_node = get_node_at_block(from); //get node from "from" path
  nto->inode_num = new;
  nto->mode = from_node->mode; //align attributes
  strcpy(nto->path, to);//assign "to" path to "node to" path 
  nto->num_blocks = from_node->num_blocks; //copy attributes
  int i;
  for (i = 0; i < nto->num_blocks; i++) {
      nto->direct[i] = from_node->direct[i];
  }//copy attributes
  nto->refs++;
  from_node->refs++;
  print_node(nto);
  print_node(from_node); //check print statements for validity and consistancy
  return 0;
}

//checks if we have access to this path or not, corresponds to get access in nufs.c
int hub_access(const char* path){
    inode* node = get_node_at_block(path);
    if (node == NULL) {
        return -1;
    } //this function is just an existance check, simple
    return 0;
}
//renames the file
int hub_rename(const char *from, const char *to){
    inode* node = get_node_at_block(from); //gets node
    memset(node->path, 0, 64); //wipes the path
    strcpy(node->path, to);// recreates the path with to, and the file has been renamed, hopefully
    return 0;
}

int hub_remove(const char* path)
{
    inode* node = get_node_at_block(path);
    if (node == NULL) { //get node from path
        return -1;
    }//existance check
    free_inode(node); //free the node
    inode* dir = get_node_at_block("/"); //deallocate the directory references to this inode
    remove_inode_from_directory(dir, node->inode_num);
    return 0; //now the node doesnt exist, nor the path, nor the links to any other data
}
