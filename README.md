# NUFS-Filesystem
A filesystem made with FUSE that can perform multiple operations on files and directories. The operations include:
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
