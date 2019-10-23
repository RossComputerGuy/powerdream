#include <kernel/dev/char.h>
#include <kernel/error.h>

PD_SLIST_HEAD(chardevs, pd_chardev_t);

static struct chardevs chardevs;

static size_t chardev_read(pd_inode_t* inode, pd_file_t* file, char* buff, size_t size) {
  pd_chardev_t* chardev = pd_chardev_fromdev(inode->dev);
  if (chardev == NULL) return -ENODEV;
  if (chardev->read == NULL) return -EIO;

  size_t r = chardev->read(chardev, file, file->offset, buff, size);
  if (r < 0) return r;

  file->offset += r;
  return r;
}

static size_t chardev_write(pd_inode_t* inode, pd_file_t* file, const char* buff, size_t size) {
  pd_chardev_t* chardev = pd_chardev_fromdev(inode->dev);
  if (chardev == NULL) return -ENODEV;
  if (chardev->read == NULL) return -EIO;

  size_t w = chardev->write(chardev, file, file->offset, buff, size);
  if (r < 0) return r;

  file->offset += r;
  return r;
}

pd_chardev_t* pd_chardev_fromname(const char* name) {
  pd_chardev_t* chardev = NULL;
  PD_SLIST_FOREACH(chardev, &chardevs, c_list) {
    if (!strcmp(chardev->name, name)) return dev;
  }
  return NULL;
}

int pd_chardev_register(pd_chardev_t* chardev) {
  if (pd_chardev_fromname(chardev->name) == NULL) return -EEXIST;
  // TODO: register with kernel device API
  PD_SLIST_INSERT_HEAD(&chardevs, chardev, c_list);
  return 0;
}