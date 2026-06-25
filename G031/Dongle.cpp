#include "GPIO_Port.h"
#include "RCC.h"
#include "PWR.h"
#include "FLASH.h"
#include "general_timer.h"
#include "UART.h"
#include "NVIC.h"
#include "NRF24.h"
#include "EXTI.h"

#include "app_nrf24.h"
#include "app_gpio.h"
#include "Procesador.h"

void inicializacion();
void configurar_relojes();
void error(void);

GPIO::pin LED(GPIO::PORTA, 0);
Procesador procesador;

extern "C" {
void EXTI4_15_IRQHandler(void) {
    if(nrf_ptr != nullptr)
        nrf_ptr->irq_handler();
    EXTI::clear_pending_interrupt(4);
    NVIC_ClearPendingIRQ(EXTI4_15_IRQn);
    nrf_ptr->clear_all_interrupts();
}
}

Buffer uart1_buf;
void callback_uart1()
{
    if(g_uart1->available())
    {
        const uint8_t b = g_uart1->read_byte();
        uart1_buf.escribir(b);
    }
}

void manage_gpio(uint8_t b)
{
    if(b == 't') {
        encender_pin_durante(tim16_ptr, LED, 1000);
    }
    else if(b == '0') {
        LED.reset_output();
    }
    else if(b == '1') {
        LED.set_output();
    }
}


int main(void)
{
    inicializacion();
    configurar_relojes();

    RCC::enable_port_clock(RCC::GPIO_Port::A);
    RCC::enable_port_clock(RCC::GPIO_Port::B);
    RCC::enable_port_clock(RCC::GPIO_Port::C);

    LED.salida();
    procesador.gpio_hook = manage_gpio;
    procesador.nrf_hook = process_buffer;

    UART uart1(UART::Peripheral::USART1, 115200);
    g_uart1 = &uart1;
    uart1.enable();
    uart1.enable_fifo();
    uart1.enable_interrupt_rx(callback_uart1);
    uart1 << "Hola soy un dongle\n";

    const GPIO::pin radio_ce(GPIO::PORTB, 1);
    const GPIO::pin radio_irq(GPIO::PORTA, 4);
    const GPIO::pin radio_nss(GPIO::PORTB, 0);

    SPI spi1(SPI::Peripheral::SPI1_I2S1);
    spi1.inicializar();

    NRF24_uart_buffer = &uart1_buf;
    NRF24 radio(spi1, radio_nss, radio_ce);
    nrf_ptr = &radio;
    radio.encender(NRF24::Modo::RX);
    radio.escribir_registro(NRF24::Registro::RF_CH, 0b100000);

    radio.rx_dr_callback = callback_nrf24_rx;
    radio.tx_ds_callback = callback_nrf24_tx_ds;
    radio.max_rt_callback = callback_nrf24_max_rt;
    radio_irq.pin_for_interrupt(EXTI4_15_IRQn);
   
    general_timer t16(GeneralTimer::TIM16);
    tim16_ptr = &t16;

    while(true)
    {
        if (uart1_buf.available()) {
            uint8_t b = uart1_buf.leer();
            *g_uart1 << b;
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
  RCC::configurar_prescaler_APB(RCC::APB_Prescaler::P16);
  RCC::configurar_prescaler_AHB(RCC::AHB_Prescaler::P1);

  if(!RCC::is_HSI_ready())
    error();

  RCC::seleccionar_SYSCLK(RCC::SystemClockSwitch::HSISYS);
  RCC::SystemClockSwitch fuente_sysclk = RCC::status_SYSCLK();

  if(fuente_sysclk != RCC::SystemClockSwitch::HSISYS)
    error();

  RCC::configurar_prescaler_APB(RCC::APB_Prescaler::P1);
  RCC::seleccionar_reloj_USART2(RCC::RelojesUsart::PCLK);
}


void error(void)
{
  /* User can add his own implementation to report the HAL error return state */
  while (1);
}
