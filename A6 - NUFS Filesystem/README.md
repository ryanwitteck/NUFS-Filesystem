# A Simple File System

In this assignment we built a [FUSE](https://en.wikipedia.org/wiki/Filesystem_in_Userspace) filesystem driver that will let you
mount a 1MB disk image (data file) as a filesystem. This implementation only supports files of size <= 4K.




## Provided Makefile and Tests

The provided [Makefile](Makefile) does the following:

- `make nufs` - compile the `nufs` binary. This binary can be run manually as follows:
  
  ```
  $ ./nufs [FUSE_OPTIONS] mount_point disk_image
  ```
- `make mount` - mount a filesystem (using `data.nufs` as the image) under `mnt/` in the current directory
- `make unmount` - unmount the filesystem
- `make test` - run some tests on your implementation. This is a subset of tests we will run on your submission. It should give you an idea whether you are on the right path. You can ignore tests for deleting files if you are not implementing that functionality.
- `make gdb` - same as `make mount`, but run the filesystem in GDB for debugging
- `make clean` - remove executables and object files, as well as test logs and the `data.nufs`.
