#pragma once

#define printk(fmt, ...) pd_klog_printf(__FILE__": "fmt, __VA_ARGS__)

/**
 * Prints a message the kernel log
 *
 * @param[in] format The text used for formatting
 */
int pd_klog_printf(const char* format, ...);
