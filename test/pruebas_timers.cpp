/** Esta documentación no está bien escrita. Hay fragmentos de diferentes épocas. */

/** Para conseguir ondas cuadradas, se debe seguir el siguiene patrón:
 * PSC, 2x - 1, x,
 * el PSC solo determina cuánto valdrá cada unidad del segundo argumento. El tercer argumento solo
 * controla lo ancho del pulso. No controla la frecuencia. La frecuencia la controlan los primeros dos.
 * 1000/8 = 125.
 * Es la última frecuencia no fraccionaria. Así que para periodos exactos de 1ms basados en frecuencia base de 16MHz,
 * nuestra frecuencia más baja será de 125 KHz.
 * 63,1,1, consigue una frecuencia de 125Khz, y un pulso simétrico de 4us
 * 7999,1,1, consigue una frecuencia de 1KHz, y un pulso simétrico de 500us
 *
 * OK! figured it out. Tu frecuencia base son 8MHz. (Tu reloj son 16 entonces es curioso esto)
 * El prescaler divide esa frecuencia por el valor del prescaler + 1. Osea, si usas prescaler de 0,
 * tu frecuencia base es de 8 MHZ. Tus pulsos se vuelven de 62.5ns dado que el periodo completo es de 125ns.
 * Entonces el precaler te ayuda a dividir esa frecuencia a lo que tu quieras.
 * Si divides 8MHz entre 8000, obtienes 1KHz. Entonces tu prescaler debe valer 7999 para obtener ese KHz de
 * frecuencia base!
 * */

/** TODO
 * Hay que hacer pruebas reales en este archivo. Que sea compilable y las pruebas ejecutables por un framework de
 * testing. */

