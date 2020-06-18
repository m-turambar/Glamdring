//
// Created by migue on 16/06/2020.
//

#ifndef G070_UART_H
#define G070_UART_H

#include <cstddef>
#include "helpers.h"

class UART {

public:

  enum class Peripheral {
    USART1 = 0x40013800,
    USART2 = 0x40004400,
    USART3 = 0x40004800,
    USART4 = 0x40004C00,
  };

  enum class WordLength {
    Eight = 0x0,
    Nine = 0x1,
    Seven = 0x2
  };

  enum class StopBits {
    One,
    PointFive,
    Two,
    OnePointFive
  };

  enum class Parity {
    None,
    Odd,
    Even
  };

  UART(const Peripheral peripheral, const size_t baud_arg, const WordLength wlen=WordLength::Eight);

  void enable_clock() const;
  void enable() const;
  void cfg_word_length(const WordLength len) const;
  void cfg_stop_bits(const StopBits bits) const;
  void cfg_parity(const Parity parity) const;
  void cfg_baud(const size_t baud) const;
  void enable_fifo() const;
  void init_gpios();

  void transmit(const uint8_t* buffer, size_t sz) const;
  void receive() const;

  const Peripheral peripheral;
  const size_t base, baud;
  const registro CR1, CR2, CR3, BRR, GTPR, RTOT, RQR, ISR, ICR, RDR, TDR, PRESC;
};

#endif //G070_UART_H
