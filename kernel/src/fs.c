#include <kernel/error.h>
#include <kernel/fs.h>
#include <malloc.h>
#include <string.h>

SLIST_HEAD(filesystems, pd_fs_t);
SLIST_HEAD(mountpoint, pd_mountpoint_t);

static struct filesystems filesystems;
static struct mountpoint mountpoints;

pd_fs_t* pd_fs_fromname(const char* name) {
  pd_fs_t* fs;
  SLIST_FOREACH(fs, &filesystems, f_list) {
    if (!strcmp(fs->name, name)) return fs;
  }
  return NULL;
}

pd_mountpoint_t* pd_mountpoint_fromtarget(const char* target) {
  pd_mountpoint_t* mountpoint;
  SLIST_FOREACH(mountpoint, &mountpoints, m_list) {
    if (!strcmp(mountpoint->target, target)) return mountpoint;
  }
  return NULL;
}

pd_mountpoint_t* pd_mountpoint_fromdev(dev_t dev) {
  pd_mountpoint_t* mountpoint;
  SLIST_FOREACH(mountpoint, &mountpoints, m_list) {
    if (mountpoint->dev == dev) return mountpoint;
  }
  return NULL;
}

pd_mountpoint_t* pd_mountpoint_fromsrc(const char* source) {
  pd_mountpoint_t* mountpoint;
  SLIST_FOREACH(mountpoint, &mountpoints, m_list) {
    if (mountpoint->source != NULL && !strcmp(mountpoint->source, source)) return mountpoint;
  }
  return NULL;
}

int pd_fs_register(pd_fs_t* fs) {
  if (pd_fs_fromname(fs->name) != NULL) return -EEXIST;
  SLIST_INSERT_HEAD(&filesystems, fs, f_list);
  return 0;
}

int pd_fs_mount(pd_fs_t* fs, pd_blkdev_t* dev, const char* source, const char* target, unsigned long flags) {
  if (dev != NULL && pd_mountpoint_fromdev(dev->dev) != NULL) return -EACCES;
  if (source != NULL && pd_mountpoint_fromsrc(source) != NULL) return -EACCES;
  if (pd_mountpoint_fromtarget(target) != NULL) return -EACCES;

  pd_mountpoint_t* mp = malloc(sizeof(pd_mountpoint_t));
  if (mp == NULL) return -ENOMEM;
  memset(mp, 0, sizeof(pd_mountpoint_t));

  if (dev != NULL) mp->dev = dev->dev;
  if (source != NULL) mp->source = source;

  mp->target = target;
  mp->flags = flags;

  int r = fs->mount(&mp->inode, dev, source, target, flags);
  if (r < 0) {
    free(mp);
    return r;
  }

  SLIST_INSERT_HEAD(&mountpoints, mp, m_list);
  return 0;
}

void pd_fs_init() {
  SLIST_INIT(&filesystems);
  SLIST_INIT(&mountpoints);
}
