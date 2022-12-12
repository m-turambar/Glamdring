//
// Created by migue on 29/12/2021.
//

#include <cstdlib>
#include <cstring>
#include <cstdio>

#include "general_timer.h"

#include "app_uart.h"
#include "app_nrf24.h"
#include "app_rele.h"
#include "app_acelerometro.h"

UART* g_uart2{nullptr};
extern general_timer* tim2_ptr;

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
  enum class Proceso {
    None,
    Accel,
    Freq,
    PWM,
  };

  void procesar_mensaje(uint8_t b)
  {
    *g_uart2 << b;
    if (b == '}') {
      *g_uart2 << "\r\n";
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

  void ejecutar_mensaje() {
    if (proceso == Proceso::PWM) {
      tim2_ptr->set_microseconds_pulse_high(pwm_pulse_width, pwm_canal);
    }
    if (proceso == Proceso::Freq) {
      tim2_ptr->set_microsecond_period(microseconds_period);
    }
    if (proceso == Proceso::Accel) {
      g_acelerometro->imprimir(*g_uart2);
    }
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
};



void parse_uart(uint8_t b)
{
  static Procesador procesador;
  if (procesador.procesando) {
    procesador.procesar_mensaje(b);
    return;
  }

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
  else if( b == 'm')
  {
    NRF24::Modo m = nrf_ptr->obtener_modo();
    if(m == NRF24::Modo::TX)
      *g_uart2 << "\r\nTransmisor\r\n";
    else
      *g_uart2 << "\r\nReceptor\r\n";
  }
  else if( b == 'n')
  {
    NRF24::Modo modo = nrf_ptr->obtener_modo();
    nrf_ptr->apagar();

    modo = (modo == NRF24::Modo::TX) ? NRF24::Modo::RX : NRF24::Modo::TX;
    nrf_ptr->encender(modo);
  }

  else if (b == '{') {
    *g_uart2 << "\n\r" << b;
    procesador.procesando = true;
  }

  else if(b == 'r') {
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
    *g_uart2 << "\r\nRF_SETUP:" << rf_setup_buf << "\r\n";
    return;
  }

  else if (b == 's') {
    char n_bytes_buf[8] = {0};
    uint8_t inicio = nrf_ptr->idx_enviar;
    uint8_t fin = nrf_ptr->idx_llenar;
    itoa(static_cast<uint8_t>(fin - inicio), n_bytes_buf, 10);
    *g_uart2 << "\r\nBytes en TX buf: " << n_bytes_buf;
    *g_uart2 << "\r\nContenido: ";
    for (auto i = inicio; i != fin; ++i)
      *g_uart2 << nrf_ptr->tx_buf[i];
  }

  else if (b == 'u') {
    uint64_t tx_addr = nrf_ptr->leer_addr_reg(NRF24::Registro::TX_ADDR);
    if (tx_addr == static_cast<uint64_t>(NRF24::DefaultAddress::P0))
      nrf_ptr->config_tx_addr(NRF24::DefaultAddress::P1);
    else
      nrf_ptr->config_tx_addr(NRF24::DefaultAddress::P0);
  }

  else if(b == 't') {
    encender_rele_durante(2000);
  }
  else if(b == '0') {
    apagar_rele_a();
  }
  else if(b == '1') {
    encender_rele_a();
  }
  else if(b == '2') {
    apagar_rele_b();
  }
  else if(b == '3') {
    encender_rele_b();
  }

  else if(b == '.') {
    parsing = false;
  }
}
