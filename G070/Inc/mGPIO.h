//
// Created by migue on 13/06/2020.
//

#ifndef G070_MGPIO_H
#define G070_MGPIO_H

#include <helpers.h>

#undef GPIO

namespace GPIO {

/* Deberíamos poner el puerto 'E' para compatibilidad con otros dispositivos? */
  enum class Ioport {
    A = 0x50000000,
    B = 0x50000400,
    C = 0x50000800,
    D = 0x50000C00,
    F = 0x50001400
  };

  /* Vamos a crear solo una instancia por cada puerto de esta clase.
   * La clase que el usuario utilizará es gpio */
  class mGPIO {
  public:
    mGPIO(const Ioport port)
        :
        base(static_cast<size_t>(port)),
        MODER(base),
        OTYPER(base+0x4),
        OSPEEDR(base+0x8),
        PUPDR(base+0xC),
        IDR(base+0x10),
        ODR(base+0x14),
        BSRR(base+0x18),
        LCKR(base+0x1C),
        AFRL(base+0x20),
        AFRH(base+0x24),
        BRR(base+0x28) { }

    const size_t base;
    const registro MODER, OTYPER, OSPEEDR, PUPDR, IDR, ODR, BSRR, LCKR, AFRL, AFRH, BRR;

    enum class Mode {
      Input = 0x0,
      Output = 0x1,
      Alternate = 0x2,
      Analog = 0x3 //reset state
    };
    /* entrada, salida, alternate, analogico */
    void set_mode(const uint8_t pin, const Mode mode) const;


    enum class OutputType {
      PushPull = 0x0,
      OpenDrain = 0x1
    };
    /* push-pull(reset state), open-drain */
    void set_output_type(const uint8_t pin, const OutputType ot) const;


    enum class Speed {
      VeryLow = 0x0,
      Low = 0x1,
      High = 0x2,
      VeryHigh = 0x3
    };
    void set_speed(const uint8_t pin, const Speed speed) const;

    enum class InternalResistor {
      NoPullUpPullDown = 0x0,
      PullUp = 0x1,
      PullDown = 0x2
    };
    void set_resistor(const uint8_t pin, const InternalResistor pupd) const;

    /* returns 1 if IDx is set or 0 otherwise */
    uint8_t read_input_register(const uint8_t pin) const;
    void set_output_register(const uint8_t pin) const;
    void clear_output_register(const uint8_t pin) const;

    /* usa el registro BSRR para conseguir escrituras atómicas */
    void set_output_register_atomic(const uint8_t pin) const;
    /* usa el registro BSRR para conseguir escrituras atómicas */
    void clear_output_register_atomic(const uint8_t pin) const;
    /* can only be written once per MCU reset */
    uint8_t lock_bits(const uint16_t bits) const;

    enum class AlternFunct {
      AF0,
      AF1,
      AF2,
      AF3,
      AF4,
      AF5,
      AF6,
      AF7,
    };

    void set_AFR(const uint8_t pin, const AlternFunct afsel) const;
    void reset_od(const uint8_t pin) const;


  };

  /* estas instancias deben pasarse al constructor de gpio*/
  extern mGPIO A, B, C, D, F;

/* Esta es la clase que el usuario debe usar. Se pasa como referencia uno de los GPIOs que ya existen, declarados en
 * este mismo archivo de encabezado. */
  class gpio {
    gpio(mGPIO& port_arg, const uint8_t pin_arg) :
        port(port_arg),
        pin(pin_arg)
        { }

    const mGPIO& port;
    const uint8_t pin;
  };

}

#endif //G070_MGPIO_H
