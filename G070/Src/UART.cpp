//
// Created by migue on 16/06/2020.
//

#include <UART.h>
#include "RCC.h"
#include "GPIO_Port.h"

UART* ptr_UART1{nullptr};
UART* ptr_UART2{nullptr};
UART* ptr_UART3{nullptr};
UART* ptr_UART4{nullptr};

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
    ptr_UART1 = this;
    break;

  case UART::Peripheral::USART2:
    ptr_UART2 = this;
    break;

  case UART::Peripheral::USART3:
    ptr_UART3 = this;
    break;

  case UART::Peripheral::USART4:
    ptr_UART4 = this;
    break;
  }

  enable_clock();
  enable_fifo();
  /** stop bits by default are 1 */
  cfg_word_length(wlen);
  cfg_baud(baud);
  init_gpios();
  enable();
}

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

void UART::enable_fifo() const
{
  const bitfield FIFOEN(1, 29);
  memoria(CR1) |= FIFOEN(1);
}


void UART::cfg_word_length(const UART::WordLength len) const
{
  const bitfield M0(1, 12);
  const bitfield M1(1, 28);
  size_t temp = (memoria(CR1) & !M0 & !M1);
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

void UART::enable() const
{
  const bitfield UE(1,0);
  memoria(CR1) |= UE(1);
}


void UART::transmit(const uint8_t* buffer, size_t sz) const
{
  const bitfield TE(1,3);
  //const bitfield TXE(1,3); //como usamos la queue interna parece no necesitarse
  const bitfield TXFNF(1, 7); /** Reset value = 1 */
  const bitfield TC(1,6);
  const bitfield tdr(9,0);
  memoria(CR1) |= TE(1);

  for(size_t i=0; i<sz; ++i)
  {
    while(TXFNF(ISR) == 0) {} /** esperamos hasta que la queue de tranmision deje de estar llena */
    memoria(TDR) = tdr(buffer[i]);
  }
  while(TC(ISR) != 0) {} /** esperamos hasta que la transmision termine */
}

void UART::receive() const
{
  const bitfield RE(1,2);
  memoria(CR1) |= RE(1);
}

void UART::init_gpios()
{
  if(peripheral == Peripheral::USART2) {
    RCC::enable_port_clock(RCC::GPIO_Port::A);
    GPIO::PORTA.pin_for_UART_or_SPI(2, GPIO::AlternFunct::AF1_USART2);
    GPIO::PORTA.pin_for_UART_or_SPI(3, GPIO::AlternFunct::AF1_USART2);
  }
}



