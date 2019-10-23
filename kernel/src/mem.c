#include <kernel/mem.h>
#include <kernel/mmu.h>

/* Basic allocator which is used in the early kernel booting */
extern unsigned long end;
static void* mem_base = NULL;

void pd_basic_mem_init() {
  int base = (int)(&end);
  base = (base / 4) * 4 + 4;
  mem_base = (void*)base;
}

void* pd_basic_alloc(size_t size) {
  void* base = mem_base;
  if (size & 3) size = (size + 4) & ~3;
  mem_base += size;
  return base;
}

/* Advanced allocator which is used throught the kernel */
static pd_mmu_context_t* kmem;

void pd_mm_init() {
  pd_mmu_context_create(&kmem, 0);
}

void* kmalloc(size_t size) {
  return NULL;
}