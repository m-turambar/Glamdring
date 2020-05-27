//
// Created by migue on 27/05/2020.
//

#ifndef G070_MPU6050_H
#define G070_MPU6050_H

#include "mi2c.h"

class MPU6050 {
public:
  MPU6050();
  int set_sampling_rate();
  int posicionar_en_registro_ax();
  int leer(uint8_t * buffer, size_t nbytes);

  /* process the received array and fill a float array with the right values */
  void convert_to_float(float* dest, uint8_t* raw_vals, uint8_t n_variables);

private:
  const size_t MPUADDR = 0xD0;

  constexpr static int16_t accel_divisor = 16384;
};

#endif //G070_MPU6050_H
