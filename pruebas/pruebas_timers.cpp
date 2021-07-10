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
    general_timer t16(GeneralTimer::TIM16, general_timer::Mode::Periodic, 0x8, 0x100); //cada tick es de 1ms, con HSI de 16 MHz
    t16.enable_output_compare(0x1);
    t16.start();
    
    basic_timer t6(BasicTimer::TIM6, basic_timer::Mode::Periodic, 0x1800, 0x800);
    t6.enable_interrupt(led_callback);
    t6.start();

    general_timer t17(GeneralTimer::TIM17, general_timer::Mode::OnePulseMode, 0x2000, 0x800);
    t17.enable_interrupt(callback1);
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
