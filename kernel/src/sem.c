#include <kernel/error.h>
#include <kernel/sem.h>

int pd_sem_init(pd_sem_t* sem, uint8_t flags, pid_t pid) {
  sem->flags = flags;

  if (sem->flags & PD_SEM_PROC) sem->pid = pid;
  else sem->pid = 0;

  sem->locked = 0;
  return 0;
}

int pd_sem_lock(pd_sem_t* sem, pid_t pid) {
  if (sem->flags & PD_SEM_PROC && sem->pid != pid) return -EPERM;

  if (sem->locked) {
    if (sem->flags & PD_SEM_WAITLOCK) {
      while (sem->locked);
    } else if (sem->flags & PD_SEM_KILLLOCK) {
      // TODO: kill process
    } else if (sem->flags & PD_SEM_PANICLOCK) {
      // TODO: panic
    }
  } else return -EINVAL;

  sem->locked = 1;
  return 0;
}

int pd_sem_unlock(pd_sem_t* sem, pid_t pid) {
  if (sem->flags & PD_SEM_PROC && sem->pid != pid) return -EPERM;

  if (sem->locked == 0) {
    if (sem->flags & PD_SEM_WAITLOCK) {
      while (sem->locked);
    } else if (sem->flags & PD_SEM_KILLLOCK) {
      // TODO: kill process
    } else if (sem->flags & PD_SEM_PANICLOCK) {
      // TODO: panic
    }
  } else return -EINVAL;

  sem->locked = 0;
  return 0;
}