/**
  ******************************************************************************
  * File Name          : gpio.c
  * Description        : This file provides code for the configuration
  *                      of all used GPIO pins.
  *****************************************************************************/

#include "gpio.h"

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/


/** Configure pins
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);

  /*Configure GPIO pin : PA5 */
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

void pinMode(GPIO_TypeDef* base, int pin, int mode)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* mike, si ya están habilitados no rehabilitarlos */
    //__HAL_RCC_GPIOC_CLK_ENABLE();
    //__HAL_RCC_GPIOA_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(base, pin, GPIO_PIN_RESET);

    /*Configurar pin - checar si para input es diferente */
    GPIO_InitStruct.Pin = pin;
    GPIO_InitStruct.Mode = mode;
    if(mode == INPUT)
        GPIO_InitStruct.Pull = GPIO_PULLUP;
    else
        GPIO_InitStruct.Pull = GPIO_NOPULL;
    
    if(mode == GPIO_MODE_OUTPUT_PP)
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(base, &GPIO_InitStruct);
}

//si tuvieramos un mapa... tal vez es hora de migrar a c++
void writePin(GPIO_TypeDef* base, int pin, int val)
{
    switch(pin)
    {
    case 0:
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, val);
        break;
    case 1:
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, val);
        break;
    case 2:
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, val);
        break;
    case 3:
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, val);
        break;
    case 4:
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, val);
        break;
    case 5:
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, val);
        break;
    case 6:
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, val);
        break;
    case 7:
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, val);
        break;
    case 8:
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, val);
        break;
    }
}

int readPin(GPIO_TypeDef* base, int pin)
{
    return HAL_GPIO_ReadPin(base, pin);
}