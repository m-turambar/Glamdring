void pedazos()
{
    UART uart2(UART::Peripheral::USART2, 115200);
    g_uart2 = &uart2;
    uart2.enable_interrupt_rx(nullptr);
    uart2.enable_fifo().enable();

    //** Hasta que no encuentre un mejor mecanismo para hacer callbacks más sofisticados,
    // * seguiré haciendo cochinadas en las interrupciones. Hay que buscar una solución.
    UART uart3(UART::Peripheral::USART3, 9600);
    uart3.enable_interrupt_rx(nullptr).enable();

}