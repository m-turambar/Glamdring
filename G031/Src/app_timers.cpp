//
// Created by migue on 29/12/2021.
//

#include "GPIO_Port.h"
#include "app_timers.h"
#include "app_nrf24.h"

general_timer* tim16_ptr{nullptr};
general_timer* tim17_ptr{nullptr};

extern GPIO::pin LED;
extern GPIO::pin Boton;

void callback_tim17()
{
  uint8_t voltaje_boton = Boton.read_input();
  if(voltaje_boton == 0) {
    LED.toggle();
    *nrf_ptr << 't';
  }
  else {
    LED.reset_output();
  }
};

void callback_tim16()
{
  LED.toggle();
};

void callback_tim6()
{
  LED.toggle();
};

void callback_tim7()
{
  uint8_t voltaje_boton = Boton.read_input();
  if(voltaje_boton == 0) {
    LED.toggle();
    *nrf_ptr << 't';
  }
  else {
    LED.reset_output();
  }
};




