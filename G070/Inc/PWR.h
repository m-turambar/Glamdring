//
// Created by migue on 25/06/2020.
//

#ifndef G070_PWR_H
#define G070_PWR_H

#include "helpers.h"

/** Qué hacen los pull-up y pull-down de PxCRn? */
/** Aguas con SR2::FLASH_RDY. Te indica si la Flash está lista después de
 * un reset. Si bootearas desde SRAM, debes checar esa flag antes de brincar
 * a Flash. */
 /** Qué quiere decir cargar Vbat? Mi idea es que Vbat está conectado a una
  * batería externa? Al parecer hay dos registros internos, de 5k y de 1.5k.
  * CR4 se encarga de esto. */

namespace PWR {

  enum class Voltaje: size_t {
    Range_1 = 1,
    Range_2 = 2
  };

  /** Configura el rango de voltajes que el regulador interno tendrá.
   * Rango 1: Alto desempeño. 1.2V Freq <= 64MHz. Flash { Lectura y escritura normales }
   * Rango 2: Bajo desempeño. 1.0V Freq <= 16MHz. Flash { Lectura más lenta. Escritura imposible }*/
  bool configurar_regulador(const Voltaje v);
};

#endif //G070_PWR_H
