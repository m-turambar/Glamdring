#include "app_nrf24.h"
#include "UART.h"
#include <cstdio>

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
    NRF24_uart_buffer->escribir(b);
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

void process_buffer(Buffer& buf)
{
    uint8_t first_char = buf.leer();
    if (first_char == '/') {
        if (nrf_ptr == nullptr || nrf_ptr->modo_cached != NRF24::Modo::TX) {
            *NRF24_uart_buffer << "NRF incapaz de transmitir\r\n";
            buf.clear();
            return;
        }
        while (buf.available()) {
            uint8_t b = buf.leer();
            *NRF24_uart_buffer << b;
            *nrf_ptr << b;
        }
    }
    else if (first_char == 'm')
    {
        NRF24::Modo m = nrf_ptr->obtener_modo();
        if(m == NRF24::Modo::TX)
            *NRF24_uart_buffer << "\r\nTransmisor\r\n";
        else
            *NRF24_uart_buffer << "\r\nReceptor\r\n";
        }
    else if (first_char == 'n')
    {
        NRF24::Modo modo = nrf_ptr->obtener_modo();
        nrf_ptr->apagar();

        modo = (modo == NRF24::Modo::TX) ? NRF24::Modo::RX : NRF24::Modo::TX;
        nrf_ptr->encender(modo);
    }

    else if (first_char == 'r') {
        char freq_buf[10] = {0};
        char rf_setup_buf[8] = {0};
        char tx_addr_buf[16] = {0};
        char rx0_addr_buf[16] = {0};

        uint8_t freq = nrf_ptr->leer_registro(NRF24::Registro::RF_CH);
        uint8_t rf_setup = nrf_ptr->leer_registro(NRF24::Registro::RF_SETUP);
        uint64_t tx_addr = nrf_ptr->leer_addr_reg(NRF24::Registro::TX_ADDR);
        uint64_t rx0_addr = nrf_ptr->leer_addr_reg(NRF24::Registro::RX_ADDR_P0);

        sprintf(tx_addr_buf, "0x%X", tx_addr);
        uint32_t ms_32bits = tx_addr >> 32;
        sprintf(tx_addr_buf + 10, "%X", ms_32bits);

        sprintf(rx0_addr_buf, "0x%X", rx0_addr);
        ms_32bits = rx0_addr >> 32;
        sprintf(rx0_addr_buf + 10, "%X", ms_32bits);

        sprintf(freq_buf, "0x%X", freq);
        sprintf(rf_setup_buf, "0x%X", rf_setup);

        *g_uart2 << "\r\nRF_CH: " << freq_buf;
        *g_uart2 << "\r\nRX_ADDR_P0:" << rx0_addr_buf;
        *g_uart2 << "\r\nTX_ADDR:" << tx_addr_buf;
        *g_uart2 << "\r\nRF_SETUP:" << rf_setup_buf << "\r\n";
        return;
    }

    else if (first_char == 's') {
        char n_bytes_buf[8] = {0};
        uint8_t inicio = nrf_ptr->idx_enviar;
        uint8_t fin = nrf_ptr->idx_llenar;
        sprintf(n_bytes_buf, "%d", (fin - inicio));
        *NRF24_uart_buffer << "\r\nBytes en TX buf: " << n_bytes_buf;
        *NRF24_uart_buffer << "\r\nContenido: ";
        for (auto i = inicio; i != fin; ++i)
            *NRF24_uart_buffer << nrf_ptr->tx_buf[i];
    }

    else if (first_char == 'u') {
        uint64_t tx_addr = nrf_ptr->leer_addr_reg(NRF24::Registro::TX_ADDR);
        if (tx_addr == static_cast<uint64_t>(NRF24::DefaultAddress::P0))
            nrf_ptr->config_tx_addr(NRF24::DefaultAddress::P1);
        else
            nrf_ptr->config_tx_addr(NRF24::DefaultAddress::P0);
    }

}

