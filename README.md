# NUFS-Filesystem
This program allows a user to create and manipulate files and directories. We created 13 commands that mimic their Unix command counterpart. With this program, you are able to: create and delete files as well as directories, nest directories, read and write to files, resize files,  access the contents of a directory, and other abilities as well.

The operations include:
nufs_access,
nufs_getattr,
nufs_readdir,
nufs_mknod,
nufs_mkdir,
nufs_link,
nufs_unlink,
nufs_rmdir,
nufs_rename,
nufs_chmod,
nufs_open,
nufs_read,
nufs_write


Files and directories are created using the bitmap and block files, using a struct to represent files and directories. The hub.c file connects the block and bitmap files to the overarching nufs.c file. The nufs.c file allows us to use the FUSE library in order to create the commands and the memory for our files.


