#include <cstring>
#include <cstdio>
#include "basic_timer.h"
#include "general_timer.h"
#include "GPIO_Port.h"
#include "RCC.h"
#include "FLASH.h"
#include "UART.h"
#include "NVIC.h"
#include "SPI.h"

#ifdef __cplusplus
extern "C" {
#endif

void inicializacion();
void configurar_relojes();
void error(void);

GPIO::pin LED_Verd(GPIO::PORTA, 5);
GPIO::pin LED_Azul(GPIO::PORTB, 7);
GPIO::pin LED_Rojo(GPIO::PORTB, 14);

uint32_t cnter{0};

UART* g_uart3{nullptr};
UART* g_uart2{nullptr};

void toggle_led()
{
  LED_Azul.toggle();
  LED_Rojo.toggle();
  LED_Verd.toggle();
};

void serial_hb()
{
  static bool b {true};
  if (b)
    *g_uart3 << "Ping";
  else
    *g_uart3 << "Pong";
  b = !b;
}

/** Estados posibles:
 * 1. procesando = false
 * 2. procesando = true
 * 3. Exito
 * 4. Error
 *
 * Exito y error ejecutan su operación y nos llevan al estado 1.
 * el estado 2 (procesando) nos puede llevar al Exito o al Error. */
struct Procesador
{
  Procesador(UART& uart) : uart(uart) {}

  enum class Proceso {
    None,
    Accel,
    Freq,
    PWM,
  };

  void procesar_mensaje(uint8_t b)
  {
    if (b == '}') {
      procesando = false;
      ejecutar_mensaje();
      clear_status();
      return;
    }

    if (proceso != Proceso::None) {
      if(!procesar_interno(b)) {
        procesando = false;
        clear_status();
      }
      return;
    }

    if (b == 'p') {
      proceso = Proceso::PWM;
    }
    else if (b == 'f') {
      proceso = Proceso::Freq;
    }
    else if (b == 'a') {
      proceso = Proceso::Accel;
    }

  }

  bool procesando{false};

private:
  bool procesar_interno(const uint8_t b) {
    if(proceso == Proceso::PWM) {
      if (pwm_canal == 0) {
        pwm_canal = b - 'a' + 1; // 'a' para canal 1, 'b' para canal 2, 'c' para 3, 'd' para 4.
        return (pwm_canal >= 1 && pwm_canal <= 4);
      }
      if (b < '0' || b > '9') {
        return false;
      }
      pwm_pulse_width = pwm_pulse_width * 10 + b - '0';
    }
    else if (proceso == Proceso::Freq) {
      if (b < '0' || b > '9') {
        return false;
      }
      microseconds_period = microseconds_period * 10 + b - '0';
    }
    else if (proceso == Proceso::Accel) {
      ;
    }
    return true;
  }

  // e.g. {pa1000}, {pb1500}, {pc800}
  void ejecutar_mensaje() {
    if (proceso == Proceso::PWM) {
      tim2_ptr->set_microseconds_pulse_high(pwm_pulse_width, pwm_canal);
    }
    if (proceso == Proceso::Freq) {
      tim2_ptr->set_microsecond_period(microseconds_period);
    }
    // if (proceso == Proceso::Accel) {
    //   g_acelerometro->imprimir(*g_uart2);
    // }
  }

  void clear_status() {
    proceso = Proceso::None;
    procesando = false;
    pwm_pulse_width = 0;
    microseconds_period = 0;
    pwm_canal = 0;
  }

  Proceso proceso {Proceso::None};
  uint16_t pwm_pulse_width{0};
  uint16_t microseconds_period{0};
  uint8_t pwm_canal{0};
  UART& uart;
};

void parse_uart(uint8_t b)
{
  static Procesador procesador(*g_uart3);
  if (procesador.procesando) {
    procesador.procesar_mensaje(b);
    return;
  }

  else if (b == '{') {
    procesador.procesando = true;
  }
}

#define UART_BUFSZ 32
struct Buffer
{
  void escribir(uint8_t b) 
  {
    buf[i_w % UART_BUFSZ] = b;
    ++i_w;
  }
  uint8_t leer() 
  {
    uint8_t b = buf[i_r % UART_BUFSZ];
    ++i_r;
    return b;
  }
  bool available()
  {
    return i_w > i_r;
  }

private:
  uint8_t buf[UART_BUFSZ] {};
  int i_r {};
  int i_w {};
};

// Imprime a través de USB (UART3) lo que te llegó
void callback_uart2()
{
  if(g_uart2->available())
  {
    const uint8_t b = g_uart2->read_byte();
    *g_uart3 << b;
  }
}

Buffer uart3_buf;
void callback_uart3()
{
  if(g_uart3->available())
  {
    const uint8_t b = g_uart3->read_byte();
    uart3_buf.escribir(b);
  }
}

int main(void)
{
  inicializacion();
  configurar_relojes();

  RCC::enable_port_clock(RCC::GPIO_Port::A);
  RCC::enable_port_clock(RCC::GPIO_Port::B);

  LED_Azul.salida();
  LED_Rojo.salida();
  LED_Rojo.set_output();
  LED_Verd.salida();

  // St-link uart
  UART uart3(UART::Peripheral::USART3, 115200);
  uart3.enable();
  g_uart3 = &uart3;
  uart3 << "hola";
  uart3.enable_interrupt_rx(callback_uart3);
  uart3.enable_fifo().enable();
  
  // UART uart2(UART::Peripheral::USART2, 9600);
  // uart2.enable();
  // g_uart2 = &uart2;
  // uart2.enable_interrupt_rx(callback_uart2);
  // uart2.enable_fifo().enable();

  // general_timer t17(GeneralTimer::TIM17, general_timer::Mode::Periodic);
  // tim17_ptr = &t17;
  // t17.configurar_periodo_ms(50);
  // t17.generate_update();
  // t17.clear_update();
  // t17.enable_interrupt(callback_tim17, general_timer::InterruptType::UIE);
  // t17.start();
  
  basic_timer t6(BasicTimer::TIM6, basic_timer::Mode::Periodic);
  t6.configurar_periodo_ms(2000);
  t6.generate_update();
  t6.clear_update();
  t6.enable_interrupt(serial_hb);
  tim6_ptr = &t6;
  t6.start();

  basic_timer t7(BasicTimer::TIM7, basic_timer::Mode::Periodic);
  t7.configurar_periodo_ms(500);
  t7.generate_update();
  t7.clear_update();
  t7.enable_interrupt(toggle_led);
  tim7_ptr = &t7;
  t7.start();

  general_timer t2(GeneralTimer::TIM2, general_timer::Mode::Periodic);
  t2.set_output_compare_microsecond_resolution(10);
  t2.set_microsecond_period(20000);
  t2.set_microseconds_pulse_high(1000, 1);
  t2.set_microseconds_pulse_high(1000, 2);
  t2.set_microseconds_pulse_high(1000, 3);
  t2.enable_output_compare(1);
  t2.enable_output_compare(2);
  t2.enable_output_compare(3);
  t2.start();
  GPIO::PORTA.pin_for_timer(0, GPIO::AlternFunct::AF1); // canal 1
  GPIO::PORTA.pin_for_timer(1, GPIO::AlternFunct::AF1); // canal 2
  GPIO::PORTA.pin_for_timer(2, GPIO::AlternFunct::AF1); // canal 3

  while (1) {
    if (uart3_buf.available())
    {
      uint8_t b = uart3_buf.leer();
      *g_uart3 << b;
      parse_uart(b);
    }
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