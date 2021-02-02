//
// Created by migue on 17/05/2020.
//

#ifndef GLAMDRING_MI2C_H
#define GLAMDRING_MI2C_H

#include <cstddef>
#include "helpers.h"

#undef I2C1
#undef I2C2

extern "C" {

/*! \brief Clase que representa un puerto I2C.
 * Usar asi:
 * I2C i2c2(I2C::Peripheral::I2C1);
 * i2c2.enable(I2C::Timing::Standard);*/
struct I2C {

  enum class Peripheral : std::size_t {
    I2C1 = 0x40005400,
    I2C2 = 0x40005800
  };

  enum class Mode {
    Standard,
    Fast,
    Fast_plus
  };
  enum class Timing : std::size_t {
    Standard = 0x00303D5B,
    Fast = 0x0010061A,
    FastPlus = 0x0000107
  };

  enum class Status {
    OK=0,
    NACKF,
    Timeout
  };

  I2C(Peripheral peripheral_arg) :
  peripheral(peripheral_arg),
  base(static_cast<size_t>(peripheral)),
  CR1(base),
  CR2(base+0x4),
  OAR1(base+0x8),
  OAR2(base+0xC),
  TIMINGR(base+0x10),
  TIMEOUTR(base+0x14),
  ISR(base+0x18),
  ICR(base+0x1C),
  PECR(base+0x20),
  RXDR(base+0x24),
  TXDR(base+0x28)
  {

  }

  void init_gpios();
  void enable_clock() const;
  void configure_timings(const Timing timing);
  void disable();
  void enable(Timing timing);

  I2C::Status comm_init(const size_t slave_addr, const uint8_t write, uint8_t* buffer, const size_t nbytes,
      const uint8_t autoend = 1) const;

  I2C::Status write(const size_t slave_addr, uint8_t* buffer, const size_t nbytes, const uint8_t autoend = 1) const;
  I2C::Status read(const size_t slave_addr, uint8_t* buffer, const size_t nbytes, const uint8_t autoend = 1) const;

  /* puede que usemos peripheral para determinar el tipo del I2C después, así que es conveniente guardarlo */
  const Peripheral peripheral;
  const size_t base;
  const registro CR1, CR2, OAR1, OAR2, TIMINGR, TIMEOUTR, ISR, ICR, PECR, RXDR, TXDR;
};


}



#endif //GLAMDRING_MI2C_H
