#include <kernel/dev/char.h>
#include <kernel/dev/tty.h>
#include <kernel/device.h>
#include <kernel/error.h>
#include <malloc.h>
#include <string.h>

PD_SLIST_HEAD(ttys, pd_tty_t);

static struct ttys ttys;

static size_t tty_read(pd_chardev_t* chardev, pd_file_t* file, off_t offset, char* buffer, size_t length) {
  return 0;
}

static size_t tty_write(pd_chardev_t* chardev, pd_file_t* file, off_t offset, const char* buffer, size_t length) {
  return 0;
}

pd_tty_t* pd_tty_fromname(const char* name) {
  pd_tty_t* tty = NULL;
  PD_SLIST_FOREACH(tty, &ttys, t_list) {
    if (!strcmp(tty->name, name)) return tty;
  }
  return NULL;
}

int pd_tty_register(pd_tty_t* tty) {
  if (pd_tty_fromname(tty->name) != NULL) return -EEXIST;

  pd_chardev_t* chardev = malloc(sizeof(pd_chardev_t));
  strcpy((char*)chardev->name, tty->name);
  chardev->dev = MKDEV(5, 0);
  chardev->size = 0;
  chardev->read = tty_read;
  chardev->write = tty_write;

  int r = pd_chardev_register(chardev);
  if (r < 0) return r;

  PD_SLIST_INSERT_HEAD(&ttys, tty, t_list);
  return 0;
}

void pd_tty_init() {
  PD_SLIST_INIT(&ttys);
}
