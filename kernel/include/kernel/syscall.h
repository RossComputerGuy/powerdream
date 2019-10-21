#pragma once

/* System call: getpid */
#define SYS_getpid 0

#define SYSCALL_MAX 1

/**
 * Setup the system calls
 */
void pd_syscalls_init();
