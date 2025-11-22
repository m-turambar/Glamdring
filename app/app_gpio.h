//
// Created by migue on 29/12/2021.
//
#pragma once
#include "GPIO_Port.h"
#include "general_timer.h"
#include "basic_timer.h"

void encender_pin_durante(basic_timer* tim, GPIO::pin& pin, uint16_t milisegundos);
void encender_pin_durante(general_timer* tim, GPIO::pin& pin, uint16_t milisegundos);

