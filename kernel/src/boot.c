#include <kernel/blkdev.h>
#include <kernel/fs.h>
#include <kernel/process.h>
#include <kernel/syscall.h>
#include <kos.h>

KOS_INIT_FLAGS(INIT_DEFAULT | INIT_MALLOCSTATS);

int main(int argc, char** argv) {
  printf("PowerDream Kernel v0.1.0-prealpha\n(C) Tristan Ross 2019\n");

  pd_blkdev_init();
  pd_fs_init();
  pd_process_init();
  pd_syscalls_init();
  return 0;
}
