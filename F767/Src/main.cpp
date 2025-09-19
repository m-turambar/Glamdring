#include <cstring>
#include <cstdio>
#include "basic_timer.h"
#include "general_timer.h"
#include "GPIO_Port.h"
#include "RCC.h"
#include "FLASH.h"
#include "UART.h"
#include "NVIC.h"
#include "SPI.h"
#include "DAC.h"

#include "app_acelerometro.h"
#include "Procesador.h"

#ifdef __cplusplus
extern "C" {
#endif

void inicializacion();
void configurar_relojes();
void error(void);

// GPIO::pin LED_Verd(GPIO::PORTA, 5);
GPIO::pin LED_Azul(GPIO::PORTB, 7);
GPIO::pin LED_Rojo(GPIO::PORTB, 14);
Procesador procesador;

void toggle_led(void* unused)
{
  LED_Azul.toggle();
  LED_Rojo.toggle();
  // LED_Verd.toggle();
};

void serial_hb()
{
  static bool b {true};
  if (b)
    *g_uart3 << "Ping";
  else
    *g_uart3 << "Pong";
  b = !b;
}


// Imprime a través de USB (UART3) lo que te llegó
void callback_uart2()
{
  if(g_uart2->available())
  {
    const uint8_t b = g_uart2->read_byte();
    *g_uart3 << b;
  }
}

Buffer uart3_buf;
void callback_uart3()
{
  if(g_uart3->available())
  {
    const uint8_t b = g_uart3->read_byte();
    uart3_buf.escribir(b);
  }
}

int main(void)
{
  inicializacion();
  configurar_relojes();

  RCC::enable_port_clock(RCC::GPIO_Port::A);
  RCC::enable_port_clock(RCC::GPIO_Port::B);

  LED_Azul.salida();
  LED_Rojo.salida();
  LED_Rojo.set_output();
  // LED_Verd.salida();

  // St-link uart
  UART uart3(UART::Peripheral::USART3, 115200);
  uart3.enable();
  g_uart3 = &uart3;
  uart3 << "hola";
  uart3.enable_interrupt_rx(callback_uart3);
  uart3.enable_fifo().enable();

  
  // UART uart2(UART::Peripheral::USART2, 9600);
  // uart2.enable();
  // g_uart2 = &uart2;
  // uart2.enable_interrupt_rx(callback_uart2);
  // uart2.enable_fifo().enable();

  I2C i2c1(I2C::Peripheral::I2C1);
  i2c1.enable(I2C::Timing::Standard);

  Acelerometro mpu(i2c1);
  g_acelerometro = &mpu;
  procesador.accel_hook = [] (uint16_t _) {
    g_acelerometro->imprimir(*g_uart3);
  };

  // general_timer t17(GeneralTimer::TIM17, general_timer::Mode::Periodic);
  // t17.configurar_periodo_ms(50);
  // t17.generate_update();
  // t17.clear_update();
  // t17.enable_interrupt(callback_tim17, general_timer::InterruptType::UIE);
  // t17.start();

  basic_timer t7(BasicTimer::TIM7);
  t7.configure_mode(basic_timer::Mode::Periodic);
  t7.configurar_periodo_ms(1000);
  t7.generate_update();
  t7.clear_update();
  t7.enable_interrupt(toggle_led);
  t7.start();

  general_timer t2(GeneralTimer::TIM2);
  t2.configure(general_timer::Mode::Periodic);
  t2.set_output_compare_microsecond_resolution(10);
  t2.set_microsecond_period(20000);
  t2.set_microseconds_pulse_high(1000, 1);
  t2.set_microseconds_pulse_high(1000, 2);
  t2.set_microseconds_pulse_high(1000, 3);
  t2.enable_output_compare(1);
  t2.enable_output_compare(2);
  t2.enable_output_compare(3);
  t2.start();
  GPIO::PORTA.pin_for_timer(0, GPIO::AlternFunct::AF1); // canal 1
  GPIO::PORTA.pin_for_timer(1, GPIO::AlternFunct::AF1); // canal 2
  GPIO::PORTA.pin_for_timer(2, GPIO::AlternFunct::AF1); // canal 3
  procesador.pwm_hook = [](uint16_t pwm_pulse_width, uint8_t pwm_canal) { 
    tim2_ptr->set_microseconds_pulse_high(pwm_pulse_width, pwm_canal);
  };

  DAC::Config dac_config
  {
    DAC::Canal::CH1,
    0x0B, // Amplitud (MAMP)
    DAC::Wave::None,
    DAC::Trigger::Timer6,
    true, // TEN
    false, // BOFF disable
  };

  DAC dac(dac_config);
  dac.enable();
  dac_ptr = &dac;
  procesador.dac_hook = [](uint16_t dac_data) { 
    dac_ptr->write_12R(dac_data);
  };

  basic_timer t6(BasicTimer::TIM6);
  t6.configure_mode(basic_timer::Mode::Periodic);
  // t6.configurar_periodo_ms(2);
  t6.configurar_periodo_us(40);
  t6.configure_master_mode(basic_timer::MasterMode::Update); // Para triggerear DAC
  t6.generate_update();
  t6.clear_update();
  tim6_ptr = &t6;
  t6.start();
  procesador.freq_hook = [](uint16_t microseconds_period) {
    tim6_ptr->configurar_periodo_us(microseconds_period);
  };

  while (1) {
    if (uart3_buf.available())
    {
      uint8_t b = uart3_buf.leer();
      *g_uart3 << b;
      procesador.procesar_mensaje(b);
    }
  }
}

void inicializacion()
{
  FLASH::prefetch_buffer_enable();
  RCC::enable_SYSCFG_clock();
  RCC::enable_power_clock();
  //NVIC_SetPriority(PendSV_IRQn, 3, 0);
  //PWR::configurar_regulador(PWR::Voltaje::Range_1); //El driver de PWR debe ser diferente para cada micro
}

void configurar_relojes()
{
  /** Configurar los relojes del sistema según la aplicación */
  RCC::encender_HSI16();

  RCC::configurar_prescaler_APB1(RCC::APB_Prescaler::P16);
  RCC::configurar_prescaler_APB2(RCC::APB_Prescaler::P16);
  RCC::configurar_prescaler_AHB(RCC::AHB_Prescaler::P1);

  RCC::seleccionar_SYSCLK(RCC::SystemClockSwitch::HSI16);
  RCC::SystemClockSwitch fuente_sysclk = RCC::status_SYSCLK();

  if(!RCC::is_HSI_ready())
    error();

  if(fuente_sysclk != RCC::SystemClockSwitch::HSI16)
    error();

  RCC::configurar_prescaler_APB1(RCC::APB_Prescaler::P1);
  RCC::configurar_prescaler_APB2(RCC::APB_Prescaler::P1);

  /** Configurar los relojes de los periféricos, sus fuentes. */
  RCC::seleccionar_reloj_USART2(RCC::RelojesUsart::PCLK);
}


void error(void)
{
  /* User can add his own implementation to report the HAL error return state */
  while (1);
}


#ifdef __cplusplus
}
#endif