# Booting the PowerDream Kernel

The PowerDream kernel's boot process is relatively simple; bootstrapping, setting the cmdline string, initializing the kernel modules, mounting the disk, then running the init program in PID 1 while the main process cleans up the zombie processes.

## Bootstrapping

PowerDream's kernel bootstrap step is very simple; initialize the device APIs, initialize the filesystems API, initialize multiprocessing, and initialize the system calls. The basic Unix filesystems are also initialized during this step of the boot process.

## Command Line Arguments

The kernel loads has two methods of loading the arguments, the first is compiled in arguments set by the CMake options. The second method is loading for a storage medium using KallistiOS's drivers. The kernel can load the arguments from a VMU, a CD-ROM, or a ramdisk. The file is required to be named `cmdline` and must contain text in order for the command line arguments to properly load.