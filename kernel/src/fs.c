#include <kernel/error.h>
#include <kernel/fs.h>
#include <kernel/mem.h>
#include <kernel/process.h>
#include <string.h>

PD_SLIST_HEAD(filesystems, pd_fs_t);
PD_SLIST_HEAD(mountpoint, pd_mountpoint_t);

static struct filesystems filesystems;
static struct mountpoint mountpoints;

pd_fs_t* pd_fs_fromname(const char* name) {
  pd_fs_t* fs;
  PD_SLIST_FOREACH(fs, &filesystems, f_list) {
    if (!strcmp(fs->name, name)) return fs;
  }
  return NULL;
}

pd_mountpoint_t* pd_mountpoint_fromtarget(const char* target) {
  pd_mountpoint_t* mountpoint;
  PD_SLIST_FOREACH(mountpoint, &mountpoints, m_list) {
    if (!strcmp(mountpoint->target, target)) return mountpoint;
  }
  return NULL;
}

pd_mountpoint_t* pd_mountpoint_fromdev(const char* dev) {
  pd_mountpoint_t* mountpoint;
  PD_SLIST_FOREACH(mountpoint, &mountpoints, m_list) {
    if (!strcmp(mountpoint->dev, dev)) return mountpoint;
  }
  return NULL;
}

pd_mountpoint_t* pd_mountpoint_fromsrc(const char* source) {
  pd_mountpoint_t* mountpoint;
  PD_SLIST_FOREACH(mountpoint, &mountpoints, m_list) {
    if (mountpoint->source != NULL && !strcmp(mountpoint->source, source)) return mountpoint;
  }
  return NULL;
}

int pd_resolve_path(pd_inode_t** inode, const char* path) {
  pd_mountpoint_t* rootmp = pd_mountpoint_fromtarget("/");
  if (rootmp == NULL) return -ENOENT;
  return pd_inode_resolve_path(rootmp->inode, inode, path);
}

int pd_fs_register(pd_fs_t* fs) {
  if (pd_fs_fromname(fs->name) != NULL) return -EEXIST;
  PD_SLIST_INSERT_HEAD(&filesystems, fs, f_list);
  return 0;
}

int pd_fs_mount(pd_fs_t* fs, pd_blkdev_t* dev, const char* source, const char* target, unsigned long flags, const void* data) {
  if (dev != NULL && pd_mountpoint_fromdev(dev->name) != NULL) return -EACCES;
  if (source != NULL && pd_mountpoint_fromsrc(source) != NULL) return -EACCES;
  if (pd_mountpoint_fromtarget(target) != NULL) return -EACCES;

  pd_mountpoint_t* mp = kmalloc(sizeof(pd_mountpoint_t));
  if (mp == NULL) return -ENOMEM;
  memset(mp, 0, sizeof(pd_mountpoint_t));

  if (dev != NULL) strcpy((char*)mp->dev, dev->name);
  if (source != NULL) mp->source = source;

  mp->fs = fs->name;
  mp->target = target;
  mp->flags = flags;

  int r = fs->mount(&mp->inode, dev, source, target, flags, data);
  if (r < 0) {
    kfree(mp);
    return r;
  }

  PD_SLIST_INSERT_HEAD(&mountpoints, mp, m_list);
  return 0;
}

int pd_fs_umount(pd_fs_t* fs, const char* target) {
  pd_mountpoint_t* mp = pd_mountpoint_fromtarget(target);
  if (mp == NULL) return -EINVAL;

  int r = fs->umount(mp->inode);
  if (r < 0) return r;

  PD_SLIST_REMOVE(&mountpoints, mp, pd_mountpoint_t, m_list);
  kfree(mp);
  return 0;
}

int mount(const char* source, const char* target, const char* fstype, unsigned long flags, const void* data) {
  pd_process_t* curr_proc = pd_process_getcurr();
  if (curr_proc != NULL) {
    if (curr_proc->uid > 0 && curr_proc->gid > 0) return -EPERM;
  }

  pd_fs_t* fs = pd_fs_fromname(fstype);
  if (fs == NULL) return -ENODEV;
  // TODO: get device from source
  return pd_fs_mount(fs, NULL, source, target, flags, data);
}

int umount(const char* target) {
  pd_process_t* curr_proc = pd_process_getcurr();
  if (curr_proc != NULL) {
    if (curr_proc->uid > 0 && curr_proc->gid > 0) return -EPERM;
  }

  pd_mountpoint_t* mp = pd_mountpoint_fromtarget(target);
  if (mp == NULL) return -EINVAL;
  
  pd_fs_t* fs = pd_fs_fromname(mp->fs);
  if (fs == NULL) return -ENODEV;

  return pd_fs_umount(fs, target);
}

void pd_fs_init() {
  PD_SLIST_INIT(&filesystems);
  PD_SLIST_INIT(&mountpoints);
}
