//
// Created by migue on 11/12/2022.
//

#include "app_acelerometro.h"
#include <cmath>

Acelerometro* g_acelerometro;

Acelerometro::Acelerometro(const I2C& i2c)
    : m_mpu(i2c)
{
    m_mpu.inicializar();
    m_mpu.set_sampling_rate();
}

void Acelerometro::reinicializar()
{
    // -_-
}

void Acelerometro::imprimir(UART& uart)
{
    char tx_buf[48] {};
    m_mpu.posicionar_en_registro(MPU6050::Reg::ACCEL_XOUT);
    m_mpu.leer(m_buf, 6);
    m_mpu.convert_to_float(m_acc, m_buf, 3);

    m_mpu.posicionar_en_registro(MPU6050::Reg::TEMP_OUT);
    m_mpu.leer(m_buf + 6, 2);
    int16_t signed_celsius = (m_buf[6] << 8) + m_buf[7];
    float deg_celsius = (static_cast<float>(signed_celsius) / 340.0) + 36.53;

//   std::sprintf(tx_buf, "ax=%.2f\t ay=%.2f\t az=%.2f\t °C=%.2f\n\r", m_acc[0], m_acc[1], m_acc[2], deg_celsius);
//   uart << tx_buf;
    auto extract_and_print = [&](float& f) {
        int integral = static_cast<int>(f);
        uart << static_cast<uint8_t>('0' + (integral / 10))
             << static_cast<uint8_t>('0' + (integral % 10));
        uart << ".";
        int whole = (int)f;
        float fraction = f - whole;
        int frac_digits = (int)(fraction * 100); // get first two digits
        uart << static_cast<uint8_t>('0' + (frac_digits / 10))
             << static_cast<uint8_t>('0' + (frac_digits % 10))
             << "\t ";
    };
    for (int i = 0; i < 3; ++i) {
        char w = 'x' + i; // x, y, z
        uart << "a" << w << "=";
        if (m_acc[i] < 0) {
            uart << "-";
            m_acc[i] *= -1;
        }
        extract_and_print(m_acc[i]);
    }
    uart << "T=";
    extract_and_print(deg_celsius);
    uart << "\n\r";
}