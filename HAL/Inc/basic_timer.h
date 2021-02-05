//
// Created by migue on 05/06/2020.
//

#ifndef G070_BASIC_TIMER_H
#define G070_BASIC_TIMER_H

#include "helpers.h"
#include "RCC.h"

#undef TIM6
#undef TIM7

/** recuerda que las interrupciones deben tener C linkage. Por eso agregar esto aquí en lugar del
 * cpp funciona. */
#if defined(STM32L475xx) || defined(STM32F767xx)
#define TIM6_IRQHandler(void) TIM6_DAC_IRQHandler(void)
#endif

extern "C"
{
void TIM6_IRQHandler(void);
void TIM7_IRQHandler(void);

enum class BasicTimer : std::size_t {
  TIM6 = 0x40001000,
  TIM7 = 0x40001400
};

class basic_timer {
public:

  enum class Mode{
    Periodic=0,
    OnePulseMode=1
  };


  /**
  \brief   Basic Timer constructor
  \details Creates a timer with basic periodic interrupt, or One_Pulse_Mode capabilities.
  \param [in]   tim         BasicTimer peripheral selector
  \param [in]   prescaler   Prescaler value
  \param [in]   autoreload  Reload value for the timer counter
 */
  basic_timer(const BasicTimer tim, const Mode mode, const uint16_t prescaler, const uint16_t autoreload);


  void configure(const Mode mode, const uint8_t auto_reload_preload = 1, const uint8_t update_request_source = 1,
      const uint8_t update_disable = 0, const uint8_t status_bit_remap = 0) const;

  /* maybe i should split this in two functions, and request just isr priority and a fn ptr as callback */
  void enable_interrupt(void (*callback_fn)(void),const uint8_t isr_priority = 3);

  void start(void) const;

  void set_prescaler(const uint16_t prescaler) const;

  void set_autoreload(const uint16_t autoreload) const;

  void (*callback)(void) {nullptr};
  const BasicTimer peripheral;
  const size_t base;
  registro CR1, CR2, DIER, SR, EGR, CNT, PSC, ARR;
};

}

#endif //G070_BASIC_TIMER_H
