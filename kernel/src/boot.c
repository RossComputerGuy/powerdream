#include <kernel/blkdev.h>
#include <kernel/fs.h>
#include <kernel/process.h>
#include <kernel/syscall.h>

void bootstrap_main() {
  pd_blkdev_init();
  pd_fs_init();
  pd_process_init();
  pd_syscalls_init();
}
