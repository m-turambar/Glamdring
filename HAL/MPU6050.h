//
// Created by migue on 27/05/2020.
//

#ifndef GLAMDRING_MPU6050_H
#define GLAMDRING_MPU6050_H

#include "I2C.h"

class MPU6050 {
  const size_t MPUADDR = 0xD0;

  constexpr static int16_t accel_divisor = 16384;
  const I2C& mi2c;
public:
  enum class Reg
  {
    ACCEL_XOUT = 0x3B,
    TEMP_OUT = 0x41
  };

  explicit MPU6050(const I2C& i2c_arg);
  I2C::Status inicializar() const;
  I2C::Status set_sampling_rate() const;
  I2C::Status posicionar_en_registro(Reg reg) const { return posicionar_en_registro(static_cast<uint8_t>(reg)); };
  I2C::Status posicionar_en_registro(const uint8_t reg) const;
  I2C::Status leer(uint8_t * buffer, size_t nbytes) const ;

  /* process the received array and fill a float array with the right values */
  void convert_to_float(float* dest, uint8_t* raw_vals, uint8_t n_variables) const;


};

#endif //GLAMDRING_MPU6050_H
