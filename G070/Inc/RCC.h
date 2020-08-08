//
// Created by migue on 06/06/2020.
//

#ifndef G070_RCC_H
#define G070_RCC_H

#include "helpers.h"

#undef RCC

/** Nota
 * Al activar el reloj de un periférico hay que esperarse 2 ciclos de reloj, pues existe un mecanismo de
 * sincronización. Hay que tener cuidado con eso.
 * */

extern "C" {

/* super incomplete - will be filled in as new functionality is needed */
namespace RCC {
  enum class PrimaryClock {
    HSI_RC = 16000000,
    HSE_OSC,
    LSI_RC,
    LSE_OSC,
    I2S_CKIN
  };

  enum class GPIO_Port : uint8_t {
    A = 0,
    B = 1,
    C = 2,
    D = 3,
    F = 5
  };

  constexpr size_t base = 0x40021000;

  constexpr registro CR{base};
  constexpr registro ICSCR{base+0x4};
  constexpr registro CFGR{base+0x8};
  constexpr registro PLLCFGR{base+0xC};
  /**********************************************/
  /**********************************************/
  constexpr registro CIER{base+0x18};
  constexpr registro CIFR{base+0x1C};

  constexpr registro CICR{base+0x20};
  constexpr registro IOPRSTR{base+0x24};
  constexpr registro AHBRSTR{base+0x28};
  constexpr registro APBRSTR1{base+0x2C};
  constexpr registro APBRSTR2{base+0x30};
  constexpr registro IOPENR{base+0x34};
  constexpr registro AHBENR{base+0x38};
  constexpr registro APBENR1{base+0x3C};

  constexpr registro APBENR2{base+0x40};
  constexpr registro IOPSMENR{base+0x44};
  constexpr registro AHBSMENR{base+0x48};
  constexpr registro APBSMENR1{base+0x4C};
  constexpr registro APBSMENR2{base+0x50};
  constexpr registro CCIPR{base+0x54};
  /**********************************************/
  constexpr registro BDCR{base+0x5C};

  constexpr registro CSR{base+0x60};

  /** Hay un periodo de espera que el software debe tomar en consideración, de 2 ciclos, después de habilitar
   * el reloj de un periférico. */
  void enable_port_clock(const GPIO_Port& port);

  void enable_I2C1_clock();
  void enable_I2C2_clock();

  void enable_USART1_clock();
  void enable_USART2_clock();
  void enable_USART3_clock();
  void enable_USART4_clock();

  void enable_TIM15_clock();
  void enable_TIM16_clock();
  void enable_TIM17_clock();
} ;

};

#endif //G070_RCC_H
