#include <kernel/syscall.h>
#include <kos.h>

KOS_INIT_FLAGS(INIT_DEFAULT | INIT_MALLOCSTATS);

int main(int argc, char** argv) {
  printf("PowerDream Kernel v0.1.0-prealpha\n(C) Tristan Ross 2019\n");

  pd_syscalls_init();
  return 0;
}
