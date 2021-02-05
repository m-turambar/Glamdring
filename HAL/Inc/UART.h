//
// Created by migue on 16/06/2020.
//

#ifndef G070_UART_H
#define G070_UART_H

#include <cstddef>
#include "helpers.h"

extern "C" {

void USART1_IRQHandler(void);
void USART2_IRQHandler(void);
void USART3_4_IRQHandler(void);

/** UARTs 1 y 2 tienen FIFOs, 3 y 4 no. Pero todas tienen DMA.
 * Muchas de las configuraciones default de los registros de la UART son las deseables.
 * Ejemplo un stop bit, no parity, Ocho bits por palabra, vienen por default después de un reseteo.
 * En este momento no me estoy molestando en modificarlas, pero por limpieza y por si reseteamos de alguna
 * manera en la que el valor de los registros no se borre, lo más saludable sería explícitamente configurar
 * esos valores creo. */
class UART {

public:

  //esto se ve horrible así, Tal vez hay que crear un archivo que se llame mem map o algo así
  enum class Peripheral {
#ifdef STM32L475xx
    USART1 = 0x40013800,
#elif defined(STM32G070xx)
    USART1 = 0x40013800,
#elif defined(STM32F767xx)
    USART1 = 0x40011000,
#endif
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

  UART(const Peripheral peripheral, const size_t baud_arg, const WordLength wlen = WordLength::Eight);

  void enable_clock() const;

  void enable() const;

  const UART& enable_interrupt_rx(void (*callback_arg)(const uint8_t byte), const uint8_t priority=3);

  void cfg_word_length(const WordLength len) const;

  void cfg_stop_bits(const StopBits bits) const;

  void cfg_parity(const Parity parity) const;

  void cfg_baud(const size_t baud) const;

  const UART& enable_fifo() const;

  void init_gpios();

  void transmitq(const uint8_t* buffer, size_t sz) const;

  void receiveq(uint8_t* buffer, size_t sz) const;

  bool available() const;

  /** Implementacion ingenua. Deberias usar DMA siempre que puedas. O interrupciones con una queue de sw. */
  uint8_t read_byte() const;

  void write_byte(const uint8_t b) const;
  const UART& operator<<(const uint8_t b) const;
  const UART& operator<<(const char* buffer) const;

  const Peripheral peripheral;
  void (*callback_rx)(const uint8_t byte) {nullptr};
  const size_t base, baud;
  const registro CR1, CR2, CR3, BRR, GTPR, RTOT, RQR, ISR, ICR, RDR, TDR, PRESC;
};


};

#endif //G070_UART_H
