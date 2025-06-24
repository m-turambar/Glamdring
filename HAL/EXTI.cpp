//
// Created by migue on 11/08/2021.
//

#include <EXTI.h>

void EXTI::enable_falling_interrupt(const uint8_t pin) {
  const flag FTn(pin);
  EXTI_FTSR1.set(FTn);

  const flag MRx(pin);
  EXTI_IMR1.set(MRx);
}

void EXTI::clear_pending_interrupt(const uint8_t pin) {
  const flag FPIFx(pin);
  EXTI_FPR1.set(FPIFx); /// son write to clear
}

