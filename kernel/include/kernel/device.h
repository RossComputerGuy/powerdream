#pragma once

#include <kernel/file.h>
#include <kernel/limits.h>
#include <kernel/list.h>
#include <kernel/types.h>

#define PD_DEV_MAJOR(dev) (dev >> 8)
#define PD_DEV_MINOR(dev) (dev & 0xFF)
#define MKDEV(major, minor) ((major << 8) | (minor))

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
  * Gets a device from the device name
  *
  * @param[in] name The device name
  * @return NULL if a device is not found or a device
  */
pd_dev_t* pd_dev_fromname(const char* name);

/**
 * Registers a device
 *
 * @param[in] device The device to register
 * @return Zero or a negative errno code
 */
int pd_dev_register(pd_dev_t* device);

/**
 * Initialize the device system
 */
void pd_dev_init();
