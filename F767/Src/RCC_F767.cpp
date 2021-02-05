//
// Created by migue on 25/01/2021.
//

#include "RCC_F767.h"
namespace RCC
{

  constexpr size_t base = 0x40023800;

  constexpr registro CR{base};
  constexpr registro PLLCFGR{base+0x4};
  constexpr registro CFGR{base+0x8};
  constexpr registro CIR{base+0xC};
  constexpr registro AHB1RSTR{base+0x10};
  constexpr registro AHB2RSTR{base+0x14};
  constexpr registro AHB3RSTR{base+0x18};
  /**********************************************/

  constexpr registro APB1RSTR1{base+0x20};
  constexpr registro APB2RSTR{base+0x24};
  /**********************************************/
  /**********************************************/
  constexpr registro AHB1ENR{base+0x30};
  constexpr registro AHB2ENR{base+0x34};
  constexpr registro AHB3ENR{base+0x38}; //QSPI, FMC?
  /**********************************************/

  constexpr registro APB1ENR{base+0x40};
  constexpr registro APB2ENR{base+0x44}; // Práctigo, es el 0x40 del G070. SYSCFG, SPI1
  /**********************************************/
  /**********************************************/
  constexpr registro AHB1LPENR{base+0x50};
  constexpr registro AHB2LPENR{base+0x54};
  constexpr registro AHB3LPENR{base+0x58};
  /**********************************************/

  constexpr registro APB1LPENR{base+0x60};
  constexpr registro APB2LPENR{base+0x64};
  /**********************************************/
  /**********************************************/
  constexpr registro BDCR{base+0x70};
  constexpr registro CSR{base+0x74};
  /**********************************************/
  /**********************************************/

  constexpr registro SSCGR{base+0x80};
  constexpr registro PLLI2SCFGR{base+0x84};
  constexpr registro PLLSAICFGR{base+0x88};
  constexpr registro DCKCFGR1{base+0x8C};
  constexpr registro DCKCFGR2{base+0x90};


  void enable_port_clock(const GPIO_Port& port)
  {
    memoria(AHB1ENR) |= (1u << static_cast<uint8_t>(port));
  }

  /********************************************/

  void enable_USART1_clock()
  {
    const flag USART1EN(14);
    APB2ENR.set(USART1EN);
  }

  void enable_USART2_clock()
  {
    const flag USART2EN(17);
    APB1ENR.set(USART2EN);
  }

  void enable_USART3_clock()
  {
    const flag USART3EN(18);
    APB1ENR.set(USART3EN);
  }

  void enable_USART4_clock()
  {
    const flag USART4EN(19);
    APB1ENR.set(USART4EN);
  }

  /*******************************************/

  void enable_I2C1_clock()
  {
    const flag I2C1EN(21);
    APB1ENR.set(I2C1EN);
  }

  void enable_I2C2_clock()
  {
    const flag I2C2EN(22);
    APB1ENR.set(I2C2EN);
  }

  /*******************************************/

  void enable_SPI1_I2S1_clock()
  {
    const flag SPI1EN(12);
    APB2ENR.set(SPI1EN);
  }

  void enable_SPI2_clock()
  {
    const flag SPI2EN(14);
    APB1ENR.set(SPI2EN);
  }

  /*******************************************/

  void enable_TIM6_clock()
  {
    const flag TIM6EN(4);
    APB1ENR.set(TIM6EN);
  }

  void enable_TIM7_clock()
  {
    const flag TIM7EN(5);
    APB1ENR.set(TIM7EN);
  }

  void enable_TIM15_clock()
  {
    //static_assert(false, "TIM15 no existe en F767");
  }

  void enable_TIM16_clock()
  {
    //static_assert(false, "TIM16 no existe en F767");
  }

  void enable_TIM17_clock()
  {
    //static_assert(false, "TIM17 no existe en F767");
  }

  /*********************************************************/
  /** Funciones para configurar o leer relojes del sistema */
  /*********************************************************/

  void enable_SYSCFG_clock()
  {
    const flag SYSCFGEN(14);
    APB2ENR.set(SYSCFGEN);
  }

  void enable_power_clock()
  {
    const flag PWREN(28);
    APB1ENR.set(PWREN);
  }

  void configurar_prescaler_APB1(APB_Prescaler pre)
  {
    const bitfield PPRE1(3, 10, static_cast<size_t>(pre));
    CFGR.write(PPRE1);
  }

  void configurar_prescaler_APB2(APB_Prescaler pre)
  {
    const bitfield PPRE2(3, 13, static_cast<size_t>(pre));
    CFGR.write(PPRE2);
  }

  void configurar_prescaler_AHB(AHB_Prescaler pre)
  {
    const bitfield HPRE(4, 4, static_cast<size_t>(pre));
    CFGR.write(HPRE);
  }

  void seleccionar_SYSCLK(SystemClockSwitch clock)
  {
    const bitfield SW(2, 0, static_cast<size_t>(clock));
    CFGR.write(SW);
  }

  SystemClockSwitch status_SYSCLK()
  {
    const bitfield SWS(2, 2, 0);
    size_t clock = CFGR.read_grounded(SWS);
    return static_cast<SystemClockSwitch>(clock);
  }

  bool is_HSI_ready()
  {
    const flag HSIRDY(1);
    return CR.is_set(HSIRDY);
  }

  void encender_HSI16()
  {
    const flag HSION(0);
    CR.set(HSION);
  }

  /***********************************************************/
  /** Funciones para el origen de los relojes de periféricos */
  /***********************************************************/

  void seleccionar_reloj_USART1(RelojesUsart reloj)
  {
    const bitfield USART1SEL(2, 0, static_cast<size_t>(reloj));
    DCKCFGR2.write(USART1SEL);
  }

  void seleccionar_reloj_USART2(RelojesUsart reloj)
  {
    const bitfield USART2SEL(2, 2, static_cast<size_t>(reloj));
    DCKCFGR2.write(USART2SEL);
  }

  void seleccionar_reloj_I2C1(RelojesI2C reloj)
  {
    const bitfield I2C1SEL(2, 12, static_cast<size_t>(reloj));
    DCKCFGR2.write(I2C1SEL);
  }

  void seleccionar_reloj_ADC(RelojesADC reloj)
  {
    //const bitfield ADCSEL(2, 28, static_cast<size_t>(reloj));
    //niidea.write(ADCSEL); ni idea
  }

  /*******************************************/

}

