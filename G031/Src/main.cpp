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
#include <EXTI.h>

#ifdef __cplusplus
extern "C" {
#endif

void inicializacion();
void configurar_relojes();
void error(void);

void encender_rele();
void apagar_rele();

general_timer* tim17_ptr{nullptr};
UART* g_uart2{nullptr};
NRF24* nrf_ptr{nullptr};
MPU6050* mpu_ptr{nullptr};

/** interrumpir esta funcion con un breakpoint hace que no vuelva a entrar. Por qué?
 * Osea si el pin se baja y no "atrapamos" ese flanco de bajada, a menos que modifiquemos el diseño
 * nunca vamos a salir de ese estado. Digo, podrías poner un watchdog o un timer a que resetee las interrupciones del
 * nrf, pero me sorprende que después de resumir la interrrupción no agarre la onda.*/
extern "C" {
void EXTI4_15_IRQHandler(void) {
  if(nrf_ptr != nullptr)
    nrf_ptr->irq_handler();

  EXTI::clear_pending_interrupt(4);
  NVIC_ClearPendingIRQ(EXTI4_15_IRQn);
  nrf_ptr->clear_all_interrupts();
}
}

void imprimir_acelerometro()
{
  char buf[16] = {0};
  uint8_t raw_buf[6] = {0};
  float acc[3] = {0};
  mpu_ptr->posicionar_en_registro_ax();
  mpu_ptr->leer(raw_buf, 6);
  mpu_ptr->convert_to_float(acc, raw_buf, 3);

  std::sprintf(buf, "ax=%.2f\t ay=%.2f\t az=%.2f\n\r", acc[0], acc[1], acc[2]);

  *g_uart2 << buf;
}

char itoa_buf[8] = {0};

bool parsing = false;
void parse_uart(uint8_t b)
{
  static uint8_t freq = 0;
  if(b == '/') {
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
    //nrf_ptr->escribir_registro(NRF24::Registro::RF_CH, freq);
    //itoa(freq, itoa_buf, 2);
    //*g_uart2 << "\r\nescribiendo freq = " << itoa_buf << "\r\n";
    //memset(itoa_buf, 0, 8);
    //parsing = false;
    //return;
  }
  //freq = freq * 10 + b;
  if(b == 'a') {
    imprimir_acelerometro();
  }
  if(b == '0') {
    apagar_rele();
  }
  if(b == '1') {
    encender_rele();
  }
}

static const char* pwd = "bishi\n";
uint8_t estado_pwd=0;

void parse_pwd(uint8_t b) {
  const char c = static_cast<char>(b);
  if(c == pwd[estado_pwd]) {
    ++estado_pwd;
    if(estado_pwd > 5)
    {
      //activar relé con un OnePulseTimer y apagarlo después.
      if(tim17_ptr != nullptr)
      {
        GPIO::PORTA.salida(1, GPIO::OutputType::OpenDrain);
        GPIO::PORTA.reset_output(1);
        tim17_ptr->start();
      }

    }
  }
  else {
    estado_pwd = 0;
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
      if(nrf_ptr != nullptr) *nrf_ptr << b;
    }
  }
}

void callback_nrf24_rx() {
  GPIO::PORTA.toggle(12);
  NRF24& nrf24 = *nrf_ptr;

  //uint8_t status = nrf24.leer_registro(NRF24::Registro::Status);
  //todo lee status para saber de qué canal vino el paquete

  uint8_t fifo_status = nrf24.leer_registro(NRF24::Registro::FIFO_STATUS);
  while (fifo_status % 2 == 0) /// el bit menos significativo de FIFO_STATUS es RX_EMPTY
  {
    uint8_t b = nrf24.leer_rx();

    if(b == '/' or parsing) {
      parse_uart(b);
    }
    else {
      *g_uart2 << b;
    }

    fifo_status = nrf24.leer_registro(NRF24::Registro::FIFO_STATUS);
  }

};

void callback_tim17()
{
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

void toggle_rele()
{
  static bool bb;
  bb = !bb;
  if(bb) {
    GPIO::PORTA.entrada(1, GPIO::PullResistor::NoPull);
  }
  else {
    GPIO::PORTA.salida(1, GPIO::OutputType::OpenDrain);
    GPIO::PORTA.reset_output(1);
  }
};

void encender_rele()
{
  GPIO::PORTA.salida(1, GPIO::OutputType::OpenDrain);
  GPIO::PORTA.reset_output(1);
}

void apagar_rele()
{
  GPIO::PORTA.reset_output(1);
  GPIO::PORTA.entrada(1, GPIO::PullResistor::NoPull);
};

int main(void)
{
  inicializacion();
  configurar_relojes();

  RCC::enable_port_clock(RCC::GPIO_Port::A);
  RCC::enable_port_clock(RCC::GPIO_Port::B);
  RCC::enable_port_clock(RCC::GPIO_Port::C);

  GPIO::PORTA.salida(12); //LED
  GPIO::PORTC.entrada(15); //pushbutton, con pull-up interno. Apretamos y se pone a GND.
  GPIO::PORTA.entrada(1, GPIO::PullResistor::NoPull);

  // Agregar un tiempo de espera de algunos milisegundos para asegurar que todo ya se inicializó

  I2C i2c1(I2C::Peripheral::I2C1);
  i2c1.enable(I2C::Timing::Standard);

  MPU6050 mpu(i2c1); //instancia que representa a nuestro acelerómetro
  mpu.set_sampling_rate();
  mpu_ptr = &mpu;

  ///////////////

  const GPIO::pin radio_en(GPIO::PORTA, 11);
  const GPIO::pin radio_irq(GPIO::PORTA, 4);
  const GPIO::pin radio_nss(GPIO::PORTB, 0);

  SPI spi1(SPI::Peripheral::SPI1_I2S1);
  spi1.inicializar();

  NRF24 radio(spi1, radio_nss, radio_en);
  nrf_ptr = &radio;
  radio.config_default();
  radio.encender(NRF24::Modo::RX);
  radio.escribir_registro(NRF24::Registro::RF_CH, 0b100000);

  radio.rx_dr_callback = callback_nrf24_rx;
  radio_irq.pin_for_interrupt(EXTI4_15_IRQn);

  ///////////////

  general_timer t17(GeneralTimer::TIM17, general_timer::Mode::Periodic);
  tim17_ptr = &t17;
  t17.configurar_periodo_ms(50);
  t17.generate_update();
  t17.clear_update();
  t17.enable_interrupt(callback_tim17, general_timer::InterruptType::UIE);
  t17.start();


  UART uart2(UART::Peripheral::USART2, 115200);
  g_uart2 = &uart2;
  uart2.enable();
  uart2.enable_interrupt_rx(callback_uart2);
  uart2 << "Hola\n";



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