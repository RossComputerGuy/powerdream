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

  r = pd_kmods_init();
  if (r < 0) {
    printk("Failed to load kernel modules: %d", -r);
    return 0;
  }

  // TODO: read boot parameters
  // TODO: load ramdisk
  // TODO: start init program as PID 1
  while (1);
  return 0;
}
