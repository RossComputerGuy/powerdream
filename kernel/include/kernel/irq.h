#pragma once

#include <kernel/types.h>

typedef struct {
  uint32_t r[16];
  uint32_t pc;
  uint32_t pr;
  uint32_t gbr;
  uint32_t vbr;
  uint32_t mach;
  uint32_t macl;
  uint32_t sr;
  uint32_t frbank[16];
  uint32_t fr[16];
  uint32_t fpscr;
  uint32_t fpul;
} pd_irq_context_t;

#define PD_EXC_ILLEGAL_INSTR 0x0180
#define PD_EXC_FPU 0x0120

#define PD_EXC_IRQ0 0x0200

typedef void (*pd_irq_handler_t)(uint32_t irq, pd_irq_context_t* context);

/**
  * Gets the current IRQ CPU register context
  *
  * @return The current register context
  */
pd_irq_context_t* pd_irq_get_context();

/**
  * Installs a handler for an IRQ
  *
  * @param[in] code The IRQ to install a handler for
  * @param[in] handler The function to use for handling the IRQ
  * @return Zero is returned on success or a negative errno code will be returned if it failed
  */
int pd_irq_set_handler(uint32_t code, pd_irq_handler_t handler);

/**
  * Get the IRQ handler for an IRQ
  *
  * @param[in] code The IRQ code
  * @return The IRQ handler for the IRQ, will be NULL if code is invalid or if there's no handler installed
  */
pd_irq_handler_t pd_irq_get_handler(uint32_t code);

/**
 * Initialize the IRQs
 */
void pd_irq_init();
