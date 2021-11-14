#include <cstring>
#include <cstdio>
//#include <array>
//#include "I2C.h"
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

extern general_timer* tim16_ptr;
extern general_timer* tim17_ptr;

basic_timer* basic_tim_ptr{nullptr};
uint8_t idx=0;

UART* g_uart2{nullptr};


bool parsing = false;
void parse_uart(uint8_t b) {
  if (b == '/') {
    if (!parsing) {
      parsing = true;
      return;
    } else {
      *g_uart2 << b;
    }
  }
}

void callback_uart2()
{
  auto& UART2 = *g_uart2;
  if(UART2.available())
  {
    const uint8_t b = UART2.read_byte();
    if(b == '/' or parsing) parse_uart(b);
    else {
      UART2 << b;
    }
  }
}

void toggle_led() {
  GPIO::PORTA.toggle(5);
};

int main(void)
{
  inicializacion();
  configurar_relojes();

  RCC::enable_port_clock(RCC::GPIO_Port::A);
  RCC::enable_port_clock(RCC::GPIO_Port::C);

  GPIO::PORTA.salida(9); //LED
  GPIO::PORTA.salida(5); //LED


  basic_timer t7(BasicTimer::TIM7, basic_timer::Mode::Periodic);
  t7.configurar_periodo_ms(1000);
  t7.generate_update();
  t7.clear_update();
  t7.enable_interrupt(toggle_led);
  basic_tim_ptr = &t7;
  t7.start();

  UART uart2(UART::Peripheral::USART2, 115200);
  g_uart2 = &uart2;
  uart2.enable_interrupt_rx(callback_uart2);
  uart2.enable_fifo().enable();


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