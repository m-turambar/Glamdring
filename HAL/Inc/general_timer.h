//
// Created by migue on 10/06/2020.
//

#ifndef G070_GENERAL_TIMER_H
#define G070_GENERAL_TIMER_H

#include "helpers.h"
#include "RCC.h"

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

/**
 *    Ejemplo:\n
    general_timer t16(GeneralTimer::TIM16, general_timer::Mode::Periodic, 0x8, 0x100); //each tick is 1ms
    t16.enable_output_compare(0x1);
    t16.start();
 */
class general_timer {
public:

  enum class Mode{
    Periodic=0,
    OnePulseMode=1,
    PWM
  };

  general_timer(const GeneralTimer tim, const Mode mode, const uint16_t prescaler, const uint16_t autoreload);


  void configure(const Mode mode, const uint8_t auto_reload_preload = 1, const uint8_t update_request_source = 1,
      const uint8_t update_disable = 0, const uint8_t status_bit_remap = 0) const;


  void enable_interrupt(void (*callback_fn)(void),const uint8_t isr_priority = 3);

  void start(void) const;

  void set_prescaler(const uint16_t prescaler) const;
  void set_autoreload(const uint16_t autoreload) const;
  void set_ccr1(const uint16_t ccr1) const;

  void enable_output_compare(uint16_t cmp) const;

  void configurar_periodo_us(uint16_t periodo);
  void configurar_periodo_ms(uint16_t periodo);
  void enable_output();


  void (*callback)(void) {nullptr};
  const GeneralTimer peripheral;
  const size_t base;
  registro CR1, CR2, DIER, SR, EGR, CNT, PSC, ARR, CCMR1, CCER, CCR1, BDTR;
};

}
#endif //G070_GENERAL_TIMER_H
