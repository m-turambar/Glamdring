//
// Created by migue on 09/12/2020.
//
#include <cstddef>
#include "helpers.h"


namespace SCB
{
  constexpr size_t base{0xE000ED00};

  constexpr registro CPUID  {base};
  constexpr registro ICSR   {base + 0x04};
  constexpr registro VTOR   {base + 0x08};
  constexpr registro AIRCR  {base + 0x0C};
  constexpr registro SCR    {base + 0x10};
  constexpr registro CCR    {base + 0x14};
  /**********************************************/
  constexpr registro SHPR2  {base + 0x1C};
  constexpr registro SHPR3  {base + 0x20};
}

extern "C"
void Bishi_Victorious()
{
  const bitfield TBLOFF(24, 6, 0x08000000 >> 6);
  SCB::VTOR.write(TBLOFF);
}