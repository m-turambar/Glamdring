//
// Created by migue on 29/12/2021.
//

#ifndef G031_APP_ACELEROMETRO_H
#define G031_APP_ACELEROMETRO_H

#include "MPU6050.h"
#include "UART.h"

class Acelerometro
{
  MPU6050 m_mpu;
  uint8_t m_buf[16] = {0};
  float m_acc[3] = {0};

public:
  Acelerometro(const I2C& i2c);
  void reinicializar();
  void imprimir(UART& uart);
};

extern Acelerometro* g_acelerometro;


#endif //G031_APP_ACELEROMETRO_H
