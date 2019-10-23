#include <kernel/blkdev.h>
#include <kernel/fs.h>
#include <kernel/irq.h>
#include <kernel/mem.h>
#include <kernel/mmu.h>
#include <kernel/process.h>
#include <kernel/sercon.h>
#include <kernel/syscall.h>

void bootstrap_main() {
  pd_basic_mem_init();
  pd_sercon_init();
  pd_irq_init();
  pd_mmu_init();
  pd_blkdev_init();
  pd_fs_init();
  pd_process_init();
  pd_syscalls_init();

  // TODO: read boot parameters
  // TODO: start loading kernel modules
  // TODO: load ramdisk
  // TODO: start init program as PID 1
}
