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

pd_irq_context_t* pd_irq_get_context();
void pd_irq_set_handler(uint32_t irq, void (*handler)(uint32_t irq, pd_irq_context_t* context));
