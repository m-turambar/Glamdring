//
// Created by migue on 10/06/2020.
//

#ifndef G070_GENERAL_TIMER_H
#define G070_GENERAL_TIMER_H

#include "helpers.h"
#include "RCC.h"

#undef TIM6
#undef TIM7

extern "C"
{

void TIM15_IRQHandler(void);
void TIM16_IRQHandler(void);
void TIM17_IRQHandler(void);

enum class GeneralTimer : std::size_t {
  TIM15 = 0x40014000,
  TIM16 = 0x40014400,
  TIM17 = 0x40014800
};

class general_timer {
public:

  enum class Mode{
    Periodic=0,
    OnePulseMode=1,
    PWM
  };


  /**
  \brief   Basic Timer constructor
  \details Creates a timer with basic periodic interrupt, or One_Pulse_Mode capabilities.
  \param [in]   tim         BasicTimer peripheral selector
  \param [in]   prescaler   Prescaler value
  \param [in]   autoreload  Reload value for the timer counter
 */
  general_timer(const GeneralTimer tim, const Mode mode, const uint16_t prescaler, const uint16_t autoreload);


  void configure(const Mode mode, const uint8_t auto_reload_preload = 1, const uint8_t update_request_source = 1,
      const uint8_t update_disable = 0, const uint8_t status_bit_remap = 0) const;


  void enable_interrupt(void (*callback_fn)(void),const uint8_t isr_priority = 3);

  void start(void) const;

  void set_prescaler(const uint16_t prescaler) const;

  void set_autoreload(const uint16_t autoreload) const;

  void (*callback)(void) {nullptr};
  const GeneralTimer peripheral;
  const size_t base;
  registro CR1, CR2, DIER, SR, EGR, CNT, PSC, ARR;
};

}
#endif //G070_GENERAL_TIMER_H