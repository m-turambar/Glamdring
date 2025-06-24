//
// Created by migue on 05/06/2020.
//

#include "basic_timer.h"
#include "NVIC.h"

basic_timer* tim6_ptr {};
basic_timer* tim7_ptr {};

void TIM6_IRQHandler(void)
{
  tim6_ptr->callback();
  NVIC_ClearPendingIRQ(TIM6_IRQn);
  memoria(tim6_ptr->SR) &= (~(1u)); // cleareamos el update interrupt flag
}

void TIM7_IRQHandler(void)
{
  tim7_ptr->callback();
  NVIC_ClearPendingIRQ(TIM7_IRQn);
  memoria(tim7_ptr->SR) &= (~(1u)); // cleareamos el update interrupt flag
}

namespace CR1_Flags {
    const flag UIFRE_MAP(11);
    const flag ARPE(7);
    const flag OPM(3);
    const flag URS(2);
    const flag UDIS(1);
    const flag CEN(0);
};

basic_timer::basic_timer(const BasicTimer tim, const Mode mode)
    : peripheral(tim)
    , base(static_cast<size_t>(tim))
    , CR1(base)
    , CR2(base+4)
    , DIER(base+0xC)
    , SR(base+0x10)
    , EGR(base+0x14)
    , CNT(base+0x24)
    , PSC(base+0x28)
    , ARR(base+0x2C)
{
  if (peripheral==BasicTimer::TIM6) {
    tim6_ptr = this;
    RCC::enable_TIM6_clock();
  }
  else if (peripheral==BasicTimer::TIM7) {
    tim7_ptr = this;
    RCC::enable_TIM7_clock();
  }

  configure_mode(mode);
}


void basic_timer::set_prescaler(const uint16_t prescaler) const
{
  const bitfield PSC_bf(16,0,prescaler);
  PSC.write(PSC_bf);
}

void basic_timer::set_autoreload(const uint16_t autoreload) const
{
  const bitfield ARR_bf(16, 0, autoreload);
  ARR.write(ARR_bf);
}

void basic_timer::configure_mode(const Mode mode)
{
  if(mode == Mode::OnePulseMode)
    set_cr1_flag(CR1_Flags::OPM);
  else
    clear_cr1_flag(CR1_Flags::OPM);
}

void basic_timer::configure_master_mode(const MasterMode& mode)
{
  bitfield MMS(3, 4, static_cast<size_t>(mode));
  CR2.write(MMS);
}

void basic_timer::enable_interrupt(void (*callback_fn)(void),const uint8_t isr_priority)
{
  callback = callback_fn;

  const IRQn_Type mIRQn = (peripheral==BasicTimer::TIM6 ? TIM6_IRQn :
                          (peripheral==BasicTimer::TIM7 ? TIM7_IRQn : HardFault_IRQn));
  const flag UIE(0);
  DIER.set(UIE);
  NVIC_SetPriority(mIRQn, isr_priority);
  NVIC_EnableIRQ(mIRQn);
}

/* solo cuidado con One Pulse Mode ya que en la primera habilitación ocurre una interrupción */
void basic_timer::start(void) const
{
  const flag CEN(0);
  CR1.set(CEN);
}

void basic_timer::stop(void) const
{
  const flag CEN(0);
  CR1.reset(CEN);
}

/** válido de 1us* a 65ms */
void basic_timer::configurar_periodo_us(uint16_t periodo)
{
  set_prescaler(15); //para que cada "tick" sea de 1us
  set_autoreload(periodo-1);
}

/** válido de 1ms a 65s */
void basic_timer::configurar_periodo_ms(uint16_t periodo)
{
  //memoria(PSC) = 7999; // para que cada "tick" sea de 1ms, divides 8MHz entre 8000
  set_prescaler(15999); // O tal vez, sí es de 16MHz? divides 16MHz entre 16000. PWM y periodic son diferentes? no.
  set_autoreload(periodo-1);
}

void basic_timer::set_cr1_flag(flag f) const {
  CR1.set(f);
}

void basic_timer::clear_cr1_flag(flag f) const {
  CR1.reset(f);
}

void basic_timer::generate_update() const {
  const flag UG(0);
  EGR.set(UG);
}

void basic_timer::clear_update() const {
  const flag UIF(0);
  SR.reset(UIF);
}

