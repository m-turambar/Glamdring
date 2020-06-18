//
// Created by migue on 05/06/2020.
//

#include "basic_timer.h"
#include "NVIC.h"

const basic_timer* tim6_ptr = nullptr;
const basic_timer* tim7_ptr = nullptr;

void TIM6_IRQHandler(void)
{
  tim6_ptr->callback();
  NVIC_ClearPendingIRQ(TIM6_IRQn);
  memoria(tim6_ptr->SR) &= (~(1u)); //clear timer's status register
}

void TIM7_IRQHandler(void)
{
  tim7_ptr->callback();
  NVIC_ClearPendingIRQ(TIM7_IRQn);
  memoria(tim7_ptr->SR) &= (~(1u)); //clear timer's status register
}

basic_timer::basic_timer(const BasicTimer tim, const Mode mode, const uint16_t prescaler, const uint16_t autoreload)
    :
    peripheral(tim),
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
  /* Habilitamos los relojes de los periféricos y configuramos los ptrs para las interrupciones */
  if (peripheral==BasicTimer::TIM6) {
    tim6_ptr = this;
    const bitfield TIM6EN(1, 4);
    memoria(RCC::APBENR1) |= TIM6EN(1);
  }
  else if (peripheral==BasicTimer::TIM7) {
    tim7_ptr = this;
    const bitfield TIM7EN(1, 5);
    memoria(RCC::APBENR1) |= TIM7EN(1);
  }

  configure(mode);
  set_autoreload(autoreload);
  set_prescaler(prescaler);
}


void basic_timer::set_prescaler(const uint16_t prescaler) const
{
  memoria(PSC) = prescaler;
}

void basic_timer::set_autoreload(const uint16_t autoreload) const
{
  memoria(ARR) = autoreload;
}


void basic_timer::configure(const Mode mode, uint8_t auto_reload_preload, uint8_t update_request_source,
    const uint8_t update_disable, const uint8_t status_bit_remap) const
{
  const bitfield UIFRE_MAP(1, 11);
  const bitfield ARPE(1, 7);
  const bitfield OPM(1, 3);
  const bitfield URS(1, 2);
  const bitfield UDIS(1, 1);
  const bitfield CEN(1, 0);
  size_t cr1 = UIFRE_MAP(status_bit_remap) | ARPE(auto_reload_preload) | OPM(static_cast<uint8_t>(mode)) |
      URS(update_request_source) | UDIS(update_disable) | CEN(0);
  memoria(CR1) |= cr1;
  //memoria(CNT) = 0; //this causes a hardfault for some reason
}

void basic_timer::enable_interrupt(void (*callback_fn)(void),const uint8_t isr_priority)
{
  callback = callback_fn;

  const IRQn_Type mIRQn = (peripheral==BasicTimer::TIM6 ? TIM6_IRQn :
                          (peripheral==BasicTimer::TIM7 ? TIM7_IRQn : HardFault_IRQn));
  const bitfield UIE(1,0);
  memoria(DIER) |= UIE(1);
  NVIC_SetPriority(mIRQn, isr_priority);
  NVIC_EnableIRQ(mIRQn);
}

/* solo cuidado con One Pulse Mode ya que en la primera habilitación ocurre una interrupción */
void basic_timer::start(void) const
{
  // no funciona para reiniciar el reloj
  //const bitfield lwr16(16, 0);
  //const bitfield upr16(16, 16);
  //memoria(CNT) = lwr16(ARR) | upr16(CNT);
  // tempoco funciona
  //memoria(EGR) |= 1;

  memoria(CR1) |= (0x1);
}
