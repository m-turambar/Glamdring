//
// Created by migue on 11/08/2021.
//

#ifndef GLAMDRING_EXTI_H
#define GLAMDRING_EXTI_H

#include "helpers.h"

namespace EXTI
{
  constexpr size_t base = 0x40021800;
  constexpr registro EXTI_RTSR1 = base;
  constexpr registro EXTI_FTSR1 = base + 0x04;
  constexpr registro EXTI_SWIER1 = base + 0x08;
  constexpr registro EXTI_RPR1 = base + 0x0C;
  constexpr registro EXTI_FPR1 = base + 0x10;
  ///-------------------------------
  constexpr registro EXTI_RTSR2 = base + 0x28;
  constexpr registro EXTI_FTSR2 = base + 0x2C;
  constexpr registro EXTI_SWIER2 = base + 0x30;
  constexpr registro EXTI_RPR2 = base + 0x34;
  constexpr registro EXTI_FPR2 = base + 0x38;
  ///-------------------------------
  constexpr registro EXTI_EXTICR1 = base + 0x60;
  constexpr registro EXTI_EXTICR2 = base + 0x64;
  constexpr registro EXTI_EXTICR3 = base + 0x68;
  constexpr registro EXTI_EXTICR4 = base + 0x6C;
  ///-------------------------------
  constexpr registro EXTI_IMR1 = base + 0x80;
  constexpr registro EXTI_EMR1 = base + 0x84;
  ///-------------------------------
  constexpr registro EXTI_IMR2 = base + 0x90;
  constexpr registro EXTI_EMR2 = base + 0x94;

  void enable_falling_interrupt(const uint8_t pin);
  void clear_pending_interrupt(const uint8_t pin);
}

#endif //GLAMDRING_EXTI_H
