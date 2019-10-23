#pragma once

struct utsname {
  char sysname[128];
  char nodename[128];
  char release[128];
  char machine[128];
};

/**
  * Get the system name and information
  *
  * @param[out] buff The pointer to store the system name and information
  * @return A negative errno number when an error occurs or 0 on success.
  */
int uname(struct utsname* buff);
