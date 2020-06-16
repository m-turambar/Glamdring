//
// Created by migue on 15/06/2020.
//

#ifndef G070_G070_GPIO_AF_H
#define G070_G070_GPIO_AF_H

#include <cstdint>

namespace GPIO {

  /* realmente no me gusta cómo dejé estructurado esto. No se me ocurre una mejor forma. */
  enum class AlternFunct : uint8_t {
    AF0_EVENTOUT     = 0x0,  /*!< EVENTOUT Alternate Function mapping */
    AF0_IR           = 0x0,  /*!< IR Alternate Function mapping */
    AF0_MCO          = 0x0,  /*!< MCO (MCO1 and MCO2) Alternate Function mapping */
    AF0_OSC          = 0x0,  /*!< OSC (By pass and Enable) Alternate Function mapping */
    AF0_OSC32        = 0x0,  /*!< OSC32 (By pass and Enable) Alternate Function mapping */
    AF0_SWJ          = 0x0,  /*!< SWJ (SWD) Alternate Function mapping */
    AF0_SPI1         = 0x0,  /*!< SPI1 Alternate Function mapping */
    AF0_SPI2         = 0x0,  /*!< SPI2 Alternate Function mapping */
    AF0_TIM14        = 0x0,  /*!< TIM14 Alternate Function mapping */
    AF0_USART1       = 0x0,  /*!< USART1 Alternate Function mapping */
    AF0_USART2       = 0x0,  /*!< USART2 Alternate Function mapping */
    AF0_USART3       = 0x0,  /*!< USART3 Alternate Function mapping */

    AF1_IR           = 0x1,  /*!< IR Alternate Function mapping */
    AF1_OSC          = 0x1,  /*!< OSC (By pass and Enable) Alternate Function mapping */
    AF1_SPI1         = 0x1,  /*!< SPI2 Alternate Function mapping */
    AF1_SPI2         = 0x1,  /*!< SPI2 Alternate Function mapping */
    AF1_TIM1         = 0x1,  /*!< TIM1 Alternate Function mapping */
    AF1_TIM3         = 0x1,  /*!< TIM3 Alternate Function mapping */
    AF1_USART1       = 0x1,  /*!< USART1 Alternate Function mapping */
    AF1_USART2       = 0x1,  /*!< USART2 Alternate Function mapping */
    AF1_USART4       = 0x1,  /*!< USART4 Alternate Function mapping */

    AF2_TIM1         = 0x2,  /*!< TIM1 Alternate Function mapping */
    AF2_TIM14        = 0x2,  /*!< TIM14 Alternate Function mapping */
    AF2_TIM15        = 0x2,  /*!< TIM15 Alternate Function mapping */
    AF2_TIM16        = 0x2,  /*!< TIM16 Alternate Function mapping */
    AF2_TIM17        = 0x2,  /*!< TIM17 Alternate Function mapping */


    AF4_SPI2         = 0x4,  /*!< SPI2 Alternate Function mapping */
    AF4_TIM14        = 0x4,  /*!< TIM14 Alternate Function mapping */
    AF4_TIM15        = 0x4,  /*!< TIM15 Alternate Function mapping */
    AF4_USART1       = 0x4,  /*!< USART1 Alternate Function mapping */
    AF4_USART3       = 0x4,  /*!< USART3 Alternate Function mapping */
    AF4_USART4       = 0x4,  /*!< USART4 Alternate Function mapping */

    AF5_SPI1         = 0x5,  /*!< SPI2 Alternate Function mapping */
    AF5_SPI2         = 0x5,  /*!< SPI2 Alternate Function mapping */
    AF5_TIM1         = 0x5,  /*!< TIM1 Alternate Function mapping */
    AF5_TIM15        = 0x5,  /*!< TIM15 Alternate Function mapping */
    AF5_TIM16        = 0x5,  /*!< TIM16 Alternate Function mapping */
    AF5_TIM17        = 0x5,  /*!< TIM17 Alternate Function mapping */
    AF5_USART3       = 0x5,  /*!< USART3 Alternate Function mapping */

    AF6_I2C1         = 0x6,  /*!< I2C1 Alternate Function mapping */
    AF6_I2C2         = 0x6,  /*!< I2C2 Alternate Function mapping */

    AF7_EVENTOUT     = 0x07  /*!< EVENTOUT Alternate Function mapping */
  };


}

#endif //G070_G070_GPIO_AF_H
