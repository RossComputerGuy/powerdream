#pragma once

#include <kos/thread.h>
#include <kernel/types.h>

/**
 * PowerDream Process
 */
typedef struct {
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
