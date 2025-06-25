cmake_minimum_required(VERSION 3.21)

set(FPU_FLAGS "-mfloat-abi=soft") #-mfloat-abi=hard requiere verificación
set(MCPU_TARGET "cortex-m7")
set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/STM32F767ZITx_FLASH.ld)

include(${CMAKE_SOURCE_DIR}/../cmake_scripts/toolchain.cmake)

project(F767 C CXX ASM)
set(TARGET_MCU STM32F767xx)

add_definitions(
    -DARM_MATH_CM4
    -DARM_MATH_MATRIX_CHECK
    -DARM_MATH_ROUNDING
    -D__FPU_PRESENT=1
)

include(${CMAKE_SOURCE_DIR}/../cmake_scripts/common_build_steps.cmake)