//
// Created by migue on 12/01/2021.
//

#include "DMA.h"

namespace DMA {
  /** En realidad sólo hay dos registros generales, y por cada canal hay cuatro registros:
   * CCRx, CNDTRx, CPARx, CMARx.
   * Los registros deben ser accedidos por words (32 bits) */

  constexpr size_t base{0x40020000};

  constexpr registro ISR{base};
  constexpr registro IFCR{base+0x04};
  constexpr registro CCR1{base+0x08};
  constexpr registro CNDTR1{base+0x0C};
  constexpr registro CPAR1{base+0x10};
  constexpr registro CMAR1{base+0x14};
  /**********************************************/
  constexpr registro CCR2{base+0x1C};

  constexpr registro CNDTR2{base+0x20};
  constexpr registro CPAR2{base+0x24};
  constexpr registro CMAR2{base+0x28};
  /**********************************************/
  constexpr registro CCR3{base+0x30};
  constexpr registro CNDTR3{base+0x34};
  constexpr registro CPAR3{base+0x38};
  constexpr registro CMAR3{base+0x3C};

  /**********************************************/
  constexpr registro CCR4{base+0x44};
  constexpr registro CNDTR4{base+0x48};
  constexpr registro CPAR4{base+0x4C};
  constexpr registro CMAR4{base+0x50};
  /**********************************************/
  constexpr registro CCR5{base+0x58};
  constexpr registro CNDTR5{base+0x5C};

  constexpr registro CPAR5{base+0x60};
  constexpr registro CMAR5{base+0x64};
  /**********************************************/
  constexpr registro CCR6{base+0x6C};
  constexpr registro CNDTR6{base+0x70};
  constexpr registro CPAR6{base+0x74};
  constexpr registro CMAR6{base+0x78};
  /**********************************************/

  constexpr registro CCR7{base+0x80};
  constexpr registro CNDTR7{base+0x84};
  constexpr registro CPAR7{base+0x88};
  constexpr registro CMAR7{base+0x8C};




}