/**
  ******************************************************************************
  * File Name          : gpio.h
  * Description        : This file contains all the functions prototypes for 
  *                      the gpio  
  ******************************************************************************
*/
#ifndef __gpio_H
#define __gpio_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

#define INPUT GPIO_MODE_INPUT
#define OUTPUT GPIO_MODE_OUTPUT_PP
#define ANALOG GPIO_MODE_ANALOG

void MX_GPIO_Init(void);
void pinMode(GPIO_TypeDef* base, int pin, int mode);
void writePin(GPIO_TypeDef* base, int pin, int val);
int readPin(GPIO_TypeDef* base, int pin);



#ifdef __cplusplus
}
#endif
#endif /*__ pinoutConfig_H */

