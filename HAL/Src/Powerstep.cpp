//
// Created by migue on 13/11/2021.
//

#include "Powerstep.h"

/** Este arreglo representa el tamaño de cada registro. */
constexpr uint8_t sz_registros[28] {
  0, 3, 2, 3, // 0-3
  3, 2, 2, 2, // 4-7
  2, 1, 1, 1, // 8-B
  1, 2, 1, 1, // C-F
  1, 1, 1, 1, // 0x10-0x13
  1, 2, 1, 1, // 0x14-0x17
  2, 1, 2, 2  // 0x18-0x1B
};

Powerstep::Powerstep(const SPI &spi_arg, const GPIO::pin &SS_pin_arg)
  : spi(spi_arg)
  , SS_pin(SS_pin_arg)
{

}

uint32_t Powerstep::GetParam(Powerstep::Registro r) {
  uint32_t ret = 0;
  SS_pin.reset();

  spi.escribir(0b100000 + static_cast<uint8_t>(r));
  volatile uint8_t valor = spi.leer();

  for(int i=sz_registros[static_cast<uint8_t>(r)]-1; i>=0; --i) {
    valor = recurrent_operation(0);
    ret += (valor << 8*i);
  }

  SS_pin.set();
  return ret;
}

void Powerstep::SetParam(Powerstep::Registro r, uint32_t valor) {
  SS_pin.reset();
  spi.escribir(static_cast<uint8_t>(r));
  volatile uint8_t vacio = spi.leer();
  for(int i=sz_registros[static_cast<uint8_t>(r)]-1; i>=0; --i) {
    uint8_t m = (valor >> i*8) & 0xFF;
    vacio = recurrent_operation(m);
  }
  SS_pin.set();
}

void Powerstep::Run(bool DIR, uint32_t speed) {
  SS_pin.reset();

  spi.escribir(0b01010000 + DIR);
  volatile uint8_t valor = spi.leer();

  for(int i=2; i>=0; --i)
  {
    uint8_t m = (speed >> i*8) & 0xFF;
    valor = recurrent_operation(m);
  }

  SS_pin.set();
}

void Powerstep::StepClock(bool DIR) {

}

void Powerstep::Move(bool DIR, uint32_t steps) {
  SS_pin.reset();

  spi.escribir(0b01000000 + DIR);
  volatile uint8_t valor = spi.leer();

  for(int i=2; i>=0; --i)
  {
    uint8_t m = (steps >> i*8) & 0xFF;
    valor = recurrent_operation(m);
  }

  SS_pin.set();
}

void Powerstep::GoTo(uint32_t abs_pos) {

}

void Powerstep::GoToDir(bool DIR, uint32_t abs_pos) {

}

void Powerstep::GoUntil(bool ACT, bool DIR, uint32_t speed) {

}

void Powerstep::ReleaseSW(bool ACT, bool DIR) {

}

void Powerstep::GoHome() {

}

void Powerstep::GoMark() {

}

void Powerstep::ResetPos() {

}

void Powerstep::ResetDevice() {

}

void Powerstep::SoftStop() {
  SS_pin.reset();
  spi.escribir(0b10110000);
  spi.leer();
  SS_pin.set();
}

void Powerstep::HardStop() {

}

void Powerstep::SoftHiZ() {

}

void Powerstep::HardHiZ() {
  SS_pin.reset();
  spi.escribir(0b10101000);
  spi.leer();
  SS_pin.set();
}

uint16_t Powerstep::GetStatus() {
  uint16_t ret = 0;
  SS_pin.reset();

  spi.escribir(0b11010000);
  volatile uint8_t valor = spi.leer();

  for(int i=1; i>=0; --i) {
    valor = recurrent_operation(0);
    ret += (valor << 8*i);
  }

  SS_pin.set();
  return ret;
}

/** Esta operación se usa en casi todos los comandos. Es un ciclo de espera para que el *endejo* Powerstep
 * pueda procesar el comando que le mandamos anteriormente por SPI. */
uint8_t Powerstep::recurrent_operation(const uint8_t m) {
  SS_pin.set();
  for(volatile int i=0; i<5; ++i) { asm("nop"); }
  SS_pin.reset();

  spi.escribir(m);
  return spi.leer();
}
