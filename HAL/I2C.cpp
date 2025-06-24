//
// Created by migue on 17/05/2020.
//

#undef I2C

#include "I2C.h"
#include "RCC.h"
#include <GPIO_Port.h>


  /* offsets in bits in CR2. Think about a better safer way to put these. */
  const bitfield SADDR(10, 0);
  const bitfield RD_WRN(1, 10);
  const bitfield START(1, 13);
  const bitfield STOP(1, 14);
  const bitfield NACK(1, 15);
  const bitfield NBYTES(8, 16);
  const bitfield AUTOEND(1, 25);
  /*        ISR               */
  const flag TXIS(1);
  const flag RXNE(2);
  const flag NACKF(4);
  const flag TC(6);
  const flag BUSY(15);
  /*OAR1*/
  const flag OA1EN(15);

  /* Habilita los relojes de los gpios que se usarán para la comunicación. Claramente la
   * API de C es más concisa, pero la mía es más type safe, y podría extenderse para configurar
   * más de 1 pin a la vez. */
  void I2C::init_gpios() const
  {

#ifdef STM32G070xx
    /**
     * I2C2 GPIO Configuration
     * PB10     ------> I2C2_SCL
     * PB11     ------> I2C2_SDA
     *
     * I2C1 GPIO Configuration
     * PA9      ------> I2C1_SCL
     * PA10     ------> I2C1_SDA
    */

    if(peripheral==Peripheral::I2C2) {
      RCC::enable_port_clock(RCC::GPIO_Port::B);
      GPIO::PORTB.pin_for_I2C(10, GPIO::AlternFunct::AF6);
      GPIO::PORTB.pin_for_I2C(11, GPIO::AlternFunct::AF6);
    }
    else if(peripheral==Peripheral::I2C1) {
      RCC::enable_port_clock(RCC::GPIO_Port::A);
      GPIO::PORTA.pin_for_I2C(9, GPIO::AlternFunct::AF6);
      GPIO::PORTA.pin_for_I2C(10, GPIO::AlternFunct::AF6);
    }

#elif defined(STM32G031xx)
    RCC::enable_port_clock(RCC::GPIO_Port::B);
    GPIO::PORTB.pin_for_I2C(6, GPIO::AlternFunct::AF6); // CLK
    GPIO::PORTB.pin_for_I2C(7, GPIO::AlternFunct::AF6); // DA

#endif
  }

  /** Nota
   * Dejarle esta responsabilidad al RCC es correcto ya que los detalles pueden cambiar de micro a micro. */
  void I2C::enable_clock() const
  {
    if(peripheral==Peripheral::I2C1) {
      RCC::enable_I2C1_clock();
    }
    else if(peripheral==Peripheral::I2C2) {
      RCC::enable_I2C2_clock();
    }
  }

  void I2C::enable(Timing timing) const
  {
    enable_clock();
    disable();
    /* seteamos el bit PE en CR1*/
    init_gpios();
    /* must be done before enabling */
    configure_timings(timing);
    /* enable own address 1 */
    OAR1.set(OA1EN);
    /* enable */
    memoria(CR1) |= 0x1;
  }
  void I2C::disable() const
  {
    memoria(CR1) = 0x0;
  }
  void I2C::configure_timings(const Timing timing) const
  {
    memoria(TIMINGR) = static_cast<size_t>(timing);
  }

  I2C::Status I2C::write(const size_t slave_addr, uint8_t* buffer, const size_t nbytes, const uint8_t autoend) const
  {
    return comm_init(slave_addr, 0, buffer, nbytes, autoend);
  }

  I2C::Status I2C::read(const size_t slave_addr, uint8_t* buffer, const size_t nbytes, const uint8_t autoend) const
  {
    return comm_init(slave_addr, 1, buffer, nbytes, autoend);
  }

  /* set write to 0 to perform a write */
  I2C::Status I2C::comm_init(const size_t slave_addr, const uint8_t write, uint8_t* buffer, const size_t nbytes, const uint8_t autoend) const
  {
    while(ISR.is_set(BUSY)); //wait if bus is initially busy
    /* CR2 register is 0 by default after reset, meaning some things we need not configure */
    /* 1. Addressing mode ADD10 value 0 means 7-bit addressing. */
    /* 2. Set the slave address to be sent */
    /* 3. set transfer direction */
    /* 4. the number of bytes to be transferred. */
    /* addendum: AUTOEND to determine what happens after NBYTES have been transferred. */
    /* 5. set the start bit. */

    size_t cr2 = SADDR(slave_addr)
        + RD_WRN(write)
        + NBYTES(nbytes)
        + AUTOEND(autoend)
        + START(1);

    memoria(CR2) = cr2;

    /* now the master automatically sends the START condition followed by the slave address as
     * soon as it detects that the bus is free (BUSY=0) and after a delay of tBUF
     * This would be interesting to see on the scope.
     * The START bit is cleared by hardware as soon as the slave addr has been sent on the bus. */

    if(ISR.is_set(NACKF))
      return Status::NACKF;

    if(write==0)
    {
      for(unsigned int i=0; i<nbytes; ++i)
      {
        while(ISR.is_reset(TXIS)) { if(ISR.is_set(NACKF)) break; }

        if(ISR.is_set(NACKF))
          return Status::NACKF;

        memoria(TXDR) = buffer[i];
      }
    }
    else{
      size_t bytes_read = 0;
      while(bytes_read < nbytes)
      {
        /* why does this loop forever */
        if(ISR.is_set(RXNE))
        {
          buffer[bytes_read] = (uint8_t)(memoria(RXDR));
          bytes_read++;
        }
      }
    }

  return Status::OK;
  }

void I2C::clear_nackf() {
  ISR.reset(NACKF);
}
