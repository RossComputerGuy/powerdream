#include <arch/timer.h>
#include <kernel/error.h>
#include <kernel/process.h>
#include <kos/genwait.h>
#include <kos/thread.h>
#include <kos/sem.h>
#include <malloc.h>
#include <string.h>

PD_SLIST_HEAD(proclist, pd_process_t);
TAILQ_HEAD(procqueue, pd_process);

static pid_t next_pid = 0;
static pd_process_t* curr_proc = NULL;
static pd_process_t* idle_proc = NULL;
static struct proclist processes;
static struct procqueue run_queue;
static semaphore_t reap_sem;

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

static void proc_start(pid_t pid, int (*entry)(int argc, char** argv), int argc, char** argv) {
  pd_process_t* proc = pd_process_frompid(pid);
  mmu_switch_context(proc->mem);
  int ret = entry(argc, argv);
  pd_process_exit(&proc, ret);
}

pid_t pd_process_create(pd_process_t** proc, int (*entry)(int argc, char** argv), int argc, char** argv) {
  if (((*proc) = malloc(sizeof(pd_process_t))) == NULL) return -ENOMEM;
  memset((*proc), 0, sizeof(pd_process_t));

  int oldirq = irq_disable();

  /* Setup basic stuff */
  (*proc)->parent_pid = 0;
  (*proc)->gid = 0;
  (*proc)->uid = 0;
  memset((*proc)->signal_handlers, 0, sizeof((*proc)->signal_handlers));
  memset(&(*proc)->saved_context, 0, sizeof(irq_context_t));
  memset(&(*proc)->files, 0, sizeof((*proc)->files));

  (*proc)->stack_size = 32768;
  if (((*proc)->stack = malloc((*proc)->stack_size)) == NULL) {
    irq_restore(oldirq);
    free((*proc));
    return -ENOMEM;
  }
  
  (*proc)->mem = mmu_context_create(1 + (*proc)->id);
  (*proc)->id = next_pid++;
  (*proc)->state = PD_PROC_READY;

  /* TODO: setup context */
  uint32_t params[4];
  params[0] = (*proc)->id;
  params[1] = (uint32_t)entry;
  params[2] = argc;
  params[3] = (uint32_t)argv;
  irq_create_context(&(*proc)->context, (uint32_t)((*proc)->stack + (*proc)->stack_size), (uint32_t)proc_start, params, 0); 

  /* Check if the current process exists */
  if (curr_proc != NULL) {
    (*proc)->parent_pid = curr_proc->id;
    (*proc)->gid = curr_proc->gid;
    (*proc)->uid = curr_proc->uid;
    strcpy((char*)(*proc)->name, curr_proc->name);
    strcpy((char*)(*proc)->pwd, curr_proc->pwd);
  } else {
    strcpy((char*)(*proc)->name, "[unnamed proc]");
    strcpy((char*)(*proc)->pwd, "/");
  }

  _REENT_INIT_PTR((&(*proc)->reent));

  PD_SLIST_INSERT_HEAD(&processes, (*proc), p_list);

  pd_process_addrunnable((*proc), 0);

  irq_restore(oldirq);
  return (*proc)->id;
}

void pd_process_exit(pd_process_t** proc, int status) {
  irq_disable();

  pd_process_t* child = NULL;
  PD_SLIST_FOREACH(child, &processes, p_list) {
    if (child->parent_pid == (*proc)->id) pd_process_exit(&child, status);
  }

  (*proc)->exitstat = status;
  _reclaim_reent(&(*proc)->reent);
  (*proc)->state = PD_PROC_ZOMBIE;
  sem_signal(&reap_sem);

  pd_process_t* parent_proc = pd_process_frompid((*proc)->parent_pid);
  if (parent_proc != NULL && (*proc)->parent_pid != 0) pd_process_kill(&parent_proc, SIGCHLD);

  thd_block_now(&(*proc)->context);
}

void pd_process_destroy(pd_process_t** proc) {
  int oldirq = irq_disable();

  pd_proces_removerunnable((*proc));
  PD_SLIST_REMOVE(&processes, (*proc), pd_process_t, p_list);
  mmu_context_destroy((*proc)->mem);
  free((*proc)->stack);
  free((*proc));
  (*proc) = NULL;

  irq_restore(oldirq);
}

pd_process_t* pd_process_frompid(pid_t pid) {
  pd_process_t* proc;
  PD_SLIST_FOREACH(proc, &processes, p_list) {
    if (proc->id == pid) return proc;
  }
  return NULL;
}

pd_process_t* pd_process_getcurr() {
  return curr_proc;
}

int pd_process_sigret(pd_process_t** proc) {
  memcpy(&(*proc)->context, &(*proc)->saved_context, sizeof(irq_context_t));
  return 0;
}

