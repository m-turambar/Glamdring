//
// Created by migue on 17/05/2020.
//

#ifndef GLAMDRING_MI2C_H
#define GLAMDRING_MI2C_H

#include <cstddef>
#include "stm32g0xx_hal.h"

extern "C"{

namespace mI2C2
{

  enum class Mode{
    Standard,
    Fast,
    Fast_plus
  };
  namespace timing
  {
      constexpr size_t Standard = 0x00303D5B;
      constexpr size_t Fast = 0x0010061A;
      constexpr size_t FastPlus = 0x0000107;
  }

  constexpr size_t base = 0x40005800; //I2C 2, 1 not implemented yet, limit 0x4005BFF
  constexpr size_t CR1 = (base);
  constexpr size_t CR2 = (base + 0x4);
  constexpr size_t OAR1 = (base + 0x8);
  constexpr size_t OAR2 = (base + 0xC);
  constexpr size_t TIMINGR = (base + 0x10);
  constexpr size_t TIMEOUTR = (base + 0x14);
  constexpr size_t ISR = (base + 0x18);
  constexpr size_t ICR = (base + 0x1C);
  constexpr size_t PECR = (base + 0x20);
  constexpr size_t RXDR = (base + 0x24);
  constexpr size_t TXDR = (base + 0x28);
  /**/

  void init_gpios();
  void enable_clock();
  void configure_timings(size_t timing);
  void disable();
  void enable();
  int comm_init(const size_t slave_addr, const uint8_t write, uint8_t* buffer, const size_t nbytes, const uint8_t autoend=1);
  int write(const size_t slave_addr, uint8_t* buffer, const size_t nbytes, const uint8_t autoend=1);
  int read(const size_t slave_addr, uint8_t* buffer, const size_t nbytes, const uint8_t autoend=1);

};

};

#endif //GLAMDRING_MI2C_H
