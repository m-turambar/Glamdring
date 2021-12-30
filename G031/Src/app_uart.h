//
// Created by migue on 29/12/2021.
//

#ifndef G031_APP_UART_H
#define G031_APP_UART_H

#include "UART.h"

extern UART* g_uart2;
extern bool parsing;

void callback_uart2();
void parse_uart(uint8_t b);

#endif //G031_APP_UART_H
