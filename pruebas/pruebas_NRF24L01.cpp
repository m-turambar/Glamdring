#include "UART.h"
#include "SPI.h"
#include "NVIC.h"
#include "NRF24.h"

/***********************************/

const char* numeros = "012345678\n";
uint8_t idx = 0;
NRF24* nrf_ptr{nullptr};

void transmitir_tx(void)
{
  GPIO::PORTA.toggle(5);
  if(nrf_ptr ==nullptr)
    return;
  nrf_ptr->clear_interrupts();
  nrf_ptr->flush_tx_fifo();
  const uint8_t b = numeros[idx%10];
  nrf_ptr->transmitir_byte(b);
  ++idx;
}

/****************************************/

int test_radio_status(NRF24& radio, const uint8_t status_esperado)
{
  uint8_t status = 0;
  uint32_t err_cnt = 0;
  for(uint32_t i=0; i<1000; ++i)
  {
    status = radio.leer_registro(NRF24::Registro::Status);
    if(status != status_esperado)
      err_cnt++;
  }
  return err_cnt;
}

int test_radio_register_write_read(NRF24& radio)
{
  uint8_t val = 0;
  uint32_t err_cnt = 0;
  for(uint32_t i=0; i<128; ++i)
  {
    radio.escribir_registro(NRF24::Registro::Config, i);
    val = radio.leer_registro(NRF24::Registro::Config);
    if(val != i)
      err_cnt++;
  }
  return err_cnt;
}

/** Al tercer byte hay problemas, 14,30. Por? MAX_RT se activa en el YC*/
int test_radio_tx_rx(NRF24& tx, NRF24& rx)
{
  uint32_t err_cnt = 0;
  for(uint32_t i=0; i<255; ++i)
  {
    tx.transmitir_byte(i);
    volatile uint8_t status = rx.leer_registro(NRF24::Registro::Status);
    volatile uint8_t status_tx = tx.leer_registro(NRF24::Registro::Status);
    /** El bit[6] del STATUS register es RX_DR e indica Data Ready. Se setea cuando llegan datos*/
    while((status & (1 << 6)) == 0) //que significa 1 << 6 por el amor de dios
    {
      status_tx = tx.leer_registro(NRF24::Registro::Status);
      if((status_tx & (1 << 4)) != 0) //Si MAX_RT...:
      {
        tx.clear_interrupts();
        tx.flush_tx_fifo();
        tx.transmitir_byte(i);
      }
      status = rx.leer_registro(NRF24::Registro::Status);
    }
    uint8_t rcvd = rx.leer_rx();
    rx.clear_interrupts(); //TODO solo clear RX_DR
    tx.clear_interrupts(); //TODO solo clear TX_DS
    rx.flush_rx_fifo();
    tx.flush_tx_fifo();
    status = rx.leer_registro(NRF24::Registro::Status);
    status_tx = tx.leer_registro(NRF24::Registro::Status);
    if((status & (1 << 6)) == 0)
    {
      uint8_t recibido_de_nuevo = rx.leer_rx();
    }

    if(rcvd != i) { err_cnt++; }
  }
  return err_cnt;
}

void demo()
{
  const GPIO::pin pin_enable_radio(GPIO::PORTC, 4); //TODO
  pin_enable_radio.salida();

  const GPIO::pin ss_radio(GPIO::PORTA, 10);
  ss_radio.salida(); //considerar hacer por hardware

  SPI spi2(SPI::Peripheral::SPI2);
  spi2.inicializar();

  NRF24 radio(spi2, ss_radio, pin_enable_radio);
  nrf_ptr = &radio;
  radio.config_default();
  auto config = radio.leer_registro(NRF24::Registro::Config);
  radio.encender(NRF24::Modo::TX);
  config = radio.leer_registro(NRF24::Registro::Config);
  
  basic_timer t7(BasicTimer::TIM7, basic_timer::Mode::Periodic, 0x1800, 0x100);
  t7.enable_interrupt(transmitir_tx);
  t7.start();
}

/* Ambos SPI funcionan.
SPI spi2(SPI::Peripheral::SPI2);
spi2.inicializar();

NRF24 radio1(spi2, ss_radio1, pin_enable_radio1);
NRF24 radio2(spi2, ss_radio2, pin_enable_radio2);

radio1.config_default();
radio2.config_default();
auto config = radio1.leer_registro(NRF24::Registro::Config);
auto config2 = radio2.leer_registro(NRF24::Registro::Config);

radio1.encender(NRF24::Modo::TX);
radio2.encender(NRF24::Modo::RX);

config = radio1.leer_registro(NRF24::Registro::Config);
config2 = radio2.leer_registro(NRF24::Registro::Config);

auto status1 = radio1.leer_registro(NRF24::Registro::Status);
auto status2 = radio2.leer_registro(NRF24::Registro::Status);

auto errors = test_radio_status(radio2, 14);

config = radio1.leer_registro(NRF24::Registro::Config);
config2 = radio2.leer_registro(NRF24::Registro::Config);
errors = test_radio_tx_rx(radio1, radio2);

status2 = radio2.leer_registro(NRF24::Registro::Status);
status1 = radio1.leer_registro(NRF24::Registro::Status);
/************************************

void pasar_caracter(uint8_t b)
{
  auto& bb = timer_cfg_buf;
  if(b == '\n')
  {
    uint16_t pre = 0;
    uint16_t arr = 0;
    uint16_t ccr1 = 0;
    int i = 0;
    while(bb[i] != ',')
    {
      pre = pre*10 + bb[i]-48;
      ++i;
    }
    ++i;

    while(bb[i] != ',')
    {
      arr = arr*10 + bb[i]-48;
      ++i;
    }
    ++i;

    while(bb[i] != ',')
    {
      ccr1 = ccr1*10 + bb[i]-48;
      ++i;
    }
    tim_ptr->set_prescaler(pre);
    tim_ptr->set_autoreload(arr);
    tim_ptr->set_ccr1(ccr1);

    idx=0;
  }

  else
  {
    bb[idx] = b;
    ++idx;
  }

}
