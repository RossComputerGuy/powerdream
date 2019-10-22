#include <kernel/system.h>
#include <kernel-config.h>

int uname(struct utsname* buff) {
  buff->sysname = "PowerDream";
  buff->nodename = "powerdream"; // TODO: store the hostname in a variable
  buff->release = PD_KERNEL_VERSION;
  buff->machine = "sh4";
  return 0;
}