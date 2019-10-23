#pragma once

#include <kernel/types.h>

struct iovec {
  void* iov_base;
  size_t iov_len;
};
