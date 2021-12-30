//
// Created by migue on 13/12/2020.
//

#include <NRF24.h>
#include <NVIC.h>
#include <cstring>

NRF24* NRF24_ptr{nullptr};

/** Las tres banderas de interrupción que se pueden leer/clearear desde el registro STATUS */
constexpr uint8_t RX_DR = (1 << 6);
constexpr uint8_t TX_DS = (1 << 5);
constexpr uint8_t MAX_RT = (1 << 4);

enum class Commands : uint8_t {
  R_REGISTER = 0b00000000, //orear con un registro
  W_REGISTER = 0b00100000,
  R_RX_PAYLOAD = 0b01100001,
  W_TX_PAYLOAD = 0b10100000,
  FLUSH_TX = 0b11100001,
  FLUSH_RX = 0b11100010,
  REUSE_TX_PL = 0b11100011,
  R_RX_PL_WID = 0b01100000,
  /**/
  /**/
  NOP = 0b11111111,
};



NRF24::NRF24(const SPI& spi_arg, const GPIO::pin& SS_pin, const GPIO::pin& CEN_pin)
  :spi(spi_arg)
  ,CEN_pin(CEN_pin)
  ,SS_pin(SS_pin)
{
  CEN_pin.salida(); //considerar hacer por hardware
  SS_pin.salida();
  SS_pin.set_output(); //disable SS
  flush_rx_fifo();
  flush_tx_fifo();
  clear_all_interrupts();
  config_payload_widths(1); //todo dinamico
  NRF24_ptr = this;
}



void NRF24::transmitir_byte(const uint8_t b) const
{
  SS_pin.reset_output();
  spi.escribir(static_cast<uint8_t>(Commands::W_TX_PAYLOAD));
  spi.leer();
  spi.escribir(b);
  spi.leer(); //necesario?
  SS_pin.set_output();
}

uint8_t NRF24::leer_rx() const
{
  SS_pin.reset_output();
  spi.escribir(static_cast<uint8_t>(Commands::R_RX_PAYLOAD));
  spi.leer();
  spi.escribir(0u);
  uint8_t rcvd = spi.leer();
  SS_pin.set_output();
  return rcvd;
}

void NRF24::encender(NRF24::Modo modo)
{
  auto config = leer_registro(Registro::Config);
  config = config | 2u | static_cast<uint8_t>(modo); //PWR_UP y modo.
  escribir_registro(Registro::Config, config);
  modo_cached = modo;

  CEN_pin.set_output(); //turn on
}

NRF24::Modo NRF24::obtener_modo() const {
  auto config = leer_registro(Registro::Config);
  return (config % 2 == 0) ? NRF24::Modo::RX : NRF24::Modo::TX;
}

void NRF24::escribir_registro(NRF24::Registro reg, uint8_t val) const
{
  SS_pin.reset_output();
  spi.escribir(static_cast<uint8_t>(Commands::W_REGISTER) | static_cast<uint8_t>(reg));
  spi.leer();
  spi.escribir(val);
  spi.leer();
  SS_pin.set_output();
}

/** El problema es el siguiente. Como el NRF24 *siempre* manda su status register al escribirle lo que sea,
 * nuestro buffer de recepción del SPI no está vacío cuando lo queremos leer. PERO al NRF24 no le ha dado tiempo de
 * responder y mandar el registro que le pedimos.
 * En otras palabras, en el lapso de poco más de 16us que involucran los 2 ciclos de 8 pulsos del SPI,
 * nuestro código está leyendo el registro del SPI a la mitad de esos dos ciclos. En esa mitad el nibble
 * donde se guardó el status register ya está poblado, pero el otro nibble donde está el registro que queremos leer
 * NO se ha poblado.
 * Por eso nos da 0 -> porque leemos 0b00000111 00000000, cuando esperábamos leer 0b00000111 00001000
 *
 * Solución: Hay un bit en el SPI que se llama FRXTH. Éste define el umbral para activar el bit RXNE.
 * Si FRXTH es 0, nuestro RXNE se activará sólo cuando tengamos 16 bits en el registro.
 * Como configuramos FRXTH con 1, RXNE se está activando cuando los 8 bits del estatus llegaron.
 * De igual importancia es que el driver de SPI usemos un pointer de 8 bits y no de 16 bits.
 * Si usas uno de 8 bits trabajamos con unidades de 8 pulsos de reloj*/
