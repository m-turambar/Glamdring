//
// Created by migue on 29/12/2021.
//


#include "app_timers.h"
#include "app_rele.h"


/*
void toggle_rele()
{
  static bool bb; // mala idea. Necesitas leer el pin. Deberia ser facil
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

void encender_rele_en_ms(GPIO::pin& rele, uint16_t milisegundos)
{
  tim16_ptr->configurar_periodo_ms(milisegundos);
  tim16_ptr->callback_update = encender_rele;
  tim16_ptr->start();
}*/


void encender_rele_durante(basic_timer* tim, GPIO::pin& rele, uint16_t milisegundos)
{
    encender_rele(rele);
    tim->configure_mode(basic_timer::Mode::OnePulseMode);
    tim->configurar_periodo_ms(milisegundos);
    tim->callback_data = &rele;
    tim->callback = [](void* arg) {
        auto p = static_cast<GPIO::pin*>(arg);
        apagar_rele(*p);
    };
    tim->generate_update();
    tim->start();
}

/**************************************************/

void encender_rele(GPIO::pin& rele)
{
  rele.set_output();
}

void apagar_rele(GPIO::pin& rele)
{
  rele.reset_output();
};
