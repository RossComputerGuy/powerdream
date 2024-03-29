#pragma once

#include <kernel/file.h>
#include <kernel/limits.h>
#include <kernel/list.h>
#include <kernel/types.h>

typedef struct pd_chardev {
  PD_SLIST_ENTRY(struct pd_chardev) c_list;

  const char name[NAME_MAX];
  dev_t dev;

  size_t size;

  size_t (*read)(struct pd_chardev* chardev, pd_file_t* file, off_t offset, char* buffer, size_t length);
  size_t (*write)(struct pd_chardev* chardev, pd_file_t* file, off_t offset, const char* buffer, size_t length);
  int (*ioctl)(struct pd_chardev* chardev, pd_file_t* file, unsigned int request, unsigned long argsize, void** arg);
} pd_chardev_t;

/**
  * Get the number of registered character devices
  *
  * @return The number of registered character devices
  */
int pd_chardev_getcount();

/**
  * Gets a character device from its index
  *
  * @param[in] i The index of the device
  * @return A character device or NULL if it cannot be found
  */
pd_chardev_t* pd_chardev_fromindex(size_t i);

/**
  * Gets a character device from a device name
  *
  * @param[in] name The name of the device
  * @return A character device or NULL if it cannot be found
  */
pd_chardev_t* pd_chardev_fromname(const char* name);

/**
  * Registers a character device
  *
  * @param[in] chardev The character device to register
  * @return Zero on success or a negative errno on failure
  */
int pd_chardev_register(pd_chardev_t* chardev);

/**
 * Initialize character devices
 */
int pd_chardev_init();
