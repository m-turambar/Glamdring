//
// Created by migue on 29/12/2021.
//

#include <cstdio>
#include <cstring>

#include "app_acelerometro.h"
#include "app_uart.h"

MPU6050* mpu_ptr{nullptr};
int mpu_init_fails = 0;

char buf[16] = {0};
uint8_t raw_buf[6] = {0};
float acc[3] = {0};

void rellenar_buffers();

void reinicializar_acelerometro()
{
  mpu_ptr->inicializar();
  mpu_ptr->set_sampling_rate();
  //*g_uart2 << "Acelerometro reinicializado.\n";
  rellenar_buffers();
}

void rellenar_buffers()
{
  I2C::Status status = mpu_ptr->posicionar_en_registro_ax();
  status = mpu_ptr->leer(raw_buf, 6);
  mpu_ptr->convert_to_float(acc, raw_buf, 3);

  std::sprintf(buf, "ax=%.2f\t ay=%.2f\t az=%.2f\n\r", acc[0], acc[1], acc[2]);
}

void limpiar_buffers()
{
  memset(buf, 0, 16);
  memset(raw_buf, 0, 6);
  memset(acc, 0, 3);
}

void imprimir_acelerometro()
{
  rellenar_buffers();

  *g_uart2 << buf;

  limpiar_buffers();
}