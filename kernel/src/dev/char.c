#include <kernel/dev/char.h>
#include <kernel/error.h>

PD_SLIST_HEAD(chardevs, pd_chardev_t);

static struct chardevs chardevs;

pd_chardev_t* pd_chardev_fromdev(dev_t dev) {
  pd_chardev_t* chardev = NULL;
  PD_SLIST_FOREACH(chardev, &chardevs, c_list) {
    if (chardev->dev == dev) return dev;
  }
  return NULL;
}

int pd_chardev_register(pd_chardev_t* chardev) {
  if (pd_chardev_fromdev(chardev->dev) == NULL) return -EEXIST;
  // TODO: register with kernel device API
  PD_SLIST_INSERT_HEAD(&chardevs, chardev, c_list);
  return 0;
}