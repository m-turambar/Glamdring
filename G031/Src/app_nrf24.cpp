//
// Created by migue on 29/12/2021.
//
#include "EXTI.h"

#include "app_nrf24.h"
#include "GPIO_Port.h"
#include "app_uart.h"

NRF24* nrf_ptr{nullptr};
extern GPIO::pin LED;

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

void callback_nrf24_rx() {
  LED.toggle();
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