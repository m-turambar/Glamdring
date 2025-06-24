//
// Created by migue on 13/12/2020.
//

#ifndef GLAMDRING_NRF24_H
#define GLAMDRING_NRF24_H

#include "SPI.h"
#include "GPIO_Port.h"

class NRF24
{
public:
  enum class Registro {
    Config = 0,
    SETUP_RETR = 4,
    RF_CH = 5,
    RF_SETUP = 6,
    Status = 7,
    RX_ADDR_P0 = 0x0A,
    TX_ADDR = 0x10,
    RX_PW_P0 = 0x11,
    RX_PW_P1 = 0x12,
    FIFO_STATUS = 0x17
  };

  enum class DefaultAddress :uint64_t {
    P0 = 0xE7E7E7E7E7,
    P1 = 0xC2C2C2C2C2,
  };

  enum class Modo : uint8_t {
    TX = 0,
    RX = 1
  };

  NRF24(const SPI& spi_arg, const GPIO::pin& SS_pin, const GPIO::pin& CEN_pin);

  void config_default() const;
  void config_payload_widths(uint8_t width) const;
  void config_tx_addr(uint64_t addr) const;
  void config_tx_addr(DefaultAddress addr) const;
  uint64_t leer_addr_reg(Registro addr_reg) const;
  void encender(Modo modo);
  void apagar();
  Modo obtener_modo() const;

  uint8_t leer_rx() const;
  uint8_t leer_registro(Registro reg) const;
  void escribir_registro(Registro reg, uint8_t val) const;

  void clear_all_interrupts() const;
  void descartar_fifo();

  /** agregar setters me parecía un desperdicio */
  void (*rx_dr_callback)() {nullptr};
  void (*tx_ds_callback)() {nullptr};
  void (*max_rt_callback)() {nullptr};

  void irq_handler();

  /** Necesitas dos índices con tu buffer circular.
   * 1. De dónde lee el NRF para enviar datos.
   * 2. A qué desplazamiento le escribes nuevos caracteres que el usuario agregue.
   * El usuario pone datos en el buffer y el nrf los va sacando. */
  uint8_t tx_buf[256] = {0};
  uint8_t idx_enviar = {0}; /// solo debe actualizarse en la interrupción de un dato exitoso enviado
  uint8_t idx_llenar = {0}; /// solo debe actualizarse cuando el usuario agregue datos al buffer.

  Modo modo_cached;

  /** Qué pasa si agregas datos al buffer durante una transmisión?
   * Debemos definir algunos estados.
   * Transmitiendo. Standby. Recibiendo. Estos tres operadores pasan de standby a transmitiendo */

  NRF24& operator<<(uint8_t byte);
  NRF24& operator<<(char c);
  NRF24& operator<<(char* buffer);

private:

  const SPI& spi;
  const GPIO::pin& CEN_pin, SS_pin;

  bool transmitiendo{false};
  void transmitir_byte(const uint8_t b) const;

  uint8_t flush_tx_fifo() const;
  uint8_t flush_rx_fifo() const;
};

#endif //GLAMDRING_NRF24_H