#pragma once

#include <sys/utsname.h>

#define PD_SC_CHILD_MAX 1
#define PD_SC_OPEN_MAX 2
#define PD_SC_PAGESIZE 3

/**
  * Set the command line arguments for the kernel
  *
  * @param[in] str The string to set as the command line arguments
  */
void pd_setcmdline(char* str);

/**
  * Checks if the cmdline has a specific argument
  *
  * @param[in] arg The argument name
  * @return One if found, 0 if not found
  */
int pd_getcmdline_hasarg(const char* arg);

/**
  * Gets the value of an argument in the command line arguments
  *
  * @param[in] arg The argument name
  * @return The argument value or NULL if not found
  */
char* pd_getcmdline_arg(const char* arg);

/**
  * Get the command line arguments for the kernel
  *
  * @return The command line arguments
  */
char* pd_getcmdline();

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
