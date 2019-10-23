#pragma once

#include <kernel/types.h>

/**
  * Initialize the basic memory allocator
  */
void pd_basic_mem_init();

/**
 * Allocate memory for the kernel without the MMU
 *
 * @param[in] size The amount of bytes to allocate
 * @return An allocated memory address
 */
void* pd_basic_alloc(size_t size);