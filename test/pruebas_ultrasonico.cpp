
general_timer t16(GeneralTimer::TIM16, general_timer::Mode::Periodic);
//algo irrelevante, pero bueno. Está esperandose 1ms después de ser iniciado, para generar un pulso de 10us.
//t16.configurar_periodo_us(1000);
t16.set_prescaler(160 - 1);
t16.set_autoreload(50000 - 1);
t16.enable_output_compare(1);
GPIO::PORTA.pin_for_timer(6,GPIO::AlternFunct::AF5);
t16.start();
tim16_ptr = &t16;

auto input_capture_callback = [] () {
  static bool rising{true};
  static uint16_t fall_cnt{0};
  static uint16_t rise_cnt{0};

  uint16_t cnt = memoria(tim17_ptr->CCR1);

  if(rising) {
    rise_cnt = cnt;
    tim17_ptr->set_capture_compare_polarity_falling();
  }

  else {
    fall_cnt = cnt;
    tim17_ptr->set_capture_compare_polarity_rising();
    uint16_t dif = (fall_cnt - rise_cnt);
    /** el ancho de pulso te dice el tiempo que la señal viajó...osea que la distancia es la mitad.
     * v = d/t -> d = t*v -> 2d = dif * 340m/s -> d = dif * 170m/s
     * Se pone complicado con las unidades. Queremos cm. Nuestra dif está en deca-micro-segundos. (10 us).
     * x metros = dif / 100,000 * 170 m/s
     * x milímetros = dif / 100 * 170
     * */
    uint16_t distancia_mm = uint16_t (float(dif * 1.7));
    uint8_t arr[6] = {0};
    int8_t idx = 0;
    while (distancia_mm > 0) {
      uint8_t digito = (distancia_mm % 10) + '0';
      arr[idx] = digito;
      ++idx;
      distancia_mm = distancia_mm / 10;
    }
    --idx;
    while(idx >= 0) {
      *g_uart2 << arr[idx];
      --idx;
    }
    *g_uart2 << '\n';
  }
  rising = !rising;
};

general_timer t17(GeneralTimer::TIM17, general_timer::Mode::Periodic);
t17.enable_input_capture(true, 10);
t17.generate_update();
t17.clear_update();
GPIO::PORTA.pin_for_timer(7,GPIO::AlternFunct::AF5);
t17.start();
t17.enable_interrupt(input_capture_callback, general_timer::InterruptType::CCIE);