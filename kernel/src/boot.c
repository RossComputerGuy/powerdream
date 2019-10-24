#include <arch/panic.h>
#include <kernel/dev/block.h>
#include <kernel/dev/char.h>
#include <kernel/dev/tty.h>
#include <kernel/device.h>
#include <kernel/fs.h>
#include <kernel/module.h>
#include <kernel/process.h>
#include <kernel/syscall.h>
#include <kernel-config.h>
#include <stdio.h>

int main(int argc, char** argv) {
  printf("PowerDream Kernel v"PD_KERNEL_VERSION" - (C) 2019 Tristan Ross (The Computer Guy)\n");
  printf("Compiled on "__DATE__", "__TIME__" by "PD_COMPILE_USER"\n");

  pd_dev_init();
  pd_blkdev_init();
  pd_chardev_init();
  pd_fs_init();
  pd_process_init();
  pd_syscalls_init();

  printf("Bootstrap completed, 2nd stage of kernel boot process starting\n");

  int r = pd_kmods_init();
  if (r < 0) arch_panic("Failed to load kernel modules");

  printf("%d kernel modules loaded\n", pd_getmodcount());

  // TODO: read boot parameters
  // TODO: load ramdisk
  // TODO: start init program as PID 1
  while (1);
  return 0;
}
