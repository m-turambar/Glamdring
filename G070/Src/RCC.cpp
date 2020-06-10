//
// Created by migue on 06/06/2020.
//

#include "RCC.h"
namespace RCC
{
  void enable_port_clock(const GPIO_Port port)
  {
    memoria(IOPENR) |= (1u << static_cast<uint8_t>(port));
  }
}

