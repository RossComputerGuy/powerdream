#pragma once

#include <kernel/limits.h>
#include <kernel/list.h>
#include <kernel/types.h>
#include <string.h>

/**
 * PowerDream Block Device
 */
typedef struct pd_blkdev {
  /* Block device list handle */
  PD_SLIST_ENTRY(struct pd_blkdev) b_list;

  const char name[NAME_MAX];
  size_t size;
  size_t blkcnt;
  size_t blksz;

  size_t (*blkread)(struct pd_blkdev* blkdev, void* buff, size_t blknumb, size_t size);
  size_t (*blkwrite)(struct pd_blkdev* blkdev, const void* buff, size_t blknumb, size_t size);
} pd_blkdev_t;

/**
  * Get the number of block devices
  *
  * @return The number of registered block devices
  */
int pd_blkdev_getcount();

/**
  * Gets a block device by its index in the registered list
  *
  * @param[in] i The index of the block device
  * @return A block device or NULL if it cannot be found.
  */
pd_blkdev_t* pd_blkdev_fromindex(size_t i);

/**
  * Returns the block device registered for a device name
  *
  * @param[in] name The name of the block device
  * @return A block device or NULL if it cannot be found.
  */
pd_blkdev_t* pd_blkdev_fromname(const char* name);

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
