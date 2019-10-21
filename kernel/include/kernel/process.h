#pragma once

#include <kos/thread.h>
#include <sys/types.h>
#include <sys/queue.h>

struct pd_process;

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
 * Get the current process
 *
 * @return The current process or NULL
 */
pd_process_t* pd_process_getcurr();

/**
 * Get the PID of the current process
 *
 * @return The current process ID or 0
 */
pid_t getpid();
