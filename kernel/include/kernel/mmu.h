#pragma once

#include <kernel/io.h>
#include <kernel/types.h>

#define PD_MMU_TOP_SHIFT 21
#define PD_MMU_TOP_BITS 10
#define PD_MMU_TOP_MASK ((1 << PD_MMU_TOP_BITS) - 1)
#define PD_MMU_BOT_SHIFT 12
#define PD_MMU_BOT_BITS 9
#define PD_MMU_BOT_MASK ((1 << PD_MMU_BOT_BITS) - 1)
#define PD_MMU_IND_SHIFT 0
#define PD_MMU_IND_BITS 12
#define PD_MMU_IND_MASK ((1 << PD_MMU_IND_BITS) - 1)

#define MMU_KERNEL_RDONLY 0
#define MMU_KERNEL_RDWR 1
#define MMU_ALL_RDONLY 2
#define MMU_ALL_RDWR 3

#define PD_MMU_NO_CACHE 1
#define PD_MMU_CACHE_BACK 2
#define PD_MMU_CACHE_WT 3
#define PD_MMU_CACHEABLE PD_MMU_CACHE_BACK

#define PD_PAGESIZE 4096
#define PD_PAGESIZE_BITS 12
#define PD_PAGEMASK (PD_PAGESIZE - 1)

typedef struct {
  uint32_t physical:18;
  uint32_t prkey:2;
  uint32_t valid:1;
  uint32_t shared:1;
  uint32_t cache:1;
  uint32_t dirty:1;
  uint32_t wthru:1;
  uint32_t blank:7;

  uint32_t pteh;
  uint32_t ptel;
} pd_mmu_page_t;

#define PD_MMU_SUB_PAGES 512

typedef pd_mmu_page_t pd_mmu_subcontext_t[PD_MMU_SUB_PAGES];

#define PD_MMU_PAGES 1024

typedef struct {
  pd_mmu_subcontext_t sub[PD_MMU_PAGES];
  int asid;
} pd_mmu_context_t;

typedef pd_mmu_page_t* (*pd_mmu_mapfunc_t)(pd_mmu_context_t* context, int virtpage);

extern pd_mmu_context_t pd_mmu_ctx_curr;

/**
  * Switches the current MMU context
  *
  * @param[in] context The MMU table context
  */
void pd_mmu_use_table(pd_mmu_context_t context);

/**
  * Creates a new MMU context
  *
  * @param[out] context The pointer to store the newly created context
  * @param[in] asid The address space ID
  */
void pd_mmu_context_create(pd_mmu_context_t* context, int asid);

/**
  * Converts a virtual page address into a physical page address
  *
  * @param[out] context The context to use
  * @param[in] virtpage The virtual page address
  * @return The physical page address
  */
int pd_mmu_virt2phys(pd_mmu_context_t* context, int virtpage);

/**
  * Switches the context
  *
  * @param[out] context The context to switch to
  */
void pd_mmu_switch_context(pd_mmu_context_t* context);

/**
  * Maps a page and sets it up
  *
  * @param[out] context The context to use for mapping
  * @param[in] virtpage The virtual page address
  * @param[in] physpage The physical page address
  * @param[in] count The number of pages to map
  * @param[in] prot The protection to set
  * @param[in] cache The cache mode to set
  * @param[in] share Sets whether to share the page or not
  * @param[in] dirty Sets whether the page is dirty or not
  */
void pd_mmu_page_map(pd_mmu_context_t* context, int virtpage, int physpage, int count, int prot, int cache, int share, int dirty);

/**
  * Copy from a page to a buffer
  *
  * @param[out] contex The context to use for copying
  * @param[in] srcaddr The source address
  * @param[in] srccnt The count for the source address
  * @param[in] buffer The destination buffer
  * @return The amount copied
  */
int pd_mmu_copyin(pd_mmu_context_t* context, uint32_t srcaddr, uint32_t srccnt, void* buffer);

/**
  * Copy from a page in one context to another context
  *
  * @param[out] context The source context
  * @param[in] iov1 The source I/O vector
  * @param[in] iovcnt1 The source I/O vector count
  * @param[out] context The destination context
  * @param[in] iov2 The destination I/O vector
  * @param[in] iovcnt2 The destination I/O vector count
  * @return The amount copied
  */
int pd_mmu_copyv(pd_mmu_context_t* context1, struct iovec* iov1, int iovcnt1, pd_mmu_context_t* context2, struct iovec* iov2, int iovcnt2);

/**
  * Initialize the MMU
  */
void pd_mmu_init();

/**
 * Allocate memory for the kernel
 *
 * @param[in] size The amount of bytes to allocate
 * @return An allocated memory address or NULL if allocation failed
 */
void* kmalloc(size_t size);

/**
 * Free memory allocated for the kernel
 *
 * @param[in] ptr The allocated pointer to free
 */
void kfree(void* ptr);
