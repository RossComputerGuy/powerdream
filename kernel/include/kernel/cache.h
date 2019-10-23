#pragma once

#include <kernel/types.h>

/**
 * Flush the instruction cache
 *
 * @param[in] start The physical address to begin flushing at
 * @param[in] count The number of bytes to flush
 */
void pd_icache_flush_range(uint32_t start, uint32_t count);

/**
 * Invalidate the data/operand cahce
 *
 * @param[in] start The physical address to begin invalidating at
 * @param[in] count The number of bytes to invalidate
 */
void pd_dcache_inval_range(uint32_t start, uint32_t count);

/**
 * Flush the data/operand cache
 *
 * @param[in] start The physical address to begin flushing at
 * @param[in] count The number of bytes to flush
 */
void pd_dcache_flush_range(uint32_t start, uint32_t count);
