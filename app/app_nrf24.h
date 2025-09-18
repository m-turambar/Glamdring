//
// Created by migue on 29/12/2021.
//

#ifndef G031_APP_NRF24_H
#define G031_APP_NRF24_H

#include "NRF24.h"

extern NRF24* nrf_ptr;
inline Buffer* NRF24_uart_buffer { nullptr };

void callback_nrf24_rx();
void callback_nrf24_tx_ds();
void callback_nrf24_max_rt();

void process_buffer(Buffer& buf);

#endif //G031_APP_NRF24_H
