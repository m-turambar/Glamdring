//
// Created by migue on 06/06/2020.
//
#ifndef G070_NVIC_H
#define G070_NVIC_H

/**
  * @brief Configuration of the Cortex-M0+ Processor and Core Peripherals
   */
#define __CM0PLUS_REV             0 /*!< Core Revision r0p0                            */
#define __MPU_PRESENT             1 /*!< STM32G0xx  provides an MPU                    */
#define __VTOR_PRESENT            1 /*!< Vector  Table  Register supported             */
#define __NVIC_PRIO_BITS          2 /*!< STM32G0xx uses 2 Bits for the Priority Levels */
#define __Vendor_SysTickConfig    0 /*!< Set to 1 if different SysTick Config is used  */

/*!< Interrupt Number Definition */
typedef enum
{
/******  Cortex-M0+ Processor Exceptions Numbers ***************************************************************/
  NonMaskableInt_IRQn         = -14,    /*!< 2 Non Maskable Interrupt                                          */
  HardFault_IRQn              = -13,    /*!< 3 Cortex-M Hard Fault Interrupt                                   */
  SVC_IRQn                    = -5,     /*!< 11 Cortex-M SV Call Interrupt                                     */
  PendSV_IRQn                 = -2,     /*!< 14 Cortex-M Pend SV Interrupt                                     */
  SysTick_IRQn                = -1,     /*!< 15 Cortex-M System Tick Interrupt                                 */
/******  STM32G0xxxx specific Interrupt Numbers ****************************************************************/
  WWDG_IRQn                   = 0,      /*!< Window WatchDog Interrupt                                         */
  RTC_TAMP_IRQn               = 2,      /*!< RTC interrupt through the EXTI line 19 & 21                       */
  FLASH_IRQn                  = 3,      /*!< FLASH global Interrupt                                            */
  RCC_IRQn                    = 4,      /*!< RCC global Interrupt                                              */
  EXTI0_1_IRQn                = 5,      /*!< EXTI 0 and 1 Interrupts                                           */
  EXTI2_3_IRQn                = 6,      /*!< EXTI Line 2 and 3 Interrupts                                      */
  EXTI4_15_IRQn               = 7,      /*!< EXTI Line 4 to 15 Interrupts                                      */
  DMA1_Channel1_IRQn          = 9,      /*!< DMA1 Channel 1 Interrupt                                          */
  DMA1_Channel2_3_IRQn        = 10,     /*!< DMA1 Channel 2 and Channel 3 Interrupts                           */
  DMA1_Ch4_7_DMAMUX1_OVR_IRQn = 11,     /*!< DMA1 Channel 4 to Channel 7 and DMAMUX1 Overrun Interrupts        */
  ADC1_IRQn                   = 12,     /*!< ADC1 Interrupts                                                   */
  TIM1_BRK_UP_TRG_COM_IRQn    = 13,     /*!< TIM1 Break, Update, Trigger and Commutation Interrupts            */
  TIM1_CC_IRQn                = 14,     /*!< TIM1 Capture Compare Interrupt                                    */
  TIM3_IRQn                   = 16,     /*!< TIM3 global Interrupt                                             */
  TIM6_IRQn                   = 17,     /*!< TIM6 global Interrupts                                            */
  TIM7_IRQn                   = 18,     /*!< TIM7 global Interrupt                                             */
  TIM14_IRQn                  = 19,     /*!< TIM14 global Interrupt                                            */
  TIM15_IRQn                  = 20,     /*!< TIM15 global Interrupt                                            */
  TIM16_IRQn                  = 21,     /*!< TIM16 global Interrupt                                            */
  TIM17_IRQn                  = 22,     /*!< TIM17 global Interrupt                                            */
  I2C1_IRQn                   = 23,     /*!< I2C1 Interrupt  (combined with EXTI 23)                           */
  I2C2_IRQn                   = 24,     /*!< I2C2 Interrupt                                                    */
  SPI1_IRQn                   = 25,     /*!< SPI1 Interrupt                                                    */
  SPI2_IRQn                   = 26,     /*!< SPI2 Interrupt                                                    */
  USART1_IRQn                 = 27,     /*!< USART1 Interrupt                                                  */
  USART2_IRQn                 = 28,     /*!< USART2 Interrupt                                                  */
  USART3_4_IRQn               = 29,     /*!< USART3, USART4 globlal Interrupts                                 */
} IRQn_Type;

/* Este header escrito por ARM es más pequeño pero requiere que los macros y enum anteriormente
 * definidos estén presentes. El objetivo era usar el código de ARM pero no la HAL de ST, porque
 * el segundo está escrito con las nalgas de un becario. Repitiendo este código que estaba en la HAL de ST,
 * podemos omitir incluir toda su HAL, y hacer uso del código de ARM. */
#include "core_cm0plus.h"
#include <cstddef>
#include "helpers.h"

#undef NVIC

/* no sé si tenga caso usar esta estructura, pues lo de arriba es CMSIS. Es más portable. */
namespace NVIC {
  const registro ISER(0xE000E100);
  const registro ICER(0xE000E180);
  const registro ISPR(0xE000E200);
  const registro ICPR(0xE000E280);
  /* solo los bits 7 y 6 de los registros de prioridad les hace caso el hardware, osea escribir un 6 como priodidad
   * seria equivalente a escribir 0. Solo tienen sentido los valores 0, 64, 1288, 192. 0 Tiene prioridad más alta. */
  // const registro IPR0_7(0xE000E400); deberia ser un arreglo de 8 registros donde cada registro guarda 4 prioridades
  const uint8_t TIM6_IRQn = 17;
  const uint8_t TIM7_IRQn = 18;
};

#endif //G070_NVIC_H
