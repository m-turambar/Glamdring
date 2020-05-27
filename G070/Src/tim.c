#include "tim.h"

TIM_HandleTypeDef htim14;
TIM_HandleTypeDef t16;

void MX_TIM14_Init(void)
{
  TIM_OC_InitTypeDef sConfigOC = {0};

  htim14.Instance = TIM14;
  //htim14.Init.Prescaler = 160-1;
  htim14.Init.Prescaler = 500-1;
  htim14.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim14.Init.Period = 2000-1;
  htim14.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim14.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim14) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim14) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 150;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim14, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_TIM_MspPostInit(&htim14);

}

void MX_TIM16_Init(void)
{
  TIM_OC_InitTypeDef sConfigOC = {0};

  t16.Instance = TIM16;
  //htim14.Init.Prescaler = 160-1;
  t16.Init.Prescaler = 100;
  t16.Init.CounterMode = TIM_COUNTERMODE_UP;
  t16.Init.Period = 2000-1;
  t16.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  t16.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&t16) != HAL_OK)
  {
    Error_Handler();
  }
/*  if (HAL_TIM_PWM_Init(&htim14) != HAL_OK)
  {
    Error_Handler();
  }

  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 150;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim14, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }*/
  HAL_TIM_MspPostInit(&t16);
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{

  if(tim_baseHandle->Instance==TIM14)
  {
    /* TIM14 clock enable */
    __HAL_RCC_TIM14_CLK_ENABLE();
  }
  if(tim_baseHandle->Instance == TIM16)
    __HAL_RCC_TIM16_CLK_ENABLE();
}
void HAL_TIM_MspPostInit(TIM_HandleTypeDef* timHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(timHandle->Instance==TIM14)
  {
    __HAL_RCC_GPIOC_CLK_ENABLE();
    /**TIM14 GPIO Configuration    
    PC12     ------> TIM14_CH1 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM14;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
  }
  if(timHandle->Instance==TIM16)
  {
    __HAL_RCC_GPIOD_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM16;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
  }

}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle)
{

  if(tim_baseHandle->Instance==TIM14)
  {
    __HAL_RCC_TIM14_CLK_DISABLE();
  }
  if(tim_baseHandle->Instance==TIM16)
  {
    __HAL_RCC_TIM16_CLK_DISABLE();
  }
} 

void set_pwm_value(uint16_t val)
{
	TIM_OC_InitTypeDef sConfigOC = {0};
	
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = val;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	
	HAL_TIM_PWM_ConfigChannel(&htim14, &sConfigOC, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim14, TIM_CHANNEL_1);
}

