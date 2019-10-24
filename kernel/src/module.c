#include <kernel/error.h>
#include <kernel/module.h>
#include <string.h>

extern uint32_t modinfo_start;
extern uint32_t modinfo_end;

#define MODCOUNT ((modinfo_end - modinfo_start) / sizeof(pd_modinfo_t))

static int* mods_inited;

size_t pd_getmodcount() {
  return MODCOUNT;
}

pd_modinfo_t* pd_module_getinfo(size_t index) {
  if (index > MODCOUNT) return NULL;
  return (pd_modinfo_t*)(modinfo_start + (index * sizeof(pd_modinfo_t)));
}

int pd_module_init(size_t i) {
  pd_modinfo_t* modinfo = pd_module_getinfo(i);
  if (modinfo == NULL) return -ENOENT;
  if (mods_inited[i]) return -EINVAL;
  int r = modinfo->init();
  if (r < 0) return r;
  mods_inited[i] = r;
  return r;
}

int pd_module_fini(size_t i) {
  pd_modinfo_t* modinfo = pd_module_getinfo(i);
  if (modinfo == NULL) return -ENOENT;
  if (!mods_inited[i]) return -EINVAL;
  modinfo->fini();
  mods_inited[i] = 0;
  return 0;
}

int pd_kmods_init() {
  mods_inited = malloc(sizeof(int) * MODCOUNT);
  if (mods_inited == NULL) return -ENOMEM;

  for (size_t i = 0; i < MODCOUNT; i++) {
    int r = pd_module_init(i);
    if (r < 0) {
      free(mods_inited);
      return r;
    }
  }
  return 0;
}