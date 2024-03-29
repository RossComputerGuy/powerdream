#include <kernel/dev/char.h>
#include <kernel/device.h>
#include <kernel/error.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

#define DEV_NULL MKDEV(1, 0)
#define DEV_RANDOM MKDEV(1, 1)

PD_SLIST_HEAD(chardevs, pd_chardev_t);

static struct chardevs chardevs;
static size_t chardev_count = 0;

static size_t chardev_read(pd_inode_t* inode, pd_file_t* file, char* buff, size_t size) {
  pd_chardev_t* chardev = pd_chardev_fromname(inode->name);
  if (chardev == NULL) chardev = inode->impl;
  if (chardev == NULL) return -ENODEV;
  if (chardev->dev == DEV_NULL) {
    memcpy(buff, 0, size);
    return size;
  }
  if (chardev->dev == DEV_RANDOM) {
    size_t i;
    for (i = 0; i < size; i++) buff[i] = rand();
    return size;
  }
  if (chardev->read == NULL) return -EIO;

  size_t r = chardev->read(chardev, file, file->offset, buff, size);
  if (r < 0) return r;

  if (file != NULL) file->offset += r;
  return r;
}

static size_t chardev_write(pd_inode_t* inode, pd_file_t* file, const char* buff, size_t size) {
  pd_chardev_t* chardev = pd_chardev_fromname(inode->name);
  if (chardev == NULL) chardev = inode->impl;
  if (chardev == NULL) return -ENODEV;
  if (chardev->dev == DEV_NULL) return size;
  if (chardev->dev == DEV_RANDOM) return -EPERM;
  if (chardev->write == NULL) return -EIO;

  size_t w = chardev->write(chardev, file, file->offset, buff, size);
  if (w < 0) return w;

  if (file != NULL) file->offset += w;
  return w;
}

static int chardev_ioctl(pd_inode_t* inode, pd_file_t* file, unsigned int request, unsigned long argsize, void** arg) {
  pd_chardev_t* chardev = pd_chardev_fromname(inode->name);
  if (chardev == NULL) chardev = inode->impl;
  if (chardev == NULL) return -ENODEV;
  if (chardev->ioctl == NULL) return -EIO;
  return chardev->ioctl(chardev, file, request, argsize, arg);
}

int pd_chardev_getcount() {
  return chardev_count;
}

pd_chardev_t* pd_chardev_fromindex(size_t i) {
  pd_chardev_t* chardev = NULL;
  size_t index = 0;
  PD_SLIST_FOREACH(chardev, &chardevs, c_list) {
    if (index == i) return chardev;
    index++;
  }
  return NULL;
}

pd_chardev_t* pd_chardev_fromname(const char* name) {
  pd_chardev_t* chardev = NULL;
  PD_SLIST_FOREACH(chardev, &chardevs, c_list) {
    if (!strcmp(chardev->name, name)) return chardev;
  }
  return NULL;
}

int pd_chardev_register(pd_chardev_t* chardev) {
  if (pd_chardev_fromname(chardev->name) != NULL) return -EEXIST;

  pd_dev_t* dev = malloc(sizeof(pd_dev_t));
  strcpy((char*)dev->name, chardev->name);
  dev->dev = chardev->dev;
  dev->opts.read = chardev_read;
  dev->opts.write = chardev_write;
  dev->opts.ioctl = chardev_ioctl;

  int r = pd_dev_register(dev);
  if (r < 0) {
    free(dev);
    return r;
  }

  chardev_count++;
  PD_SLIST_INSERT_HEAD(&chardevs, chardev, c_list);
  return 0;
}

#define DEF_CHARDEV(fname, idev) { \
  pd_chardev_t* chardev = malloc(sizeof(pd_chardev_t)); \
  if (chardev == NULL) return -ENOMEM; \
  strcpy((char*)chardev->name, fname); \
  chardev->dev = idev; \
  int r = pd_chardev_register(chardev); \
  if (r < 0) { \
    free(chardev); \
    return r; \
  } \
}

int pd_chardev_init() {
  chardev_count = 0;
  PD_SLIST_INIT(&chardevs);

  /* null character device */
  DEF_CHARDEV("null", DEV_NULL);

  /* random character device */
  DEF_CHARDEV("random", DEV_RANDOM);
  return 0;
}
