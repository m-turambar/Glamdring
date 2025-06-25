#ifdef STM32L475xx
#include "NVIC_L475.h"
#elif defined(STM32G070xx)
#include "G070/NVIC_G070.h"
#elif defined(STM32G031xx)
#include "G031/NVIC_G031.h"
#elif defined(STM32F767xx)
#include "F767/NVIC_F767.h"
#endif