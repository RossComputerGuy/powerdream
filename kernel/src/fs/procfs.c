#include <kernel/error.h>
#include <kernel/fs.h>
#include <kernel/process.h>
#include <malloc.h>
#include <string.h>

static pd_fs_t* procfs = NULL;

int pd_procfs_init() {
  if ((procfs = malloc(sizeof(pd_fs_t))) == NULL) return -ENOMEM;
  memset(procfs, 0, sizeof(pd_fs_t));
  procfs->name = "procfs";
  procfs->type = PD_FS_PSEUDO;

  int r = pd_fs_register(procfs);
  // TODO: add sync code
  return r;
}
