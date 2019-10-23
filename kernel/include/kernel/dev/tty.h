#pragma once

#include <kernel/limits.h>
#include <kernel/list.h>
#include <kernel/types.h>

#define PD_TTY_HASBAUD (1 << 0)

struct winsize {
	uint16_t ws_row;
	uint16_t ws_col;
	uint16_t ws_xpixel;
  uint16_t ws_ypixel;
};

struct termios {
	uint8_t iflag;
	uint8_t oflag;
	uint8_t cflag;
	uint8_t lflag;
	unsigned char line;
	unsigned char cc[19];
};

typedef struct pd_tty {
  PD_SLIST_ENTRY(struct pd_tty) t_list;

  uint8_t flags;
  uint16_t baud;

  const char name[NAME_MAX];

  int (*getwinsize)(struct pd_tty* tty, uint16_t* row, uint16_t* col, uint16_t* x, uint16_t* y);
  int (*setwinsize)(struct pd_tty* tty, uint16_t row, uint16_t col, uint16_t x, uint16_t y);
  size_t (*write)(struct pd_tty* tty, const void* buffer, size_t length);
  size_t (*read)(struct pd_tty* tty, void* buffer, size_t length);
} pd_tty_t;

/**
  * Gets a tty from its name
  *
  * @param[in] name The tty name
  * @return A tty if found, NULL if not found
  */
pd_tty_t* pd_tty_fromname(const char* name);

/**
  * Register a tty
  *
  * @param[in] tty The tty to register
  * @return A negative errno code on failure or zero on success.
  */
int pd_tty_register(pd_tty_t* tty);