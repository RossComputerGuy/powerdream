#include <arch/irq.h>
#include <kernel/error.h>
#include <kernel/process.h>
#include <stdlib.h>
#include <string.h>

SLIST_HEAD(proclist, pd_process_t);

static pid_t next_pid = 0;
static struct proclist processes;

#define PROC_SLIST_REMOVE(elem) do { \
  if (((pd_process_t*)processes.slh_first)->id == (elem)->id) { \
    processes.slh_first = (pd_process_t*)((pd_process_t*)processes.slh_first)->p_list.sle_next; \
  } else { \
    pd_process_t* curelm = (pd_process_t*)processes.slh_first; \
    while (curelm->p_list.sle_next->id != (elem)->id) curelm = curelm->p_list.sle_next; \
    curelm->p_list.sle_next = curelm->p_list.sle_next->p_list.sle_next; \
  } \
} while (0)

static void proc_handle_irq(irq_t source, irq_context_t* context) {
  pd_process_t* curr_proc = pd_process_getcurr();
  switch (source) {
    case EXC_ILLEGAL_INSTR:
      pd_process_kill(&curr_proc, SIGILL);
      break;
    case EXC_FPU:
      pd_process_kill(&curr_proc, SIGFPE);
      break;
  }
}

pid_t pd_process_create(pd_process_t** proc, void* (*entry)(void* param), void* param) {
  if (((*proc) = malloc(sizeof(pd_process_t))) == NULL) return -ENOMEM;

  /* Setup basic stuff */
  (*proc)->id = next_pid++;
  (*proc)->parent_pid = 0;
  (*proc)->gid = 0;
  (*proc)->uid = 0;
  memset((*proc)->signal_handlers, 0, sizeof((*proc)->signal_handlers));
  memset(&(*proc)->saved_context, 0, sizeof(irq_context_t));

  /* Check if the current process exists */
  pd_process_t* curr_proc = pd_process_getcurr();
  if (curr_proc != NULL) {
    (*proc)->parent_pid = curr_proc->id;
    (*proc)->gid = curr_proc->gid;
    (*proc)->uid = curr_proc->uid;
    strcpy((*proc)->thread->label, curr_proc->thread->label);
    strcpy((*proc)->thread->pwd, curr_proc->thread->pwd);
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

pd_process_t* pd_process_frompid(pid_t pid) {
  pd_process_t* proc;
  SLIST_FOREACH(proc, &processes, p_list) {
    if (proc->id == pid) return proc;
  }
  return NULL;
}

pd_process_t* pd_process_getcurr() {
  pd_process_t* proc;
  SLIST_FOREACH(proc, &processes, p_list) {
    if (proc->thread->tid == thd_get_current()->tid) return proc;
  }
  return NULL;
}

int pd_process_sigret(pd_process_t** proc) {
  memcpy(&(*proc)->thread->context, &(*proc)->saved_context, sizeof(irq_context_t));
  return 0;
}

int pd_process_kill(pd_process_t** proc, int sig) {
  if (sig < 0 || sig >= SIGNAL_MAX) return -EINVAL;

  if (sig == SIGKILL) {
    if ((*proc)->signal_handlers[sig] != 0) {
      irq_context_t* context = irq_get_context();
      if (context == NULL) memcpy(&(*proc)->saved_context, context, sizeof(irq_context_t));
      else memcpy(&(*proc)->saved_context, &(*proc)->thread->context, sizeof(irq_context_t));
      (*proc)->thread->context.pc = (uint32)(*proc)->signal_handlers[sig];
    }
    pd_process_t* parent_proc = pd_process_frompid((*proc)->parent_pid);
    if (parent_proc != NULL && (*proc)->parent_pid != 0) pd_process_kill(&parent_proc, SIGCHLD);
    (*proc)->thread->state = STATE_FINISHED;
    PROC_SLIST_REMOVE((*proc));
    pd_process_destroy(&(*proc));
  } else if ((*proc)->signal_handlers[sig] == 0 && (sig == SIGILL || sig == SIGFPE)) {
    pd_process_t* parent_proc = pd_process_frompid((*proc)->parent_pid);
    if (parent_proc != NULL && (*proc)->parent_pid != 0) pd_process_kill(&parent_proc, SIGCHLD);
    (*proc)->thread->state = STATE_FINISHED;
    PROC_SLIST_REMOVE((*proc));
    pd_process_destroy(&(*proc));
  } else if ((*proc)->signal_handlers[sig] != 0) {
    irq_context_t* context = irq_get_context();
    if (context == NULL) memcpy(&(*proc)->saved_context, context, sizeof(irq_context_t));
    else memcpy(&(*proc)->saved_context, &(*proc)->thread->context, sizeof(irq_context_t));
    (*proc)->thread->context.pc = (uint32)(*proc)->signal_handlers[sig];
  }

  thd_pass();
  return 0;
}

pid_t getpid() {
  pd_process_t* curr_proc = pd_process_getcurr();
  return curr_proc == NULL ? 0 : curr_proc->id;
}

int getcwd(char* buf, size_t size) {
  pd_process_t* curr_proc = pd_process_getcurr();
  if (curr_proc == NULL) return -ESRCH;
  if (buf == NULL) return -EINVAL;
  if (size < 0 && size > sizeof(curr_proc->thread->pwd)) return -ERANGE;
  strncpy(buf, curr_proc->thread->pwd, size);
  buf[size] = 0;
  return size;
}

int sigret() {
  pd_process_t* curr_proc = pd_process_getcurr();
  if (curr_proc == NULL) return -ESRCH;
  return pd_process_sigret(&curr_proc);
}

int signal(int sig, void (*func)(int)) {
  pd_process_t* curr_proc = pd_process_getcurr();
  if (curr_proc == NULL) return -ESRCH;
  if (sig < 0 || sig >= SIGNAL_MAX) return -EINVAL;
  curr_proc->signal_handlers[sig] = func;
  return 0;
}

int kill(pid_t pid, int sig) {
  pd_process_t* curr_proc = pd_process_getcurr();
  pd_process_t* proc = pd_process_frompid(pid);
  if (proc == NULL) return -ESRCH;

  if (curr_proc != NULL) {
    if (proc->gid < curr_proc->gid || proc->uid < curr_proc->uid) return -EPERM;
  }

  return pd_process_kill(&proc, sig);
}

void pd_process_init() {
  irq_set_handler(EXC_ILLEGAL_INSTR, proc_handle_irq);
  irq_set_handler(EXC_FPU, proc_handle_irq);
  SLIST_INIT(&processes);
}
