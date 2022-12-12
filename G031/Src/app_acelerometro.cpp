//
// Created by migue on 11/12/2022.
//

#include "app_acelerometro.h"
#include "cstdio"

Acelerometro* g_acelerometro;

Acelerometro::Acelerometro(const I2C& i2c)
: m_mpu(i2c)
{
  m_mpu.set_sampling_rate();
}

void Acelerometro::reinicializar()
{

}

void Acelerometro::imprimir(UART& uart)
{
  char tx_buf[32] {};
  m_mpu.posicionar_en_registro_ax();
  m_mpu.leer(m_buf, 6);
  m_mpu.convert_to_float(m_acc, m_buf, 3);

  std::sprintf(tx_buf, "ax=%.2f\t ay=%.2f\t az=%.2f\n\r", m_acc[0], m_acc[1], m_acc[2]);
  uart << tx_buf;
}