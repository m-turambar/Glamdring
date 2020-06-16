//
// Created by migue on 17/05/2020.
//

#undef I2C

#include "I2C.h"
#include "helpers.h"
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
  const bitfield TXIS(1,1);
  const bitfield RXNE(1,2);
  const bitfield NACKF(1, 4);
  const bitfield TC(1, 6);
  const bitfield BUSY(1, 15);
  /*OAR1*/
  const bitfield OA1EN(1, 15);

  /* Habilita los relojes de los gpios que se usarán para la comunicación. Claramente la
   * API de C es más concisa, pero la mía es más type safe, y podría extenderse para configurar
   * más de 1 pin a la vez. */
  void I2C::init_gpios()
  {
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

      GPIO::pin pin10(GPIO::PORTB, 10);
      pin10.cfg_mode(GPIO::Mode::Alternate);
      pin10.cfg_alternate(GPIO::AlternFunct::AF6_I2C2);
      pin10.cfg_pull(GPIO::PullResistor::PullUp);
      pin10.cfg_output_type(GPIO::OutputType::OpenDrain);
      pin10.cfg_speed(GPIO::Speed::High);

      GPIO::pin pin11(GPIO::PORTB, 11);
      pin11.cfg_mode(GPIO::Mode::Alternate);
      pin11.cfg_alternate(GPIO::AlternFunct::AF6_I2C2);
      pin11.cfg_pull(GPIO::PullResistor::PullUp);
      pin11.cfg_output_type(GPIO::OutputType::OpenDrain);
      pin11.cfg_speed(GPIO::Speed::High);
    }
    else if(peripheral==Peripheral::I2C1) {
      RCC::enable_port_clock(RCC::GPIO_Port::A);
/*
      GPIO_InitTypeDef GPIO_InitStruct = {0};
      GPIO_InitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_9;
      GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
      GPIO_InitStruct.Pull = GPIO_PULLUP;
      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
      GPIO_InitStruct.Alternate = GPIO_AF6_I2C1; //vale lo mismo que el dos, asi como muchos otros perifericos
      HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
*/

      const GPIO::pin pin10(GPIO::PORTA, 10);
      pin10.cfg_mode(GPIO::Mode::Alternate);
      pin10.cfg_alternate(GPIO::AlternFunct::AF6_I2C1);
      pin10.cfg_pull(GPIO::PullResistor::PullUp);
      pin10.cfg_output_type(GPIO::OutputType::OpenDrain);
      pin10.cfg_speed(GPIO::Speed::High);

      const GPIO::pin pin9(GPIO::PORTA, 9);
      pin9.cfg_mode(GPIO::Mode::Alternate);
      pin9.cfg_alternate(GPIO::AlternFunct::AF6_I2C1);
      pin9.cfg_pull(GPIO::PullResistor::PullUp);
      pin9.cfg_output_type(GPIO::OutputType::OpenDrain);
      pin9.cfg_speed(GPIO::Speed::High);

    }
  }

  void I2C::enable_clock() const
  {
    if(peripheral==Peripheral::I2C1) {
      const bitfield I2C1EN(1, 21);
      memoria(RCC::APBENR1) |= I2C1EN(1);
    }
    else if(peripheral==Peripheral::I2C2) {
      const bitfield I2C2EN(1, 22);
      memoria(RCC::APBENR1) |= I2C2EN(1);
    }
  }

  void I2C::enable(Timing timing)
  {
    enable_clock();
    disable();
    /* seteamos el bit PE en CR1*/
    init_gpios();
    /* must be done before enabling */
    configure_timings(timing);
    /* enable own address 1 */
    memoria(OAR1) |= OA1EN(1);
    /* enable */
    memoria(CR1) |= 0x1;
  }
  void I2C::disable()
  {
    memoria(CR1) = 0x0;
  }
  void I2C::configure_timings(const Timing timing) {
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
    while(memoria(ISR) & BUSY(1)); //wait if bus is initially busy
    /* CR2 register is 0 by default after reset, meaning some things we need not configure */
    size_t cr2 = 0;
    /* 1. Addressing mode ADD10 value 0 means 7-bit addressing. */
    /* 2. Set the slave address to be sent */
    cr2 |= SADDR(slave_addr); //boy this feels better
    /* 3. set transfer direction */
    cr2 |= RD_WRN(write);
    /* 4. the number of bytes to be transferred. */
    cr2 |= NBYTES(nbytes);
    /* addendum: AUTOEND to determine what happens after NBYTES have been transferred. */
    cr2 |= AUTOEND(autoend);

    /* 5. set the start bit. The above mustn't ocurr if this bit is set. */
    cr2 |= START(1);

    /* write cr2 */
    memoria(CR2) = cr2;

    /* now the master automatically sends the START condition followed by the slave address as
     * soon as it detects that the bus is free (BUSY=0) and after a delay of tBUF
     * This would be interesting to see on the scope.
     * The START bit is cleared by hardware as soon as the slave addr has been sent on the bus. */

    int nackf = memoria(ISR) & NACKF(1);
    if(nackf==1)
      return Status::NACKF;

    uint32_t tickstart = HAL_GetTick();
    const uint32_t wait_max = 25;
    if(write==0)
    {
      for(unsigned int i=0; i<nbytes; ++i)
      {
        int txis;
        do{
          nackf = memoria(ISR) & NACKF(1);
          txis = memoria(ISR) & TXIS(1);
          size_t current_tick = HAL_GetTick();
          if(current_tick - tickstart > wait_max) {
            return Status::Timeout;
          }
        } while(txis==0 && nackf==0);

        if(nackf==1)
          return Status::NACKF;
        /* write TXDR */
        memoria(TXDR) = buffer[i];
      }
    }
    else{
      size_t bytes_read = 0;
      while(bytes_read < nbytes)
      {
        /* why does this loop forever */
        if((memoria(ISR) & RXNE(1)) != 0)
        {
          buffer[bytes_read] = (uint8_t)(memoria(RXDR));
          bytes_read++;
        }
      }
    }

  return Status::OK;
  }
