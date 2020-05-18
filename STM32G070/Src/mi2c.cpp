//
// Created by migue on 17/05/2020.
//

#include "../Inc/mi2c.h"

namespace mI2C2 {

  /* offsets in bits in CR2. Think about a better safer way to put these. */
  constexpr uint8_t SADDR = 0;
  constexpr uint8_t RD_WRN = 10;
  constexpr uint8_t START = 13;
  constexpr uint8_t STOP = 14;
  constexpr uint8_t NACK = 15;
  constexpr uint8_t NBYTES = 16;
  constexpr uint8_t AUTOEND = 25;

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
    /* seteamos el bit PE en CR1*/
    init_gpios();
    enable_clock();
    /* must be done before enabling */
    configure_timings(timing::Standard);
    /* enable */
    *reinterpret_cast<size_t*>(CR1) |= 0x1;
  }
  void disable()
  {
    *reinterpret_cast<size_t*>(CR1) |= 0x0;
  }
  void configure_timings(size_t timing) {
    *reinterpret_cast<size_t*>(TIMINGR) = timing;
  }

  /* set write to 0 to perform a write */
  void comm_init(const size_t slave_addr, const uint8_t write, const size_t nbytes, const uint8_t autoend)
  {
    /* CR2 register is 0 by default after reset, meaning some things we need not configure */
    /* 1. Addressing mode ADD10 value 0 means 7-bit addressing. */
    /* 2. Set the slave address to be sent */
    *reinterpret_cast<size_t*>(CR2) |= ((slave_addr&0x3FF) << SADDR); //boy this feels brittle. 10 bits.
    /* 3. set transfer direction */
    *reinterpret_cast<size_t*>(CR2) |= ((write&1)<<RD_WRN); //boy this feels brittle. 1 for 1 bit.
    /* 4. the number of bytes to be transferred. */
    *reinterpret_cast<size_t*>(CR2) |= ((nbytes&0xFF)<<NBYTES); //boy this feels terrible. FF for 8 bits.
    /* addendum: AUTOEND to determine what happens after NBYTES have been transferred. */
    *reinterpret_cast<size_t*>(CR2) |= ((autoend&1)<<AUTOEND);

    /* 5. set the start bit. The above mustn't ocurr if this bit is set. */
    *reinterpret_cast<size_t*>(CR2) |= (0x1<<START); //boy this feels terrible. FF for 8 bits.

    /* now the master automatically sends the START condition followed by the slave address as
     * soon as it detects that the bus is free (BUSY=0) and after a delay of tBUF
     * This would be interesting to see on the scope.
     * The START bit is cleared by hardware as soon as the slave addr has been sent on the bus. */
  }

}