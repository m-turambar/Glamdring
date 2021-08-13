//
// Created by migue on 13/12/2020.
//

#ifndef G070_NRF24_H
#define G070_NRF24_H

#endif //G070_NRF24_H

#include "SPI.h"
#include "GPIO_Port.h"

class NRF24
{
public:
  enum class Registro {
    Config = 0,
    SETUP_RETR = 4,
    RF_CH = 5,
    Status = 7,
    RX_PW_P0 = 0x11,
    FIFO_STATUS = 0x17
  };

  enum class Modo : uint8_t {
    TX = 0,
    RX = 1
  };

  NRF24(const SPI& spi_arg, const GPIO::pin& SS_pin, const GPIO::pin& CEN_pin, const GPIO::pin& IRQ_pin);

  void config_default() const;
  void encender(Modo modo) const;

  uint8_t leer_rx() const;

  uint8_t leer_registro(Registro reg) const;
  void escribir_registro(Registro reg, uint8_t val) const;
  uint8_t flush_tx_fifo() const;
  uint8_t flush_rx_fifo() const;
  void clear_all_interrupts() const;
  void config_payload_widths(uint8_t width) const;

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

  /** Qué pasa si agregas datos al buffer durante una transmisión?
   * Debemos definir algunos estados.
   * Transmitiendo. Standby. Recibiendo. Estos tres operadores pasan de standby a transmitiendo */

  NRF24& operator<<(uint8_t byte);
  NRF24& operator<<(char c);
  NRF24& operator<<(char* buffer);

private:

  const SPI& spi;
  const GPIO::pin& CEN_pin, SS_pin, IRQ_pin;

  void transmitir_byte(const uint8_t b) const;
  bool transmitiendo{false};
};