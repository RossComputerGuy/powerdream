#include <kernel/dev/block.h>
#include <kernel/dev/char.h>
#include <kernel/dev/tty.h>
#include <kernel/device.h>
#include <kernel/error.h>
#include <kernel/fs.h>
#include <malloc.h>
#include <stdio.h>

static pd_fs_t* devfs = NULL;

static pd_inode_t** devfs_blocks = NULL;
static size_t devfs_blkcnt = 0;

static pd_inode_t** devfs_chars = NULL;
static size_t devfs_chrcnt = 0;

static int devfs_sync(pd_inode_t* inode) {
  /* Block devices */
  if (devfs_blkcnt != pd_blkdev_getcount()) {
    pd_inode_t** inodes = malloc(sizeof(pd_inode_t) * pd_blkdev_getcount());
    if (inodes == NULL) return -ENOMEM;
    size_t i;
    for (i = 0; i < pd_blkdev_getcount(); i++) {
      pd_blkdev_t* blkdev = pd_blkdev_fromindex(i);
      pd_dev_t* device = pd_dev_fromname(blkdev->name);
      int found = 0;
      if (devfs_blocks != NULL) {
        size_t x;
        for (x = 0; x < devfs_blkcnt; x++) {
          if (devfs_blocks[x]->impl == blkdev) {
            inodes[i] = devfs_blocks[x];
            found = 1;
            break;
          }
        }
      }
      if (!found) {
        char* name = malloc(NAME_MAX);
        if (name == NULL) return -ENOMEM;
        snprintf(name, NAME_MAX, "block%d", i);

        int r = pd_inode_create(&inodes[i], name);
        free(name);
        if (r < 0) return r;

        inodes[i]->mode = PD_NODE_BLK;
        inodes[i]->dev = device->dev;
        inodes[i]->opts.read = device->opts.read;
        inodes[i]->opts.write = device->opts.write;
        inodes[i]->opts.open = device->opts.open;
        inodes[i]->opts.close = device->opts.close;
        inodes[i]->opts.ioctl = device->opts.ioctl;
        inodes[i]->opts.sync = device->opts.sync;
        inodes[i]->impl = blkdev;
      }
    }
    if (devfs_blocks != NULL) free(devfs_blocks);
    devfs_blkcnt = pd_blkdev_getcount();
    devfs_blocks = inodes;
  }

  /* Character devices */
  if (devfs_chrcnt != pd_chardev_getcount()) {
    pd_inode_t** inodes = malloc(sizeof(pd_inode_t) * pd_chardev_getcount());
    if (inodes == NULL) return -ENOMEM;
    size_t i;
    for (i = 0; i < pd_chardev_getcount(); i++) {
      pd_chardev_t* chardev = pd_chardev_fromindex(i);
      pd_dev_t* device = pd_dev_fromname(chardev->name);
      int found = 0;
      if (devfs_chars != NULL) {
        size_t x;
        for (x = 0; x < devfs_chrcnt; x++) {
          if (devfs_chars[x]->impl == chardev) {
            inodes[i] = devfs_chars[x];
            found = 1;
            break;
          }
        }
      }
      if (!found) {
        int r = pd_inode_create(&inodes[i], chardev->name);
        if (r < 0) return r;

        inodes[i]->mode = PD_NODE_CHR;
        inodes[i]->dev = device->dev;
        inodes[i]->opts.read = device->opts.read;
        inodes[i]->opts.write = device->opts.write;
        inodes[i]->opts.open = device->opts.open;
        inodes[i]->opts.close = device->opts.close;
        inodes[i]->opts.ioctl = device->opts.ioctl;
        inodes[i]->opts.sync = device->opts.sync;
        inodes[i]->impl = chardev;
      }
    }
    if (devfs_chars != NULL) free(devfs_chars);
    devfs_chrcnt = pd_chardev_getcount();
    devfs_chars = inodes;
  }
  return 0;
}

static int devfs_readnode(pd_inode_t* inode, pd_inode_t** cinode, unsigned int index) {
  if (index >= 0 && index < devfs_blkcnt) {
    /* Block devices */
    *cinode = devfs_blocks[index];
  } else if (index > devfs_blkcnt && index < (devfs_chrcnt + devfs_blkcnt)) {
    /* Character devices */
    *cinode = devfs_chars[(index - devfs_blkcnt)];
  } else return -ENOENT;
  return 0;
}

static int devfs_mount(pd_inode_t** inode, pd_blkdev_t* dev, const char* source, const char* target, unsigned long flags, const void* data) {
  int r = pd_inode_create(inode, "/");
  if (r < 0) return r;
  (*inode)->mode = PD_NODE_DIR;
  (*inode)->uid = (*inode)->gid = 0;
  (*inode)->opts.sync = devfs_sync;
  (*inode)->opts.readnode = devfs_readnode;
  return 0;
}

static int devfs_umount(pd_inode_t* inode) {
  return 0;
}

int pd_devfs_init() {
  if ((devfs = malloc(sizeof(pd_fs_t))) == NULL) return -ENOMEM;
  memset(devfs, 0, sizeof(pd_fs_t));
  devfs->name = "devfs";
  devfs->type = PD_FS_PSEUDO;
  devfs->mount = devfs_mount;
  devfs->umount = devfs_umount;

  int r = pd_fs_register(devfs);
  if (r < 0) return r;

  r = devfs_sync(NULL);
  if (r < 0) {
    pd_fs_unregister(devfs);
    if (devfs_blocks != NULL) free(devfs_blocks);
    free(devfs);
    return r;
  }
  return 0;
}
