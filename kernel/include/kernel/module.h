#pragma once

#include <kernel/types.h>

typedef int (*pd_modinit_t)();
typedef void (*pd_modfini_t)();

typedef struct {
  const char name[128];
  const char author[128];
  const char license[128];
  const char version[128];

  pd_modinit_t init;
  pd_modfini_t exit;
} pd_modinfo_t;

#define PD_MODDEF(id) const pd_modinfo_t kmod_##id## __attribute__((section(".modinfo")))

/**
  * Get the number of installed kernel modules
  */
size_t pd_getmodcount();

/**
  * Get kernel module info by its index in the modinfo array
  *
  * @param[in] i The index of the kernel module to load
  * @return A pointer to the modinfo entry or NULL if not found
  */
pd_modinfo_t* pd_module_getinfo(size_t index);

/**
  * Initializes a kernel module by its index in the modinfo array
  *
  * @param[in] i The index of the kernel module to load
  * @return Zero on success or a negative errno code on failure
  */
int pd_module_init(size_t i);

/**
  * Finishes a kernel module, unloads it
  *
  * @param[in] i The index of the kernel module to load
  * @return Zero on success or a negative errno code on failure
  */
int pd_module_fini(size_t i);

/**
  * Initialize the kernel module system
  *
  * @return Zero on success or a negative errno code on failure
  */
int pd_kmods_init();