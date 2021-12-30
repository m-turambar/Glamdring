//
// Created by migue on 13/06/2020.
//

#ifndef G070_GPIO_PORT_H
#define G070_GPIO_PORT_H

#include <helpers.h>
#include <gpio_af.h>
#include <NVIC.h>

namespace GPIO {
  class pin;

#ifdef STM32L475xx

  enum class Port {
    A = 0x48000000,
    B = 0x48000400,
    C = 0x48000800,
    D = 0x48000C00,
    F = 0x48001400
  };

#elif defined(STM32G070xx) || defined(STM32G031xx)

  /* Deberíamos poner el puerto 'E' para compatibilidad con otros dispositivos? */
  enum class Port {
    A = 0x50000000,
    B = 0x50000400,
    C = 0x50000800,
    D = 0x50000C00,
    F = 0x50001400
  };

#elif defined(STM32F767xx)

  enum class Port {
    A = 0x40020000,
    B = 0x40020400,
    C = 0x40020800,
    D = 0x40020C00,
    E = 0x40021000,
    F = 0x40021400,
    G = 0x40021800,
    H = 0x40021C00,
    I = 0x40022000,
    J = 0x40022400,
    K = 0x40022800
  };

#endif

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
    const GPIO_Port& cfg_mode(const uint8_t pin, const Mode mode) const;
    /* push-pull(reset state), open-drain */
    const GPIO_Port& cfg_output_type(const uint8_t pin, const OutputType ot) const;

    const GPIO_Port& cfg_speed(const uint8_t pin, const Speed speed) const;

    const GPIO_Port& cfg_pull(const uint8_t pin, const PullResistor pupd) const;

    const GPIO_Port& cfg_alternate(const uint8_t pin, const AlternFunct afsel) const;

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

    /* Configura un pin como entrada digital con pull-up = alto (por default)*/
    GPIO::pin entrada(const uint8_t pin, const PullResistor pupd=PullResistor::PullUp) const;
    /* Configura un pin como salida digital */
    GPIO::pin salida(const uint8_t pin, const OutputType out_t=OutputType::PushPull) const;

    /* Configura un pin para usarse con I2C, OpenDrain con PullUp */
    void pin_for_I2C(const uint8_t pin, const AlternFunct af) const;
    /* Configura un pin con PushPull noPull */
    void pin_for_UART(const uint8_t pin, const AlternFunct af) const;
    void pin_for_SPI(const uint8_t pin, const AlternFunct af) const;
    void pin_for_timer(const uint8_t pin, const AlternFunct af) const;
    void pin_for_interrupt(const uint8_t pin, const IRQn_Type IRQn) const;
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
    pin(const GPIO_Port& port_arg, const uint8_t num_arg) :
        port(port_arg),
        num(num_arg) { }

    const pin& cfg_mode(const Mode mode) const;
    const pin& cfg_output_type(const OutputType ot) const;
    const pin& cfg_speed(Speed speed) const;
    const pin& cfg_pull(PullResistor pupd) const;
    const pin& cfg_alternate(AlternFunct afsel) const;
    const pin& entrada(const PullResistor pupd=PullResistor::PullUp) const;
    const pin& salida(const OutputType out_t=OutputType::PushPull) const;
    const pin& pin_for_interrupt(IRQn_Type IRQn) const;

    uint8_t read_input() const;
    uint8_t read_output() const;
    void set_output() const;
    void reset_output() const;
    void toggle() const;

  private:
    const GPIO_Port& port;
    const uint8_t num;
  };

}

#endif //G070_GPIO_PORT_H
