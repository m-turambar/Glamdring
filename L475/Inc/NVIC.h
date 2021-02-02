//
// Created by migue on 06/06/2020.
//
#ifndef L475_NVIC_H
#define L475_NVIC_H

/**
  * @brief Configuration of the Cortex-M0+ Processor and Core Peripherals
   */
#define __CM0PLUS_REV             0 /*!< Core Revision r0p0                            */
#define __MPU_PRESENT             1 /*!< STM32G0xx  provides an MPU                    */
#define __VTOR_PRESENT            1 /*!< Vector  Table  Register supported             */
#define __NVIC_PRIO_BITS          2 /*!< STM32G0xx uses 2 Bits for the Priority Levels */
#define __Vendor_SysTickConfig    0 /*!< Set to 1 if different SysTick Config is used  */

/*!< Interrupt Number Definition
 * Esta lista es gigantesca en el CM4. Me da hueva terminarla. Llénala según la vayas usando.
 * Cambia mucho del M0 al M4*/
typedef enum
{
/******  Partial and incomplete Cortex-M4 Processor Exceptions Numbers ****************************************/
  NonMaskableInt_IRQn         = -2,    /*!< 2 Non Maskable Interrupt                                          */
  HardFault_IRQn              = -1,    /*!< 3 Cortex-M Hard Fault Interrupt                                   */
  SVC_IRQn                    = 3,     /*!< 11 Cortex-M SV Call Interrupt                                     */
  PendSV_IRQn                 = 5,     /*!< 14 Cortex-M Pend SV Interrupt                                     */
  SysTick_IRQn                = 6,     /*!< 15 Cortex-M System Tick Interrupt                                 */
/******  Partial and incomplete STM32L475 specific Interrupt Numbers ******************************************/
  WWDG_IRQn                   = 7,      /*!< Window WatchDog Interrupt                                         */
  RTC_TAMP_IRQn               = 9,      /*!< RTC interrupt through the EXTI line 19 & 21                       */
  FLASH_IRQn                  = 11,      /*!< FLASH global Interrupt                                            */
  RCC_IRQn                    = 12,      /*!< RCC global Interrupt                                              */
  EXTI0_IRQn                  = 13,      /*!< EXTI 0                                                           */
  EXTI1_IRQn                  = 14,      /*!< EXTI 1                                                           */
  EXTI2_IRQn                  = 15,      /*!< EXTI 2                                                           */
  EXTI3_IRQn                  = 16,      /*!< EXTI 3                                                           */
  TIM3_IRQn                   = 29,     /*!< TIM3 global Interrupt                                             */
  TIM6_IRQn                   = 54,     /*!< TIM6 global Interrupts                                            */
  TIM7_IRQn                   = 55,     /*!< TIM7 global Interrupt                                             */
  TIM15_IRQn                  = 24,     /*!< TIM15 global Interrupt                                            */
  TIM16_IRQn                  = 25,     /*!< TIM16 global Interrupt                                            */
  TIM17_IRQn                  = 26,     /*!< TIM17 global Interrupt                                            */
  I2C1_IRQn                   = 31,     /*!< I2C1 Interrupt  (combined with EXTI 23)                           */
  I2C2_IRQn                   = 33,     /*!< I2C2 Interrupt                                                    */
  SPI1_IRQn                   = 35,     /*!< SPI1 Interrupt                                                    */
  SPI2_IRQn                   = 36,     /*!< SPI2 Interrupt                                                    */
  USART1_IRQn                 = 37,     /*!< USART1 Interrupt                                                  */
  USART2_IRQn                 = 38,     /*!< USART2 Interrupt                                                  */
  USART3_4_IRQn               = 39,     /*!< USART3, USART4 globlal Interrupts                                 */
} IRQn_Type;

/* Este header escrito por ARM es más pequeño pero requiere que los macros y enum anteriormente
 * definidos estén presentes. El objetivo era usar el código de ARM pero no la HAL de ST, porque
 * el segundo está escrito con las nalgas de un becario. Repitiendo este código que estaba en la HAL de ST,
 * podemos omitir incluir toda su HAL, y hacer uso del código de ARM. */
#include "core_cm4.h"

#endif //G070_NVIC_H
