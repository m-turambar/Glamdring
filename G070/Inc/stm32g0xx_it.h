/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32g0xx_it.h
  * @brief   This file contains the headers of the interrupt handlers.
  ******************************************************************************
*/
#ifndef __STM32G0xx_IT_H
#define __STM32G0xx_IT_H

#ifdef __cplusplus
 extern "C" {
#endif 

void NMI_Handler(void);
void HardFault_Handler(void);
void DMA1_Channel1_IRQHandler(void);
void TIM3_IRQHandler(void);
void USART2_IRQHandler(void);


#ifdef __cplusplus
}
#endif

#endif /* __STM32G0xx_IT_H */

