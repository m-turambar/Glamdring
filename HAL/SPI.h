//
// Created by migue on 11/12/2020.
//

#ifndef GLAMDRING_SPI_H
#define GLAMDRING_SPI_H

#include "helpers.h"
#include "GPIO_Port.h"

/** También incluye funcionalidad de I2S (audio), pero no está implementado aún.
 * Los registros que explícitamente mencionan I2S no usan SPI.
 *
 * Procedimiento de configuración, tomado del manual:
 * Es casi el mismo para maestro que para esclavo.
 * Para comunicaciones normales:
 * 1. Configurar los GPIOS de MOSI, MISO, y SCK
 *
 * 2. Configurar CR1:
 *   a) Conf baud rate
 *   b) Conf CPOL y CPHA para definir una de las cuatro posibles modalidades de clockeo
 *   c) Seleccionar simplex o half-duplex mediant RXONLY o BIDIMODE y BIDIOE.
 *      RXONLY y BIDIMODE no pueden ser configurados al mismo tiempo.
 *   d) Conf LSBFIRST para definir el formato de los bits
 *   e) Conf CRCL y CRCEN si se necesitase CRC
 *   f) Conf SSM y SSI
 *   g) Conf MSTR (ojo si fuera multi master, ver manual)
 *
 * 3. Configurar CR2:
 *   a) Conf DS para seleccionar longitud de datos
 *   b) Conf SSOE
 *   c) Settear FRF si se necesita TI (?)
 *   d) Setear NSSP si el pulso NSS entre dos unidades de datos se necesita.
 *      Mantener CHPA y TI cleareados en modo NSSP
 *   e) Conf el FRXTH bit. siguele mañana. lol*/
class SPI
{
public:
  enum class Peripheral {
    SPI1_I2S1 = 0x40013000,
    SPI2 = 0x40003800
  };

  /** Entre cuánto se va a dividir PCLK para nuestro reloj */
  enum class PCLK_div {
    d2 = 0,
    d4 = 1,
    d8 = 2,
    d16 = 3,
    d32 = 4,
    d64 = 5,
    d128 = 6,
    d256 = 7,
  };

  enum class Role {
    Slave = 0,
    Master = 1
  };

  /** CPOL y CPHA. Polaridad y Fase del reloj. Idle low/high, rising/falling edge*/
  enum class Mode {
    LowRising = 0,
    LowFalling = 1,
    HighRising = 2,
    HighFalling = 3
  };


  constexpr SPI(SPI::Peripheral p):
      CR1(static_cast<size_t>(p)),
      CR2(static_cast<size_t>(p) + 0x04),
      SR(static_cast<size_t>(p) + 0x08),
      DR(static_cast<size_t>(p) + 0x0C),
      CRCPR(static_cast<size_t>(p) + 0x10),
      RXCRCR(static_cast<size_t>(p) + 0x14),
      TXCRCR(static_cast<size_t>(p) + 0x18),
      I2SCFGR(static_cast<size_t>(p) + 0x1C),
      I2SPR(static_cast<size_t>(p) + 0x20),
      peripheral(p)
  {
    enable_clock();
  }

  void inicializar() const;
  void config_baudrate(PCLK_div div) const;
  void config_role(Role m) const;
  void escribir(const uint8_t msg) const;
  uint8_t leer() const;
  void init_gpios() const;
  void config_LSB_first() const;
  void config_mode(Mode m) const; //solo necesario si el dispositivo es "raro".
  void config_software_slave_management() const;

  /** usar con números entre 3 y 15.
   * El tamaño a configurar es uno más que el valor en el registro.
   * e.g. config_data_size(8) -> 8-bit transfers -> se escribe 7 al registro en la impl. */
  void config_data_size(uint8_t size) const;

  void habilitar_interrupciones_rx() const;

private:
  void enable_clock() const;
  void enable() const;

  const reg16 CR1;
  const reg16 CR2;
  const reg16 SR;
  const reg16 DR;
  const reg16 CRCPR;
  const reg16 RXCRCR;
  const reg16 TXCRCR;
  const reg16 I2SCFGR;
  const reg16 I2SPR;
  const Peripheral peripheral;
};

#endif //GLAMDRING_SPI_H