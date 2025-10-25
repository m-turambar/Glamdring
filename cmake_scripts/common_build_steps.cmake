add_subdirectory("../HAL" HAL)
file(GLOB_RECURSE SOURCES "*.s")

# This should later probably be moved to each target
add_library(App OBJECT)
target_include_directories(App PRIVATE ../HAL)
target_sources(App PRIVATE
    ../app/app_acelerometro.h
    ../app/app_acelerometro.cpp
    ../app/app_gpio.h
    ../app/app_gpio.cpp
    ../app/app_nrf24.h
    ../app/app_nrf24.cpp
    ../app/Procesador.h
    ../app/Procesador.cpp
)