# Glamdring
Experiments with STM32 microcontrollers.
The idea is to not use any of the HAL that ST provides. Only use ARM-specific core code.
Focusing on the NUCLEO-G070RB Cortex-M0+ development unit.
Now also 

Developing code to practice device driver writing, and some libraries to interact with sensors.

Libraries developed so far:
Basic timers (One shot, output compare, input capture)
General purpose timers (incomplete for more complex functions)
I2C
SPI
UART
MPU6050 i2c library.
NRF2401 spi library


Logs:
May/2020:
We (I) now develop using Clion.

This is far superior to NP++, since debugging is possible with register viewing. It is the best embedded environment I have developed in so far.

Feb/2021:
Porting G070 code to L475 took about a day.
You needed to be mindful of the RCC, the NVIC, the startup of the clocks, and the alternate functions of the GPIOs. It took one day.
The RCC changed in the registers that update specific peripherals. In many cases the offsets in these registers was the exact same.
The NVIC changed the interrupt offsets, so you had to modify NVIC.h, which is incomplete for the L475. Comms peripherals also now have interrupts for errors.
You also spent some time conditionally defining function names, since the C linkage must be taken into account. So this must be done inside the header file.
Example, that conditionally changes the definition of the interrupt handler for TIM6:

#ifdef STM32L475xx
#define TIM6_IRQHandler(void) TIM6_DAC_IRQHandler(void)
#endif

Porting to other MCUs will be easy, but we need to split application code from driver code, and share drivers that can be shared.

****************************************

OK Porting to other MCUs is *not* easy.

I just ported to STM32F767 and there were nuances.

Ok español: El M7 tiene cosas en común con el M0+. La enumeración de
selección de relojes para SYSCLK es más parecida al M0+ que al M4.
Supongo que esto es por arquitectura de ST y no de ARM.

El módulo PWR también debe ser específico de cada micro. Varios bitfields cambian
de offset en registros como CR. El M7 bootea más "listo" que el M4, similar
al M0+. Para un arranque con el HSI16 no hace falta demasiada configuración.

En el RCC el bitfield SYSCFGEN me lo movieron también y tomó tiempo verlo.

La dirección de UART1 también cambia. UART 2,3,4 tienen las mismas direcciones.
SPI 1 y 2 tienen las mismas direcciones.

Dividí el proyecto en folders de aplicación para cada micro, y una HAL en
común para todos.

******************************************

Problemas con el USART vs UART - no son lo mismo

*****************************************

Lee esto después: Todo el documento pues https://www.mouser.mx/news/stmicroelectronics-stspin-ebook/mobile/index.html#p=18