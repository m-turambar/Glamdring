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
    RF_CH = 5,
    Status = 7,
    RX_PW_P0 = 0x11,
    FIFO_STATUS = 0x17
  };

  enum class Modo : uint8_t {
    TX = 0,
    RX = 1
  };

  NRF24(const SPI& spi_arg, const GPIO::pin& SS_pin, const GPIO::pin& CEN_pin);

  void config_default() const;
  void encender(Modo modo) const;

  void transmitir_byte(const uint8_t b) const;
  uint8_t leer_rx() const;

  uint8_t leer_registro(Registro reg) const;
  void escribir_registro(Registro reg, uint8_t val) const;
  uint8_t flush_tx_fifo() const;
  uint8_t flush_rx_fifo() const;
  void clear_interrupts() const;
  void config_payload_widths(uint8_t width) const;

private:

  const SPI& spi;
  const GPIO::pin& CEN_pin, SS_pin;
};