#include <kernel/error.h>
#include <kernel/inode.h>
#include <malloc.h>
#include <string.h>

static ino_t next_inode = 0;

int pd_inode_open(pd_inode_t* inode, pd_file_t* file, int mode) {
  if (inode->opts.open == NULL) return -EINVAL;
  if (file->is_open == 1) return -EACCES;
  file->is_open = 1;
  file->mode = mode;
  return inode->opts.open(inode, file);
}

int pd_inode_close(pd_inode_t* inode, pd_file_t* file) {
  if (inode->opts.close == NULL) return -EINVAL;
  if (file->is_open == 0) return -EACCES;
  file->is_open = 0;
  file->mode = 0;
  return inode->opts.close(inode, file);
}

int pd_inode_create(pd_inode_t** inode, const char* name) {
  if (strlen(name) >= PATH_MAX) return -ENAMETOOLONG;
  if (((*inode) = malloc(sizeof(pd_inode_t))) == NULL) return -ENOMEM;
  memset((*inode), 0, sizeof(pd_inode_t));

  strcpy((char*)(*inode)->name, (char*)name);
  (*inode)->ino = next_inode++;
  return 0;
}
