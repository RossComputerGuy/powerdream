#pragma once

#define SYS_getpid 0
#define SYS_getcwd 1
#define SYS_sigret 2
#define SYS_signal 3
#define SYS_kill 4

#define SYS_mount 5
#define SYS_umount 6

#define SYSCALL_MAX 7

/**
 * Setup the system calls
 */
void pd_syscalls_init();
