#include <arch/timer.h>
#include <kernel/klog.h>
#include <malloc.h>
#include <stdarg.h>
#include <stdio.h>

int pd_klog_printf(const char* format, ...) {
  va_list ap;
  va_start(ap, format);
  char* prefix = NULL;
  size_t r = asprintf(&prefix, "[%f] %s\n", timer_us_gettime64() / 10000.0, format);
  if (r < 0) {
    va_end(ap);
    return r;
  }
  r = vprintf(prefix, ap);
  free(prefix);
  va_end(ap);
  return r;
}
