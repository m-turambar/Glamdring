//
// Created by migue on 29/12/2021.
//

#include <cstdlib>
#include <cstring>

#include "app_uart.h"
#include "app_nrf24.h"
#include "app_rele.h"
#include "app_acelerometro.h"

UART* g_uart2{nullptr};
bool parsing = false;

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
  if(b == 'r') {
    char buf[8] = {0};
    uint8_t freq = nrf_ptr->leer_registro(NRF24::Registro::RF_CH);
    itoa(freq, buf, 2);
    *g_uart2 << "\r\nfreq == " << buf << "\r\n";
    memset(buf, 0, 8);
    parsing = false;
    return;
  }
  if(b == '.') {
    parsing = false;
    return;
  }
  if(b == 'a') {
    imprimir_acelerometro();
  }
  if(b == 'e') {
    char buf[4] {};
    itoa(mpu_init_fails, buf, 10);
    *g_uart2 << buf;
  }
  if(b == 't') {
    encender_rele_durante(500);
  }
  if(b == '0') {
    apagar_rele();
  }
  if(b == '1') {
    encender_rele();
  }
}
