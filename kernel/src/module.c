#include <kernel/error.h>
#include <kernel/klog.h>
#include <kernel/module.h>
#include <malloc.h>
#include <string.h>

#define MODINFO_MAX 10

static int mod_count = 0;
static pd_modinfo_t modules[MODINFO_MAX];
static int mods_inited[MODINFO_MAX];

size_t pd_getmodcount() {
  return mod_count;
}

pd_modinfo_t* pd_module_getinfo(size_t index) {
  if (index > mod_count) return NULL;
  return &modules[index];
}

int pd_module_init(size_t i) {
  pd_modinfo_t* modinfo = pd_module_getinfo(i);
  if (modinfo == NULL) return -ENOENT;
  if (mods_inited[i]) return -EINVAL;
  printk("Initializing kernel module %s", modinfo->name);
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

int pd_kmod_register(pd_modinfo_t modinfo) {
  if (mod_count + 1 == MODINFO_MAX) return -ENOMEM;
  pd_modinfo_t* m = &modules[mod_count++];
  strcpy((char*)m->name, modinfo.name);
  strcpy((char*)m->author, modinfo.author);
  strcpy((char*)m->license, modinfo.license);
  strcpy((char*)m->version, modinfo.version);
  m->init = modinfo.init;
  m->fini = modinfo.fini;
  return 0;
}

int pd_kmods_init() {
  memset(mods_inited, 0, sizeof(int) * MODINFO_MAX);

  size_t i;
  for (i = 0; i < mod_count; i++) {
    int r = pd_module_init(i);
    if (r < 0) {
      free(mods_inited);
      return r;
    }
  }
  return 0;
}
