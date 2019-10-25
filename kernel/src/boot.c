#include <arch/arch.h>
#include <kernel/dev/block.h>
#include <kernel/dev/char.h>
#include <kernel/dev/tty.h>
#include <kernel/fs/devfs.h>
#include <kernel/fs/procfs.h>
#include <kernel/device.h>
#include <kernel/fs.h>
#include <kernel/klog.h>
#include <kernel/module.h>
#include <kernel/process.h>
#include <kernel/syscall.h>
#include <kernel-config.h>
#ifndef PD_COMPILED_CMDLINE
#include <dc/fs_iso9660.h>
#include <dc/fs_vmu.h>
#include <kos/fs.h>
#endif
#include <malloc.h>
#include <stdio.h>

int main(int argc, char** argv) {
  printk("PowerDream Kernel v"PD_KERNEL_VERSION" - (C) 2019 Tristan Ross (The Computer Guy)"
#if PD_BUILD_TYPE != Release
  " - Compiled on "__DATE__", "__TIME__" by "PD_COMPILE_USER
#endif
  , 0);

  pd_dev_init();
  pd_blkdev_init();
  int r = pd_chardev_init();
  if (r < 0) {
    printk("Failed to initialize character devices: %d", -r);
    return 0;
  }
  pd_fs_init();
  pd_process_init();
  pd_syscalls_init();

  r = pd_devfs_init();
  if (r < 0) {
    printk("Failed to initialize device filesystem: %d", -r);
    return 0;
  }

  r = pd_procfs_init();
  if (r < 0) {
    printk("Failed to initialize process filesystem: %d", -r);
    return 0;
  }

  printk("Bootstrap completed, 2nd stage of kernel boot process starting", 0);

#ifndef PD_COMPILED_CMDLINE
  fs_iso9660_init();
  fs_vmu_init();
  file_t cmdline_fd = fs_open("/cd/cmdline.txt", O_RDONLY);
  if (!cmdline_fd) cmdline_fd = fs_open("/rd/cmdline", O_RDONLY);
  if (!cmdline_fd) cmdline_fd = fs_open("/rd/boot/cmdline", O_RDONLY);
  if (!cmdline_fd) cmdline_fd = fs_open("/vmu/a1/cmdline", O_RDONLY);
  if (!cmdline_fd) cmdline_fd = fs_open("/vmu/a1/boot/cmdline", O_RDONLY);
  if (cmdline_fd) {
    char* cmdline = malloc(fs_total(cmdline_fd));
    if (cmdline == NULL) {
      printk("Failed to allocate RAM for the command line: %d bytes needed", fd_total(cmdline_fd));
      fs_close(cmdline_fd);
    } else {
      memset(cmdline, 0, fs_total(cmdline_fd));
      fs_read(cmdline_fd, cmdline, fs_total(cmdline_fd));
      pd_setcmdline(cmdline);
      fs_close(cmdline_fd);
    }
  } else printk("Command line arguments file not found on the CD-ROM, VMU, and ramdisk");
  fs_vmu_shutdown();
  fs_iso9600_shutdown();
#endif

  {
    size_t i;
    for (i = 0; i < pd_fs_getcount(); i++) {
      pd_fs_t* fs = pd_fs_fromindex(i);
      if (fs->type != PD_FS_PHYS) continue;
      size_t x;
      for (x = 0; x < pd_blkdev_getcount(); x++) {
        pd_blkdev_t* blkdev = pd_blkdev_fromindex(x);
        r = pd_fs_mount(fs, blkdev, NULL, "/", 0, NULL);
        if (r < 0) continue;
      }
    }

    if (pd_mountpoint_fromtarget("/") == NULL) {
      printk("Failed to mount root filesystem");
      return 0;
    }
  }

  r = pd_kmods_init();
  if (r < 0) {
    printk("Failed to load kernel modules: %d", -r);
    return 0;
  }

  pd_inode_t* init_inode;
  r = pd_resolve_path(&init_inode, "/init");
  if (r < 0) {
    if ((r = pd_resolve_path(&init_inode, "/sbin/init")) < 0) {
      if ((r = pd_resolve_path(&init_inode, "/bin/init")) < 0) {
        arch_panic("Init program not found at expected paths: /init, /sbin/init, and /bin/init");
      }
    }
  }

  void* init_prog = malloc(init_inode->size);
  if (init_prog == NULL) arch_panic("Not enough RAM to allocate for init program");
  if (!S_ISFILE(init_inode->mode)) arch_panic("Init program is not a file");
  if (init_inode->opts.read == NULL) arch_panic("Init inode does not implement read call");
  r = init_inode->opts.read(init_inode, NULL, init_prog, init_inode->size);
  if (r < 0) {
    printk("Inode read failed: %d", -r);
    arch_panic("Failed to read init program into buffer");
  }

  pd_process_t* init_proc;
  pid_t init_pid = pd_process_create(&init_proc, init_prog, 0, NULL);
  if (init_pid < 0) {
    printf("Process creation failed: %d", -r);
    arch_panic("Failed to start init program");
  }

  while (1) pd_process_cleanup();
  return 0;
}
