cmake_minimum_required(VERSION 3.21)

set(FPU_FLAGS "-mfloat-abi=soft")
set(MCPU_TARGET "cortex-m0")
set(TARGET_MCU STM32G070xx)
set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/STM32G070RBTx_FLASH.ld)

include(${CMAKE_SOURCE_DIR}/../cmake_scripts/toolchain.cmake)

project(G070 C CXX ASM)

include(${CMAKE_SOURCE_DIR}/../cmake_scripts/common_build_steps.cmake)

include(${CMAKE_SOURCE_DIR}/../cmake_scripts/utils.cmake)
build_target(G070)