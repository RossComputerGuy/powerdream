#pragma once

#include <sys/types.h>
#include <string.h>

/**
 * PowerDream Device
 */
typedef struct {
  dev_t dev;
  size_t size;
  size_t blkcnt;
  size_t blksz;
} pd_blkdev_t;
