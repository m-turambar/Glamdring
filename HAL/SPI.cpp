//
// Created by migue on 11/12/2020.
//

#include <SPI.h>
#include "RCC.h"

const flag TXE(1);
const flag BSY(7);
const flag RXNE(0);

void SPI::enable_clock()
{
  if (peripheral == SPI::Peripheral::SPI1_I2S1)
    RCC::enable_SPI1_I2S1_clock();

  else if(peripheral == SPI::Peripheral::SPI2)
    RCC::enable_SPI2_clock();
}

void SPI::inicializar()
{
  config_baudrate(PCLK_div::d8);
  init_gpios();
  disable();
  config_role(Role::Master);
  config_software_slave_management();
  config_data_size(8);
  enable();

}

void SPI::enable()
{
  const flag SPE(6);
  CR1.set(SPE);
}

void SPI::disable()
{
  const flag SPE(6);
  CR1.reset(SPE);
  while (SR.is_set(RXNE)) {
    volatile uint8_t rcv = *reinterpret_cast<uint8_t*>(DR.addr);
  }
}

void SPI::config_baudrate(SPI::PCLK_div div)
{
  const bitfield BR(3, 3, static_cast<size_t>(div));
  CR1.write(BR);
}

void SPI::config_role(SPI::Role role)
{
  const flag MSTR(2);
  if(role == Role::Master)
    CR1.set(MSTR);
  else if(role == Role::Slave)
    CR1.reset(MSTR);
}


void SPI::config_data_size(uint8_t size)
{
  size = (size-1) & 0xF;
  const bitfield DS(4, 8, size);
  CR2.write(DS);
}

void SPI::habilitar_interrupciones_rx()
{
  const flag RXNEIE(6);
  CR2.set(RXNEIE);
}

void SPI::config_mode(SPI::Mode m)
{
  const bitfield CPHA_y_CPOL(2, 0, static_cast<size_t>(m));
  CR1.write(CPHA_y_CPOL);
}

void SPI::config_LSB_first()
{
  const flag LSBFIRST(7);
  CR1.set(LSBFIRST);
}

/** Habilita los relojes de los gpios que se usarán para la comunicación. */
void SPI::init_gpios() const
{
#ifdef STM32G070xx
  /**
   * No son las únicas posibles configuraciones. Hay más GPIOS que pueden usarse para cada función.
   * También no necesariamente tienes que usar todos en AF0 o todos en AF1
   * Resulta ser el caso que terminas modificando estos valores según la aplicación.
   * Es probable que esto debas delegarlo a código de aplicación en lugar de modificar el driver cada vez.
  */

  if(peripheral==Peripheral::SPI1_I2S1) {
    RCC::enable_port_clock(RCC::GPIO_Port::A);
    GPIO::PORTA.pin_for_SPI(5, GPIO::AlternFunct::AF0);
    GPIO::PORTA.pin_for_SPI(6, GPIO::AlternFunct::AF0);
    GPIO::PORTA.pin_for_SPI(7, GPIO::AlternFunct::AF0);
    //GPIO::PORTA.pin_for_UART_or_SPI(4, GPIO::AlternFunct::AF0_SPI1); //Cómo usarlo con dos esclavos en bus?
  }
  else if(peripheral==Peripheral::SPI2) {
    RCC::enable_port_clock(RCC::GPIO_Port::B);
    GPIO::PORTB.pin_for_SPI(11, GPIO::AlternFunct::AF0);
    GPIO::PORTB.pin_for_SPI(14, GPIO::AlternFunct::AF0);
    GPIO::PORTB.pin_for_SPI(13, GPIO::AlternFunct::AF0);
    //GPIO::PORTB.pin_for_UART_or_SPI(12, GPIO::AlternFunct::AF0_SPI2);
  }

#elif defined(STM32G031xx)
  if(peripheral==Peripheral::SPI1_I2S1) {
    RCC::enable_port_clock(RCC::GPIO_Port::A);
    GPIO::PORTA.pin_for_SPI(5, GPIO::AlternFunct::AF0);
    GPIO::PORTA.pin_for_SPI(6, GPIO::AlternFunct::AF0);
    GPIO::PORTA.pin_for_SPI(7, GPIO::AlternFunct::AF0);
  }

  #elif defined(STM32F767xx)
  if(peripheral==Peripheral::SPI1_I2S1) {
    RCC::enable_port_clock(RCC::GPIO_Port::A);
    //GPIO::PORTA.pin_for_SPI(4, GPIO::AlternFunct::AF5); // NSS
    GPIO::PORTA.pin_for_SPI(5, GPIO::AlternFunct::AF5); // CLK
    GPIO::PORTA.pin_for_SPI(6, GPIO::AlternFunct::AF5); //MISO
    GPIO::PORTA.pin_for_SPI(7, GPIO::AlternFunct::AF5); //MOSI
  }
#endif

}


/** Cuidado con el acceso lectura/escritura a DR */
uint8_t SPI::escribir(const uint8_t msg)
{
  while(SR.is_reset(TXE)) {}
  volatile uint8_t* dr_ptr = reinterpret_cast<uint8_t*>(DR.addr); //Importante que sea un pointer a uint8_t y no a uint16_t
  *dr_ptr = msg;

  while(SR.is_reset(RXNE)) {}
  volatile uint8_t rcv = *dr_ptr;
  return rcv;
}

void SPI::config_software_slave_management()
{
  const flag SSM(9);
  CR1.set(SSM);
  const flag SSI(8);
  CR1.set(SSI);
  //const flag NSSP(3);
  //CR2.set(NSSP);
  const flag SSOE(3);
  CR2.set(SSOE); // Deshabilita multi-masters
  //const flag FRF(4);
  //CR2.set(FRF); // Motorola vs TI mode
  const flag FRXTH(12);
  CR2.set(FRXTH);
}
