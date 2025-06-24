#ifndef GLAMDRING_RCC_H
#define GLAMDRING_RCC_H

#ifdef STM32L475xx
#include "RCC_L475.h"
#elif defined(STM32G070xx)
#include "RCC_G070.h"
#elif defined(STM32G031xx)
#include "RCC_G031.h"
#elif defined(STM32F767xx)
#include "RCC_F767.h"
#endif

#endif //GLAMDRING_RCC_H