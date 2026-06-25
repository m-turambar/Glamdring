#include "app_nrf24.h"
#include "UART.h"
#include <cstdio>

extern GPIO::pin LED;

void callback_nrf24_rx() {
  LED.toggle();

// TODO: lee status para saber de qué canal vino el paquete
//   uint8_t status = nrf_ptr->leer_registro(NRF24::Registro::Status);
//   uint8_t data_pipe_number = (status >> 1) & 0x7;
//   *NRF24_uart_buffer << "\r\nchannel: " << static_cast<char>(data_pipe_number + 48) << '\n';

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

// e.g. encender rele remoto: {n/{gt}}
void process_buffer(Buffer& buf)
{
    uint8_t first_char = buf.leer();
    if (first_char == '/') {
        if (nrf_ptr == nullptr || nrf_ptr->modo_cached != NRF24::Modo::TX) {
            *NRF24_uart_buffer << "NRF error\r\n";
            buf.clear();
            return;
        }
        while (buf.available()) {
            uint8_t b = buf.leer();
            *nrf_ptr << b;
        }
    }
    // Configurar canal
    else if (first_char == 'c') {
        uint8_t canal;
        if (buf.available()) {
            canal = buf.leer();
            switch (canal) {
                case '0': nrf_ptr->config_tx_addr(static_cast<uint64_t>(NRF24::DefaultAddress::P0)); break;
                case '1': nrf_ptr->config_tx_addr(static_cast<uint64_t>(NRF24::DefaultAddress::P1)); break;
                case '2': nrf_ptr->config_tx_addr(static_cast<uint64_t>(NRF24::DefaultAddress::P2)); break;
                case '3': nrf_ptr->config_tx_addr(static_cast<uint64_t>(NRF24::DefaultAddress::P3)); break;
                case '4': nrf_ptr->config_tx_addr(static_cast<uint64_t>(NRF24::DefaultAddress::P4)); break;
                case '5': nrf_ptr->config_tx_addr(static_cast<uint64_t>(NRF24::DefaultAddress::P5)); break;
                default: return;
            }
        }
        uint64_t tx_addr = nrf_ptr->leer_addr_reg(NRF24::Registro::TX_ADDR);
        char tx_addr_buf[16] = {0};
        sprintf(tx_addr_buf, "0x%X", tx_addr);
        uint32_t ms_32bits = tx_addr >> 32;
        sprintf(tx_addr_buf + 10, "%X", ms_32bits);
        *NRF24_uart_buffer << "TX_ADDR:" << tx_addr_buf << "\r\n";
    }
    else if (first_char == 'm')
    {
        NRF24::Modo modo;
        if (!buf.available()) {
            modo = nrf_ptr->obtener_modo();
            
        } else {
            uint8_t c = buf.leer();
            if (c != '0' && c != '1')
                return;
            modo = (c == '1' ? NRF24::Modo::RX : NRF24::Modo::TX);
            nrf_ptr->apagar();
            nrf_ptr->encender(modo);
        }
        *NRF24_uart_buffer << (modo == NRF24::Modo::RX ? "PRX" : "PTX") << "\r\n";
    }
    else if (first_char == 'r') {
        char freq_buf[10] = {0};
        char rf_setup_buf[8] = {0};
        char tx_addr_buf[16] = {0};
        char rx0_addr_buf[16] = {0};

        uint8_t freq = nrf_ptr->leer_registro(NRF24::Registro::RF_CH);
        uint8_t rf_setup = nrf_ptr->leer_registro(NRF24::Registro::RF_SETUP);

        sprintf(freq_buf, "0x%X", freq);
        sprintf(rf_setup_buf, "0x%X", rf_setup);

        *NRF24_uart_buffer << "\r\nRF_CH: " << freq_buf;
        *NRF24_uart_buffer << "\r\nRF_SETUP:" << rf_setup_buf << "\r\n";
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
}

