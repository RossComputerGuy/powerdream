#pragma once

#include <kernel/blkdev.h>
#include <kernel/inode.h>
#include <sys/types.h>
#include <sys/queue.h>

#define PD_FS_PSEUDO 0
#define PD_FS_VIRT 1
#define PD_FS_PHYS 2

/**
 * PowerDream Filesystem
 */
typedef struct pd_fs {
  /* Filesystem list handle */
  SLIST_ENTRY(pd_fs) f_list;

  /**
   * A function pointer to mount a filesystem inode for a specific device or path
   *
   * @param[out] inode The inode that represents the rootfs.
   * @param[in] dev The source block device to use, will be NULL if the source argument is set.
   * @param[in] source The source path, will be NULL if the device argument is set.
   * @param[in] target The target mount path.
   * @param[in] flags The mount flags used.
   * @return Returns 0 on success or an errno code on error.
   */
  int (*mount)(pd_inode_t** inode, pd_blkdev_t* dev, const char* source, const char* target, unsigned long flags);

  /**
   * A function pointer to unmount a filesystem inode for a specific device or path
   *
   * @param[in] inode The inode that was created by the mount operation.
   * @return Returns 0 on success or an errno code on error.
   */
  int (*umount)(pd_inode_t* inode);

  /* The type of filesystem */
  int type;

  /* The name of the filesystem */
  const char* name;
} pd_fs_t;

/**
 * PowerDream Mount Point
 */
typedef struct pd_mountpoint {
  /* Mount point list handle */
  SLIST_ENTRY(pd_mountpoint) m_list;

  /* The inode for the filesystem */
  pd_inode_t* inode;

  /* The name of the filesystem mounted */
  const char* fs;

  /* The device used to mount the filesystem */
  dev_t dev;

  /* The source path used to mount the filesystem */
  const char* source;

  /* The target location */
  const char* target;

  /* Mount flags */
  unsigned long flags;
} pd_mountpoint_t;

/**
 * Gets a filesystem by its name
 *
 * @param[in] name The name of the filesystem to get
 * @return A filesystem or NULL if it cannot be found
 */
pd_fs_t* pd_fs_fromname(const char* name);

/**
 * Get a mount point by its target path
 *
 * @param[in] target The target path
 * @return A mount point or NULL if it cannot be found
 */
pd_mountpoint_t* pd_mountpoint_fromtarget(const char* target);

/**
 * Get a mount point by its device
 *
 * @param[in] dev The device to use
 * @return A mount point or NULL if it cannot be found
 */
pd_mountpoint_t* pd_mountpoint_fromdev(dev_t dev);

/**
 * Get a mount point by its source
 *
 * @param[in] source The source path
 * @reutnr A mount pount or NULL if it cannot be found
 */
pd_mountpoint_t* pd_mountpoint_fromsrc(const char* source);

/**
 * Register a new filesystem
 *
 * @param[in] fs The filesystem to register.
 * @return 0 on success or a negative errno code on failure.
 */
int pd_fs_register(pd_fs_t* fs);

/**
 * Mount a filesystem
 *
 * @param[in] fs The filesystem to mount
 * @param[in] dev The block device to mount, can be NULL
 * @param[in] source The source to mount, can be NULL
 * @param[in] target The resulting mount path
 * @param[in] flags The mount flags
 * @return 0 on success, negative errno on failure
 */
int pd_fs_mount(pd_fs_t* fs, pd_blkdev_t* dev, const char* source, const char* target, unsigned long flags);

/**
 * Initialize the filesystems
 */
void pd_fs_init();
