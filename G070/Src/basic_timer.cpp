//
// Created by migue on 05/06/2020.
//

#include "basic_timer.h"

void basic_timer::set_prescaler(uint16_t prescaler)
{
  memoria(PSC) = prescaler;
}

void basic_timer::set_autoreload(uint16_t autoreload)
{
  memoria(ARR) = autoreload;
}