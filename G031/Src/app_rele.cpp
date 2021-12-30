//
// Created by migue on 29/12/2021.
//

#include "GPIO_Port.h"
#include "app_timers.h"

extern GPIO::pin Rele;

void toggle_rele()
{
  static bool bb;
  bb = !bb;
  if(bb) {
    Rele.entrada(GPIO::PullResistor::NoPull);
  }
  else {
    Rele.salida(GPIO::OutputType::OpenDrain);
    Rele.reset_output();
  }
};

void encender_rele()
{
  Rele.salida(GPIO::OutputType::OpenDrain);
  Rele.reset_output();
}

void apagar_rele()
{
  Rele.reset_output();
  Rele.entrada(GPIO::PullResistor::NoPull);
};

void encender_rele_en_ms(uint16_t milisegundos)
{
  tim16_ptr->configurar_periodo_ms(milisegundos);
  tim16_ptr->callback_update = encender_rele;
  tim16_ptr->start();
}

void encender_rele_durante(uint16_t milisegundos)
{
  encender_rele();
  tim16_ptr->configurar_periodo_ms(milisegundos);
  tim16_ptr->callback_update = apagar_rele;
  tim16_ptr->generate_update();
  tim16_ptr->start();
}
