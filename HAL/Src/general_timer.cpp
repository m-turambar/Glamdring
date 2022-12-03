//
// Created by migue on 10/06/2020.
//

#include "general_timer.h"
#include "NVIC.h"

general_timer* tim2_ptr = nullptr;
static general_timer* tim15_ptr = nullptr;
static general_timer* tim16_ptr = nullptr;
static general_timer* tim17_ptr = nullptr;

/** Las flags para habilitar cada interrupción en DIER tienen el mismo offset que para leer su estado en SR.
 * Excepto por la OverCapture interrupt (CC1OF), que no he usado aún. */

static const flag UIF(0); // Update Interrupt
static const flag CC1IF(1); // Capture/Compare Interrupt
static const flag COMIF(5); // COM Interrupt
static const flag BIF(7); // Break Interrupt
// static const flag CC1OF(9); // Break Interrupt

void general_timer::callback_selector() {
  if(SR.is_set(UIF)) {
    SR.reset(UIF);
    if(callback_update != nullptr)
      callback_update();
  }
  if(SR.is_set(CC1IF)) {
    SR.reset(CC1IF);
    if(callback_capture_compare != nullptr)
      callback_capture_compare();
  }
  if(SR.is_set(COMIF)) {
    SR.reset(COMIF);
    if(callback_COM != nullptr)
      callback_COM();
  }
  if(SR.is_set(BIF)) {
    SR.reset(BIF);
    if(callback_break != nullptr)
      callback_break();
  }
}

#ifdef STM32G031xx
void TIM2_IRQHandler(void)
{
  tim2_ptr->callback_selector();
  NVIC_ClearPendingIRQ(TIM2_IRQn);
}
#endif

void TIM15_IRQHandler(void)
{
  tim15_ptr->callback_selector();
  NVIC_ClearPendingIRQ(TIM15_IRQn);
}

void TIM16_IRQHandler(void)
{
  tim16_ptr->callback_selector();
  NVIC_ClearPendingIRQ(TIM16_IRQn);
}

void TIM17_IRQHandler(void)
{
  tim17_ptr->callback_selector();
  NVIC_ClearPendingIRQ(TIM17_IRQn);
}

general_timer::general_timer(const GeneralTimer tim, const Mode mode)
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
    CCMR2(base+0x1C),
    CCER(base+0x20),
    CCR1(base+0x34),
    CCR2(base+0x38),
    CCR3(base+0x3C),
    CCR4(base+0x40),
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
#ifdef STM32G031xx
  else if (peripheral==GeneralTimer::TIM2) {
    tim2_ptr = this;
    RCC::enable_TIM2_clock();
  }
#endif

  configure(mode);
}


void general_timer::set_prescaler(const uint16_t prescaler) const
{
  memoria(PSC) = prescaler;
}

