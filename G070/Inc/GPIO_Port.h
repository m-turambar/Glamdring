//
// Created by migue on 13/06/2020.
//

#ifndef G070_GPIO_PORT_H
#define G070_GPIO_PORT_H

#include <helpers.h>
#include <g070_gpio_af.h>

#undef GPIO

namespace GPIO {

  /* Deberíamos poner el puerto 'E' para compatibilidad con otros dispositivos? */
  enum class Port {
    A = 0x50000000,
    B = 0x50000400,
    C = 0x50000800,
    D = 0x50000C00,
    F = 0x50001400
  };


  enum class Mode {
    Input = 0x0,
    Output = 0x1,
    Alternate = 0x2,
    Analog = 0x3 //reset state
  };

  enum class OutputType {
    PushPull = 0x0,
    OpenDrain = 0x1
  };

  enum class Speed {
    VeryLow = 0x0,
    Low = 0x1,
    High = 0x2,
    VeryHigh = 0x3
  };

  enum class PullResistor {
    NoPull = 0x0,
    PullUp = 0x1,
    PullDown = 0x2
  };


  /* Vamos a crear solo una instancia por cada puerto de esta clase.
   * La clase que el usuario utilizará es gpio */
  class GPIO_Port {
  public:
    GPIO_Port(const Port port)
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

    /* entrada, salida, alternate, analogico */
    void cfg_mode(const uint8_t pin, const Mode mode) const;
    /* push-pull(reset state), open-drain */
    void cfg_output_type(const uint8_t pin, const OutputType ot) const;

    void cfg_speed(const uint8_t pin, const Speed speed) const;

    void cfg_pull(const uint8_t pin, const PullResistor pupd) const;

    void cfg_alternate(const uint8_t pin, const AlternFunct afsel) const;

    /* returns 1 if IDx is set or 0 otherwise */
    uint8_t read_input(const uint8_t pin) const;

    /* El registro ODR se debe modificar indirectamente a través de escrituras al
    * registro BSRR, para asegurar atomicidad */
    uint8_t read_output(const uint8_t pin) const;

    /* usa el registro BSRR para conseguir escrituras atómicas */
    void set_output(const uint8_t pin) const;

    /* usa el registro BSRR para conseguir escrituras atómicas */
    void reset_output(const uint8_t pin) const;

    void toggle(const uint8_t pin) const;

    /* Configura un pin como entrada digital */
    void entrada(const uint8_t pin, const PullResistor pupd=PullResistor::PullUp) const;
    /* Configura un pin como salida digital */
    void salida(const uint8_t pin, const OutputType out_t=OutputType::PushPull) const;

    /* can only be written once per MCU reset */
    uint8_t lock_bits(const uint16_t bits) const;

  private:

    const size_t base;
    /* tanto BSRR como BRR modifican ODR atómicamente. La diferencia es que BSRR puede
     * setear y resetear, y BRR solo resetear. ODR es útil tenerlo por si lo queremos leer.
     * Podríamos sólo usar BSRR y omitir BRR por completo */
    const registro MODER, OTYPER, OSPEEDR, PUPDR, IDR, ODR, BSRR, LCKR, AFRL, AFRH, BRR;

  };

  /* estas instancias deben pasarse al constructor de gpio*/
  extern GPIO_Port PORTA, PORTB, PORTC, PORTD, PORTF;

/* Esta es la clase que el usuario debe usar. Se pasa como referencia uno de los GPIOs que ya existen, declarados en
 * este mismo archivo de encabezado. */
  class pin {
  public:
    pin(GPIO_Port& port_arg, const uint8_t num_arg) :
        port(port_arg),
        num(num_arg) { }

    void cfg_mode(const Mode mode) const;
    void cfg_output_type(const OutputType ot) const;
    void cfg_speed(Speed speed) const;
    void cfg_pull(PullResistor pupd) const;
    void cfg_alternate(AlternFunct afsel) const;
    uint8_t read_input() const;
    uint8_t read_output() const;
    void set_output() const;
    void reset_output() const;
    void toggle_output() const;

  private:
    const GPIO_Port& port;
    const uint8_t num;
  };

}

#endif //G070_GPIO_PORT_H
