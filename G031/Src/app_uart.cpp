//
// Created by migue on 29/12/2021.
//

#include <cstdlib>
#include <cstring>
#include <cstdio>

#include "app_uart.h"
#include "app_nrf24.h"
#include "app_rele.h"

UART* g_uart2{nullptr};
bool parsing = true;

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

void parse_uart(uint8_t b)
{
  if(b == '/') {
    if(!parsing) {
      parsing = true;
      return;
    } else
    {
      *g_uart2 << b;
      if(nrf_ptr != nullptr && nrf_ptr->modo_cached == NRF24::Modo::TX) *nrf_ptr << b;
    }
  }
  if( b == 'm')
  {
    NRF24::Modo m = nrf_ptr->obtener_modo();
    if(m == NRF24::Modo::TX)
      *g_uart2 << "Transmisor\n";
    else
      *g_uart2 << "Receptor\n";
  }
  if( b == 'n')
  {
    NRF24::Modo m = nrf_ptr->obtener_modo();
    nrf_ptr->apagar();

    if(m == NRF24::Modo::TX)
      nrf_ptr->encender(NRF24::Modo::RX);
    else
      nrf_ptr->encender(NRF24::Modo::TX);
  }
  if(b == 'r') {
    char freq_buf[8] = {0};
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

    itoa(freq, freq_buf, 2);
    itoa(rf_setup, rf_setup_buf, 2);

    *g_uart2 << "\r\nRF_CH: " << freq_buf;
    *g_uart2 << "\r\nRX_ADDR_P0:" << rx0_addr_buf;
    *g_uart2 << "\r\nTX_ADDR:" << tx_addr_buf;
    *g_uart2 << "\r\nRF_SETUP:" << rf_setup_buf;
    return;
  }

  if (b == 's') {
    char n_bytes_buf[8] = {0};
    uint8_t inicio = nrf_ptr->idx_enviar;
    uint8_t fin = nrf_ptr->idx_llenar;
    itoa(static_cast<uint8_t>(fin - inicio), n_bytes_buf, 10);
    *g_uart2 << "\r\nBytes en TX buf: " << n_bytes_buf;
    *g_uart2 << "\r\nContenido: ";
    for (auto i = inicio; i != fin; ++i)
      *g_uart2 << nrf_ptr->tx_buf[i];
  }

  if (b == 'u') {
    uint64_t tx_addr = nrf_ptr->leer_addr_reg(NRF24::Registro::TX_ADDR);
    if (tx_addr == static_cast<uint64_t>(NRF24::DefaultAddress::P0))
      nrf_ptr->config_tx_addr(NRF24::DefaultAddress::P1);
    else
      nrf_ptr->config_tx_addr(NRF24::DefaultAddress::P0);
  }

  if(b == 't') {
    encender_rele_durante(2000);
  }
  if(b == '0') {
    apagar_rele_a();
  }
  if(b == '1') {
    encender_rele_a();
  }
  if(b == '2') {
    apagar_rele_b();
  }
  if(b == '3') {
    encender_rele_b();
  }

  if(b == '.') {
    parsing = false;
    return;
  }
}
