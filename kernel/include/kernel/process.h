#pragma once

#include <arch/irq.h>
#include <kernel/file.h>
#include <kernel/signal.h>
#include <kos/thread.h>
#include <sys/types.h>
#include <sys/queue.h>

struct pd_process;

#define OPEN_MAX 1024

/**
 * PowerDream Process
 */
typedef struct pd_process {
  /* Process list handle */
  SLIST_ENTRY(pd_process) p_list;

  /* Process ID */
  pid_t id;
  
  /* Parent process's ID */
  pid_t parent_pid;

  /* Process user ID */
  uid_t uid;

  /* Process group ID */
  gid_t gid;

  /* Process entry pointer */
  void* entry;

  /* The process's thread */
  kthread_t* thread;

  /* Function pointers to store the signal handlers */
  void* signal_handlers[SIGNAL_MAX];

  /* The context that was saved before a signal */
  irq_context_t saved_context;

  /* Files opened by the process */
  pd_file_t files[OPEN_MAX];
} pd_process_t;

/**
 * Creates a new process
 *
 * @param[out] proc The pointer to store the newly created process
 * @param[in] entry The entry function pointer.
 * @param[in] param The argument to use for the entry function.
 * @return Negative errno number on error, pid on success.
 */
pid_t pd_process_create(pd_process_t** proc, void* (entry)(void* param), void* param);

/**
 * Destroys a process and sets the pointer to NULL.
 *
 * @param[out] proc The pointer to store and use for the process
 */
void pd_process_destroy(pd_process_t** proc);

/**
 * Gets a process by its PID
 *
 * @param[in] pid The process ID to use.
 * @return The process that is mapped to the specified PID or NULL
 */
pd_process_t* pd_process_frompid(pid_t pid);

/**
 * Get the current process
 *
 * @return The current process or NULL
 */
pd_process_t* pd_process_getcurr();

/**
 * Exists from the signal handler in a process
 *
 * @param[in] proc The process to use
 * @return Negative errno number on error, 0 on success.
 */
int pd_process_sigret(pd_process_t** proc);

/**
 * Signals the process
 *
 * @param[out] proc The process to signal
 * @param[in] sig The signal to send
 * @return Negative errno number on error or 0 for success.
 */
int pd_process_kill(pd_process_t** proc, int sig);

/**
 * Get the PID of the current process
 *
 * @return The current process ID or 0
 */
pid_t getpid();

/**
 * Get the current working directory of the current process
 *
 * @param[out] buf The buffer to write the path to.
 * @param[in] size Number of bytes to write into buf from the path.
 * @return Negative errno number on error or the number of bytes that were written.
 */
int _getcwd(char* buf, size_t size);

/**
 * Exists from the signal handler in the current process.
 *
 * @return Negative errno number on error, 0 on success.
 */
int sigret();

/**
 * Sets the current process's signal handler for the specified signal.
 *
 * @param[in] sig The signal to set
 * @param[in] func The function pointer which will handle the signal.
 * @return Negative errno number on error or 0 on succes.
 */
int _signal(int sig, void (*func)(int));

/**
 * Sends a signal to a process
 *
 * @param[in] pid The process to signal
 * @param[in] sig The signal to send
 * @return 0 on success, negative errno on error.
 */
int kill(pid_t pid, int sig);

/**
 * Initialize multiprocessing
 */
void pd_process_init();
