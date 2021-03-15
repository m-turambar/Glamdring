
void set_pwm_value(uint16_t val)
{
  TIM_OC_InitTypeDef sConfigOC = {0};

  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = val;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;

  HAL_TIM_PWM_ConfigChannel(&htim14, &sConfigOC, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim14, TIM_CHANNEL_1);
}

set_pwm_value(pwm_val);

general_timer* tim_ptr{nullptr};
std::array<uint8_t, 16> timer_cfg_buf{0xFF};
uint8_t idx=0;

void pasar_caracter(uint8_t b)
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

}

/** Esto también es código de aplicación */
UART* g_uart2{nullptr};

void callback_uart2()
{
  auto& UART2 = *g_uart2;
  constexpr static flag RXNE(5);
  if(UART2.ISR.is_set(RXNE)) {
    const uint8_t b = UART2.read_byte();
    UART2 << b;
    if(tim_ptr != nullptr)
      pasar_caracter(b);
  }
}


int main(void)
{
  inicializacion();
  configurar_relojes();

  RCC::enable_port_clock(RCC::GPIO_Port::A);
  RCC::enable_port_clock(RCC::GPIO_Port::C);

  GPIO::PORTA.salida(5); //LED
  GPIO::PORTA.pin_for_timer(6,GPIO::AlternFunct::AF5);

  auto toggle_led = []() { GPIO::PORTA.toggle(5); };

  basic_timer t7(BasicTimer::TIM7, basic_timer::Mode::Periodic, 0x1800, 0x200);
  t7.enable_interrupt(toggle_led);
  t7.start();

  UART uart2(UART::Peripheral::USART2, 115200);
  g_uart2 = &uart2;
  uart2.enable_interrupt_rx(callback_uart2);
  uart2.enable_fifo().enable();


  general_timer t16(GeneralTimer::TIM16, general_timer::Mode::Periodic, 0x13f, 0x3e8);
  t16.configurar_periodo_us(8);
  t16.enable_output_compare(4);
  t16.start();
  tim_ptr = &t16;

  while(true)
  {

  }

}