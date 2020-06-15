//
// Created by migue on 06/06/2020.
//

#ifndef G070_RCC_H
#define G070_RCC_H

#include "helpers.h"

#undef RCC

/* super incomplete - will be filled in as new functionality is needed */
namespace RCC {
  enum class GPIO_Port : uint8_t {
    A=0,
    B=1,
    C=2,
    D=3,
    F=5
  };

  constexpr size_t base = 0x40021000;
  const registro IOPENR{base + 0x34};
  const registro APBENR1{base + 0x3C};
  const registro APBENR2{base + 0x40};
  void enable_port_clock(const GPIO_Port port);
};

#endif //G070_RCC_H
