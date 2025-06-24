//
// Created by migue on 05/06/2020.
//

#ifndef GLAMDRING_BASIC_TIMER_H
#define GLAMDRING_BASIC_TIMER_H

#include "helpers.h"
#include "RCC.h"

class basic_timer;
extern basic_timer* tim6_ptr;
extern basic_timer* tim7_ptr;

#if defined(STM32L475xx) || defined(STM32F767xx)
#define TIM6_IRQHandler(void) TIM6_DAC_IRQHandler(void)
#endif

/** recuerda que las interrupciones deben tener C linkage. Por eso agregar esto aquí en lugar del
 * cpp funciona. */
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

  enum class Mode {
    Periodic=0,
    OnePulseMode=1
  };

  // Usado para el DAC. Usa MasterMode::Update para disparar una actualización del DAC. No necesitas habilitar interrupciones.
  enum class MasterMode {
    Reset,
    Enable,
    Update
  };

  basic_timer(const BasicTimer tim, const Mode mode);


  //void configure(const Mode mode, const uint8_t auto_reload_preload = 1, const uint8_t update_request_source = 1,
  //  const uint8_t update_disable = 0, const uint8_t status_bit_remap = 0) const;

  void configure_mode(const Mode mode);
  void configure_master_mode(const MasterMode& mode);
  void configurar_periodo_us(uint16_t periodo);
  void configurar_periodo_ms(uint16_t periodo);

  /* maybe i should split this in two functions, and request just isr priority and a fn ptr as callback */
  void enable_interrupt(void (*callback_fn)(void),const uint8_t isr_priority = 3);

  void start(void) const;
  void stop(void) const;

  void set_prescaler(const uint16_t prescaler) const;

  void set_autoreload(const uint16_t autoreload) const;

  void generate_update() const;
  void clear_update() const;

  void (*callback)(void) {nullptr};
  const BasicTimer peripheral;
  const size_t base;
  registro CR1, CR2, DIER, SR, EGR, CNT, PSC, ARR;

  void set_cr1_flag(flag f) const;
  void clear_cr1_flag(flag f) const;
};

}

#endif //GLAMDRING_BASIC_TIMER_H
