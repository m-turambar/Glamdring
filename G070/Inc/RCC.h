//
// Created by migue on 06/06/2020.
//

#ifndef G070_RCC_H
#define G070_RCC_H

#include "helpers.h"

#undef RCC

/* super incomplete - will be filled in as new functionality is needed */
namespace RCC {
  constexpr size_t base = 0x40021000;
  const registro APBENR1{base + 0x3C};
};

#endif //G070_RCC_H
