#ifndef __adc_H
#define __adc_H
#ifdef __cplusplus
 extern "C" {
#endif

#include "main.h"

extern ADC_HandleTypeDef adc1;
void ADC1_Init(void);

#ifdef __cplusplus
}
#endif
#endif /*__ adc_H */