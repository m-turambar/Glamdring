add_library(HAL OBJECT)
target_include_directories(HAL PRIVATE . CMSIS/Include)

if (NOT TARGET_MCU MATCHES "STM32G031xx")
    target_sources(HAL PRIVATE
        basic_timer.h
        basic_timer.cpp
    )
endif()

target_sources(HAL PRIVATE
    # ADC.h
    DMA.h
    DMA.cpp
    CRC.h
    CRC.cpp
    EXTI.h
    EXTI.cpp
    FLASH.h
    FLASH.cpp
    general_timer.h
    general_timer.cpp
    gpio_af.h
    GPIO_Port.h
    GPIO_Port.cpp
    helpers.h
    helpers.cpp
    I2C.h
    I2C.cpp
    IWDG.h
    IWDG.cpp
    MPU6050.h
    MPU6050.cpp
    NRF24.h
    NRF24.cpp
    NVIC.h
    NVIC.cpp
    Powerstep.h   #meh
    Powerstep.cpp #meh
    PWR.h
    PWR.cpp
    RCC.h   # !
    SCB.cpp # !
    SPI.h
    SPI.cpp
    UART.h
    UART.cpp
)
if (TARGET_MCU MATCHES "STM32F767xx")
    target_sources(HAL PRIVATE
        DAC.h
        DAC.cpp
        F767/NVIC_F767.h
        F767/RCC_F767.cpp
        F767/RCC_F767.h
    )
elseif (TARGET_MCU MATCHES "STM32G031xx")
    target_sources(HAL PRIVATE
        G031/NVIC_G031.h
        G031/RCC_G031.cpp
        G031/RCC_G031.h
    )
elseif (TARGET_MCU MATCHES "STM32G070xx")
    target_sources(HAL PRIVATE
        G070/NVIC_G070.h
        G070/RCC_G070.cpp
        G070/RCC_G070.h
    )
endif()
