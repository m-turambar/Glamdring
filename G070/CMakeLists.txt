set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
cmake_minimum_required(VERSION 3.7)

include(${CMAKE_SOURCE_DIR}/../toolchain.cmake)

set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/STM32G070RBTx_FLASH.ld)

set(FPU_FLAGS "-mfloat-abi=soft")
set(COMMON_FLAGS
    "-mcpu=cortex-m0 ${FPU_FLAGS} -mthumb -mthumb-interwork -ffunction-sections -fdata-sections \
    -g -fno-common -fmessage-length=0 -specs=nosys.specs -specs=nano.specs")
set(CMAKE_CXX_FLAGS_INIT "${COMMON_FLAGS} -std=c++14")
set(CMAKE_C_FLAGS_INIT "${COMMON_FLAGS} -std=gnu99")
set(CMAKE_EXE_LINKER_FLAGS_INIT "-Wl,-gc-sections,--print-memory-usage -T ${LINKER_SCRIPT}")

PROJECT(G070 C CXX ASM)
set(CMAKE_CXX_STANDARD 17)
set(TARGET_MCU STM32G070xx)

add_definitions(
    -D__weak=__attribute__\(\(weak\)\) -D__packed=__attribute__\(\(__packed__\)\)
    -DUSE_HAL_DRIVER -DSTM32G070xx)

file(GLOB_RECURSE SOURCES "startup/*.*" "Src/*.cpp")

include_directories(../HAL ../HAL/CMSIS/Include Src)

add_subdirectory("../HAL" HAL)
add_executable(${PROJECT_NAME}.elf $<TARGET_OBJECTS:HAL> ${SOURCES} ${LINKER_SCRIPT})

set(CMAKE_EXE_LINKER_FLAGS
    "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map -u _printf_float")

set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
        COMMENT "Building ${HEX_FILE} \nBuilding ${BIN_FILE}")