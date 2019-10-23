#pragma once

#include <kernel/limits.h>
#include <kernel/list.h>
#include <kernel/types.h>

typedef struct pd_chardev {
  PD_SLIST_ENTRY(struct pd_chardev) c_list;

  const char name[NAME_MAX];
  dev_t dev;

  size_t size;

  size_t (*read)(off_t offset, char* buffer, size_t length);
  size_t (*write)(off_t offset, const char* buffer, size_t length);
} pd_chardev_t;

/**
  * Gets a character device from a device
  *
  * @param[in] dev The device
  * @return A character device or NULL if it cannot be found
  */
pd_chardev_t* pd_chardev_fromdev(dev_t dev);

/**
  * Registers a character device
  *
  * @param[in] chardev The character device to register
  * @return Zero on success or a negative errno on failure
  */
int pd_chardev_register(pd_chardev_t* chardev);