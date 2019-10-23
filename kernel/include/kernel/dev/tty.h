#pragma once

#include <kernel/limits.h>
#include <kernel/list.h>
#include <kernel/types.h>

typedef struct pd_tty {
  PD_SLIST_ENTRY(struct pd_tty) t_list;

  const char name[NAME_MAX];
  dev_t dev;

  size_t (*write)(struct pd_tty* tty, const void* buffer, size_t length);
  size_t (*read)(struct pd_tty* tty, void* buffer, size_t length);
} pd_tty_t;

/**
  * Gets a tty from a device
  *
  * @param[in] dev The device to use
  * @return A tty if found, NULL if not found
  */
pd_tty_t* pd_tty_fromdev(dev_t dev);