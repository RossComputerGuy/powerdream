#include <kernel/system.h>
#include <kernel-config.h>
#include <string.h>

int uname(struct utsname* buff) {
  strcpy(buff->sysname, "PowerDream");
  strcpy(buff->nodename, "powerdream"); // TODO: store the hostname in a variable
  strcpy(buff->release, PD_KERNEL_VERSION);
  strcpy(buff->machine, "sh4");
  return 0;
}
