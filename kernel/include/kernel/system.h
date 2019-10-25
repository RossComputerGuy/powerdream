#pragma once

#include <sys/utsname.h>

#define _SC_CHILD_MAX 1
#define _SC_OPEN_MAX 2
#define _SC_PAGESIZE 3

/**
 * Gets the system configuration value
 *
 * @param[in] name The system configuration to get
 * @return The value of the system configuration
 */
long sysconf(int name);

/**
  * Get the system name and information
  *
  * @param[out] buff The pointer to store the system name and information
  * @return A negative errno number when an error occurs or 0 on success.
  */
int uname(struct utsname* buff);
