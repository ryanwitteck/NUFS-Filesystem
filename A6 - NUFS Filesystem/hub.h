#ifndef NUFS_HUB_H
#define NUFS_HUB_H

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <fuse.h>
// This file is the hub, which I used to combine both the storage and directory header files into one, with more appropriate methods.

//inits the program
void hub_init(const char* path);
//gets the status of the path, linked to get_attr
int hub_get_stat(const char* path, struct stat* st);
//reads
int hub_read(const char *path, char *buf, size_t size, off_t offset);
//writes
int hub_write(const char *path, const void *buf, size_t size, off_t offset);
//creates a file at the given path with the given mode
int hub_mknod(const char *path, mode_t mode);
//creates a dir at the given path with the given mode
int hub_mkdir(const char *path, mode_t mode);
//read the directory, corresponds to readdir
int hub_readdir(const char* path, void* buf, fuse_fill_dir_t filler);
//checks if we have access to this path or not, corresponds to get access in nufs.c
int hub_access(const char* path);
//renames the file
int hub_rename(const char *from, const char *to);
//deletes a file
int hub_remove(const char* path);
//links a target file to another name, corresponds to link/unlink from nufs.c
int hub_link(const char* target, const char* link_name);
#endif
