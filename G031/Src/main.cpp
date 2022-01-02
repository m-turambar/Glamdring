#include "MPU6050.h"
#include "GPIO_Port.h"
#include "RCC.h"
#include "PWR.h"
#include "FLASH.h"
#include "general_timer.h"
#include "UART.h"
#include "NVIC.h"
#include "NRF24.h"

#include "app_nrf24.h"
#include "app_uart.h"
#include "app_acelerometro.h"
#include "app_timers.h"

void inicializacion();
void configurar_relojes();
void error(void);

GPIO::pin LED(GPIO::PORTA, 12);
GPIO::pin Boton(GPIO::PORTC,15); // con pull-up interno. Apretamos y se pone a GND.
GPIO::pin Rele(GPIO::PORTA,1); // con pull-up interno. Apretamos y se pone a GND.


bool esperar_inicializar = true;

int main(void)
{
  inicializacion();
  configurar_relojes();

  RCC::enable_port_clock(RCC::GPIO_Port::A);
  RCC::enable_port_clock(RCC::GPIO_Port::B);
  RCC::enable_port_clock(RCC::GPIO_Port::C);

  LED.salida();
  Boton.entrada(); // con pull-up interno. Apretamos y se pone a GND.
  GPIO::PORTA.entrada(1, GPIO::PullResistor::NoPull); // relé

  /////////////////

  I2C i2c1(I2C::Peripheral::I2C1);
  i2c1.enable(I2C::Timing::Standard);

  /** Este chato tarda un ratito en inicializarse. Cuando no estás debuggeando, se necesita una reinicialización posterior
   * Estoy haciendo eso con un OPM timer que después se usará para otras cosas
   * */
  MPU6050 mpu(i2c1);
  mpu.inicializar();
  mpu.set_sampling_rate();
  mpu_ptr = &mpu;

  ///////////////

  UART uart2(UART::Peripheral::USART2, 115200);
  g_uart2 = &uart2;
  uart2.enable();
  uart2.enable_interrupt_rx(callback_uart2);
  uart2 << "Hola\n";

  //////////////

  const GPIO::pin radio_en(GPIO::PORTA, 11);
  const GPIO::pin radio_irq(GPIO::PORTA, 4);
  const GPIO::pin radio_nss(GPIO::PORTB, 0);

  SPI spi1(SPI::Peripheral::SPI1_I2S1);
  spi1.inicializar();

  NRF24 radio(spi1, radio_nss, radio_en);
  nrf_ptr = &radio;
  radio.config_default();
  radio.encender(NRF24::Modo::TX);
  radio.escribir_registro(NRF24::Registro::RF_CH, 0b100000);

  radio.rx_dr_callback = callback_nrf24_rx;
  radio.max_rt_callback = callback_nrf24_max_rt; //
  radio_irq.pin_for_interrupt(EXTI4_15_IRQn);

  ///////////////

  general_timer t17(GeneralTimer::TIM17, general_timer::Mode::Periodic);
  tim17_ptr = &t17;
  t17.configurar_periodo_ms(50);
  t17.generate_update();
  t17.clear_update();
  t17.enable_interrupt(callback_tim17, general_timer::InterruptType::UIE);
  t17.start();

  /** Este timer se está usando dos veces. Una para inicializar el acelerómetro x ms después de arrancar.
   * La segunda para el retraso automático para apagar el relevador. */
  general_timer t16(GeneralTimer::TIM16, general_timer::Mode::OnePulseMode);
  t16.configurar_periodo_ms(100);
  t16.start();
  t16.generate_update();
  t16.clear_update();
  tim16_ptr = &t16;
  t16.enable_interrupt(reinicializar_acelerometro, general_timer::InterruptType::UIE);
  t16.start();

  while(true)
  {

  }

}

void inicializacion()
{
  FLASH::prefetch_buffer_enable();
  RCC::enable_SYSCFG_clock();
  RCC::enable_power_clock();
  PWR::configurar_regulador(PWR::Voltaje::Range_1);
}

void configurar_relojes()
{
  /** Configurar los relojes del sistema según la aplicación */
  RCC::configurar_prescaler_APB(RCC::APB_Prescaler::P16);
  RCC::configurar_prescaler_AHB(RCC::AHB_Prescaler::P1);

  if(!RCC::is_HSI_ready())
    error();

  RCC::seleccionar_SYSCLK(RCC::SystemClockSwitch::HSISYS);
  RCC::SystemClockSwitch fuente_sysclk = RCC::status_SYSCLK();

  if(fuente_sysclk != RCC::SystemClockSwitch::HSISYS)
    error();

  RCC::configurar_prescaler_APB(RCC::APB_Prescaler::P1);

  /** Configurar los relojes de los periféricos, sus fuentes. */
  RCC::seleccionar_reloj_USART2(RCC::RelojesUsart::PCLK);
}


void error(void)
{
  /* User can add his own implementation to report the HAL error return state */
  while (1);
}
