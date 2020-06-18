//
// Created by migue on 16/06/2020.
//

#include <UART.h>
#include "RCC.h"

void UART::set_word_length(const UART::WordLength len) const
{
  const bitfield M0(1, 12);
  const bitfield M1(1, 28);
  size_t temp = (memoria(CR1) & !M0 & !M1);
  temp = temp | M0(0x01 & static_cast<uint8_t>(len)) | M1(0x10 & static_cast<uint8_t>(len));
  memoria(CR1) |= temp;
}

UART::UART(const UART::Peripheral peripheral):
    peripheral(peripheral),
    base(static_cast<size_t>(peripheral)),
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
  enable_clock();
}

/** Nota
 * Es preferible delegarle los detalles de la implementación al RCC porque éste último puede cambiar de micro a micro
 * y nuestro driver permanecer relativamente constante. Así hacemos una apuesta hacia mayor portabilidad. */
void UART::enable_clock()
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
