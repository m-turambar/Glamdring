//
// Created by migue on 25/06/2020.
//

#include "PWR.h"

namespace PWR
{
  constexpr size_t base = 0x40007000;

  constexpr registro CR1{base};
  /**********************************************/
  constexpr registro CR3{base+0x8};
  constexpr registro CR4{base+0xC};
  constexpr registro SR1{base+0x10};
  constexpr registro SR2{base+0x14};
  constexpr registro SCR{base+0x18};
  /**********************************************/

  constexpr registro PUCRA{base+0x20};
  constexpr registro PDCRA{base+0x24};
  constexpr registro PUCRB{base+0x28};
  constexpr registro PDCRB{base+0x2C};
  constexpr registro PUCRC{base+0x30};
  constexpr registro PDCRC{base+0x34};
  constexpr registro PUCRD{base+0x38};
  constexpr registro PDCRD{base+0x3C};

  /**********************************************/
  /**********************************************/
  constexpr registro PUCRF{base+0x48};
  constexpr registro PDCRF{base+0x4C};



  bool configurar_regulador(const Voltaje v)
  {
    const flag VOSF(10);
    const bitfield VOS(2, 9, static_cast<const size_t>(v));
    bool res = false;
    CR1.write(VOS);
    /** Un retraso se requiere para que el regulador interno esté listo
     * después de que el "voltage scaling" haya sido cambiado. Un valor
     * de 0 indica que el regulador está listo en el rango de voltaje
     * seleccionado. */

    for(auto i=0; i<100000; ++i)
    {
      if(SR2.is_reset(VOSF))
      {
        res = true;
        break;
      }
    }
    return res;
  }
}