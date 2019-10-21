#include <kernel/error.h>
#include <kernel/process.h>
#include <stdlib.h>

static pid_t next_pid = 0;
static pd_process_t** curr_proc = NULL;

pid_t pd_process_create(pd_process_t** proc, void* (*entry)(void* param), void* param) {
  if (((*proc) = malloc(sizeof(pd_process_t))) == NULL) return -ENOMEM;

  /* Setup basic stuff */
  (*proc)->id = next_pid++;
  (*proc)->parent_pid = 0;
  (*proc)->gid = 0;
  (*proc)->uid = 0;

  /* Check if the current process exists */
  if (curr_proc != NULL) {
    (*proc)->parent_pid = (*curr_proc)->id;
    (*proc)->gid = (*curr_proc)->gid;
    (*proc)->uid = (*curr_proc)->uid;
  }

  (*proc)->thread = thd_create(0, entry, param);
  return (*proc)->id;
}

void pd_process_destroy(pd_process_t** proc) {
  thd_destroy((*proc)->thread);
  free((*proc));
  (*proc) = NULL;
}
