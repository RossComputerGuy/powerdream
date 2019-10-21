#include <arch/irq.h>
#include <kernel/fs.h>
#include <kernel/process.h>
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

  /* Setup the system call table */
  syscall_table[SYS_getpid] = getpid;
  syscall_table[SYS_getcwd] = _getcwd;
  syscall_table[SYS_sigret] = sigret;
  syscall_table[SYS_signal] = _signal;
  syscall_table[SYS_kill] = kill;

  syscall_table[SYS_mount] = mount;

  /* Set the system call handler */
  irq_set_handler(EXC_IRQ0, handle_syscall);
}
