#include <kernel/dev/block.h>
#include <kernel/device.h>
#include <kernel/error.h>

PD_SLIST_HEAD(blkdevs, pd_blkdev_t);

static struct blkdevs blkdevs;

pd_blkdev_t* pd_blkdev_fromname(const char* name) {
  pd_blkdev_t* blkdev;
  PD_SLIST_FOREACH(blkdev, &blkdevs, b_list) {
    if (!strcmp(blkdev->name, name)) return blkdev;
  }
  return NULL;
}

int pd_blkdev_register(pd_blkdev_t* blkdev) {
  if (pd_blkdev_fromname(blkdev->name) != NULL) return -EEXIST;
  // TODO: register with kernel device API
  PD_SLIST_INSERT_HEAD(&blkdevs, blkdev, b_list);
  return 0;
}

void pd_blkdev_init() {
  PD_SLIST_INIT(&blkdevs);
}
