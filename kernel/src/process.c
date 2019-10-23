#include <kernel/error.h>
#include <kernel/mem.h>
#include <kernel/mmu.h>
#include <kernel/process.h>
#include <string.h>

PD_SLIST_HEAD(proclist, pd_process_t);

static pid_t next_pid = 0;
static pid_t curr_pid = 0;
static struct proclist processes;

static void proc_handle_irq(uint32_t source, pd_irq_context_t* context) {
  pd_process_t* curr_proc = pd_process_getcurr();
  switch (source) {
    case PD_EXC_ILLEGAL_INSTR:
      pd_process_kill(&curr_proc, SIGILL);
      break;
    case PD_EXC_FPU:
      pd_process_kill(&curr_proc, SIGFPE);
      break;
  }
}

pid_t pd_process_create(pd_process_t** proc, void* (*entry)(void* param), void* param) {
  if (((*proc) = kmalloc(sizeof(pd_process_t))) == NULL) return -ENOMEM;

  /* Setup basic stuff */
  (*proc)->id = next_pid++;
  (*proc)->parent_pid = 0;
  (*proc)->gid = 0;
  (*proc)->uid = 0;
  memset((*proc)->signal_handlers, 0, sizeof((*proc)->signal_handlers));
  memset(&(*proc)->saved_context, 0, sizeof(pd_irq_context_t));
  memset(&(*proc)->files, 0, sizeof((*proc)->files));

  pd_mmu_context_create(&(*proc)->mem, 1 + (*proc)->id);

  /* TODO: setup context */

  /* Check if the current process exists */
  pd_process_t* curr_proc = pd_process_getcurr();
  if (curr_proc != NULL) {
    (*proc)->parent_pid = curr_proc->id;
    (*proc)->gid = curr_proc->gid;
    (*proc)->uid = curr_proc->uid;
    strcpy((char*)(*proc)->name, curr_proc->name);
    strcpy((char*)(*proc)->pwd, curr_proc->pwd);
  }

  PD_SLIST_INSERT_HEAD(&processes, (*proc), p_list);
  return (*proc)->id;
}

void pd_process_destroy(pd_process_t** proc) {
  kfree((*proc));
  (*proc) = NULL;
}

pd_process_t* pd_process_frompid(pid_t pid) {
  pd_process_t* proc;
  PD_SLIST_FOREACH(proc, &processes, p_list) {
    if (proc->id == pid) return proc;
  }
  return NULL;
}

pd_process_t* pd_process_getcurr() {
  pd_process_t* proc;
  PD_SLIST_FOREACH(proc, &processes, p_list) {
    if (proc->id == curr_pid) return proc;
  }
  return NULL;
}

int pd_process_sigret(pd_process_t** proc) {
  memcpy(&(*proc)->context, &(*proc)->saved_context, sizeof(pd_irq_context_t));
  return 0;
}

int pd_process_kill(pd_process_t** proc, int sig) {
  if (sig < 0 || sig >= SIGNAL_MAX) return -EINVAL;

  if (sig == SIGKILL) {
    if ((*proc)->signal_handlers[sig] != 0) {
      pd_irq_context_t* context = pd_irq_get_context();
      if (context == NULL) memcpy(&(*proc)->saved_context, context, sizeof(pd_irq_context_t));
      else memcpy(&(*proc)->saved_context, &(*proc)->context, sizeof(pd_irq_context_t));
      (*proc)->context.pc = (uint32_t)(*proc)->signal_handlers[sig];
    }
    pd_process_t* parent_proc = pd_process_frompid((*proc)->parent_pid);
    if (parent_proc != NULL && (*proc)->parent_pid != 0) pd_process_kill(&parent_proc, SIGCHLD);
    (*proc)->state = PD_STATE_FINISHED;
    PD_SLIST_REMOVE(&processes, (*proc), pd_process_t, p_list);
    pd_process_destroy(&(*proc));
  } else if ((*proc)->signal_handlers[sig] == 0 && (sig == SIGILL || sig == SIGFPE)) {
    pd_process_t* parent_proc = pd_process_frompid((*proc)->parent_pid);
    if (parent_proc != NULL && (*proc)->parent_pid != 0) pd_process_kill(&parent_proc, SIGCHLD);
    (*proc)->state = PD_STATE_FINISHED;
    PD_SLIST_REMOVE(&processes, (*proc), pd_process_t, p_list);
    pd_process_destroy(&(*proc));
  } else if ((*proc)->signal_handlers[sig] != 0) {
    pd_irq_context_t* context = pd_irq_get_context();
    if (context == NULL) memcpy(&(*proc)->saved_context, context, sizeof(pd_irq_context_t));
    else memcpy(&(*proc)->saved_context, &(*proc)->context, sizeof(pd_irq_context_t));
    (*proc)->context.pc = (uint32_t)(*proc)->signal_handlers[sig];
  }

  /* TODO: update scheduler */
  return 0;
}

pid_t getpid() {
  pd_process_t* curr_proc = pd_process_getcurr();
  return curr_proc == NULL ? 0 : curr_proc->id;
}

int _getcwd(char* buf, size_t size) {
  pd_process_t* curr_proc = pd_process_getcurr();
  if (curr_proc == NULL) return -ESRCH;
  if (buf == NULL) return -EINVAL;
  if (size < 0 && size > sizeof(curr_proc->pwd)) return -ERANGE;
  strncpy(buf, curr_proc->pwd, size);
  buf[size] = 0;
  return size;
}

int sigret() {
  pd_process_t* curr_proc = pd_process_getcurr();
  if (curr_proc == NULL) return -ESRCH;
  return pd_process_sigret(&curr_proc);
}

int _signal(int sig, void (*func)(int)) {
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
  pd_irq_set_handler(PD_EXC_ILLEGAL_INSTR, proc_handle_irq);
  pd_irq_set_handler(PD_EXC_FPU, proc_handle_irq);
  PD_SLIST_INIT(&processes);
}
