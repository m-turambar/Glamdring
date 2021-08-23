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

bool parsing = false;

static const char* pwd = "bishi\n";

void callback_uart2()
{
  auto& UART2 = *g_uart2;
  if(UART2.available()) {
    const uint8_t b = UART2.read_byte();

    if(b == '/' or parsing) {
      //parse_radio(b);
    }
    else {
      UART2 << b;
      if(nrf_ptr != nullptr) {
        *nrf_ptr << b;
      }
    }

  }
}

int main(void)
{
  inicializacion();
  configurar_relojes();

  RCC::enable_port_clock(RCC::GPIO_Port::A);
  RCC::enable_port_clock(RCC::GPIO_Port::B);
  RCC::enable_port_clock(RCC::GPIO_Port::C);

  GPIO::PORTA.salida(12); //LED
  GPIO::PORTC.entrada(15); //pushbutton, con pull-up interno. Apretamos y se pone a GND.

  // Agregar un tiempo de espera de algunos milisegundos para asegurar que todo ya se inicializó
  ///////////////

  auto callback_rx = []() {
    GPIO::PORTA.toggle(12);
    NRF24& rx = *nrf_ptr;

    //uint8_t status = rx.leer_registro(NRF24::Registro::Status);
    //todo lee status para saber de qué canal vino el paquete

    uint8_t fifo_status = rx.leer_registro(NRF24::Registro::FIFO_STATUS);
    while (fifo_status % 2 == 0) /// el bit menos significativo de FIFO_STATUS es RX_EMPTY
    {
      uint8_t dato = rx.leer_rx();
      *g_uart2 << dato;
      fifo_status = rx.leer_registro(NRF24::Registro::FIFO_STATUS);
    }

  };

  const GPIO::pin radio_en(GPIO::PORTA, 11);
  const GPIO::pin radio_nss(GPIO::PORTB, 0);
  const GPIO::pin radio_irq(GPIO::PORTA, 4);

  SPI spi1(SPI::Peripheral::SPI1_I2S1);
  spi1.inicializar();

  NRF24 radio(spi1, radio_nss, radio_en, radio_irq);

  nrf_ptr = &radio;
  radio.config_default();
  radio.encender(NRF24::Modo::TX);
  radio.escribir_registro(NRF24::Registro::RF_CH, 0b100000);
  radio.rx_dr_callback = callback_rx;

  auto callback_boton = []() {
    uint8_t v_boton = GPIO::PORTC.read_input(15);
    if(v_boton == 0) {
      if(nrf_ptr != nullptr) {
        GPIO::PORTA.toggle(12);
        *nrf_ptr << pwd;
      }
    }
    else {
      GPIO::PORTA.reset_output(12);
    }
  };

  general_timer t17(GeneralTimer::TIM17, general_timer::Mode::Periodic);
  t17.configurar_periodo_ms(50);
  t17.generate_update();
  t17.clear_update();
  t17.enable_interrupt(callback_boton, general_timer::InterruptType::UIE);
  tim17_ptr = &t17;
  t17.start();


  UART uart2(UART::Peripheral::USART2, 115200);
  g_uart2 = &uart2;
  uart2.enable();
  uart2.enable_interrupt_rx(callback_uart2);
  uart2 << "boom\n";


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