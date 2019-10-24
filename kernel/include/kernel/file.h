#pragma once

#include <kernel/types.h>
#include <kernel/limits.h>
#include <kernel/types.h>

struct pd_inode;
struct pd_file;

struct dirent {
  long d_ino;
  long d_off;
  unsigned short d_reclen;
  size_t d_namlen;
  int d_type;
  char d_name[PATH_MAX];
};

/**
 * PowerDream File Operations
 */
typedef struct {
  int (*sync)(struct pd_inode_t* inode);
  int (*open)(struct pd_inode* inode, struct pd_file* file);
  int (*close)(struct pd_inode* inode, struct pd_file* file);
  size_t (*read)(struct pd_inode* inode, struct pd_file* file, char* buff, size_t size);
  size_t (*write)(struct pd_inode* inode, struct pd_file* file, const char* buff, size_t size);
  int (*ioctl)(struct pd_inode* inode, struct pd_file* file, unsigned int request, unsigned long argsize, void** arg);
  off_t (*seek)(struct pd_inode* inode, struct pd_file* file, off_t offset, int whence);
  int (*readnode)(struct pd_inode* inode, struct pd_inode** cinode, unsigned int index);
} pd_file_opts_t;

/**
 * PowerDream File
 */
typedef struct pd_file {
  ino_t inode;
  int mode;
  pid_t pid;
  uid_t uid;
  gid_t gid;
  off_t offset;
  int is_open;
} pd_file_t;

#include <kernel/inode.h>
