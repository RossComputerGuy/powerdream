#include <kernel/dev/block.h>
#include <kernel/device.h>
#include <kernel/error.h>
#include <malloc.h>

PD_SLIST_HEAD(blkdevs, pd_blkdev_t);

static struct blkdevs blkdevs;
static int blkdev_count = 0;

static size_t blkdev_read(pd_inode_t* inode, pd_file_t* file, char* buff, size_t size) {
  pd_blkdev_t* blkdev = pd_blkdev_fromname(inode->name);
  if (blkdev == NULL) blkdev = inode->impl;
  if (blkdev == NULL) return -ENODEV;
  if (blkdev->blkread == NULL) return -EIO;

  int r = blkdev->blkread(blkdev, buff, blkdev->blksize * file->offset, size);
  if (r < 0) return r;

  file->offset++;
  return 0;
}

static size_t blkdev_write(pd_inode_t* inode, pd_file_t* file, const char* buff, size_t size) {
  pd_blkdev_t* blkdev = pd_blkdev_fromname(inode->name);
  if (blkdev == NULL) blkdev = inode->impl;
  if (blkdev == NULL) return -ENODEV;
  if (blkdev->blkwrite == NULL) return -EIO;

  int r = blkdev->blkwrite(blkdev, buff, blkdev->blksize * file->offset, size);
  if (r < 0) return r;

  file->offset++;
  return 0;
}

int pd_blkdev_getcount() {
  return blkdev_count;
}

pd_blkdev_t* pd_blkdev_fromindex(size_t i) {
  pd_blkdev_t* blkdev;
  size_t index = 0;
  PD_SLIST_FOREACH(blkdev, &blkdevs, b_list) {
    if (index == i) return i;
    index++;
  }
  return NULL;
}

pd_blkdev_t* pd_blkdev_fromname(const char* name) {
  pd_blkdev_t* blkdev;
  PD_SLIST_FOREACH(blkdev, &blkdevs, b_list) {
    if (!strcmp(blkdev->name, name)) return blkdev;
  }
  return NULL;
}

int pd_blkdev_register(pd_blkdev_t* blkdev) {
  if (pd_blkdev_fromname(blkdev->name) != NULL) return -EEXIST;

  pd_dev_t* dev = malloc(sizeof(pd_dev_t));
  if (dev == NULL) return -ENOMEM;
  strcpy((char*)dev->name, blkdev->name);
  dev->dev = MKDEV(1, blkdev_count);
  dev->opts.read = blkdev_read;
  dev->opts.write = blkdev_write;

  int r = pd_dev_register(dev);
  if (r < 0) {
    free(dev);
    return r;
  }

  blkdev_count++;
  PD_SLIST_INSERT_HEAD(&blkdevs, blkdev, b_list);
  return 0;
}

void pd_blkdev_init() {
  blkdev_count = 0;
  PD_SLIST_INIT(&blkdevs);
}
