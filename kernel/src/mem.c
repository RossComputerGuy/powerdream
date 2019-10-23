#include <kernel/mem.h>

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