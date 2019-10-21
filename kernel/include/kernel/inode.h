#pragma once

#include <kernel/file.h>
#include <kos/limits.h>
#include <sys/types.h>

/**
 * PowerDream Inode
 */
typedef struct pd_inode {
  /* The name of the file */
  const char name[PATH_MAX];

  /* The owner's group ID */
  gid_t gid;

  /* The owner's user ID */
  uid_t uid;

  /* The inode number */
  ino_t ino;

  /* The size of the contents in bytes */
  size_t size;
  
  /* Inode mode */
  mode_t mode;
  
  /* Hard link count */
  nlink_t nlink;

  /* The inode's relative device */
  dev_t rdev;

  /* Device for the inode */
  dev_t dev;
  
  /* Block size */
  blksize_t blksize;

  /* Block count */
  blkcnt_t blkcount;

  /* Access time */
  time_t atime;

  /* Modify time */

  time_t mtime;
  /* Create time */
  time_t ctime;

  /* Operations for the inode */
  pd_file_opts_t opts;

  /* Inode implementation stuff */
  void* impl;
} pd_inode_t;

/**
 * Open an inode
 *
 * @param[in] inode The inode to open
 * @param[out] file The pointer to store the descriptor
 * @param[in] mode The file mode to open with
 * @return Returns negative errno codes when errors, 0 will be returned on success.
 */
int pd_inode_open(pd_inode_t* inode, pd_file_t* file, int mode);

/**
 * Close an inode
 *
 * @param[in] inode The inode to close
 * @param[out] file The pointer of the descriptor
 * @return Returns netagive errno codes when errors, 0 will be returned on success.
 */
int pd_inode_close(pd_inode_t* inode, pd_file_t* file);

/**
 * Creates an inode
 *
 * @param[out] inode The pointer to store the inode
 * @param[in] name The name of the inode
 * @return Returns negative errno codes when errors, 0 will be returned on success.
 */
int pd_inode_create(pd_inode_t** inode, const char* name);

/**
 * Resolves a child inode from a top parent
 *
 * @param[in] root The root to search in
 * @param[out] inode The pointer to store the child
 * @param[in] path The path to resolve
 * @return Returns negative error codes when errors, 0 will be returned on success.
 */
int pd_inode_resolve_path(pd_inode_t* root, pd_inode_t** inode, const char* path);
