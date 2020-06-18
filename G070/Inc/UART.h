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
    Eight_8 = 0x0,
    Nine_9 = 0x1,
    Seven_7 = 0x2
  };

  UART(const Peripheral peripheral);

  void enable_clock();
  void set_word_length(const WordLength len) const;

  const Peripheral peripheral;
  const size_t base;
  const registro CR1, CR2, CR3, BRR, GTPR, RTOT, RQR, ISR, ICR, RDR, TDR, PRESC;
};

#endif //G070_UART_H
