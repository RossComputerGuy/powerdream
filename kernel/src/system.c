#include <arch/arch.h>
#include <kernel/error.h>
#include <kernel/limits.h>
#include <kernel/system.h>
#include <kernel-config.h>
#include <string.h>

static char* cmdline
#ifdef PD_COMPILED_CMDLINE
= PD_CMDLINE;
#else
= NULL;
#endif

char* pd_getcmdline() {
  return cmdline;
}

int pd_getcmdline_hasarg(const char* arg) {
  if (cmdline == NULL) return 0;
  char* p = strtok(cmdline, " ");
  while (p != NULL) {
    if (!strncmp(p, arg, strlen(arg))) return 1;
    p = strtok(NULL, " ");
  }
  return 0;
}

char* pd_getcmdline_arg(const char* arg) {
  if (cmdline == NULL) return NULL;
  char* p = strtok(cmdline, " ");
  while (p != NULL) {
    if (!strncmp(p, arg, strlen(arg))) {
      size_t size = strlen(arg);
      if (p[size] == '=') size++;
      char* str = malloc(size);
      if (str == NULL) return NULL;
      strcpy(str, p + size);
      return str;
    }
    p = strtok(NULL, " ");
  }
  return NULL;
}

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
