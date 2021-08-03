#include <cstring>
#include <cstdio>
#include <cstdlib>
//#include <array>
#include "MPU6050.h"
#include "basic_timer.h"
#include "GPIO_Port.h"
#include "RCC.h"
#include "PWR.h"
#include "FLASH.h"
#include "general_timer.h"
#include "UART.h"
#include "NVIC.h"
#include "NRF24.h"

#ifdef __cplusplus
extern "C" {
#endif

void inicializacion();
void configurar_relojes();
void error(void);

extern general_timer* tim17_ptr;
UART* g_uart2{nullptr};
NRF24* nrf_ptr{nullptr};
MPU6050* mpu_ptr{nullptr};

char itoa_buf[8] = {0};

bool parsing = false;
void parse_radio(uint8_t b)
{
  static uint8_t freq = 0;
  if(b == '/') {
    freq = 0;
    parsing = true;
    return;
  }
  if(b == 'r') {
    uint8_t freq = nrf_ptr->leer_registro(NRF24::Registro::RF_CH);
    itoa(freq, itoa_buf, 2);
    *g_uart2 << "\r\nfreq == " << itoa_buf << "\r\n";
    memset(itoa_buf, 0, 8);
    parsing = false;
    return;
  }
  if(b == '.') {
    nrf_ptr->escribir_registro(NRF24::Registro::RF_CH, freq);
    itoa(freq, itoa_buf, 2);
    *g_uart2 << "\r\nescribiendo freq = " << itoa_buf << "\r\n";
    memset(itoa_buf, 0, 8);
    parsing = false;
    return;
  }
  freq = freq * 10 + b;
}

void callback_uart2()
{
  auto& UART2 = *g_uart2;
  if(UART2.available()) {
    const uint8_t b = UART2.read_byte();
    /*
    if(b == '/' or parsing) {
      parse_radio(b);
    }
    else {
      UART2 << b;
    }
     */
    UART2 << b;
  }
}



const char* numeros = "012345678\n";
uint16_t idx = 0;

void transmitir_tx(void)
{
  if(nrf_ptr ==nullptr)
    return;
  nrf_ptr->clear_interrupts();
  nrf_ptr->flush_tx_fifo();
  const uint8_t b = numeros[idx%10];
  nrf_ptr->transmitir_byte(b);
  ++idx;
}

bool checar_rx(uint8_t& dato)
{
  NRF24& rx = *nrf_ptr;
  volatile uint8_t status = rx.leer_registro(NRF24::Registro::Status);
  /** El bit[6] del STATUS register es RX_DR e indica Data Ready. Se setea cuando llegan datos*/
  if((status & (1 << 6))) //que significa 1 << 6 por el amor de dios
  {
    dato = rx.leer_rx();
    rx.clear_interrupts(); //TODO solo clear RX_DR
    rx.flush_rx_fifo();
    return true;
  }
  return false;
}

int main(void)
{
  inicializacion();
  configurar_relojes();

  RCC::enable_port_clock(RCC::GPIO_Port::A);
  RCC::enable_port_clock(RCC::GPIO_Port::B);

  GPIO::PORTA.salida(12); //LED

  I2C i2c1(I2C::Peripheral::I2C1);
  i2c1.enable(I2C::Timing::Standard);

  MPU6050 mpu(i2c1); //instancia que representa a nuestro acelerómetro
  mpu.set_sampling_rate();
  mpu_ptr = &mpu;

  ///////////////
/*
  const GPIO::pin cen(GPIO::PORTA, 11); //TODO
  cen.salida();

  const GPIO::pin nss(GPIO::PORTB, 0);
  nss.salida(); //considerar hacer por hardware

  SPI spi1(SPI::Peripheral::SPI1_I2S1);
  spi1.inicializar();

  NRF24 radio(spi1, nss, cen);
  nrf_ptr = &radio;
  radio.config_default();
  auto config = radio.leer_registro(NRF24::Registro::Config);
  radio.encender(NRF24::Modo::RX);
  config = radio.leer_registro(NRF24::Registro::Config);
  auto rf_ch = radio.leer_registro(NRF24::Registro::RF_CH);
  radio.escribir_registro(NRF24::Registro::RF_CH, 0b111111);
  rf_ch = radio.leer_registro(NRF24::Registro::RF_CH);

  ///////////////


  auto callback_rx = []() {
    GPIO::PORTA.toggle(12);
    //*g_uart2 << "huh\n";
    uint8_t dato = 0;
    if(checar_rx(dato)) {
      *g_uart2 << dato;
    }
  };

  auto callback_tx = []() {
    GPIO::PORTA.toggle(12);
    transmitir_tx();
  };
*/



  auto callback_MPU = []() {
    uint8_t buf[16] = {0};
    char tx_buf[32] = {0};
    float acc[3] = {0};

    GPIO::PORTA.toggle(12);

    mpu_ptr->posicionar_en_registro_ax();
    mpu_ptr->leer(buf, 6);
    mpu_ptr->convert_to_float(acc, buf, 3);

    std::sprintf(tx_buf, "ax=%.2f\t ay=%.2f\t az=%.2f\n\r", acc[0], acc[1], acc[2]);

    *g_uart2 << tx_buf;
  };


  general_timer t17(GeneralTimer::TIM17, general_timer::Mode::Periodic);
  t17.configurar_periodo_ms(200);
  t17.generate_update();
  t17.clear_update();
  t17.enable_interrupt(callback_MPU, general_timer::InterruptType::UIE);
  tim17_ptr = &t17;
  t17.start();


  UART uart2(UART::Peripheral::USART2, 115200);
  g_uart2 = &uart2;
  uart2.enable();
  uart2.enable_interrupt_rx(callback_uart2);
  uart2 << "boot\n";



  while(true)
  {

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


#ifdef __cplusplus
}
#endif