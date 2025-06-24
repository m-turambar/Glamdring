//
// Created by migue on 07/12/2020.
//

#include "FLASH.h"

namespace FLASH
{
  constexpr size_t base{0x40022000};

  constexpr registro ACR{base};
  /**********************************************/
  constexpr registro KEYR{base+0x08};
  constexpr registro OPT_KEYR{base+0x0C};
  constexpr registro SR{base+0x10};
  constexpr registro CR{base+0x14};
  constexpr registro ECCR{base+0x18};
  /**********************************************/

  constexpr registro OPTR{base+0x20};
  /**********************************************/
  /**********************************************/
  constexpr registro WRP1AR{base+0x2C};
  constexpr registro WRP1BR{base+0x30};


  void prefetch_buffer_enable()
  {
    constexpr flag PRFTEN(8);
    ACR.set(PRFTEN);
  }


}

