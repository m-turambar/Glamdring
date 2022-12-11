//
// Created by migue on 29/12/2021.
//

#include "GPIO_Port.h"
#include "app_timers.h"
#include "app_rele.h"

//extern GPIO::pin Rele;
extern GPIO::pin ReleA;
extern GPIO::pin ReleB;

/*
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
*/

/*
void encender_rele_durante(uint16_t milisegundos)
{
  encender_rele_a();
  tim16_ptr->configure(general_timer::Mode::OnePulseMode);
  tim16_ptr->configurar_periodo_ms(milisegundos);
  tim16_ptr->callback_update = apagar_rele_a;
  tim16_ptr->generate_update();
  tim16_ptr->start();
}
*/

void encender_rele_durante(uint16_t milisegundos)
{
  encender_rele_a();
  tim6_ptr->configure_mode(basic_timer::Mode::OnePulseMode);
  tim6_ptr->configurar_periodo_ms(milisegundos);
  tim6_ptr->callback = apagar_rele_a;
  tim6_ptr->generate_update();
  tim6_ptr->start();
}

/**************************************************/

void encender_rele_a()
{
  ReleA.set_output();
}

void encender_rele_b()
{
  ReleB.set_output();
}

void apagar_rele_a()
{
  ReleA.reset_output();
};

void apagar_rele_b()
{
  ReleB.reset_output();
};
