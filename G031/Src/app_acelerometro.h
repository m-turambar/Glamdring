//
// Created by migue on 29/12/2021.
//

#ifndef G031_APP_ACELEROMETRO_H
#define G031_APP_ACELEROMETRO_H

#include "MPU6050.h"

extern MPU6050* mpu_ptr;
extern int mpu_init_fails;

void imprimir_acelerometro();

#endif //G031_APP_ACELEROMETRO_H
