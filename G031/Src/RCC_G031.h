//
// Created by migue on 06/06/2020.
//

#ifndef G031_RCC_H
#define G031_RCC_H

#include "helpers.h"

#undef RCC

/** Nota
 * Al activar el reloj de un periférico hay que esperarse 2 ciclos de reloj, pues existe un mecanismo de
 * sincronización. Hay que tener cuidado con eso.
 * */

extern "C" {

/* super incomplete - will be filled in as new functionality is needed */
namespace RCC {
  /** Cuál es el valor de los otros relojes? Son precisos? Dependen de cada aplicación.
   * No deberían ser constantes, o sí? */
  enum class PrimaryClock {
    HSI_RC = 16000000,
    HSE_OSC,
    LSI_RC = 32000,
    LSE_OSC = 32768,
    I2S_CKIN
  };

  enum class GPIO_Port : uint8_t {
    A = 0,
    B = 1,
    C = 2,
    D = 3,
    F = 5
  };

  /** Esto se ve sospechosamente ineficiente, pero lo es? */
  enum class APB_Prescaler : uint8_t {
    P1 = 0,
    P2 = 4,
    P4 = 5,
    P8 = 6,
    P16 = 7
  };

  enum class AHB_Prescaler : uint8_t {
    P1 = 0,
    P2 = 8,
    P4 = 9,
    P8 = 10,
    P16 = 11,
    P64 = 12,
    P128 = 13,
    P256 = 14,
    P512 = 0b1111,
  };

  enum class SystemClockSwitch : uint8_t {
    HSISYS = 0,
    HSE = 1,
    PLLRCLK = 2,
    LSI = 3,
    LSE = 4
  };

  enum class RelojesUsart : uint8_t {
    PCLK = 0,
    SYSCLK = 1,
    HSI16 = 2,
    LSE = 3
  };

  enum class RelojesI2C : uint8_t {
    PCLK = 0,
    SYSCLK = 1,
    HSI16 = 2
  };

  /** Sí. Para el ADC, SYSCLK no es 1. */
  enum class RelojesADC : uint8_t {
    SYSCLK = 0,
    PLLPCLK = 1,
    HSI16 = 2
  };

  /** Hay un periodo de espera que el software debe tomar en consideración, de 2 ciclos, después de habilitar
   * el reloj de un periférico. */
  void enable_port_clock(const GPIO_Port& port);

  void enable_I2C1_clock();
  void enable_I2C2_clock();

  void enable_USART1_clock();
  void enable_USART2_clock();
  void enable_USART3_clock();
  void enable_USART4_clock();

  void enable_SPI1_I2S1_clock();
  void enable_SPI2_clock();

  void enable_TIM15_clock();
  void enable_TIM16_clock();
  void enable_TIM17_clock();
  void enable_TIM2_clock();

  void enable_TIM6_clock();
  void enable_TIM7_clock();

  /** Funciones para relojes del sistema */
  void enable_SYSCFG_clock();
  void enable_power_clock();

  void configurar_prescaler_APB(APB_Prescaler pre);
  void configurar_prescaler_AHB(AHB_Prescaler pre);
  void seleccionar_SYSCLK(SystemClockSwitch clock);
  SystemClockSwitch status_SYSCLK();
  bool is_HSI_ready();

  /** Funciones para origen de relojes de periféricos */
  void seleccionar_reloj_USART1(RelojesUsart reloj);
  void seleccionar_reloj_USART2(RelojesUsart reloj);
  void seleccionar_reloj_I2C1(RelojesI2C reloj);
  void seleccionar_reloj_ADC(RelojesADC reloj);
} ;

};

#endif //G031_RCC_H
