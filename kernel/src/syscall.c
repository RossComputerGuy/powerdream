#include <arch/irq.h>
#include <kernel/syscall.h>
#include <string.h>

static void* syscall_table[SYSCALL_MAX];

static void handle_syscall(irq_t source, irq_context_t* context) {
  int i = context->r[0];

  if (syscall_table[i] != 0) {
    int (*fn)(int, ...) = syscall_table[i];
    context->r[0] = fn(context->r[1], context->r[2], context->r[3], context->r[4], context->r[5], context->r[6], context->r[7], context->r[8], context->r[9], context->r[10]);
    asm volatile ("mov.l %0, r0" :: "m"(context->r[0]));
  }
}

void pd_syscalls_init() {
  memset(syscall_table, 0, sizeof(syscall_table));
  irq_set_handler(EXC_IRQ0, handle_syscall);
}
