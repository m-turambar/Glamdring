//
// Created by migue on 16/06/2020.
//

#include <UART.h>
#include "RCC.h"
#include "GPIO_Port.h"
#include "NVIC.h"
#include <cstring>

/** Banderas que se usan mucho en las rutinas de comunicacion. Declararlas en el stack una y otra
 * vez no tiene mucho sentido, así que las hacemos globales */
constexpr static flag RXNE(5);
constexpr static flag TE(3);
constexpr static flag TXFNF(7); /** Reset value = 1. Exactamente el mismo bit con o sin FIFO:TXE*/
constexpr static flag TC(6);
/** No queue */
const flag TXE(3);

/** sospechoso */
UART* UART1_ptr{nullptr};
UART* UART2_ptr{nullptr};
UART* UART3_ptr{nullptr};
UART* UART4_ptr{nullptr};

/** Rutinas de serviceo a interrupciones. Igual. Es aplicación. */
void USART1_IRQHandler(void)
{
  auto& UART1 = *UART1_ptr;
  if(UART1.ISR.is_set(RXNE)) {
    const uint8_t b = UART1.read_byte();
    UART1.callback_rx(b);
  }
  NVIC_ClearPendingIRQ(USART1_IRQn);
}

/** Por ahora no hagas la diferencia */
/** Esto es código de aplicación en el driver. Está mal. Cambiarlo. O moverlo a código de aplicación. */
void USART3_4_IRQHandler(void)
{
  auto& UART3 = *UART3_ptr;
  auto& UART4 = *UART4_ptr;
  if(UART3.ISR.is_set(RXNE)) {
    const uint8_t b = UART3.read_byte();
    /** El problema de esto es que ¿cómo le paso una referencia al UART2 al callback?
     * Tienes que hacer el UART2 global, para que no sea un argumento del callback.
     * Ya hay que encontrar una solución a este patrón recurrente. */
    //UART3.callback_rx(b);
    *UART2_ptr << b; //vaya pero qué necedad
  }
  else if(UART4.ISR.is_set(RXNE)) {

  }
  NVIC_ClearPendingIRQ(USART3_4_IRQn);
}

UART::UART(const UART::Peripheral peripheral, size_t baud_arg, WordLength wlen):
    peripheral(peripheral),
    base(static_cast<size_t>(peripheral)),
    baud(baud_arg),
    CR1(base),
    CR2(base + 0x4),
    CR3(base + 0x8),
    BRR(base + 0xC),
    GTPR(base + 0x10),
    RTOT(base + 0x14),
    RQR(base + 0x18),
    ISR(base + 0x1C),
    ICR(base + 0x20),
    RDR(base + 0x24),
    TDR(base + 0x28),
    PRESC(base + 0x2C)
{
  switch (peripheral) {
  case UART::Peripheral::USART1:
    UART1_ptr = this;
    break;

  case UART::Peripheral::USART2:
    UART2_ptr = this;
    break;

  case UART::Peripheral::USART3:
    UART3_ptr = this;
    break;

  case UART::Peripheral::USART4:
    UART4_ptr = this;
    break;
  }

  enable_clock();
  cfg_word_length(wlen);
  cfg_baud(baud);
  /** stop bits by default are 1 */
  init_gpios();
}

/** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** */
/** ** ** ** ** ** ** ** ** ** ** ENABLERS ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** */
/** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** */

/** Nota
 * Es preferible delegarle los detalles de la implementación al RCC porque éste último puede cambiar de micro a micro
 * y nuestro driver permanecer relativamente constante. Así hacemos una apuesta hacia mayor portabilidad. */
void UART::enable_clock() const
{
  switch (peripheral) {
  case UART::Peripheral::USART1:
    RCC::enable_USART1_clock();
    break;

  case UART::Peripheral::USART2:
    RCC::enable_USART2_clock();
    break;

  case UART::Peripheral::USART3:
    RCC::enable_USART3_clock();
    break;

  case UART::Peripheral::USART4:
    RCC::enable_USART4_clock();
    break;
  }
}

const UART& UART::enable_fifo() const
{
  flag FIFOEN(29);
  CR1.set(FIFOEN);
  return *this;
}

void UART::enable() const
{
  flag UE(0);
  CR1.set(UE);

  /** Habilitamos la recepción de datos */
  flag RE(2);
  CR1.set(RE);
}

const UART& UART::enable_interrupt_rx(void (*callback_arg)(const uint8_t byte), uint8_t priority)
{
  callback_rx = callback_arg;
  flag RXNEIE(5);
  CR1.set(RXNEIE);
  const IRQn_Type mIRQn = (peripheral==Peripheral::USART1 ? USART1_IRQn :
                           (peripheral==Peripheral::USART2 ? USART2_IRQn:
                           (peripheral==Peripheral::USART3 ? USART3_4_IRQn:
                           (peripheral==Peripheral::USART4 ? USART3_4_IRQn: HardFault_IRQn))));

  NVIC_SetPriority(mIRQn, priority);
  NVIC_EnableIRQ(mIRQn);
  return *this;
}

/** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** */
/** ** ** ** ** ** ** ** ** ** ** CONFIGURATORS  ** ** ** ** ** ** ** ** ** ** ** ** ** ** */
/** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** */

void UART::cfg_word_length(const UART::WordLength len) const
{
  constexpr bitfield M0(1, 12);
  constexpr bitfield M1(1, 28);
  size_t temp = (memoria(CR1) & !M0 & !M1); /** súper críptico. Claramente hay que mejorar la API. */
  temp = temp | M0(0x01 & static_cast<uint8_t>(len)) | M1(0x10 & static_cast<uint8_t>(len));
  memoria(CR1) |= temp;
}

void UART::cfg_stop_bits(const UART::StopBits bits) const
{
  const bitfield STOP(2, 12);
  memoria(CR3) |= STOP(static_cast<size_t>(bits));
}

void UART::cfg_baud(size_t baud) const
{
  /** Hasta que no hayamos programado el RCC: */
  const size_t freq = 16000000u;
  const bitfield brr(16, 0);
  memoria(BRR) |= brr(freq/baud);
}

void UART::cfg_parity(const UART::Parity parity) const
{

}

void UART::init_gpios()
{
#if defined(STM32G070xx)
  if(peripheral == Peripheral::USART2) {
    RCC::enable_port_clock(RCC::GPIO_Port::A);
    GPIO::PORTA.pin_for_UART(2, GPIO::AlternFunct::AF1);
    GPIO::PORTA.pin_for_UART(3, GPIO::AlternFunct::AF1);
  }
  else if(peripheral == Peripheral::USART3) {
    /** No es la única configuración posible, también es viable en puerto C 4,11 con otro AF val */
    RCC::enable_port_clock(RCC::GPIO_Port::D);
    GPIO::PORTD.pin_for_UART(8, GPIO::AlternFunct::AF0); // TX
    GPIO::PORTD.pin_for_UART(9, GPIO::AlternFunct::AF0); // RX
  }

#elif defined(STM32F767xx)
  if(peripheral == Peripheral::USART3) {
    RCC::enable_port_clock(RCC::GPIO_Port::D);
    GPIO::PORTD.pin_for_UART(8, GPIO::AlternFunct::AF7); // TX
    GPIO::PORTD.pin_for_UART(9, GPIO::AlternFunct::AF7); // RX
  }
#endif

}
/** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** */
/** ** ** ** ** ** ** ** ** ** ** COMMS RX TX ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** */
/** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** */

void UART::receiveq(uint8_t* buffer, size_t sz) const
{
  constexpr flag RXFNE(5);

  for(size_t i=0; i<sz; ++i)
  {
    while(ISR.is_reset(RXFNE)) {}
    buffer[i] = memoria(RDR);
  }
}

bool UART::available() const
{
  return ISR.is_set(RXNE);
}

uint8_t UART::read_byte() const
{
  while(ISR.is_reset(RXNE)) { }
  return memoria(RDR);
}

void UART::transmitq(const uint8_t* buffer, size_t sz) const
{
  bitfield tdr(9,0);
  CR1.set(TE);

  for(size_t i=0; i<sz; ++i)
  {
    while(ISR.is_reset(TXFNF)) {} /** esperamos hasta que la queue de tranmision deje de estar llena */
    memoria(TDR) = tdr(buffer[i]);
  }
  while(ISR.is_reset(TC)) {} /** esperamos hasta que la transmision termine */
}

void UART::write_byte(uint8_t b) const
{
  CR1.set(TE);
  while(ISR.is_reset(TXFNF)) {} /** Es lo mismo que TXE. Es el mismo bit en el mismo registro */
  memoria(TDR) = b;
  while(ISR.is_reset(TC)) {} /** esperamos hasta que la transmision termine */
}

const UART& UART::operator<<(uint8_t b) const
{
  write_byte(b);
  return *this;
}

/** No estoy 100% feliz con esta implementación. Duplica mucho código. Sacrifico este espacio por la conveniencia
 * de usar char* como tipo base en esta operación, vs unsigned char *. Probablemente el segundo es más seguro pero
 * el tiempo y las pruebas confirmarán. */
const UART& UART::operator<<(const char* buffer) const
{
  const size_t sz = std::strlen(buffer);
  bitfield tdr(9,0);
  CR1.set(TE);

  for(size_t i=0; i<sz; ++i)
  {
    while(ISR.is_reset(TXFNF)) {} /** esperamos hasta que la queue de tranmision deje de estar llena */
    memoria(TDR) = tdr(buffer[i]);
  }
  while(ISR.is_reset(TC)) {} /** esperamos hasta que la transmision termine */
  return *this;
}


