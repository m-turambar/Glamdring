#pragma once

#ifdef STM32L475xx
#include "RCC_L475.h"
#elif defined(STM32G070xx)
#include "G070/RCC_G070.h"
#elif defined(STM32G031xx)
#include "G031/RCC_G031.h"
#elif defined(STM32F767xx)
#include "F767/RCC_F767.h"
#endif
