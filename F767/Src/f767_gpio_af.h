//
// Created by migue on 05/Feb/2021.
//

#ifndef L475_GPIO_AF_H
#define L475_GPIO_AF_H

#include <cstdint>

namespace GPIO {

  /* realmente no me gusta cómo dejé estructurado esto. No se me ocurre una mejor forma.
   * y ni puta idea de cómo voy a portear esto al F767 */
  enum class AlternFunct : uint8_t {
    AF0     = 0x0,
    AF1     = 0x1,
    AF2     = 0x2,
    AF3     = 0x3,
    AF4     = 0x4,
    AF5     = 0x5,
    AF6     = 0x6,
    AF7     = 0x7,
    AF8     = 0x8,
  };


}

#endif //L475_GPIO_AF_H