uint8_t NRF24::leer_registro(NRF24::Registro reg) const
{
  SS_pin.reset_output();
  spi.escribir(static_cast<uint8_t>(reg));
  /** Automáticamente se envía el status register del NRF24, por eso vamos a descartar esta lectura */
  volatile uint8_t valor = spi.leer();
  /** Escribimos lo que sea. El NRF24 nos mandará el contenido del registro que pedimos anteriormente. */
  spi.escribir(0u);
  valor = spi.leer();

  SS_pin.set_output();
  return valor;
}

void NRF24::config_default() const
{
  const uint8_t default_cfg = (1 << 3); //EN_CRC
  escribir_registro(Registro::Config, default_cfg);

  const uint8_t setup_retr = 8 + (1 << 4); /// 8 intentos de transmitir, 500uS entre cada intento
  escribir_registro(Registro::SETUP_RETR, setup_retr);
}

uint8_t NRF24::flush_tx_fifo() const
{
  SS_pin.reset_output();
  spi.escribir(static_cast<uint8_t>(Commands::FLUSH_TX));
  uint8_t status = spi.leer();
  SS_pin.set_output();
  return status;
}

uint8_t NRF24::flush_rx_fifo() const
{
  SS_pin.reset_output();
  spi.escribir(static_cast<uint8_t>(Commands::FLUSH_RX));
  uint8_t status = spi.leer();
  SS_pin.set_output();
  return status;
}


void NRF24::config_payload_widths(uint8_t width) const
{
  width = width & 0b11111;
  escribir_registro(Registro::RX_PW_P0, width);
}

void NRF24::clear_all_interrupts() const
{
  SS_pin.reset_output();
  uint8_t status = leer_registro(Registro::Status);
  /** Clear by writing 1 to MAX_RT, TX_DS and TX_DS */
  status |= (1 << 4);
  status |= (1 << 5);
  status |= (1 << 6);
  escribir_registro(Registro::Status, status);
  SS_pin.set_output();
}

/** Este callback lo pondremos dentro de la interrupción del pin.
 * El pin del NRF24 interrumpirá cuando cualquiera de estas condiciones se cumpla,
 * siempre y cuando estén habilitadas en CONFIG.
 * El loop de lectura de la RX_FIFO debe ocurrir en el rx_dr_callback(). */
void NRF24::irq_handler() {
  volatile uint8_t status = leer_registro(Registro::Status);
  if(status & RX_DR) {
    if(rx_dr_callback != nullptr) {
      rx_dr_callback();
    }
  }

  if(status & TX_DS) {
    ++idx_enviar;
    if(idx_enviar == idx_llenar) { /// si ya transmitimos lo que teníamos que transmitir...
      transmitiendo = false;
    }
    else {
      transmitir_byte(tx_buf[idx_enviar]);
    }

    if(tx_ds_callback != nullptr) {
      tx_ds_callback();
    }
  }

  if(status & MAX_RT) {
    /// No hay necesidad de iniciar una re-transmisión aquí. Lo que podrías hacer es telemetría y medir errores.
    if(max_rt_callback != nullptr) {
      max_rt_callback();
    }
  }

}

NRF24& NRF24::operator<<(uint8_t byte) {
  tx_buf[idx_llenar] = byte;
  ++idx_llenar;
  if(transmitiendo == false) {
    transmitiendo = true;
    transmitir_byte(tx_buf[idx_enviar]);
  }
  return *this;
}

NRF24& NRF24::operator<<(char c) {
  return this->operator<<(static_cast<uint8_t>(c));
}

NRF24& NRF24::operator<<(char *buffer) {
  const auto sz = std::strlen(buffer);
  for(int i = 0; i < sz; ++i) {
    tx_buf[idx_llenar] = buffer[i];
    ++idx_llenar;
  }
  if(transmitiendo == false) {
    transmitiendo = true;
    transmitir_byte(tx_buf[idx_enviar]);
  }

  return *this;
}

NRF24 &NRF24::operator<<(const char *buffer) {
  const auto sz = std::strlen(buffer);
  for(int i = 0; i < sz; ++i) {
    tx_buf[idx_llenar] = buffer[i];
    ++idx_llenar;
  }
  if(transmitiendo == false) {
    transmitiendo = true;
    transmitir_byte(tx_buf[idx_enviar]);
  }

  return *this;
}