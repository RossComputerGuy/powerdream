#include <arch/arch.h>
#include <kernel/error.h>
#include <kernel/limits.h>
#include <kernel/system.h>
#include <kernel-config.h>
#include <string.h>

long sysconf(int name) {
  switch (name) {
    case _SC_CHILD_MAX: return CHILD_MAX;
    case _SC_OPEN_MAX: return OPEN_MAX;
    case _SC_PAGESIZE: return PAGESIZE;
  }
  return -EINVAL;
}

int uname(struct utsname* buff) {
  strcpy(buff->sysname, "PowerDream");
  strcpy(buff->nodename, "powerdream"); // TODO: store the hostname in a variable
  strcpy(buff->release, PD_KERNEL_VERSION);
  strcpy(buff->machine, "sh4");
  return 0;
}
