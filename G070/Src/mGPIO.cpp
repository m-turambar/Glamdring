//
// Created by migue on 13/06/2020.
//

#include <mGPIO.h>

#include "mGPIO.h"


namespace GPIO
{

  mGPIO A(Ioport::A), B(Ioport::B), C(Ioport::C), D(Ioport::D), F(Ioport::F);

  /* Varias de estas funciones hacen read-modify-write, y si en ese proceso ocurre una interrupción que haga lo
   * mismo, podemos tener problemas. Por eso es importante usar los registros de bloqueo del gpio, para realizar
   * operaciones atómicas. */
  void mGPIO::set_mode(const uint8_t pin, const GPIO::mGPIO::Mode mode) const
  {
    const bitfield MODEx(2, pin*2);
    const size_t temp = memoria(MODER) & !MODEx;
    memoria(MODER) = temp | MODEx(static_cast<uint8_t>(mode)); //parece trabalenguas, pero es preciso
  }

  void mGPIO::set_output_type(const uint8_t pin, const GPIO::mGPIO::OutputType ot) const
  {
    const bitfield OTx(1, pin);
    const size_t temp = memoria(OTYPER) & !OTx;
    memoria(OTYPER) = temp | OTx(static_cast<size_t>(ot));
  }

  void mGPIO::set_speed(const uint8_t pin, const mGPIO::Speed speed) const
  {
    const bitfield OSPEEDx(2, pin*2);
    const size_t temp = memoria(OSPEEDR) & !OSPEEDx;
    memoria(OSPEEDR) = temp | OSPEEDx(static_cast<uint8_t>(speed));
  }

  void mGPIO::set_resistor(uint8_t pin, const mGPIO::InternalResistor pupd) const
  {
    const bitfield PUPDx(2, pin*2);
    const size_t temp = memoria(PUPDR) & !PUPDx;
    memoria(PUPDR) = temp | PUPDx(static_cast<size_t>(pupd));
  }

  /* returns 1 if IDx is set or 0 otherwise */
  uint8_t mGPIO::read_input_register(uint8_t pin) const
  {
    const bitfield IDx(1,pin);
    return ((memoria(IDR) & IDx(1)) >> pin);
  }

  void mGPIO::set_output_register(uint8_t pin) const
  {
    const bitfield ODx(1,pin);
    const size_t temp = memoria(ODR) & !ODx;
    memoria(ODR) = temp | ODx(1);
  }

  void mGPIO::clear_output_register(uint8_t pin) const
  {
    const bitfield ODx(1, pin);
    memoria(ODR) &= !ODx;
  }

  void mGPIO::set_output_register_atomic(uint8_t pin) const
  {
    const bitfield BSx(1, pin);
    memoria(BSRR) |= BSx(1);
  }

  void mGPIO::clear_output_register_atomic(uint8_t pin) const
  {
    const bitfield BRx(1, pin+0x10); // offset 16 bits, i.e, the higher bits of the same reg as above.
    memoria(BSRR) |= BRx(1);
  }

  uint8_t mGPIO::lock_bits(uint16_t bits) const
  {
    const bitfield LCKK(1, 16);
    const bitfield LCKx(16, 0);
    const volatile size_t lckr = memoria(LCKR) & !LCKx; //asi ya no hay que hacer bitfield::() volátil
    /* hay que checar que esta expresión no se optimice */
    memoria(LCKR) = lckr | LCKx(bits) | LCKK(1);
    memoria(LCKR) = lckr | LCKx(bits) | LCKK(0);
    memoria(LCKR) = lckr | LCKx(bits) | LCKK(1);
    const uint8_t res = (memoria(LCKR) & LCKK(1)) >> 16;
    return res;
  }

  void mGPIO::set_AFR(uint8_t pin, const mGPIO::AlternFunct afsel) const
  {
    /* Hay dos registros que manejan pines bajos y pintes altos respectivamente, por eso el %32 funciona */
    const bitfield AFSEL(4, (pin*4)%32);
    if(pin < 8) {
      const size_t temp = memoria(AFRL) & !AFSEL;
      memoria(AFRL) = temp | AFSEL(static_cast<size_t>(afsel));
    } else {
      const size_t temp = memoria(AFRH) & !AFSEL;
      memoria(AFRH) = temp | AFSEL(static_cast<size_t>(afsel));
    }
  }

  void mGPIO::reset_od(uint8_t pin) const
  {
    const bitfield BRx(1, pin);
    memoria(BRR) |= BRx(1);
  }
}