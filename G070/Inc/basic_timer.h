//
// Created by migue on 05/06/2020.
//

#ifndef G070_BASIC_TIMER_H
#define G070_BASIC_TIMER_H

#include "helpers.h"

enum class BasicTimer : std::size_t {
  TIM6=0x40001000,
  TIM7=0x40001400
};

class basic_timer {
public:
  basic_timer(BasicTimer tim):
  base(static_cast<size_t>(tim)),
  CR1(base),
  CR2(base+4),
  DIER(base+0xC),
  SR(base+0x10),
  EGR(base+0x14),
  CNT(base+0x24),
  PSC(base+0x28),
  ARR(base+0x2C)
  { }

  void set_prescaler(const uint16_t prescaler);
  void set_autoreload(const uint16_t autoreload);

  const size_t base;
  registro CR1, CR2, DIER, SR, EGR, CNT, PSC, ARR;
};

#endif //G070_BASIC_TIMER_H
