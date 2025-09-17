#pragma once

#include "UART.h"

void null_execution(uint8_t b = 0);
void null_execution(uint16_t a = 0);
void null_execution(uint16_t a = 0, uint8_t b = 0);
void null_execution(Buffer& buf);

struct Procesador
{
    Procesador()
    {}

    enum class Proceso {
        None,
        Accel,
        DAC,
        Freq,
        PWM,
        NRF,
        Relay,
    };

    void procesar_mensaje(uint8_t b);
    void (*dac_hook)(uint16_t dac_data) { &null_execution };
    void (*freq_hook)(uint16_t microseconds_period) { &null_execution };
    void (*accel_hook)(uint16_t) { &null_execution };
    void (*pwm_hook)(uint16_t pwm_pulse_width, uint8_t pwm_canal) { &null_execution };
    void (*nrf_hook)(Buffer& buffer) { &null_execution };
    void (*relay_hook)(uint8_t b) { &null_execution };

private:
    bool procesar_interno(const uint8_t b) ;
    void ejecutar_mensaje();
    void clear_status();

    bool procesando{false};
    Proceso proceso {Proceso::None};
    
    uint16_t pwm_pulse_width {0};
    uint16_t microseconds_period {0};
    uint16_t dac_data {0};
    uint8_t pwm_canal {0};
    Buffer nrf_buf;
    uint8_t relay_byte {0};
};