#pragma once

#include <kernel/types.h>

#define PD_SEM_PROC (1 << 0)
#define PD_SEM_KERN (0 << 0)

/**
  * This flag will make the semaphore wait for it to be unlocked before locking
  */
#define PD_SEM_WAITLOCK (1 << 1)

/**
  * This flag will make the process go down if the semaphore is trying to be locked while its locked
  */
#define PD_SEM_KILLLOCK (2 << 1)

/**
  * This flag will make the kernel panic if the semaphore is trying to be locked while its locked
  */
#define PD_SEM_PANICLOCK (0 << 1)

typedef struct {
  uint8_t flags;
  pid_t pid;
  int locked;
} pd_sem_t;

/**
  * Initializes a semaphore
  *
  * @param[out] sem The semaphore to initialize
  * @param[in] flags The flags to use for initializing
  * @param[in] pid The process ID to use for this semaphore, will only be usable if PD_SEM_PROC is set in flags
  * @return Zero on success or a negative errno code
  */
int pd_sem_init(pd_sem_t* sem, uint8_t flags, pid_t pid);

/**
  * Locks a semaphore
  *
  * @param[out] sem The semaphore to lock
  * @param[in] pid The current process ID, set to 0 if this is being called for a kernel function
  * @return Zero on success or a negative errno code
  */
int pd_sem_lock(pd_sem_t* sem, pid_t pid);

/**
  * Unlocks a semaphore
  *
  * @param[out] sem The semaphore to unlock
  * @param[in] pid The current process ID, set to 0 if this is being called for a kernel function
  * @return Zero on success or a negative errno code
  */
int pd_sem_unlock(pd_sem_t* sem, pid_t pid);