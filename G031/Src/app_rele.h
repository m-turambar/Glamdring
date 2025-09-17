//
// Created by migue on 29/12/2021.
//
#pragma once
#include "GPIO_Port.h"

void toggle_rele(GPIO::pin& rele);
void encender_rele_en_ms(GPIO::pin& rele, uint16_t milisegundos);
void encender_rele_durante(basic_timer* tim, GPIO::pin& rele, uint16_t milisegundos);

/*************************************************/
void encender_rele(GPIO::pin& rele);
void apagar_rele(GPIO::pin& rele);
