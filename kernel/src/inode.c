#include <kernel/error.h>
#include <kernel/inode.h>
#include <kernel/mmu.h>
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
  if (strlen(name) >= NAME_MAX) return -ENAMETOOLONG;
  if (((*inode) = kmalloc(sizeof(pd_inode_t))) == NULL) return -ENOMEM;
  memset((*inode), 0, sizeof(pd_inode_t));

  strcpy((char*)(*inode)->name, (char*)name);
  (*inode)->ino = next_inode++;
  return 0;
}

int pd_inode_resolve_path(pd_inode_t* root, pd_inode_t** inode, const char* path) {
  if (root->opts.readnode == NULL) return -EINVAL;
  char* p = strtok((char*)path, "/");
  if (p == NULL) return 0;
  size_t index = 0;
  int r = 0;
  pd_inode_t* parent = NULL;
  pd_inode_t* tmp = NULL;
  while ((r = root->opts.readnode(root, &parent, index++)) >= 0) {
    if (!S_ISDIR(parent->mode)) return -ENOTDIR;
    if (!strcmp((*inode)->name, p)) {
      if (strlen(path + strlen(p) + 1) == 0) {
        *inode = parent;
        return 0;
      } else if ((r = pd_inode_resolve_path(parent, &tmp, path + strlen(p))) >= 0) {
        if (strlen(path + strlen(p) + 1) == 0) {
          *inode = tmp;
          return 0;
        }
      }
    }
  }
  return -ENOENT;
}
