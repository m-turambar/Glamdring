//
// Created by migue on 17/05/2020.
//

#include "../Inc/mi2c.h"

/* el namespace anónimo le da storage estático */
namespace {
  volatile size_t& memoria(const size_t loc)
  {
    return *reinterpret_cast<size_t*>(loc);
  }
}

namespace mI2C2 {

  /* offsets in bits in CR2. Think about a better safer way to put these. */
  bitfield SADDR(10, 0);
  bitfield RD_WRN(1, 10);
  bitfield START(1, 13);
  bitfield STOP(1, 14);
  bitfield NACK(1, 15);
  bitfield NBYTES(8, 16);
  bitfield AUTOEND(1, 25);
  /*        ISR               */
  bitfield TXIS(1,1);
  bitfield RXNE(1,2);
  bitfield NACKF(1, 4);
  bitfield TC(1, 6);
  bitfield BUSY(1, 15);
  /*OAR1*/
  bitfield OA1EN(1, 15);

  void init_gpios()
  {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**I2C2 GPIO Configuration
    PB10     ------> I2C2_SCL
    PB11     ------> I2C2_SDA
    */
    GPIO_InitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF6_I2C2;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  }

  void enable_clock()
  {
    __HAL_RCC_I2C2_CLK_ENABLE();
  }

  void enable()
  {
    disable();
    /* seteamos el bit PE en CR1*/
    init_gpios();
    enable_clock();
    /* must be done before enabling */
    configure_timings(timing::Standard);
    /* enable own address 1 */
    memoria(OAR1) |= OA1EN(1);
    /* enable */
    memoria(CR1) |= 0x1;
  }
  void disable()
  {
    memoria(CR1) = 0x0;
  }
  void configure_timings(size_t timing) {
    memoria(TIMINGR) = timing;
  }

  /* set write to 0 to perform a write */
  int comm_init(const size_t slave_addr, const uint8_t write, uint8_t* buffer, const size_t nbytes, const uint8_t autoend)
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
      return -1;

    if(write==0)
    {
      for(unsigned int i=0; i<nbytes; ++i)
      {
        int txis;
        do{
          nackf = memoria(ISR) & NACKF(1);
          txis = memoria(ISR) & TXIS(1);
        } while(txis==0 && nackf==0);
        if(nackf==1)
          break;
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

  return nackf;
  }

}