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


  struct registro
  {
    registro(const size_t addr_) :
    addr(addr_) {}
    const size_t addr;
  };

  /* abstracción para interactuar con los campos de bits de un registro */
  struct bitfield
  {
    /* sz is the number of bits this bitfield has, offset is number of bits offset from register base address */
    bitfield(const size_t size, const uint8_t offset):
    mask((1U << size) - 1U), offset(offset) {}
    /* Does not write the register. Returns a number that you can OR with other bitfields to then write the register. */
    const size_t operator()(const size_t val) { return (val&mask) << offset; } //(slave_addr&0x3FF) << SADDR
    const size_t mask;
    const uint8_t offset;
  };

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
  void enable();
  int comm_init(const size_t slave_addr, const uint8_t write, uint8_t* buffer, const size_t nbytes, const uint8_t autoend=1);

  void disable();

  void write();
};

};

#endif //GLAMDRING_MI2C_H
