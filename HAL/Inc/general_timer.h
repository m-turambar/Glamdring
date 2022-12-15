//
// Created by migue on 10/06/2020.
//

#ifndef GLAMDRING_GENERAL_TIMER_H
#define GLAMDRING_GENERAL_TIMER_H

#include "helpers.h"
#include "RCC.h"

extern "C"
{

void TIM2_IRQHandler(void);
void TIM15_IRQHandler(void);
void TIM16_IRQHandler(void);
void TIM17_IRQHandler(void);

enum class GeneralTimer : std::size_t {
  TIM2  = 0x40000000,
  TIM15 = 0x40014000,
  TIM16 = 0x40014400,
  TIM17 = 0x40014800
};


class general_timer {
public:

  enum class Mode{
    Periodic=0,
    OnePulseMode=1,
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

  void enable_output_compare(const uint8_t canal) const; //algunos timers tienen más de un canal
  void set_output_compare_microsecond_resolution(uint16_t resolution);
  void set_microseconds_pulse_high(const uint16_t microseconds, const uint8_t canal);

  void set_microsecond_period(uint16_t periodo);
  void configurar_periodo_ms(uint16_t milisegundos);

  void generate_update() const;
  void clear_update() const;
  void enable_input_capture(bool rising_edge, uint16_t microsegundos_por_conteo, uint8_t filtro=0, const uint8_t canal=1) const;

  // atajos, para rápidamente hacer toggle del tipo de flanco que dispara la interrupción, para medir pulsos.
  void set_capture_compare_polarity_rising() const;
  void set_capture_compare_polarity_falling() const;

  void callback_selector(); //ahorra un poco de código
  void (*callback_update)(void) {nullptr}; // el callback normal que se usa en el basic timer
  void (*callback_capture_compare)(void) {nullptr}; // un callback para el evento de CC.
  void (*callback_COM)(void) {nullptr}; // un callback para el evento COM.
  void (*callback_break)(void) {nullptr}; // un callback para el evento break.
  const GeneralTimer peripheral;
  const size_t base;
  registro CR1, CR2, DIER, SR, EGR, CNT, PSC, ARR, CCMR1, CCMR2, CCER, CCR1, CCR2, CCR3, CCR4, BDTR;
  unsigned int output_compare_microsecond_resolution{1};
};

}
#endif //GLAMDRING_GENERAL_TIMER_H
