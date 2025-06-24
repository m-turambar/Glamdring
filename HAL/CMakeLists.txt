add_library(HAL OBJECT)
target_include_directories(HAL PRIVATE Inc CMSIS/Include)
target_sources(HAL PRIVATE
    # ADC.h
    basic_timer.h
    basic_timer.cpp
    DMA.h
    DMA.cpp
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
    )
endif()
