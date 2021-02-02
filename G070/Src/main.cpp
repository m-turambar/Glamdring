#include <cstring>
#include <cstdio>
#include "I2C.h"
#include "MPU6050.h"
#include "basic_timer.h"
#include "GPIO_Port.h"
#include "RCC.h"
#include "PWR.h"
#include "FLASH.h"
#include "general_timer.h"
#include "UART.h"
#include "SPI.h"
#include "NVIC.h"
#include "NRF24.h"

#ifdef __cplusplus
extern "C" {
#endif

void inicializacion();
void configurar_relojes();
void error(void);
void itoa(int num, unsigned char* buffer, int base);

void test_callback(void)
{
  GPIO::PORTA.toggle(5);
}

volatile uint8_t glb_flag=0;
void callback1(void)
{
  ++glb_flag;
}

/** Esto también es código de aplicación */
UART* g_uart2{nullptr};
void USART2_IRQHandler(void)
{
  auto& UART2 = *g_uart2;
  constexpr static flag RXNE(5);
  if(UART2.ISR.is_set(RXNE)) {
    const uint8_t b = UART2.read_byte();
    //UART2.callback_rx(b);
    UART2 << b;
  }
  NVIC_ClearPendingIRQ(USART2_IRQn);
}

int spi_loopback_test(SPI& spi)
{
  int n_errors = 0;
  for(uint8_t i = 0; i<255; ++i)
  {
    spi.escribir(i);
    uint8_t msg = spi.leer();
    if(msg != i)
      n_errors++;
  }
  return n_errors;
}

int test_radio_status(NRF24& radio, const uint8_t status_esperado)
{
  uint8_t status = 0;
  uint32_t err_cnt = 0;
  for(uint32_t i=0; i<1000; ++i)
  {
    status = radio.leer_registro(NRF24::Registro::Status);
    if(status != status_esperado)
      err_cnt++;
  }
  return err_cnt;
}

int test_radio_register_write_read(NRF24& radio)
{
  uint8_t val = 0;
  uint32_t err_cnt = 0;
  for(uint32_t i=0; i<128; ++i)
  {
    radio.escribir_registro(NRF24::Registro::Config, i);
    val = radio.leer_registro(NRF24::Registro::Config);
    if(val != i)
      err_cnt++;
  }
  return err_cnt;
}

/** Al tercer byte hay problemas, 14,30. Por? MAX_RT se activa en el YC*/
int test_radio_tx_rx(NRF24& tx, NRF24& rx)
{
  uint32_t err_cnt = 0;
  for(uint32_t i=0; i<255; ++i)
  {
    tx.transmitir_byte(i);
    volatile uint8_t status = rx.leer_registro(NRF24::Registro::Status);
    volatile uint8_t status_tx = tx.leer_registro(NRF24::Registro::Status);
    while((status & (1 << 6)) == 0)
    {
      status_tx = tx.leer_registro(NRF24::Registro::Status);
      if((status_tx & (1 << 4)) != 0) //Si MAX_RT...:
      {
        tx.clear_interrupts();
        tx.flush_tx_fifo();
        tx.transmitir_byte(i);
      }
      status = rx.leer_registro(NRF24::Registro::Status);
    }
    uint8_t rcvd = rx.recibir_byte();
    rx.clear_interrupts(); //TODO solo clear RX_DR
    tx.clear_interrupts(); //TODO solo clear TX_DS
    rx.flush_rx_fifo();
    tx.flush_tx_fifo();
    status = rx.leer_registro(NRF24::Registro::Status);
    status_tx = tx.leer_registro(NRF24::Registro::Status);
    if((status & (1 << 6)) == 0)
    {
      uint8_t recibido_de_nuevo = rx.recibir_byte();
    }

    if(rcvd != i) { err_cnt++; }
  }
  return err_cnt;
}

int main(void)
{
  inicializacion();
  configurar_relojes();
  //mIWDG::set_and_go(6, 0xFF);

  /* Initialize all configured peripherals */
  /************************************/
  RCC::enable_port_clock(RCC::GPIO_Port::A);
  RCC::enable_port_clock(RCC::GPIO_Port::C);
  GPIO::PORTA.entrada(1);
  GPIO::PORTA.salida(5);
  GPIO::PORTA.salida(6).cfg_mode(GPIO::Mode::Alternate).cfg_alternate(GPIO::AlternFunct::AF5_TIM16);
  auto blue_btn = GPIO::PORTC.entrada(13);

  /************************************/

  general_timer t16(GeneralTimer::TIM16, general_timer::Mode::Periodic, 0x8, 0x100); /* each tick is 1ms*/
  t16.enable_output_compare(0x1);
  t16.start();

  UART uart2(UART::Peripheral::USART2, 115200);
  g_uart2 = &uart2;
  uart2.enable_interrupt_rx(nullptr);
  uart2.enable_fifo().enable();

  /** Hasta que no encuentre un mejor mecanismo para hacer callbacks más sofisticados,
   * seguiré haciendo cochinadas en las interrupciones. Hay que buscar una solución. */
  UART uart3(UART::Peripheral::USART3, 9600);
  uart3.enable_interrupt_rx(nullptr).enable();

  /************************************/
  const GPIO::pin pin_enable_radio1(GPIO::PORTC, 5); //TODO
  GPIO::PORTC.salida(5);
  const GPIO::pin pin_enable_radio2(GPIO::PORTC, 4); //TODO
  GPIO::PORTC.salida(6);
  GPIO::pin ss_radio1(GPIO::PORTA, 9);
  GPIO::PORTA.salida(9);
  GPIO::pin ss_radio2(GPIO::PORTA, 10);
  GPIO::PORTA.salida(10);

  /** SPI1 tal vez sí funcionaba, pero por la paz vamos spi 2. */
  SPI spi2(SPI::Peripheral::SPI2);
  spi2.inicializar();

  NRF24 radio1(spi2, ss_radio1, pin_enable_radio1);
  NRF24 radio2(spi2, ss_radio2, pin_enable_radio2);

  radio1.config_default();
  radio2.config_default();
  auto config = radio1.leer_registro(NRF24::Registro::Config);
  auto config2 = radio2.leer_registro(NRF24::Registro::Config);

  radio2.encender(NRF24::Modo::RX);
  radio1.encender(NRF24::Modo::TX);
  auto status1 = radio1.leer_registro(NRF24::Registro::Status);
  auto status2 = radio2.leer_registro(NRF24::Registro::Status);

  auto errors = test_radio_status(radio2, 14);

  config = radio1.leer_registro(NRF24::Registro::Config);
  config2 = radio2.leer_registro(NRF24::Registro::Config);
  errors = test_radio_tx_rx(radio1, radio2);

  status2 = radio2.leer_registro(NRF24::Registro::Status);
  status1 = radio1.leer_registro(NRF24::Registro::Status);
  /************************************/

  char tx_buf[64] = "---\n";
  char greetings[32] = "Basura 123456789 Basura\n";

  auto led_callback = [](void) -> void {
    GPIO::PORTA.toggle(5);
  };
  basic_timer t6(BasicTimer::TIM6, basic_timer::Mode::Periodic, 0x1800, 0x800);
  t6.enable_interrupt(led_callback);
  t6.start();

  general_timer t17(GeneralTimer::TIM17, general_timer::Mode::OnePulseMode, 0x2000, 0x800);
  t17.enable_interrupt(callback1);

  I2C i2c2(I2C::Peripheral::I2C1);
  i2c2.enable(I2C::Timing::Standard);

  MPU6050 mpu(i2c2); //instancia que representa a nuestro acelerómetro
  mpu.set_sampling_rate();

  uart2.transmitq((const uint8_t *)greetings, strlen((const char*)greetings));
  uart2 << greetings;
  uint8_t buf[16] = {0};
  float acc[3] = {0};

  while (1) {

    if(glb_flag%2 == 1) {
      mpu.posicionar_en_registro_ax();
      mpu.leer(buf, 6);
      mpu.convert_to_float(acc, buf, 3);

      std::sprintf(tx_buf, "ax=%.2f\t ay=%.2f\t az=%.2f\n\r", acc[0], acc[1], acc[2]);

      //uart2.transmitq(tx_buf, std::strlen((const char*)tx_buf));
      uart2 << tx_buf;
      glb_flag = 0;
    }

    if(blue_btn.read_input() == 0) {
      t17.start();
    }

    if(uart2.available())
      uart3 << uart2.read_byte();
  }
}

void inicializacion()
{
  FLASH::prefetch_buffer_enable();
  RCC::enable_SYSCFG_clock();
  RCC::enable_power_clock();
  //NVIC_SetPriority(PendSV_IRQn, 3, 0);
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

/*
void set_pwm_value(uint16_t val);void set_pwm_value(uint16_t val)
{
  TIM_OC_InitTypeDef sConfigOC = {0};

  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = val;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;

  HAL_TIM_PWM_ConfigChannel(&htim14, &sConfigOC, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim14, TIM_CHANNEL_1);
}*/
//const volatile size_t pwm_val = (rx_buf[0]-'0')*100+(rx_buf[1]-'0')*10+(rx_buf[2]-'0');
// set_pwm_value(pwm_val);

#ifdef __cplusplus
}
#endif