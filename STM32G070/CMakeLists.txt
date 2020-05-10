cmake_minimum_required (VERSION 3.7)
project (Glamdring)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)

enable_language(C ASM)
set(CMAKE_C_STANDARD 99)

##############################################################
set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(AS arm-none-eabi-as)
set(AR arm-none-eabi-ar)
set(OBJCOPY arm-none-eabi-objcopy)
set(OBJDUMP arm-none-eabi-objdump)
set(SIZE arm-none-eabi-size)
set(GDB arm-none-eabi-gdb)
##############################################################

set(M0 "-mcpu=cortex-m0plus -mthumb")
set(CMAKE_C_FLAGS "-Wall ${M0} -fdata-sections -ffunction-sections -gdwarf-2 -MMD -MP")

set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER})
set(CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS} -x assembler-with-cpp")
##############################################################

set(ldscript "STM32G070RBTx_FLASH.ld")
set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/${ldscript})
set(CMAKE_EXE_LINKER_FLAGS_INIT "-Wl,-gc-sections,--print-memory-usage --specs=nosys.specs -T $${LINKER_SCRIPT}")
#target_link_options(glam PUBLIC SHELL:${M0} -specs=nano.specs -T../STM32G070RBTx_FLASH.ld -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections)
#target_link_libraries(glam c m nosys)

set(target ${PROJECT_NAME}.elf)

add_executable(${target}
        Src/dma.c
        Src/gpio.c Src/usart.c
        Src/main.c
        Src/stm32g0xx_hal_msp.c
        Src/stm32g0xx_hal_timebase_tim.c
        Src/stm32g0xx_it.c
        Src/system_stm32g0xx.c
        Src/tim.c
        Src/adc.c
        Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal_adc.c
        Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal_adc_ex.c
        Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal_gpio.c
        Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal_exti.c
        Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal_tim.c
        Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal_tim_ex.c
        Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal_uart.c
        Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal_uart_ex.c
        Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal_rcc.c
        Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal_rcc_ex.c
        Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_rcc.c
        Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal_flash.c
        Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal_flash_ex.c
        Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal_dma.c
        Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal_dma_ex.c
        Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal_pwr.c
        Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal_pwr_ex.c
        Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal_cortex.c
        Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal.c
        startup_stm32g070xx.s
        ${LINKER_SCRIPT} )

target_include_directories(${target} PUBLIC
        Inc
        Drivers/STM32G0xx_HAL_Driver/Inc
        Drivers/STM32G0xx_HAL_Driver/Inc/Legacy
        Drivers/CMSIS/Include
        Drivers/CMSIS/Device/ST/STM32G0xx/Include
        )

target_compile_definitions(${target} PUBLIC USE_HAL_DRIVER STM32G070xx)

set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map")

set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)

add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
        COMMAND ${OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
        COMMENT "Generando ${HEX_FILE}
        Generando ${BIN_FILE}"
        )

#add_custom_target(UPLOAD
        #)