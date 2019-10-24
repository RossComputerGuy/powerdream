#pragma once

#include <arch/irq.h>
#include <arch/mmu.h>
#include <kernel/file.h>
#include <kernel/limits.h>
#include <kernel/list.h>
#include <kernel/signal.h>
#include <kernel/types.h>
#include <sys/reent.h>
#include <sys/queue.h>

struct pd_process;

#define PD_PROC_READY 0
#define PD_PROC_ZOMBIE 1
#define PD_PROC_RUNNING 2
#define PD_PROC_WAIT 3
#define PD_PROC_FINISHED 4

#define PD_PROC_QUEUED (1 << 0)

/**
 * PowerDream Process
 */
typedef struct pd_process {
  /* Process list handle */
  PD_SLIST_ENTRY(struct pd_process) p_list;

  /* Process queue handle */
  TAILQ_ENTRY(pd_process) procq;

  /* Process timer queue handle */
  TAILQ_ENTRY(pd_process) timerq;

  /* Process name */
  const char name[128];

  /* Process current working directory */
  const char pwd[PATH_MAX];

  /* Process flags */
  uint8_t flags;

  /* Process priority */
  uint32_t priority;

  /* The process exist status */
  int exitstat;

  /* Process ID */
  pid_t id;

  /* Parent process's ID */
  pid_t parent_pid;

  /* Process user ID */
  uid_t uid;

  /* Process group ID */
  gid_t gid;

  /* Process state */
  int state;

  /* Process stack pointer */
  void* stack;

  /* Process stack size */
  uint32_t stack_size;

  /* Newlib reentry */
  struct _reent reent;

  /* Process MMU context */
  mmucontext_t* mem;

  /* Process context */
  irq_context_t context;

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
pid_t pd_process_create(pd_process_t** proc, int (*entry)(int argc, char** argv), int argc, char** argv);

/**
 * Have the process exit
 *
 * @param[out] proc The process to exit
 * @param[in] status The exit status
 */
void pd_process_exit(pd_process_t** proc, int status);

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
 * Adds a process to the run queue
 *
 * @param[in] proc The process to add to the queue
 * @param[in] fol Set to 1 to add the process to the front of the line
 */
void pd_process_addrunnable(pd_process_t* proc, int fol);

/**
 * Removes a process from the run queue
 *
 * @param[in] proc The process to remove from the queue
 */
void pd_proces_removerunnable(pd_process_t* proc);

/**
 * Schedules for the next process to run
 *
 * @param[in] fol Set to 1 to add the process to the front of the line
 * @param[in] now The current timer value
 */
void pd_process_schedule(int fol, uint64_t now);

/**
 * Initialize multiprocessing
 */
void pd_process_init();
