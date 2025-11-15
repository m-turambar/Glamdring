cmake_minimum_required(VERSION 3.21)

set(FPU_FLAGS "-mfloat-abi=soft")
set(MCPU_TARGET "cortex-m0")
set(TARGET_MCU STM32G031xx)
set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/STM32G031F8Px_FLASH.ld)

include(${CMAKE_SOURCE_DIR}/../cmake_scripts/toolchain.cmake)

project(G031 C CXX ASM)

include(${CMAKE_SOURCE_DIR}/../cmake_scripts/common_build_steps.cmake)

include(${CMAKE_SOURCE_DIR}/../cmake_scripts/utils.cmake)
build_target(Control)
build_target(Reles)
build_target(Dongle)