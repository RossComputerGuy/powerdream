#pragma once

#include <kernel/klog.h>
#include <kernel/types.h>
#include <stdio.h>

typedef int (*pd_modinit_t)();
typedef void (*pd_modfini_t)();

typedef struct {
  const char name[128];
  const char author[128];
  const char license[128];
  const char version[128];

  pd_modinit_t init;
  pd_modfini_t fini;
} pd_modinfo_t;

#define PD_MODINIT(id) int kmod_ ## id ## _init()
#define PD_MODFINI(id) void kmod_ ## id ## _fini()
#define PD_MODDEF(id, name, author, license, version) pd_modinfo_t kmod_##id = { name, author, license, version, .init = kmod_ ## id ## _init, .fini = kmod ## id ## _fini }; \
  __attribute__((constructor)) void kmod_ ## id ## _register() { \
    int r = pd_kmod_register(kmod_##id); \
    if (r < 0) printk("Failed to register kernel module %s: %d", #id, -r); \
    else printk("Registered kernel module: %s", #id); \
  }

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
 * Registeres a kernel module
 *
 * @param[in] modinfo The kernel module information
 * @return Zero on success or a negative errno code on failure
 */
int pd_kmod_register(pd_modinfo_t modinfo);

/**
  * Initialize the kernel module system
  *
  * @return Zero on success or a negative errno code on failure
  */
int pd_kmods_init();
