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

int main(void)
{
  inicializacion();
  configurar_relojes();

  RCC::enable_port_clock(RCC::GPIO_Port::A);
  RCC::enable_port_clock(RCC::GPIO_Port::C);
  GPIO::PORTA.salida(5);
  GPIO::PORTC.salida(9);

  auto led_callback = [](void) -> void {
    GPIO::PORTA.toggle(5);
    GPIO::PORTC.toggle(9);
  };

  basic_timer t7(BasicTimer::TIM7, basic_timer::Mode::Periodic, 0x1800, 0x1000);
  t7.enable_interrupt(led_callback);
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
  PWR::configurar_regulador(PWR::Voltaje::Range_1);
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