#pragma once

#include <kos/limits.h>
#include <sys/types.h>
#include <sys/queue.h>
#include <string.h>

/**
 * PowerDream Block Device
 */
typedef struct pd_blkdev {
  /* Block device list handle */
  SLIST_ENTRY(pd_blkdev) b_list;

  const char name[PATH_MAX];
  dev_t dev;
  size_t size;
  size_t blkcnt;
  size_t blksz;

  size_t (*blkread)(struct pd_blkdev* blkdev, void* buff, size_t blknumb, size_t size);
  size_t (*blkwrite)(struct pd_blkdev* blkdev, const void* buff, size_t blknumb, size_t size);
} pd_blkdev_t;

/**
 * Returns the block device registered for a device
 *
 * @param[in] dev The device that the blkdev is mapped to
 * @return A block device or NULL if it cannot be found.
 */
pd_blkdev_t* pd_blkdev_fromdev(dev_t dev);

/**
 * Registeres a block device
 *
 * @param[in] blkdev The block device to register
 * @return A negative errno code on failure, 0 on success.
 */
int pd_blkdev_register(pd_blkdev_t* blkdev);

/**
 * Initializes the block devices
 */
void pd_blkdev_init();
