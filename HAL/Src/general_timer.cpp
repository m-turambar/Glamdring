//
// Created by migue on 10/06/2020.
//

#include "general_timer.h"
#include "NVIC.h"

const general_timer* tim15_ptr = nullptr;
const general_timer* tim16_ptr = nullptr;
const general_timer* tim17_ptr = nullptr;

void TIM15_IRQHandler(void)
{
  tim15_ptr->callback();
  NVIC_ClearPendingIRQ(TIM6_IRQn);
  memoria(tim15_ptr->SR) &= (~(1u)); //clear timer's status register
  
}void TIM16_IRQHandler(void)
{
  tim16_ptr->callback();
  NVIC_ClearPendingIRQ(TIM6_IRQn);
  memoria(tim16_ptr->SR) &= (~(1u)); //clear timer's status register
}

void TIM17_IRQHandler(void)
{
  tim17_ptr->callback();
  NVIC_ClearPendingIRQ(TIM7_IRQn);
  memoria(tim17_ptr->SR) &= (~(1u)); //clear timer's status register
}

general_timer::general_timer(const GeneralTimer tim, const Mode mode, const uint16_t prescaler, const uint16_t autoreload)
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
    ARR(base+0x2C),
    /*** general timer ***/
    CCMR1(base+0x18),
    CCER(base+0x20),
    CCR1(base+0x34),
    BDTR(base+0x44)
{
  /* Habilitamos los relojes de los periféricos y configuramos los ptrs para las interrupciones */
  /** TODO: move this to RCC */
  if (peripheral==GeneralTimer::TIM15) {
    tim15_ptr = this;
    RCC::enable_TIM15_clock();
  }
  else if (peripheral==GeneralTimer::TIM16) {
    tim16_ptr = this;
    RCC::enable_TIM16_clock();
  }
  else if (peripheral==GeneralTimer::TIM17) {
    tim17_ptr = this;
    RCC::enable_TIM17_clock();
  }

  configure(mode);
  set_autoreload(autoreload);
  set_prescaler(prescaler);
}


void general_timer::set_prescaler(const uint16_t prescaler) const
{
  memoria(PSC) = prescaler;
}

void general_timer::set_autoreload(const uint16_t autoreload) const
{
  memoria(ARR) = autoreload;
}

void general_timer::set_ccr1(const uint16_t ccr1) const
{
  memoria(CCR1) = ccr1;
}


void general_timer::configure(const Mode mode, uint8_t auto_reload_preload, uint8_t update_request_source,
    const uint8_t update_disable, const uint8_t status_bit_remap) const
{
  bitfield UIFRE_MAP(1, 11);
  bitfield ARPE(1, 7);
  bitfield OPM(1, 3);
  bitfield URS(1, 2);
  bitfield UDIS(1, 1);
  bitfield CEN(1, 0);
  size_t cr1 = UIFRE_MAP(status_bit_remap) | ARPE(auto_reload_preload) | OPM(static_cast<uint8_t>(mode)) | URS(update_request_source) |
      UDIS(update_disable) | CEN(0);
  memoria(CR1) |= cr1;
}

void general_timer::enable_interrupt(void (*callback_fn)(void),const uint8_t isr_priority)
{
  callback = callback_fn;
  const IRQn_Type mIRQn =
        (peripheral==GeneralTimer::TIM15 ? TIM15_IRQn :
        (peripheral==GeneralTimer::TIM16 ? TIM16_IRQn :
        (peripheral==GeneralTimer::TIM17 ? TIM17_IRQn : HardFault_IRQn)));

  flag UIE(0);
  DIER.set(UIE);

  NVIC_SetPriority(mIRQn, isr_priority);
  NVIC_EnableIRQ(mIRQn);
}

/* solo cuidado con One Pulse Mode ya que en la primera habilitación ocurre una interrupción */
void general_timer::start(void) const
{
  memoria(CR1) |= (0x1);
}

/** Esto lo revistaste comparandolo con la HAL de ST.
 * Al final todo era un problema de GPIOs */
void general_timer::enable_output_compare(uint16_t cmp) const
{
  /** Configuramos el registro CCMR1. Vamos a indicar que es una salida, y que es modo PWM. Hay 7 modos. */
  const bitfield CC1S(2,0, 0); /** Nos aseguramos de que valga 0, para ser salida */
  const bitfield OC1M(3,4, 6); /** Valor 6 para este bitfield es PWM modo 1. */
  CCMR1.write(CC1S);
  CCMR1.write(OC1M);

  /** Habilitamos lectura/escritura al Preload Register. Sin esto, el timer no sacaba ninguna señal.
   * Seguramente es porque la escritura a CCR1 no estaba teniendo ningún efecto. */
  const flag OC1PE(3);
  CCMR1.set(OC1PE);

  /** Configuramos el umbral de conteo para PWM. Osea a los cuantos ticks el pulso se va a bajar (en modo 1). */
  memoria(CCR1) = cmp;

  /** Experimentalmente, vimos que sí necesitas setear este par de bits. Ahora, si el registro no existe en TIM14,
   * cómo le haces en ese? */
   const flag MOE(15);
   const flag BKP(13);
   BDTR.set(MOE);
   BDTR.set(BKP);

   /** Experimentalmente vimos que estos bits están igual diferentes: */
   const flag URS(2);
   const flag ARPE(7);
   CR1.reset(URS);
   CR1.reset(ARPE);

  /** Finalmente, encendemos el modo OC. Otras opciones en este registro modifican la polaridad,
   * o nos permiten encender el canal negado. */
  const flag CC1E(0);
  CCER.set(CC1E);
}

/** Notas 06/Jul/2021
 * Para configurar una señal PWM, se usan tres registros:
 * PSC (prescaler)
 * ARR (umbral)
 * CCR1 (comparador)
 * El código de parser que se encuentra en pruebas_timers.cpp incluye un parser de UART para experimentar con
 * estos valores. Permite un control fino sobre la frecuencia, el tiempo arriba, y el tiempo abajo de cada timer.
 * Falta crear un algoritmo correcto para establecer periodos fijos*/

/** válido de 1us* a 65ms */
void general_timer::configurar_periodo_us(uint16_t periodo)
{
  memoria(PSC) = 0x1; // 16, para que cada "tick" sea de 1us. 16 qué?
  memoria(ARR) = periodo-1;
}

/** válido de 1ms a 65s */
void general_timer::configurar_periodo_ms(uint16_t periodo)
{
  memoria(PSC) = 0x3E80; // 16000, para que cada "tick" sea de 1ms
  memoria(ARR) = periodo-1;
}

void general_timer::enable_output()
{
  constexpr flag CC1E(0);
  CCER.set(CC1E);
}