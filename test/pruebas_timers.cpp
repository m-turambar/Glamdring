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

void pedazos()
{
  // este fragmento habilita dos canales del mismo timer, T2. Tiene hasta 4 canales.

  general_timer t2(GeneralTimer::TIM2, general_timer::Mode::Periodic);
  t2.set_output_compare_microsecond_resolution(10);
  t2.set_microsecond_period(20000);
  t2.set_microseconds_pulse_high(700, 1);
  t2.set_microseconds_pulse_high(700, 2);
  t2.enable_output_compare(1);
  t2.enable_output_compare(2);
  t2.start();
  GPIO::PORTA.pin_for_timer(0, GPIO::AlternFunct::AF2); // canal 1
  GPIO::PORTA.pin_for_timer(1, GPIO::AlternFunct::AF2); // canal 2
}

/*
* -void pasar_caracter(uint8_t b)
{
  auto& bb = timer_cfg_buf;
  if(b == '\n')
  {
    uint16_t pre = 0;
    uint16_t arr = 0;
    uint16_t ccr1 = 0;
    int i = 0;
    while(bb[i] != ',')
    {
      pre = pre*10 + bb[i]-48;
      ++i;
    }
    ++i;

    while(bb[i] != ',')
    {
      arr = arr*10 + bb[i]-48;
      ++i;
    }
    ++i;

    while(bb[i] != ',')
    {
      ccr1 = ccr1*10 + bb[i]-48;
      ++i;
    }
    tim_ptr->set_prescaler(pre);
    tim_ptr->set_autoreload(arr);
    tim_ptr->set_ccr1(ccr1);

    idx=0;
  }

  else
  {
    bb[idx] = b;
    ++idx;
  }

}*/

void parser(uint8_t b)
{
    static volatile uint16_t n1;
    static volatile uint16_t n2;
    static volatile uint16_t n3;
    static volatile uint8_t estado = 0;
    if(b == ',') {
        ++estado;
        if(estado > 2) {
            memoria(tim_ptr->PSC) = n1;
            memoria(tim_ptr->ARR) = n2;
            memoria(tim_ptr->CCR1) = n3;
            n1 = n2 = n3 = 0;
            estado = 0;
        }
        return;
    }

    b = b - '0';
    if(estado == 0) {
        n1 *= 10;
        n1 += b;
    }
    else if(estado == 1) {
        n2 *= 10;
        n2 += b;
    }
    else if(estado == 2) {
        n3 *= 10;
        n3 += b;
    }
}
