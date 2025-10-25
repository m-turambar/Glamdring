#include "GPIO_Port.h"
#include "RCC.h"
#include "PWR.h"
#include "FLASH.h"
#include "general_timer.h"
#include "UART.h"
#include "NVIC.h"
#include "NRF24.h"
#include "EXTI.h"

#include "app_acelerometro.h"
#include "app_nrf24.h"
#include "app_gpio.h"
#include "Procesador.h"

void inicializacion();
void configurar_relojes();
void error(void);

GPIO::pin LED(GPIO::PORTA, 12);
GPIO::pin Boton(GPIO::PORTC,15); // con pull-up interno. Apretamos y se pone a GND.
Procesador procesador;

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

Buffer uart2_buf;
void callback_uart2()
{
    if(g_uart2->available())
    {
        const uint8_t b = g_uart2->read_byte();
        uart2_buf.escribir(b);
    }
}

void callback_tim17(void* arg)
{
  uint8_t voltaje_boton = Boton.read_input();
  if(voltaje_boton == 0) {
    LED.toggle();
    *nrf_ptr << "{gt}";
  }
  else {
    LED.reset_output();
  }
};

int main(void)
{
    inicializacion();
    configurar_relojes();

    RCC::enable_port_clock(RCC::GPIO_Port::A);
    RCC::enable_port_clock(RCC::GPIO_Port::B);
    RCC::enable_port_clock(RCC::GPIO_Port::C);

    LED.salida();
    Boton.entrada(); // con pull-up interno. Apretamos y se pone a GND.
    procesador.nrf_hook = process_buffer;

    ///////////////

    UART uart2(UART::Peripheral::USART2, 115200);
    g_uart2 = &uart2;
    uart2.enable();
    uart2.enable_interrupt_rx(callback_uart2);
    uart2 << "Hola soy un Control\n";

    //////////////

    const GPIO::pin radio_en(GPIO::PORTA, 11);
    const GPIO::pin radio_irq(GPIO::PORTA, 4);
    const GPIO::pin radio_nss(GPIO::PORTB, 0);

    SPI spi1(SPI::Peripheral::SPI1_I2S1);
    spi1.inicializar();

    NRF24_uart_buffer = &uart2_buf;
    NRF24 radio(spi1, radio_nss, radio_en);
    nrf_ptr = &radio;
    radio.config_default();
    radio.encender(NRF24::Modo::TX);
    radio.escribir_registro(NRF24::Registro::RF_CH, 0b100000);

    radio.rx_dr_callback = callback_nrf24_rx;
    radio.tx_ds_callback = callback_nrf24_tx_ds;
    radio.max_rt_callback = callback_nrf24_max_rt;
    radio_irq.pin_for_interrupt(EXTI4_15_IRQn);

    // I2C i2c1(I2C::Peripheral::I2C1);
    // i2c1.enable(I2C::Timing::Standard);

    // Acelerometro mpu(i2c1);
    // g_acelerometro = &mpu;

    // ///////////////
    // auto callback_MPU = []() {
    //     g_acelerometro->imprimir(*g_uart2);
    // };


    general_timer t17(GeneralTimer::TIM17);
    tim17_ptr = &t17;
    t17.configure(general_timer::Mode::Periodic);
    t17.configurar_periodo_ms(50);
    t17.enable_interrupt(callback_tim17, general_timer::InterruptType::UIE);
    t17.start();

    while(true)
    {
        if (uart2_buf.available()) {
            uint8_t b = uart2_buf.leer();
            *g_uart2 << b;
            procesador.procesar_mensaje(b);
        }
    }
}

void inicializacion()
{
  FLASH::prefetch_buffer_enable();
  RCC::enable_SYSCFG_clock();
  RCC::enable_power_clock();
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
