#ifdef STM32L475xx
#include "NVIC_L475.h"
#elif defined(STM32G070xx)
#include "NVIC_G070.h"
#elif defined(STM32F767xx)
#include "NVIC_F767.h"
#endif