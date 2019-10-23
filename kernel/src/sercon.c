#include <kernel/error.h>
#include <kernel/irq.h>
#include <kernel/sercon.h>
#include <kernel/types.h>
#include <kernel-config.h>

#define SCIFREG08(x) *((volatile uint8_t*)(x))
#define SCIFREG16(x) *((volatile uint16_t*)(x))
#define SCSMR2 SCIFREG16(0xffe80000)
#define SCBRR2 SCIFREG08(0xffe80004)
#define SCSCR2 SCIFREG16(0xffe80008)
#define SCFTDR2 SCIFREG08(0xffe8000C)
#define SCFSR2 SCIFREG16(0xffe80010)
#define SCFRDR2 SCIFREG08(0xffe80014)
#define SCFCR2 SCIFREG16(0xffe80018)
#define SCFDR2 SCIFREG16(0xffe8001C)
#define SCSPTR2 SCIFREG16(0xffe80020)
#define SCLSR2 SCIFREG16(0xffe80024)

static int serial_baud = PD_SERIAL_BAUD;
static int serial_fifo = PD_SERIAL_FIFO;
static int serinited = 0;

#define BUFFSIZE 1024
static uint8_t recvbuff[BUFFSIZE];
static int rb_head = 0;
static int rb_tail = 0;
static int rb_cnt = 0;
static int rb_paused = 0;

int pd_sercon_putc(const char c) {
  if (!serinited) return -EIO;

  int timeout = 800000;
  while (!(SCFSR2 & 0x20) && timeout > 0) timeout--;

  if (timeout <= 0) {
    serinited = 0;
    return -EIO;
  }

  SCFTDR2 = c;
  SCFSR2 &= 0xFF9F;
  return 1;
}

int pd_sercon_flush() {
  if (!serinited) return -EIO;

  int timeout = 800000;
  while (!(SCFSR2 & 0x20) && timeout > 0) timeout--;

  if (timeout <= 0) {
    serinited = 0;
    return -EIO;
  }

  SCFSR2 &= 0xBF;
  return 0;
}

void pd_sercon_init() {}
