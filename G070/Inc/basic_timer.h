//
// Created by migue on 05/06/2020.
//

#ifndef G070_BASIC_TIMER_H
#define G070_BASIC_TIMER_H

#include "helpers.h"
#include "RCC.h"

#undef TIM6
#undef TIM7

extern "C"
{

void TIM6_IRQHandler(void);

enum class BasicTimer : std::size_t {
  TIM6 = 0x40001000,
  TIM7 = 0x40001400
};

class basic_timer {
public:
  basic_timer(BasicTimer tim, const uint16_t prescaler, const uint16_t autoreload)
      :
      base(static_cast<size_t>(tim)),
      CR1(base),
      CR2(base+4),
      DIER(base+0xC),
      SR(base+0x10),
      EGR(base+0x14),
      CNT(base+0x24),
      PSC(base+0x28),
      ARR(base+0x2C)
  {
    /* Habilitamos los relojes de los periféricos */
    if (tim==BasicTimer::TIM6) {
      bitfield TIM6EN(1, 4);
      memoria(RCC::APBENR1) |= TIM6EN(1);
    }
    else if (tim==BasicTimer::TIM7) {
      bitfield TIM7EN(1, 5);
      memoria(RCC::APBENR1) |= TIM7EN(1);
    }

    configure();
    set_autoreload(autoreload);
    set_prescaler(prescaler);
  }

  void
  configure(const uint8_t auto_reload_preload = 1, const uint8_t one_pulse = 0, const uint8_t update_request_source = 1,
      const uint8_t update_disable = 0, const uint8_t status_bit_remap = 0) const;

  void configure_interrupts(const uint8_t isr_enable = 0, const uint8_t dma_enable = 0);

  void enable(void) const;

  void set_prescaler(const uint16_t prescaler) const;

  void set_autoreload(const uint16_t autoreload) const;

  const size_t base;
  registro CR1, CR2, DIER, SR, EGR, CNT, PSC, ARR;
};

}

#endif //G070_BASIC_TIMER_H
