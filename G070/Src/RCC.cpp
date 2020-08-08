//
// Created by migue on 06/06/2020.
//

#include "RCC.h"
namespace RCC
{



  void enable_port_clock(const GPIO_Port& port)
  {
    memoria(IOPENR) |= (1u << static_cast<uint8_t>(port));
  }

  /********************************************/

  void enable_USART1_clock()
  {
    const bitfield USART1EN(1, 14);
    memoria(RCC::APBENR2) |= USART1EN(1);
  }

  void enable_USART2_clock()
  {
    const bitfield USART2EN(1, 17);
    memoria(RCC::APBENR1) |= USART2EN(1);
  }

  void enable_USART3_clock()
  {
    const bitfield USART3EN(1, 18);
    memoria(RCC::APBENR1) |= USART3EN(1);
  }

  void enable_USART4_clock()
  {
    const bitfield USART4EN(1, 19);
    memoria(RCC::APBENR1) |= USART4EN(1);
  }

  /*******************************************/

  void enable_I2C1_clock()
  {
    const bitfield I2C1EN(1, 21);
    memoria(RCC::APBENR1) |= I2C1EN(1);
  }

  void enable_I2C2_clock()
  {
    const bitfield I2C2EN(1, 22);
    memoria(RCC::APBENR1) |= I2C2EN(1);
  }

  void enable_TIM15_clock()
  {
    flag TIM15EN(16);
    RCC::APBENR2.set(TIM15EN);
  }

  void enable_TIM16_clock()
  {
    flag TIM16EN(17);
    RCC::APBENR2.set(TIM16EN);
  }

  void enable_TIM17_clock()
  {
    flag TIM17EN(18);
    RCC::APBENR2.set(TIM17EN);
  }

  /*******************************************/

}

