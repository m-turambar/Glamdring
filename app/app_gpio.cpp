//
// Created by migue on 29/12/2021.
//

#include "app_gpio.h"

void encender_pin_durante(basic_timer* tim, GPIO::pin& pin, uint16_t milisegundos)
{
    pin.set_output();
    tim->configure_mode(basic_timer::Mode::OnePulseMode);
    tim->configurar_periodo_ms(milisegundos);
    tim->callback_data = &pin;
    tim->callback = [](void* arg) {
        auto p = static_cast<GPIO::pin*>(arg);
        p->reset_output();
    };
    tim->enable_interrupt(tim->callback);
    tim->start();
}

void encender_pin_durante(general_timer* tim, GPIO::pin& pin, uint16_t milisegundos)
{
    pin.set_output();
    tim->configure(general_timer::Mode::OnePulseMode);
    tim->configurar_periodo_ms(milisegundos);
    tim->callback_data = &pin;
    tim->callback_update = [](void* arg) {
        auto p = static_cast<GPIO::pin*>(arg);
        p->reset_output();
    };
    tim->enable_interrupt(tim->callback_update, general_timer::InterruptType::UIE);
    tim->start();
}
