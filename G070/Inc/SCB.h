//
// Created by migue on 06/06/2020.
//

#ifndef G070_SCB_H
#define G070_SCB_H

#include <helpers.h>

#undef SCB

/* El SCB es un periférico del Cortex-M0+, osea, cualquier fabricante que licencia este procesador lo usa. */
namespace SCB {
  constexpr size_t base = 0xE000ED00;

  const registro CPUID(base + 0), ICSR(base + 4), VTOR(base + 8), AIRCR(base + 0xC),
                SCR(base + 0x10), CCR(base + 0x14), SHPR2(base + 0x1C), SHPR3(base + 0x20);
};

#endif //G070_SCB_H
