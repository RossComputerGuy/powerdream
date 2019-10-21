#pragma once

#include <kernel/file.h>
#include <sys/types.h>

#define MKDEV(a, b) ((a << 8) | (b))

/**
 * PowerDream Device
 */
typedef struct {
  dev_t dev;
  pd_file_opts_t opts;
} pd_dev_t;