void general_timer::set_autoreload(const uint16_t autoreload) const
{
  memoria(ARR) = autoreload;
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

void general_timer::enable_interrupt(void (*callback_fn)(void), InterruptType it, const uint8_t isr_priority)
{
  switch(it) {
    case InterruptType::UIE:
      callback_update = callback_fn;
      break;
    case InterruptType::CCIE:
      callback_capture_compare = callback_fn;
      break;
    case InterruptType::COMIE:
      callback_COM = callback_fn;
      break;
    case InterruptType::BIE:
      callback_break = callback_fn;
  }

  IRQn_Type mIRQn = HardFault_IRQn;
  switch (peripheral) {
#ifdef STM32G031xx
    case GeneralTimer::TIM2:
      mIRQn = TIM2_IRQn;
      break;//TODO hay que arreglar esto
#endif
    case GeneralTimer::TIM15:
      mIRQn = TIM15_IRQn;
      break;
    case GeneralTimer::TIM16:
      mIRQn = TIM16_IRQn;
      break;
    case GeneralTimer::TIM17:
      mIRQn = TIM17_IRQn;
      break;
  }

  const flag tipo_interr(static_cast<uint8_t>(it)); // Así no duplicamos código. Puede que necesites más callbacks.
  DIER.set(tipo_interr);

  NVIC_SetPriority(mIRQn, isr_priority);
  NVIC_EnableIRQ(mIRQn);
}

/* solo cuidado con One Pulse Mode ya que en la primera habilitación ocurre una interrupción */
void general_timer::start(void) const
{
  memoria(CR1) |= (0x1);
}

void general_timer::enable_output_compare(const uint8_t canal) const
{
  const registro& CCMRx = (canal <= 2 ? CCMR1 : CCMR2);

  const uint8_t offset_canal = ((canal % 2) == 0) ? 8 : 0; // los canales nones no llevan offset
  /** Configuramos el registro CCMRx. Vamos a indicar que es una salida, y que es modo PWM. Hay 7 modos. */
  const bitfield CCxS(2,0 + offset_canal, 0); /** Nos aseguramos de que valga 0, para ser salida */
  const bitfield OCxM(3,4 + offset_canal, 6); /** Valor 6 para este bitfield es PWM modo 1. */
  CCMRx.write(CCxS);
  CCMRx.write(OCxM);

  /** Habilitamos lectura/escritura al Preload Register. Sin esto, el timer no sacaba ninguna señal.
   * Seguramente es porque la escritura a CCR1 no estaba teniendo ningún efecto. */
  const flag OCxPE(3 + offset_canal);
  CCMRx.set(OCxPE);

  /** Experimentalmente, vimos que sí necesitas setear este par de bits. Ahora, si el registro no existe en TIM14,
   * cómo le haces en ese?
   const flag MOE(15);
   const flag BKP(13);
   BDTR.set(MOE);
   BDTR.set(BKP);*/

   /** Experimentalmente vimos que estos bits están igual diferentes: */
   const flag URS(2);
   const flag ARPE(7);
   CR1.reset(URS);
   CR1.reset(ARPE);

  /** Finalmente, encendemos el modo OC. Otras opciones en este registro modifican la polaridad,
   * o nos permiten encender el canal negado. */
  const flag CCxE((canal - 1)*4); // esa pequeña formula es para generalizar a varios canales
  CCER.set(CCxE);
}

void general_timer::set_output_compare_microsecond_resolution(uint16_t resolution) {
  output_compare_microsecond_resolution = resolution;
}

void general_timer::set_microseconds_pulse_high(const uint16_t microseconds, const uint8_t canal) {
  const registro& CCRx =
      (canal == 1 ? CCR1 :
       (canal == 2 ? CCR2 :
        (canal == 3 ? CCR3 :
         (canal == 4 ? CCR4 : CCR1))));
  /** Configuramos el umbral de conteo para PWM. Osea a los cuantos ticks el pulso se va a bajar (en modo 1). */
  memoria(CCRx) = (microseconds / output_compare_microsecond_resolution);
}

/**
 *    Para configurar un timer como Input Capture:
 *    CCER->CCxE = 1    Habilitar captura
 *    CCER->CCxP = 0 o 1    Captura en Rising/Falling edge
 *    Para ver el comportamiento del filtro, consulta el datasheet.
 */
void general_timer::enable_input_capture(bool rising_edge, uint16_t microsegundos_por_conteo, uint8_t filtro, const uint8_t canal) const {
  /** Configuramos el registro CCMR1. Vamos a indicar que es una entrada.
   * Parece que esto debe ocurrir antes de escribir al CCER.
   * De otro modo, CCMR1 no se estaba actualizando. */
  const registro& CCMRx = (canal <= 2 ? CCMR1 : CCMR2);

  const uint8_t offset_canal = ((canal % 2) == 0) ? 8 : 0; // los canales nones no llevan offset
  const bitfield CCxS(2,0 + offset_canal, 1); /** Puede valer 1, 2 o 3. Qué son TI1, TI2 y TRC? */
  CCMRx.write(CCxS);

  const bitfield ICxF(4, 4 + offset_canal, filtro & 0x0F); // el BW AND es para solo tomar los 4 bits menores.
  CCMRx.write(ICxF);

  const flag CCxE((canal - 1)*4);
  CCMRx.set(CCxE);

  const flag CCxP((canal - 1)*4 + 1);
  if(!rising_edge)
    CCER.set(CCxP);
  else
    CCER.reset(CCxP);

  /** Ya sea que queremos medir ancho de pulso o frecuencia, el contador va a hacer overflow.
   * La aritmética en C y C++ hace que al haber overflows o underflows se tome el modulo del tipo de la variable que
   * hizo overflow.
   * Así que restar e.g. 6 - 4999 nos da (-4993) % 2^16 - 1. Esto no es igual al 7 que nos gustaría tener.
   * Por ende hacer el ARR 2^16 - 1 hace que esa resta nos dé el resultado correcto.
   */
  set_autoreload(0xFFFF);

  /** Para obtener un conteo de 1us (actualización de CNT), prescaler debe valer 15. (16 MHz entre 16 es 1 MHz).
   * Para que cada conteo valga 1ms, debe valer 15999.
   * Para que cada conteo valga 1 segundo? no alcanza. */
  set_prescaler(microsegundos_por_conteo*16 - 1);
}


/** Notas 06/Jul/2021 - 27/Jul/2022
 * Para configurar una señal PWM, se usan tres registros:
 * PSC (prescaler)
 * ARR (umbral)
 * CCRx (comparador)
 * */


void general_timer::set_microsecond_period(uint16_t periodo)
{
  set_prescaler((16 * output_compare_microsecond_resolution) - 1);
  set_autoreload((periodo / output_compare_microsecond_resolution) - 1);
}

void general_timer::generate_update() const {
  const flag UG(0);
  EGR.set(UG);
}

void general_timer::clear_update() const {
  const flag UIF(0);
  SR.reset(UIF);
}

void general_timer::set_capture_compare_polarity_rising() const {
  const flag CC1P(1);
  CCER.reset(CC1P);
}

void general_timer::set_capture_compare_polarity_falling() const {
  const flag CC1P(1);
  CCER.set(CC1P);
}

void general_timer::configurar_periodo_ms(uint16_t milisegundos) {
  set_prescaler(15999);
  set_autoreload(milisegundos-1);
}
