#include "MPU6050.h"

MPU6050::MPU6050()
{
  uint8_t buf[2];
  buf[0] = 0x6B;
  buf[1] = 0;
  mI2C2::write(MPUADDR, buf, 2);
}

int MPU6050::set_sampling_rate()
{
  uint8_t buf[2];
  buf[0] = 0x19;
  buf[1] = 0x07;
  return mI2C2::write(MPUADDR, buf, 2); //configurar frecuencia de muestreo a 1KHz
}

int MPU6050::posicionar_en_registro_ax()
{
  uint8_t buf[2];
  buf[0] = 0x3B;
  buf[1] = 0x0;
  return mI2C2::write(MPUADDR, buf, 1); //registro del acelerometro X.
}

int MPU6050::leer(uint8_t *rx_buf, const size_t nbytes)
{
  return mI2C2::read(MPUADDR, rx_buf, nbytes); //lectura
}

void MPU6050::convert_to_float(float *dest, uint8_t *raw_vals, uint8_t n_variables)
{
  for(int i=0; i<n_variables; ++i)
  {
    const int16_t accel_int = (raw_vals[i*2] << 8) + raw_vals[i*2+1];
    const float aceleracion = (float)accel_int/(float)MPU6050::accel_divisor;
    dest[i] = aceleracion;
  }
}