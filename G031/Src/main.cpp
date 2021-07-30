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

/** Esto también es código de aplicación */
UART* g_uart2{nullptr};

void callback_uart2()
{
  auto& UART2 = *g_uart2;
  constexpr static flag RXNE(5);
  if(UART2.ISR.is_set(RXNE)) {
    const uint8_t b = UART2.read_byte();

    UART2 << b;
  }
}

int main(void)
{
  inicializacion();
  configurar_relojes();

  RCC::enable_port_clock(RCC::GPIO_Port::A);

  GPIO::PORTA.salida(12); //LED

  auto toggle_led = []() {
    GPIO::PORTA.toggle(12);
  };

  general_timer t17(GeneralTimer::TIM17, general_timer::Mode::Periodic);
  t17.configurar_periodo_ms(100);
  t17.generate_update();
  t17.clear_update();
  t17.enable_interrupt(toggle_led, general_timer::InterruptType::UIE);
  tim17_ptr = &t17;
  t17.start();

  UART uart2(UART::Peripheral::USART2, 115200);
  g_uart2 = &uart2;
  //uart2.enable_interrupt_rx(callback_uart2);
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