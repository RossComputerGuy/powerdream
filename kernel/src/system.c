#include <arch/arch.h>
#include <kernel/error.h>
#include <kernel/limits.h>
#include <kernel/system.h>
#include <kernel-config.h>
#include <malloc.h>
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

void pd_setcmdline(const char* str) {
  cmdline = (char*)str;
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
    case PD_SC_CHILD_MAX: return CHILD_MAX;
    case PD_SC_OPEN_MAX: return OPEN_MAX;
    case PD_SC_PAGESIZE: return PAGESIZE;
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

char* basename(char* path) {
	if (path == NULL || *path == '\0')	return ".";
	char* p = path + strlen(path) - 1;
	while (*p == '/') {
		if (p == path) return path;
		*p-- = '\0';
	}
	while (p >= path && *p != '/') p--;
	return p + 1;
}
