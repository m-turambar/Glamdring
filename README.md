# Glamdring
Infraestructura para microcontroladores STM32.
Desarrollo de hardware: STM32G031

El proyecto cumple en usar una capa de abstracción del hardware (HAL) completamente deslindada de la que ST provee.
Contiene una HAL compartida por todos los micros, y un folder de aplicación para cada uno.

El desarrollo de hardware para el STM32G031 fue todo un éxito. El acceso al MPU6050 y el NRF24 es bastante sólido.
En general, el proyecto Glamdring ha sido un éxito.

**************************************************
El compilador ARM-none-eabi se puede descargar desde https://developer.arm.com/downloads/-/gnu-rm
El flasher OpenOCD desde: https://openocd.org/pages/getting-openocd.html
También necesitas los drivers de ST. https://www.st.com/en/development-tools/stsw-link009.html
Cuando configures OpenOCD, usa board\st_nucleo_g0.cfg. Si lees su contenido internamente llama a target/stm32g0x.cfg así como configurar otras dependencias.
Para depurar que tus drivers estén en orden, ejecuta OpenOCD: openocd.exe -f ..\openocd\scripts\board\st_nucleo_g0.cfg

**************************************************

Las librerias desarrolladas incluyen pero no están limitadas a:
Basic timers (One shot, output compare, input capture)
General purpose timers (incomplete for more complex functions)
I2C
SPI
UART
MPU6050 i2c library.
NRF2401 spi library


Logs:
May/2020:
Desarrollo en Clion, en lugar de Notepad++.

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


****************************************

Port a STM32F767.

El M7 tiene cosas en común con el M0+. La enumeración de
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

*****************************************

https://www.youtube.com/watch?v=ew0XWd-blwE&t=2509s


**********************************************

Para configurar con CMake cualquiera de los proyectos, usa:

`cmake -S . -B build -G "MinGW Makefiles"`