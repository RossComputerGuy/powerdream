#pragma once

#include <kernel/blkdev.h>
#include <sys/types.h>
#include <kos/fs.h>

#define PD_FS_PSEUDO 0
#define PD_FS_VIRT 1
#define PD_FS_PHYS 2

/**
 * PowerDream Filesystem
 */
typedef struct {
  /**
   * A function pointer to mount a filesystem handler for a specific device or path
   *
   * @param[out] handler The pointer to store the VFS handler.
   * @param[in] dev The source block device to use, will be NULL if the source argument is set.
   * @param[in] source The source path, will be NULL if the device argument is set.
   * @param[in] target The target mount path.
   * @param[in] flags The mount flags used.
   * @return Returns 0 on success or an errno code on error.
   */
  int (*mount)(vfs_handler_t** handler, pd_blkdev_t* dev, const char* source, const char* target, unsigned long flags);

  /**
   * A function pointer to unmount a filesystem handler for a specific device or path
   *
   * @param[in] handler The handler that was created by the mount operation.
   * @return Returns 0 on success or an errno code on error.
   */
  int (*umount)(vfs_handler_t* handler);

  /* The type of filesystem */
  int type;

  /* The name of the filesystem */
  const char* name;
} pd_fs_t;

/**
 * Register a new filesystem
 *
 * @param[in] fs The filesystem to register.
 * @return 0 on error or an errno code on failure.
 */
int pd_fs_register(pd_fs_t* fs);
