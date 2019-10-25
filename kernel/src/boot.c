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

  r = pd_kmods_init();
  if (r < 0) {
    printk("Failed to load kernel modules: %d", -r);
    return 0;
  }

  // TODO: read boot parameters
  // TODO: load disk

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
