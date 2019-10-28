#include <kernel/error.h>
#include <kernel/fs.h>
#include <kernel/klog.h>
#include <kernel/module.h>
#include <errno.h>
#include <kos.h>
#include <libgen.h>
#include <malloc.h>
#include <string.h>

static pd_fs_t* initrdfs = NULL;

extern uint8 initrd[];
KOS_INIT_ROMDISK(initrd);

static int initrd_create_node(pd_inode_t** inode, const char* path) {
  char* p;
  asprintf(&p, "/rd%s", path);
  file_t fd = fs_open(p, O_RDONLY);
  if (fd == FILEHND_INVALID) {
    printk("Failed to open %s from KallistiOS: %s", p, strerror(errno));
    free(p);
    return -EIO;
  }
  free(p);

  int r = pd_inode_create(inode, basename((char*)path));
  if (r < 0) {
    fs_close(fd);
    return r;
  }

  fs_close(fd);
  return 0;
}

static int initrd_mount(pd_inode_t** inode, pd_blkdev_t* dev, const char* source, const char* target, unsigned long flags, const void* data) {
  return initrd_create_node(inode, "/");
}

static int initrd_umount(pd_inode_t* inode) {
  return 0;
}

PD_MODINIT(initrd) {
  if ((initrdfs = malloc(sizeof(pd_fs_t))) == NULL) return -ENOMEM;
  initrdfs->type = PD_FS_VIRT;
  initrdfs->name = "initrd";
  initrdfs->mount = initrd_mount;
  initrdfs->umount = initrd_umount;
  return pd_fs_register(initrdfs);
}

PD_MODFINI(initrd) {
  pd_fs_unregister(initrdfs);
  free(initrdfs);
}

PD_MODDEF(initrd, "initrd", "Tristan Ross", "GPL-3.0", "0.1.0")
