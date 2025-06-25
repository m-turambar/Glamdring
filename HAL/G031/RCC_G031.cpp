//
// Created by migue on 06/06/2020.
//

#include "RCC_G031.h"
namespace RCC
{

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


  void enable_port_clock(const GPIO_Port& port)
  {
    memoria(IOPENR) |= (1u << static_cast<uint8_t>(port));
  }

  /********************************************/

  void enable_USART1_clock()
  {
    const flag USART1EN(14);
    APBENR2.set(USART1EN);
  }

  void enable_USART2_clock()
  {
    const flag USART2EN(17);
    APBENR1.set(USART2EN);
  }

  void enable_USART3_clock()
  {
    const flag USART3EN(18);
    APBENR1.set(USART3EN);
  }

  void enable_USART4_clock()
  {
    const flag USART4EN(19);
    APBENR1.set(USART4EN);
  }

  /*******************************************/

  void enable_I2C1_clock()
  {
    const flag I2C1EN(21);
    APBENR1.set(I2C1EN);
  }

  void enable_I2C2_clock()
  {
    const flag I2C2EN(22);
    APBENR1.set(I2C2EN);
  }

  /*******************************************/

  void enable_SPI1_I2S1_clock()
  {
    const flag SPI1EN(12);
    APBENR2.set(SPI1EN);
  }

  void enable_SPI2_clock()
  {
    const flag SPI2EN(14);
    APBENR1.set(SPI2EN);
  }

  /*******************************************/

  void enable_TIM6_clock()
  {
    const flag TIM6EN(4);
    APBENR1.set(TIM6EN);
  }

  void enable_TIM7_clock()
  {
    const flag TIM7EN(5);
    APBENR1.set(TIM7EN);
  }

  void enable_TIM15_clock()
  {
    const flag TIM15EN(16);
    APBENR2.set(TIM15EN);
  }

  void enable_TIM16_clock()
  {
    const flag TIM16EN(17);
    APBENR2.set(TIM16EN);
  }

  void enable_TIM17_clock()
  {
    const flag TIM17EN(18);
    APBENR2.set(TIM17EN);
  }

  void enable_TIM2_clock()
  {
    const flag TIM2EN(0);
    APBENR1.set(TIM2EN);
  }

  /*********************************************************/
  /** Funciones para configurar o leer relojes del sistema */
  /*********************************************************/

  void enable_SYSCFG_clock()
  {
    const flag SYSCFGEN(0);
    APBENR2.set(SYSCFGEN);
  }

  void enable_power_clock()
  {
    const flag PWREN(28);
    APBENR1.set(PWREN);
  }

  void configurar_prescaler_APB(APB_Prescaler pre)
  {
    const bitfield PPRE(3, 12, static_cast<size_t>(pre));
    CFGR.write(PPRE);
  }

  void configurar_prescaler_AHB(AHB_Prescaler pre)
  {
    const bitfield HPRE(4, 8, static_cast<size_t>(pre));
    CFGR.write(HPRE);
  }

  void seleccionar_SYSCLK(SystemClockSwitch clock)
  {
    const bitfield SW(3, 0, static_cast<size_t>(clock));
    CFGR.write(SW);
  }

  SystemClockSwitch status_SYSCLK()
  {
    const bitfield SWS(3, 3, 0);
    size_t clock = CFGR.read_grounded(SWS);
    return static_cast<SystemClockSwitch>(clock);
  }

  bool is_HSI_ready()
  {
    const flag HSIRDY(10);
    return CR.is_set(HSIRDY);
  }
  /***********************************************************/
  /** Funciones para el origen de los relojes de periféricos */
  /***********************************************************/

  void seleccionar_reloj_USART1(RelojesUsart reloj)
  {
    const bitfield USART1SEL(2, 0, static_cast<size_t>(reloj));
    CCIPR.write(USART1SEL);
  }

  void seleccionar_reloj_USART2(RelojesUsart reloj)
  {
    const bitfield USART2SEL(2, 2, static_cast<size_t>(reloj));
    CCIPR.write(USART2SEL);
  }

  void seleccionar_reloj_I2C1(RelojesI2C reloj)
  {
    const bitfield I2C1SEL(2, 12, static_cast<size_t>(reloj));
    CCIPR.write(I2C1SEL);
  }

  void seleccionar_reloj_ADC(RelojesADC reloj)
  {
    const bitfield ADCSEL(2, 30, static_cast<size_t>(reloj));
    CCIPR.write(ADCSEL);
  }

  /*******************************************/

}

