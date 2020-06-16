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
  void GPIO_Port::cfg_mode(const uint8_t pin, const Mode mode) const
  {
    const bitfield MODEx(2, pin*2);
    const size_t temp = memoria(MODER) & !MODEx;
    memoria(MODER) = temp | MODEx(static_cast<size_t>(mode)); //parece trabalenguas, pero es preciso
  }

  void GPIO_Port::cfg_output_type(const uint8_t pin, const GPIO::OutputType ot) const
  {
    const bitfield OTx(1, pin);
    const size_t temp = memoria(OTYPER) & !OTx;
    memoria(OTYPER) = temp | OTx(static_cast<size_t>(ot));
  }

  void GPIO_Port::cfg_speed(const uint8_t pin, const Speed speed) const
  {
    const bitfield OSPEEDx(2, pin*2);
    const size_t temp = memoria(OSPEEDR) & !OSPEEDx;
    memoria(OSPEEDR) = temp | OSPEEDx(static_cast<size_t>(speed));
  }

  void GPIO_Port::cfg_pull(const uint8_t pin, const PullResistor pupd) const
  {
    const bitfield PUPDx(2, pin*2);
    const size_t temp = memoria(PUPDR) & !PUPDx;
    memoria(PUPDR) = temp | PUPDx(static_cast<size_t>(pupd));
  }

  void GPIO_Port::cfg_alternate(const uint8_t pin, const AlternFunct afsel) const
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
    const bitfield BSx(1, pin);
    memoria(BSRR) |= BSx(1);
  }

  /* Usa BRR para resetear atómicamente ODR. Podría usar los 16 MSb de BSRR para lograr
   * lo mismo, simplemente me pareció agradable usar BRR. Si algún día quieres ahorrar
   * unos pocos bytes de RAM o flash, puedes eliminar BRR de esta clase y hacerlo todo
   * con BSRR. */
  void GPIO_Port::reset_output(const uint8_t pin) const
  {
    /* offset 16 bits, i.e, the higher bits of the same reg as above. */
    const bitfield BRx(1, pin);
    memoria(BRR) |= BRx(1);
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

  /*************************************************************************************************************/
  /*************************************************************************************************************/
  /*************************************************************************************************************/


  void pin::cfg_mode(Mode mode) const
  {
    port.cfg_mode(num, mode);
  }

  void pin::cfg_output_type(OutputType ot) const
  {
    port.cfg_output_type(num, ot);
  }

  void pin::cfg_speed(Speed speed) const
  {
    port.cfg_speed(num, speed);
  }

  void pin::cfg_pull(PullResistor pupd) const
  {
    port.cfg_pull(num, pupd);
  }

  void pin::cfg_alternate(AlternFunct afsel) const
  {
    port.cfg_alternate(num, afsel);
  }

  uint8_t pin::read_input() const
  {
    return port.read_input(num);
  }

  uint8_t pin::read_output() const
  {
    return port.read_output(num);
  }

  void pin::set_output() const
  {
    port.set_output(num);
  }

  void pin::reset_output() const
  {
    port.reset_output(num);
  }
}