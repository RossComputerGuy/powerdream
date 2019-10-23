#pragma once

/**
  * Prints a character to the serial console
  *
  * @param[in] c The character to print
  * @return A negative errno code on failure or the number of characters printed
  */
int pd_sercon_putc(const char c);

/**
  * Flushes the serial console buffer
  *
  * @return Zero on success or a negative errno code on failure
  */
int pd_sercon_flush();

/**
  * Initialize the serial console
  */
void pd_sercon_init();