//
// Created by migue on 29/12/2021.
//

#ifndef G031_APP_TIMERS_H
#define G031_APP_TIMERS_H

#include "general_timer.h"

extern general_timer* tim17_ptr;
extern general_timer* tim16_ptr;

void callback_tim16();
void callback_tim17();

#endif //G031_APP_TIMERS_H