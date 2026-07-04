#include "NVIC.h"
#include "Procesador.h"
#include "UART.h"

void null_execution(Buffer& buf) {}
void null_execution(uint16_t a, uint8_t b) {}
void null_execution(uint16_t a) { null_execution(a, 42);}
void null_execution(uint8_t b) {}
void default_reset_hook(uint8_t b) {
    if (b == 'r') {
        if (g_uart1) {
            *g_uart1 << "System reset.\n";
        }
        __NVIC_SystemReset();
    }
};

Procesador::Proceso seleccionar_proceso(uint8_t b)
{
    using Proceso = Procesador::Proceso;
    Procesador::Proceso proceso { Proceso::None };
    switch(b) {
        case 'p':
            proceso = Proceso::PWM;
            break;
        case 'f':
            proceso = Proceso::Freq;
            break;
        case 'd':
            proceso = Proceso::DAC;
            break;
        case 'a':
            proceso = Proceso::Accel;
            break;
        case 'n':
            proceso = Proceso::NRF;
            break;
        case 'g':
            proceso = Proceso::GPIO;
            break;
        case 'r':
            proceso = Proceso::Reset;
            break;
        case '/':
            proceso = Proceso::Test;
            break;
        default:
            proceso = Proceso::None;
    }
    return proceso;
}


// e.g. {pa1000}, {pb1500}, {pc800}
void Procesador::procesar_mensaje(uint8_t b)
{
    if (!procesando) {
        if (b == '{') {
            procesando = true;
        }
        return;
    }

    if (b == '}' && (brace_cnt == 0)) {
        procesando = false;
        ejecutar_mensaje();
        clear_status();
        return;
    }

    if (proceso == Proceso::None) {
        proceso = seleccionar_proceso(b);
    } else {
        if (b == '{')
            ++brace_cnt;
        if (b == '}')
            --brace_cnt;
        bool result = procesar_interno(b);
        if (!result) {
            procesando = false;
            clear_status();
        }
    }

    return;
}

bool Procesador::procesar_interno(const uint8_t b)
{
    switch (proceso) {
    case Proceso::PWM:
        if (pwm_canal == 0) {
            pwm_canal = b - 'a' + 1; // 'a' para canal 1, 'b' para canal 2, 'c' para 3, 'd' para 4.
            return (pwm_canal >= 1 && pwm_canal <= 4);
        }
        if (b < '0' || b > '9')
            return false;
        pwm_pulse_width = pwm_pulse_width * 10 + b - '0';
        break;
    case Proceso::Freq:
        if (b < '0' || b > '9')
            return false;
        microseconds_period = microseconds_period * 10 + b - '0';
        break;
    case Proceso::DAC:
        if (b < '0' || b > '9')
            return false;
        dac_data = dac_data * 10 + b - '0';
        break;
    case Proceso::Accel:
        ;
        break;
    case Proceso::NRF:
        nrf_buf.escribir(b);
        break;
    case Proceso::GPIO:
        gpio_byte = b;
        break;
    case Proceso::Reset:
        reset_byte = b;
        break;
    case Proceso::Test:
        test_byte = b;
        break;
    default:
        return false;
    }
    return true;
}

// Que los consumidores implementen sus propios hooks!
// Así vuelves a procesador más portable
void Procesador::ejecutar_mensaje()
{
    if (proceso == Proceso::PWM) {
        pwm_hook(pwm_pulse_width, pwm_canal);
        // sample hook:
        // tim2_ptr->set_microseconds_pulse_high(pwm_pulse_width, pwm_canal);
    }
    if (proceso == Proceso::Freq) {
        freq_hook(microseconds_period);
        // sample hook:
        // tim6_ptr->configurar_periodo_us(microseconds_period);
    }
    if (proceso == Proceso::DAC) {
        dac_hook(dac_data);
        // sample hook:
        // dac_ptr->write_12R(dac_data);
    }
    if (proceso == Proceso::Accel) {
        accel_hook(0); 
        // sample hook:
        // g_acelerometro->imprimir(*g_uart2);
    }
    if (proceso == Proceso::NRF) {
        nrf_hook(nrf_buf);
    }
    if (proceso == Proceso::GPIO) {
        gpio_hook(gpio_byte);
    }
    if (proceso == Proceso::Reset) {
        reset_hook(reset_byte);
    }
    if (proceso == Proceso::Test) {
        test_hook(test_byte);
    }
}

void Procesador::clear_status()
{
    proceso = Proceso::None;
    procesando = false;
    pwm_pulse_width = 0;
    microseconds_period = 0;
    dac_data = 0;
    pwm_canal = 0;
    nrf_buf.clear();
    gpio_byte = 0;
    test_byte = 0;
}