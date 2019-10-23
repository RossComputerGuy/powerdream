#include <kernel/error.h>
#include <kernel/irq.h>

static pd_irq_handler_t irq_handlers[0x100];
static pd_irq_handler_t trapa_handlers[0x100];
static pd_irq_handler_t irq_hnd_global;

static pd_irq_context_t irq_context_default;

int pd_irq_set_handler(uint32_t code, pd_irq_handler_t handler) {
  if (code >= 0x1000 && (code & 0x000F)) return -EINVAL;
  irq_handlers[code >> 4] = handler;
  return 0;
}

pd_irq_handler_t pd_irq_get_handler(uint32_t code) {
  if (code >= 0x1000 && (code & 0x000F)) return NULL;
  return irq_handlers[code >> 4];
}

int pd_trapa_set_handler(uint32_t code, pd_irq_handler_t handler) {
  if (code > 0xFF) return -EINVAL;
  trapa_handlers[code] = handler;
  return 0;
}