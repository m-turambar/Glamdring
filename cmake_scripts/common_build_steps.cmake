add_definitions(
    -D__weak=__attribute__\(\(weak\)\)
    -D__packed=__attribute__\(\(__packed__\)\)
    -D${TARGET_MCU}
)

file(GLOB_RECURSE SOURCES "startup/*.*" "src/*.cpp")

add_subdirectory("../HAL" HAL)
add_executable(${PROJECT_NAME}.elf $<TARGET_OBJECTS:HAL> ${SOURCES} ${LINKER_SCRIPT})
include_directories(
    ../HAL
    ../HAL/CMSIS/Include
    src
    ../app
)

target_sources(${PROJECT_NAME}.elf PUBLIC
    ../app/app_acelerometro.h
    ../app/app_acelerometro.cpp
    ../app/app_nrf24.h
    ../app/app_nrf24.cpp
    ../app/Procesador.h
    ../app/Procesador.cpp
)

set(CMAKE_EXE_LINKER_FLAGS
    "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map")# -u _printf_float")

set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
        COMMENT "Building ${HEX_FILE} \nBuilding ${BIN_FILE}")