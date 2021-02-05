//
// Created by migue on 13/06/2020.
//

#include "GPIO_Port.h"
#include "RCC.h"


namespace GPIO
{

  GPIO_Port PORTA(Port::A), PORTB(Port::B), PORTC(Port::C), PORTD(Port::D), PORTF(Port::F);

  /* Varias de estas funciones hacen read-modify-write, y si en ese proceso ocurre una interrupción que haga lo
   * mismo, podemos tener problemas. Por eso es importante usar los registros de bloqueo del gpio, para realizar
   * operaciones atómicas. */
  const GPIO_Port& GPIO_Port::cfg_mode(const uint8_t pin, const Mode mode) const
  {
    //const bitfield MODEx(2, pin*2);
    //const size_t temp = memoria(MODER) & !MODEx;
    //memoria(MODER) = temp | MODEx(static_cast<size_t>(mode)); //parece trabalenguas, pero es preciso
    const bitfield MODERx(2, pin*2, static_cast<uint8_t>(mode));
    MODER.write(MODERx);
    return *this;
  }

  const GPIO_Port& GPIO_Port::cfg_output_type(const uint8_t pin, const GPIO::OutputType ot) const
  {
    const bitfield OTx(1, pin);
    const size_t temp = memoria(OTYPER) & !OTx;
    memoria(OTYPER) = temp | OTx(static_cast<size_t>(ot));
    return *this;
  }

  const GPIO_Port& GPIO_Port::cfg_speed(const uint8_t pin, const Speed speed) const
  {
    const bitfield OSPEEDx(2, pin*2);
    const size_t temp = memoria(OSPEEDR) & !OSPEEDx;
    memoria(OSPEEDR) = temp | OSPEEDx(static_cast<size_t>(speed));
    return *this;
  }

  const GPIO_Port& GPIO_Port::cfg_pull(const uint8_t pin, const PullResistor pupd) const
  {
    const bitfield PUPDx(2, pin*2);
    const size_t temp = memoria(PUPDR) & !PUPDx;
    memoria(PUPDR) = temp | PUPDx(static_cast<size_t>(pupd));
    return *this;
  }

  const GPIO_Port& GPIO_Port::cfg_alternate(const uint8_t pin, const AlternFunct afsel) const
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
    return *this;
  }

  /* returns 1 if IDx is set or 0 otherwise */
  uint8_t GPIO_Port::read_input(const uint8_t pin) const
  {
    const bitfield IDx(1,pin);
    return ((memoria(IDR) & IDx(1)) >> pin);
  }


  uint8_t GPIO_Port::read_output(const uint8_t pin) const
  {
    const bitfield ODx(1,pin);
    return ((memoria(ODR) & ODx(1)) >> pin);
  }

  /* usa BSRR para conseguir una escritura atómica a ODR */
  void GPIO_Port::set_output(const uint8_t pin) const
  {
    const flag BSx(pin);
    BSRR.set(BSx);
  }

  /* Tras ver que el M7 NO tiene registro BRR, y todo lo hace a través del BSRR,
   * decidí cambiar esta implementación a que usara BSRR para lograr la máxima portabilidad
   * posible. */
  void GPIO_Port::reset_output(const uint8_t pin) const
  {
    /* offset 16 bits, i.e, the higher bits of the same reg as above. */
    const flag BRx(pin + 16u);
    BSRR.set(BRx);
  }

  uint8_t GPIO_Port::lock_bits(const uint16_t bits) const
  {
    const bitfield LCKK(1, 16);
    const bitfield LCKx(16, 0);
    const size_t lckr = memoria(LCKR) & !LCKx;
    /* segun yo, al ser memoria(registro) volatile, esta triada no se optimizará y la secuencia se respetará */
    memoria(LCKR) = lckr | LCKx(bits) | LCKK(1);
    memoria(LCKR) = lckr | LCKx(bits) | LCKK(0);
    memoria(LCKR) = lckr | LCKx(bits) | LCKK(1);
    const uint8_t res = (memoria(LCKR) & LCKK(1)) >> 16;
    return res;
  }

  void GPIO_Port::toggle(const uint8_t pin) const
  {
    const uint8_t pin_val = read_output(pin);
    if(pin_val==0)
      set_output(pin);
    else
      reset_output(pin);
  }

  GPIO::pin GPIO_Port::entrada(const uint8_t pin, const PullResistor pupd) const
  {
    reset_output(pin);
    cfg_mode(pin, Mode::Input);
    cfg_pull(pin, pupd);
    cfg_speed(pin, Speed::VeryLow);
    return GPIO::pin(*this, pin);
  }

  GPIO::pin GPIO_Port::salida(const uint8_t pin, const OutputType out_t) const
  {
    reset_output(pin);
    cfg_mode(pin, Mode::Output);
    cfg_pull(pin, PullResistor::NoPull);
    cfg_output_type(pin, out_t);
    cfg_speed(pin, Speed::VeryLow);
    return GPIO::pin(*this, pin);
  }

  void GPIO_Port::pin_for_UART(const uint8_t pin, const AlternFunct af) const
  {
    reset_output(pin);
    cfg_mode(pin, Mode::Alternate);
    cfg_alternate(pin, af);
    cfg_output_type(pin, OutputType::PushPull);
    cfg_pull(pin, PullResistor::NoPull);
    cfg_speed(pin, Speed::VeryLow); //ayuda a reducir el ruido
  }

  void GPIO_Port::pin_for_SPI(const uint8_t pin, const AlternFunct af) const
  {
    reset_output(pin);
    cfg_mode(pin, Mode::Alternate);
    cfg_alternate(pin, af);
    cfg_output_type(pin, OutputType::PushPull);
    cfg_pull(pin, PullResistor::NoPull);
    cfg_speed(pin, Speed::VeryLow); //ayuda a reducir el ruido
  }

  void GPIO_Port::pin_for_I2C(uint8_t pin, AlternFunct af) const
  {
    cfg_mode(pin, Mode::Alternate);
    cfg_alternate(pin, af);
    cfg_output_type(pin, OutputType::OpenDrain);
    cfg_pull(pin, PullResistor::PullUp);
    cfg_speed(pin, Speed::VeryLow);
  }

  /*************************************************************************************************************/
  /*************************************************************************************************************/
  /*************************************************************************************************************/


  const pin& pin::cfg_mode(Mode mode) const
  {
    port.cfg_mode(num, mode);
    return *this;
  }

  const pin& pin::cfg_output_type(OutputType ot) const
  {
    port.cfg_output_type(num, ot);
    return *this;
  }

  const pin& pin::cfg_speed(Speed speed) const
  {
    port.cfg_speed(num, speed);
    return *this;
  }

  const pin& pin::cfg_pull(PullResistor pupd) const
  {
    port.cfg_pull(num, pupd);
    return *this;
  }

  const pin& pin::cfg_alternate(AlternFunct afsel) const
  {
    port.cfg_alternate(num, afsel);
    return *this;
  }

  uint8_t pin::read_input() const
  {
    return port.read_input(num);
  }

  uint8_t pin::read_output() const
  {
    return port.read_output(num);
  }

  void pin::set() const
  {
    port.set_output(num);
  }

  void pin::reset() const
  {
    port.reset_output(num);
  }
}