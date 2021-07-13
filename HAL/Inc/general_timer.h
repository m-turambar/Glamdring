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


class general_timer {
public:

  enum class Mode{
    Periodic=0,
    OnePulseMode=1,
    PWM,
    InputCapture
  };

  enum class InterruptType {
    UIE = 0,
    CCIE = 1,
    COMIE = 5,
    BIE = 7
  };

  general_timer(const GeneralTimer tim, const Mode mode);


  void configure(const Mode mode, const uint8_t auto_reload_preload = 1, const uint8_t update_request_source = 1,
      const uint8_t update_disable = 0, const uint8_t status_bit_remap = 0) const;


  void enable_interrupt(void (*callback_fn)(void), InterruptType it, const uint8_t isr_priority = 3);

  void start(void) const;

  void set_prescaler(const uint16_t prescaler) const;
  void set_autoreload(const uint16_t autoreload) const;
  void set_ccr1(const uint16_t ccr1) const;

  void enable_output_compare(uint16_t cmp) const;

  void configurar_periodo_us(uint16_t periodo);
  void configurar_periodo_ms(uint16_t periodo);
  void enable_output();

  void generate_update() const;
  void clear_update() const;
  void enable_input_capture(bool rising_edge) const;

  void callback_selector(); //ahorra un poco de código
  void (*callback_update)(void) {nullptr}; // el callback normal que se usa en el basic timer
  void (*callback_capture_compare)(void) {nullptr}; // un callback para el evento de CC.
  void (*callback_COM)(void) {nullptr}; // un callback para el evento COM.
  void (*callback_break)(void) {nullptr}; // un callback para el evento break.
  const GeneralTimer peripheral;
  const size_t base;
  registro CR1, CR2, DIER, SR, EGR, CNT, PSC, ARR, CCMR1, CCER, CCR1, BDTR;
};

}
#endif //G070_GENERAL_TIMER_H
