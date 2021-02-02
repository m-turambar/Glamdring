//
// Created by migue on 12/01/2021.
//

#ifndef G070_DMA_H
#define G070_DMA_H

#include "helpers.h"

/** DMA
 * Es un bus master y periférico al mismo tiempo
 * Se usa para realizar transferencias programables de datos entre periféricos
 * mapeados a memoria y/o memoria.
 * Arquitectura AHB master (?)
 * Una instancia de DMA con 7 canales.
 * Cada canal se dedica a manejar peticiones de acceso a memoria por parte de
 * uno o más periféricos.
 * El DMA incluye un árbitro para manejar la prioridad de las peticiones.
 *
 * Cada canal es independientemente configurable:
 * Cada canal se asocia ya sea como una señal proveniente de un periférico, o
 * con un trigger de software para transferencias de memoria a memoria.
 * Esta configuración se realiza por software.
 * La prioridad entre las peticiones se programa por software. 4 Niveles por canal.
 * Los empates se deciden por hardware. Canal 1 tiene más prioridad que canal 2.
 *
 * El tamaño de la transferencia de la fuente al destino son independientes:
 * byte, half-word, word.
 * La fuente y el destino deben estár alineados en el tamaño de los datos.
 *
 * Cada transferencia puede ser de 0 a 2^16-1 bytes
 *
 * Generación de interrupción por cada canal. Cada interrupción puede ser causada
 * por cualquiera de los 3 eventos del DMA:
 *
 * 1. Transferencia completa
 * 2. Media Transferencia
 * 3. Error de transferencia
 *
 * */

namespace DMA {

}



#endif //G070_DMA_H
