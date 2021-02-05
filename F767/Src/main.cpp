#include <cstring>
#include <cstdio>
#include "basic_timer.h"
#include "GPIO_Port.h"
#include "RCC.h"
#include "PWR.h"
#include "FLASH.h"
#include "general_timer.h"
#include "UART.h"
#include "NVIC.h"

#ifdef __cplusplus
extern "C" {
#endif

void inicializacion();
void configurar_relojes();
void error(void);

uint32_t cnter{0};

void WWDG_IRQHandler(void)
{
  ++cnter;
}

UART* uart_ref{nullptr};

void print_uart(void)
{
  if(uart_ref!=nullptr)
  {
    const char* buf = "123456789abcdefghijklmnopqrstuvxyz";
    *uart_ref << buf;
  }
}

int main(void)
{
  inicializacion();
  configurar_relojes();

  RCC::enable_port_clock(RCC::GPIO_Port::B);
  //RCC::enable_port_clock(RCC::GPIO_Port::C);
  GPIO::PORTB.salida(7);
  //GPIO::PORTC.salida(9);

  UART uart3(UART::Peripheral::USART3, 115200);
  uart3.enable();
  uart_ref = &uart3;
  uart3 << "hola";

  uint8_t val{0};
  auto led_callback = [](void) -> void {
    GPIO::PORTB.toggle(7);
    //GPIO::PORTC.toggle(9);
  };

  basic_timer t6(BasicTimer::TIM6, basic_timer::Mode::Periodic, 0x1800, 0x800);
  t6.enable_interrupt(led_callback);
  t6.start();

  basic_timer t7(BasicTimer::TIM7, basic_timer::Mode::Periodic, 0x1800, 0x1000);
  t7.enable_interrupt(print_uart);
  t7.start();



  while (1) {

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