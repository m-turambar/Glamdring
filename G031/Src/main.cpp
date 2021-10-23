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

general_timer* tim17_ptr;
general_timer* tim2_ptr;
UART* g_uart2{nullptr};

bool escalera = false;
uint16_t pulse_width=0;
void parse_motores(char c)
{
  if (c == ';') {
    if(tim2_ptr != nullptr) {
      tim2_ptr->set_ccr2(pulse_width);
      pulse_width = 0;
      return;
    }
  }
  if (c == 'z') {
    pulse_width = 160;
    tim2_ptr->set_ccr2(pulse_width);
    pulse_width = 0;
    return;
  }
  if(c == 'w') {
    escalera = !escalera;
    return;
  }
  pulse_width = pulse_width*10 + (c - '0');
}


void callback_uart2()
{
  auto& UART2 = *g_uart2;
  if(UART2.available()) {
    const uint8_t b = UART2.read_byte();

    parse_motores(static_cast<char>(b));
    UART2 << b;
  }
}



int main(void)
{
  inicializacion();
  configurar_relojes();

  RCC::enable_port_clock(RCC::GPIO_Port::A);
  RCC::enable_port_clock(RCC::GPIO_Port::B);

  GPIO::PORTA.salida(12); //LED
  GPIO::PORTA.salida(1);
  GPIO::PORTA.pin_for_timer(1, GPIO::AlternFunct::AF2);


  ///////////////

  const GPIO::pin radio_en(GPIO::PORTA, 11);
  const GPIO::pin radio_nss(GPIO::PORTB, 0);
  const GPIO::pin radio_irq(GPIO::PORTA, 4);



  auto callback_led = []() {
    static uint16_t val = 49;
    if(escalera) {
      ++val;
      if(val > 200) val = 49;
      tim2_ptr->set_ccr2(val);
    }
    GPIO::PORTA.toggle(12);
  };


  general_timer t17(GeneralTimer::TIM17, general_timer::Mode::Periodic);
  t17.configurar_periodo_ms(500);
  t17.generate_update();
  t17.clear_update();
  //t17.enable_interrupt(callback_led, general_timer::InterruptType::UIE);
  tim17_ptr = &t17;
  t17.start();

  general_timer t2(GeneralTimer::TIM2, general_timer::Mode::Periodic);
  //t2.configurar_periodo_us(20000);
  t2.set_prescaler(159);
  t2.set_autoreload(1999);
  t2.enable_output_compare(160, 2);
  t2.generate_update();
  t2.clear_update();
  tim2_ptr = &t2;
  t2.enable_interrupt(callback_led, general_timer::InterruptType::UIE);
  t2.start();


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