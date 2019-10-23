#pragma once

#include <kernel/file.h>
#include <kernel/limits.h>
#include <kernel/list.h>
#include <kernel/types.h>

#define MKDEV(a, b) ((a << 8) | (b))

#define PD_BLK_MINOR 1
#define PD_CHAR_MINOR 2
#define PD_TTY_MINOR 3

/**
 * PowerDream Device
 */
typedef struct pd_dev {
  PD_SLIST_ENTRY(struct pd_dev) d_list;

  const char name[NAME_MAX];
  dev_t dev;
  pd_file_opts_t opts;
} pd_dev_t;

/**
  * Gets a device from the device number
  *
  * @param[in] dev The device identification
  * @return NULL if a device is not found or a device
  */
pd_dev_t* pd_dev_fromdev(dev_t dev);

int pd_dev_register(pd_dev_t* device);