#include "app_nrf24.h"
#include "UART.h"

extern GPIO::pin LED;

static bool parsing = true; // todo delete

void callback_nrf24_rx() {
  LED.toggle();

  // TODO: lee status para saber de qué canal vino el paquete
  uint8_t status = nrf_ptr->leer_registro(NRF24::Registro::Status);
  uint8_t data_pipe_number = (status >> 1) & 0x7;

  *NRF24_uart_buffer << "\r\nchannel: " << static_cast<char>(data_pipe_number + 48) << '\n';

  uint8_t fifo_status = nrf_ptr->leer_registro(NRF24::Registro::FIFO_STATUS);
  while (fifo_status % 2 == 0) /// el bit menos significativo de FIFO_STATUS es RX_EMPTY
  {
    uint8_t b = nrf_ptr->leer_rx();

    if(b == '/' or parsing) {
    //    parse_uart(b);
    }
    else {
        NRF24_uart_buffer->escribir(b);
    }

    fifo_status = nrf_ptr->leer_registro(NRF24::Registro::FIFO_STATUS);
  }

};

void callback_nrf24_tx_ds()
{
  LED.toggle();
}

void callback_nrf24_max_rt() {
  /** Descartamos voluntariamente los paquetes que podrían haberse quedado en el buffer circular, para evitar que
   * cuando la señal del radio vuelva a enlazar, se envíen paquetes a destiempo. Sería peligroso para un garage
   * el que la señal de abrir/cerrar la puerta llegara 5 segundos después de que el usuario apretara el botón. */
  //nrf_ptr->descartar_fifo();
}

