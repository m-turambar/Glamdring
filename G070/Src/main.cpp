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

general_timer* tim_ptr{nullptr};
basic_timer* basic_tim_ptr{nullptr};
uint8_t idx=0;

/** Esto también es código de aplicación */
UART* g_uart2{nullptr};
void parser(uint8_t b);

void callback_uart2()
{
  auto& UART2 = *g_uart2;
  constexpr static flag RXNE(5);
  if(UART2.ISR.is_set(RXNE)) {
    const uint8_t b = UART2.read_byte();

    if(tim_ptr != nullptr)
        parser(b);

    UART2 << b;
  }
}

void parser(uint8_t b)
{
    static volatile uint16_t n1;
    static volatile uint16_t n2;
    static volatile uint16_t n3;
    static volatile uint8_t estado = 0;
    if(b == 'z') {
        n1 = n2 = n3 = 0;
        estado = 0;
        return;
    }
    if(b == 't') {
        if(basic_tim_ptr != nullptr) {
            basic_tim_ptr->start();
        }
    }
    if(b == ',') {
        ++estado;
        if(estado > 2) {
            memoria(tim_ptr->PSC) = n1;
            memoria(tim_ptr->ARR) = n2;
            memoria(tim_ptr->CCR1) = n3;
            n1 = n2 = n3 = 0;
            estado = 0;
        }
        return;
    }

    b = b - '0';
    if(estado == 0) {
        n1 *= 10;
        n1 += b;
    }
    else if(estado == 1) {
        n2 *= 10;
        n2 += b;
    }
    else if(estado == 2) {
        n3 *= 10;
        n3 += b;
    }
}

int main(void)
{
  inicializacion();
  configurar_relojes();

  RCC::enable_port_clock(RCC::GPIO_Port::A);
  RCC::enable_port_clock(RCC::GPIO_Port::C);

  GPIO::PORTA.salida(5); //LED


  auto toggle_led = []() {
    GPIO::PORTA.toggle(5);
  };

  basic_timer t7(BasicTimer::TIM7, basic_timer::Mode::OnePulseMode);
  t7.configurar_periodo_ms(1000);
  t7.generate_update();
  t7.clear_update();
  t7.enable_interrupt(toggle_led);
  basic_tim_ptr = &t7;
  //t7.start();

  UART uart2(UART::Peripheral::USART2, 115200);
  g_uart2 = &uart2;
  uart2.enable_interrupt_rx(callback_uart2);
  uart2.enable_fifo().enable();


  general_timer t16(GeneralTimer::TIM16, general_timer::Mode::Periodic);
  t16.configurar_periodo_us(1000);
  t16.enable_output_compare(4);
  GPIO::PORTA.pin_for_timer(6,GPIO::AlternFunct::AF5);
  t16.start();
  tim_ptr = &t16;

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