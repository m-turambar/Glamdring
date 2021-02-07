//
// Created by migue on 11/12/2020.
//

#include <SPI.h>
#include "RCC.h"

const flag TXE(1);
const flag BSY(7);
const flag RXNE(0);

void SPI::enable_clock() const
{
  if (peripheral == SPI::Peripheral::SPI1_I2S1)
    RCC::enable_SPI1_I2S1_clock();

  else if(peripheral == SPI::Peripheral::SPI2)
    RCC::enable_SPI2_clock();
}

void SPI::inicializar() const
{
  enable_clock();
  config_baudrate(PCLK_div::d8);
  init_gpios();
  config_role(Role::Master);
  config_software_slave_management();
  config_data_size(8);
  enable();
}

void SPI::enable() const
{
  const flag SPE(6);
  CR1.set(SPE);
}

void SPI::config_baudrate(SPI::PCLK_div div) const
{
  const bitfield BR(3, 3, static_cast<size_t>(div));
  CR1.write(BR);
}

void SPI::config_role(SPI::Role role) const
{
  const flag MSTR(2);
  if(role == Role::Master)
    CR1.set(MSTR);
  else if(role == Role::Slave)
    CR1.reset(MSTR);
}


void SPI::config_data_size(uint8_t size) const
{
  size = (size-1) & 0xF;
  const bitfield DS(4, 8, size);
  CR2.write(DS);
}

void SPI::habilitar_interrupciones_rx() const
{
  const flag RXNEIE(6);
  CR2.set(RXNEIE);
}

void SPI::config_mode(SPI::Mode m) const
{
  const bitfield CPHA_y_CPOL(2, 0, static_cast<size_t>(m));
  CR1.write(CPHA_y_CPOL);
}

void SPI::config_LSB_first() const
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
   *
   * SPI1 GPIO Configuration AF0
   * PA12     ------> SPI1_MOSI
   * PA11     ------> SPI1_MISO
   * PA1      ------> SPI1_SCK
   * PA4      ------> SPI1_NSS (opcional, modo Motorola SPI master)
   *
   * SPI2 GPIO Configuration AF0
   * PB11/15  ------> SPI2_MOSI
   * PB14     ------> SPI2_MISO
   * PB13     ------> SPI2_SCK
   * PB12     ------> SPI2_NSS (opcional, modo Motorola SPI master)
   *
   * ó
   *
   * SPI2 GPIO Configuration AF1
   * PB7/PC3  ------> SPI2_MOSI
   * PB2/PC2  ------> SPI2_MISO
   * PA0/PB8  ------> SPI2_SCK
   * PD0     ------> SPI2_NSS (opcional, modo Motorola SPI master)

  */

  if(peripheral==Peripheral::SPI1_I2S1) {
    RCC::enable_port_clock(RCC::GPIO_Port::A);
    GPIO::PORTA.pin_for_SPI(12, GPIO::AlternFunct::AF0);
    GPIO::PORTA.pin_for_SPI(11, GPIO::AlternFunct::AF0);
    GPIO::PORTA.pin_for_SPI(1, GPIO::AlternFunct::AF0);
    //GPIO::PORTA.pin_for_UART_or_SPI(4, GPIO::AlternFunct::AF0_SPI1); //Cómo usarlo con dos esclavos en bus?
  }
  else if(peripheral==Peripheral::SPI2) {
    RCC::enable_port_clock(RCC::GPIO_Port::B);
    GPIO::PORTB.pin_for_SPI(11, GPIO::AlternFunct::AF0);
    GPIO::PORTB.pin_for_SPI(14, GPIO::AlternFunct::AF0);
    GPIO::PORTB.pin_for_SPI(13, GPIO::AlternFunct::AF0);
    //GPIO::PORTB.pin_for_UART_or_SPI(12, GPIO::AlternFunct::AF0_SPI2);
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


/** Un acceso de escritura a DR guarda los datos en el TXFIFO al final de una queue a enviar.
 *
 * No mames. Con razon! Cada vez que escribes usando tu API de registros, estás leyendo.
 * Por eso tienes tantos pedos. */
void SPI::escribir(const uint8_t msg) const
{
  while(SR.is_reset(TXE)) {}
  /** Tu comando de acceso a registros hacía una lectura(!) antes de escribir. Por eso tenías tantas irregularidades. */
  volatile uint8_t* dr_ptr = reinterpret_cast<uint8_t*>(DR.addr); //Importante que sea un pointer a uint8_t y no a uint16_t, de lo contrario tendrás dos ciclos
  *dr_ptr = msg;
}

/** Si tienes problemas de lectura checa el manual. Hay muchas combinaciones para leer.
 * Un acceso de lectura a DR regresa el valor mas viejo guardado en la RXFIFO.
 * Puede ser conveniente hacer algunas lecturas para flushear la queue de recepción.
 * Esta función bloquea*/
uint8_t SPI::leer() const
{
  while(SR.is_reset(RXNE)) {}
  volatile uint8_t rcv = 0;
  volatile uint8_t* dr_ptr = reinterpret_cast<uint8_t*>(DR.addr); //Importante que sea un pointer a uint8_t y no a uint16_t
  rcv = *dr_ptr;
  while(SR.is_set(RXNE)) {}
  return rcv;
}

void SPI::config_software_slave_management() const
{
  const flag SSM(9);
  CR1.set(SSM);
  const flag SSI(8);
  CR1.set(SSI);
  //const flag NSSP(3);
  //CR2.set(NSSP);
  const flag SSOE(3); //creo que es irrelevante para mi aplicación. Permite multi-masters
  CR2.set(SSOE);
  //const flag FRF(4);
  //CR2.set(FRF);
  const flag FRXTH(12);
  CR2.set(FRXTH);
}
