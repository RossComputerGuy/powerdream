#include <kernel/dev/char.h>
#include <kernel/dev/tty.h>
#include <kernel/error.h>

PD_SLIST_HEAD(ttys, pd_tty_t);

static struct ttys ttys;

pd_tty_t* pd_tty_fromdev(dev_t dev) {
  pd_tty_t* tty = NULL;
  PD_SLIST_FOREACH(tty, &ttys, t_list) {
    if (tty->dev == dev) return tty;
  }
  return NULL;
}

int pd_tty_register(pd_tty_t* tty) {
  if (pd_tty_fromdev(tty->dev) != NULL) return -EEXIST;
  // TODO: register character device
  PD_SLIST_INSERT_HEAD(&ttys, tty, t_list);
  return 0;
}