int pd_process_kill(pd_process_t** proc, int sig) {
  if (sig < 0 || sig >= SIGNAL_MAX) return -EINVAL;

  if (sig == SIGKILL) {
    if ((*proc)->signal_handlers[sig] != 0) {
      irq_context_t* context = irq_get_context();
      if (context == NULL) memcpy(&(*proc)->saved_context, context, sizeof(irq_context_t));
      else memcpy(&(*proc)->saved_context, &(*proc)->context, sizeof(irq_context_t));
      (*proc)->context.pc = (uint32_t)(*proc)->signal_handlers[sig];
    }
    pd_process_exit(proc, -SIGKILL);
  } else if ((*proc)->signal_handlers[sig] == 0 && (sig == SIGILL || sig == SIGFPE)) {
    pd_process_exit(proc, -sig);
  } else if ((*proc)->signal_handlers[sig] != 0) {
    irq_context_t* context = irq_get_context();
    if (context == NULL) {
      context->pc += 2;
      memcpy(&(*proc)->saved_context, context, sizeof(irq_context_t));
    } else {
      (*proc)->context.pc += 2;
      memcpy(&(*proc)->saved_context, &(*proc)->context, sizeof(irq_context_t));
    }
    (*proc)->context.pc = (uint32_t)(*proc)->signal_handlers[sig];
  }
  return 0;
}

pid_t getpid() {
  return curr_proc == NULL ? 0 : curr_proc->id;
}

int _getcwd(char* buf, size_t size) {
  if (curr_proc == NULL) return -ESRCH;
  if (buf == NULL) return -EINVAL;
  if (size < 0 && size > sizeof(curr_proc->pwd)) return -ERANGE;
  strncpy(buf, curr_proc->pwd, size);
  buf[size] = 0;
  return size;
}

int sigret() {
  if (curr_proc == NULL) return -ESRCH;
  return pd_process_sigret(&curr_proc);
}

int _signal(int sig, void (*func)(int)) {
  if (curr_proc == NULL) return -ESRCH;
  if (sig < 0 || sig >= SIGNAL_MAX) return -EINVAL;
  curr_proc->signal_handlers[sig] = func;
  return 0;
}

int kill(pid_t pid, int sig) {
  pd_process_t* proc = pd_process_frompid(pid);
  if (proc == NULL) return -ESRCH;

  if (curr_proc != NULL) {
    if (proc->gid < curr_proc->gid || proc->uid < curr_proc->uid) return -EPERM;
  }

  return pd_process_kill(&proc, sig);
}

void pd_process_addrunnable(pd_process_t* proc, int fol) {
  pd_process_t* p = NULL;

  if (proc->flags & PD_PROC_QUEUED) return;

  int done = 0;

  if (fol) {
    TAILQ_FOREACH(p, &run_queue, procq) {
      if (p->priority >= proc->priority) {
        TAILQ_INSERT_BEFORE(p, proc, procq);
        done = 1;
        break;
      }
    }
  } else {
    TAILQ_FOREACH(p, &run_queue, procq) {
      if (p->priority < proc->priority) {
        TAILQ_INSERT_BEFORE(p, proc, procq);
        done = 1;
      }
    }
  }

  if (!done) TAILQ_INSERT_TAIL(&run_queue, proc, procq);

  proc->flags |= PD_PROC_QUEUED;
}

void pd_proces_removerunnable(pd_process_t* proc) {
  if (!(proc->flags & PD_PROC_QUEUED)) return;

  proc->flags &= ~PD_PROC_QUEUED;
  TAILQ_REMOVE(&run_queue, proc, procq);
}

void pd_process_schedule(int fol, uint64_t now) {
  int dontenq = curr_proc == NULL;
  if (now == 0) now = timer_ms_gettime64();

  if (fol && !dontenq && curr_proc->state == PD_PROC_RUNNING) {
    curr_proc->state = PD_PROC_READY;
    pd_process_addrunnable(curr_proc, fol);
  }

  genwait_check_timeouts(now);

  pd_process_t* proc = NULL;
  TAILQ_FOREACH(proc, &run_queue, procq) {
    if (proc->state != PD_PROC_READY) break;
  }

  if (!fol && !dontenq && curr_proc->state == PD_PROC_RUNNING) {
    curr_proc->state = PD_PROC_READY;
    pd_process_addrunnable(curr_proc, fol);
    if (proc == NULL || proc == idle_proc) proc = curr_proc;
  }

  if (proc == NULL) return;

  pd_proces_removerunnable(proc);

  curr_proc = proc;
  _impure_ptr = &proc->reent;
  curr_proc->state = PD_PROC_RUNNING;
  irq_set_context(&curr_proc->context);
}

void pd_process_init() {
  irq_set_handler(EXC_ILLEGAL_INSTR, proc_handle_irq);
  irq_set_handler(EXC_FPU, proc_handle_irq);
  PD_SLIST_INIT(&processes);
  TAILQ_INIT(&run_queue);
}
