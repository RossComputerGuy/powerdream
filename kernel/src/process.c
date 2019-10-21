#include <kernel/error.h>
#include <kernel/process.h>
#include <stdlib.h>

SLIST_HEAD(proclist, pd_process_t);

static pid_t next_pid = 0;
static struct proclist processes;

pid_t pd_process_create(pd_process_t** proc, void* (*entry)(void* param), void* param) {
  if (((*proc) = malloc(sizeof(pd_process_t))) == NULL) return -ENOMEM;

  /* Setup basic stuff */
  (*proc)->id = next_pid++;
  (*proc)->parent_pid = 0;
  (*proc)->gid = 0;
  (*proc)->uid = 0;

  /* Check if the current process exists */
  pd_process_t* curr_proc = pd_process_getcurr();
  if (curr_proc != NULL) {
    (*proc)->parent_pid = curr_proc->id;
    (*proc)->gid = curr_proc->gid;
    (*proc)->uid = curr_proc->uid;
  }

  (*proc)->thread = thd_create(0, entry, param);
  SLIST_INSERT_HEAD(&processes, (*proc), p_list);
  return (*proc)->id;
}

void pd_process_destroy(pd_process_t** proc) {
  thd_destroy((*proc)->thread);
  free((*proc));
  (*proc) = NULL;
}

pd_process_t* pd_process_getcurr() {
  pd_process_t* proc;
  SLIST_FOREACH(proc, &processes, p_list) {
    if (proc->thread->tid == thd_get_current()->tid) return proc;
  }
  return NULL;
}

pid_t getpid() {
  pd_process_t* curr_proc = pd_process_getcurr();
  return curr_proc == NULL ? 0 : curr_proc->id;
}
