#ifndef MIKE_RCC_H
#define MIKE_RCC_H

#ifdef STM32L475xx
#include "RCC_L475.h"
#else
#include "RCC_G070.h"
#endif

#endif