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

/** Esto también es código de aplicación */
UART* g_uart2{nullptr};
void parser(uint8_t b);

void callback_uart2()
{
  auto& UART2 = *g_uart2;
  constexpr static flag RXNE(5);
  if(UART2.ISR.is_set(RXNE)) {
    const uint8_t b = UART2.read_byte();

    if(basic_tim_ptr != nullptr)
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
        if(tim16_ptr != nullptr) {
            tim16_ptr->start();
        }
    }
    if(b == ',') {
        ++estado;
        if(estado > 2) {
            memoria(tim16_ptr->PSC) = n1;
            memoria(tim16_ptr->ARR) = n2;
            memoria(tim16_ptr->CCR1) = n3;
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

  GPIO::PORTA.salida(9); //LED


  auto toggle_led = []() {
    //GPIO::PORTA.toggle(5);
    static bool ent = false;
    if(!ent) {
      GPIO::PORTA.entrada(9, GPIO::PullResistor::NoPull);
      ent = true;
    }
    else {
      GPIO::PORTA.salida(9);
      GPIO::PORTA.reset_output(9);
      ent = false;
    }
  };

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


  general_timer t16(GeneralTimer::TIM16, general_timer::Mode::Periodic);
  //algo irrelevante, pero bueno. Está esperandose 1ms después de ser iniciado, para generar un pulso de 10us.
  //t16.configurar_periodo_us(1000);
  t16.set_prescaler(160 - 1);
  t16.set_autoreload(50000 - 1);
  t16.enable_output_compare(1);
  GPIO::PORTA.pin_for_timer(6,GPIO::AlternFunct::AF5);
  t16.start();
  tim16_ptr = &t16;

  auto input_capture_callback = [] () {
    static bool rising{true};
    static uint16_t fall_cnt{0};
    static uint16_t rise_cnt{0};

    uint16_t cnt = memoria(tim17_ptr->CCR1);

    if(rising) {
      rise_cnt = cnt;
      tim17_ptr->set_capture_compare_polarity_falling();
    }

    else {
      fall_cnt = cnt;
      tim17_ptr->set_capture_compare_polarity_rising();
      uint16_t dif = (fall_cnt - rise_cnt);
      /** el ancho de pulso te dice el tiempo que la señal viajó...osea que la distancia es la mitad.
       * v = d/t -> d = t*v -> 2d = dif * 340m/s -> d = dif * 170m/s
       * Se pone complicado con las unidades. Queremos cm. Nuestra dif está en deca-micro-segundos. (10 us).
       * x metros = dif / 100,000 * 170 m/s
       * x milímetros = dif / 100 * 170
       * */
      uint16_t distancia_mm = uint16_t (float(dif * 1.7));
      uint8_t arr[6] = {0};
      int8_t idx = 0;
      while (distancia_mm > 0) {
        uint8_t digito = (distancia_mm % 10) + '0';
        arr[idx] = digito;
        ++idx;
        distancia_mm = distancia_mm / 10;
      }
      --idx;
      while(idx >= 0) {
        *g_uart2 << arr[idx];
        --idx;
      }
      *g_uart2 << '\n';
    }
    rising = !rising;
  };

  general_timer t17(GeneralTimer::TIM17, general_timer::Mode::Periodic);
  t17.enable_input_capture(true, 10);
  t17.generate_update();
  t17.clear_update();
  GPIO::PORTA.pin_for_timer(7,GPIO::AlternFunct::AF5);
  t17.start();
  t17.enable_interrupt(input_capture_callback, general_timer::InterruptType::CCIE);

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