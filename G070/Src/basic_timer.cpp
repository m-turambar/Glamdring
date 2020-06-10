//
// Created by migue on 05/06/2020.
//

#include "basic_timer.h"
#include "gpio.h"

const basic_timer* tim6_ptr = nullptr;

void TIM6_IRQHandler(void)
{
  tim6_ptr->callback();
  NVIC_ClearPendingIRQ(TIM6_IRQn);
  memoria(tim6_ptr->SR) &= (~(1u)); //clear timer's status register
}

void basic_timer::set_prescaler(const uint16_t prescaler) const
{
  memoria(PSC) = prescaler;
}

void basic_timer::set_autoreload(const uint16_t autoreload) const
{
  memoria(ARR) = autoreload;
}


void basic_timer::configure(uint8_t auto_reload_preload, uint8_t one_pulse, uint8_t update_request_source,
    const uint8_t update_disable, const uint8_t status_bit_remap) const
{
  bitfield UIFRE_MAP(1, 11);
  bitfield ARPE(1, 7);
  bitfield OPM(1, 3);
  bitfield URS(1, 2);
  bitfield UDIS(1, 1);
  bitfield CEN(1, 0);
  size_t cr1 = UIFRE_MAP(status_bit_remap) | ARPE(auto_reload_preload) | OPM(one_pulse) | URS(update_request_source) |
      UDIS(update_disable) | CEN(0);
  memoria(CR1) |= cr1;
}

void basic_timer::configure_interrupts(uint8_t isr_enable, uint8_t dma_enable)
{
  bitfield UIE(1,0);
  bitfield UDE(1,8);
  memoria(DIER) = UIE(isr_enable) | UDE(dma_enable);
  if(isr_enable > 0)
  {
    NVIC_SetPriority(TIM6_IRQn, 3);
    NVIC_EnableIRQ(TIM6_IRQn);
  }
}

void basic_timer::enable(void) const
{
  tim6_ptr = this;
  memoria(CR1) |= 0x1;
}
