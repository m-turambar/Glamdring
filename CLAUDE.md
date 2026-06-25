# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Glamdring is a bare-metal firmware and hardware project for STM32 microcontrollers. It uses a custom Hardware Abstraction Layer (HAL) instead of ST's provided HAL drivers. Language is C++17 with C for startup/linker files. Comments and identifiers are in Spanish.

## Build

### Prerequisites
- ARM toolchain: `arm-none-eabi-gcc/g++`
- MinGW (`mingw32-make`)
- CMake 3.21+
- OpenOCD + ST drivers (for flashing)

### First-time setup
```bat
update_path.bat    :: adds ARM toolchain to PATH
config_all.bat     :: creates build/ dirs and runs CMake for F767, G031, G070
```

### Compile
```bat
build_all.bat      :: runs mingw32-make for active targets
```
Currently `build_all.bat` only builds G031 (F767 and G070 lines are commented out). To build a specific target manually:
```bat
mingw32-make --silent --directory=build/G031
```
Build artifacts (`.elf`, `.hex`, `.bin`) land in `build/<TARGET>/`.

### Flash & Debug
Flashing is done via OpenOCD through the VSCode Cortex-Debug extension (`.vscode/launch.json`). There are debug profiles for Control, Reles, Dongle (all G031), G070, and F767.

## Architecture

### Layer structure
```
HAL/          ← shared hardware abstraction (all MCU targets use this)
app/          ← shared application logic (sensors, protocol, GPIO utils)
G031/         ← STM32G031 targets: Control.cpp, Reles.cpp, Dongle.cpp
G070/         ← STM32G070 target: G070.cpp
F767/         ← STM32F767 target: F767.cpp
L475/         ← STM32L475 target (uses ST HAL, not custom HAL)
cmake_scripts/← shared toolchain.cmake, common_build_steps.cmake, utils.cmake
```

Each MCU target directory has its own `CMakeLists.txt` that sets MCU-specific flags (`MCPU_TARGET`, `FPU_FLAGS`, `TARGET_MCU` define, linker script) before including the shared scripts.

### HAL design patterns
- **`registro` / `reg16`** — wrapper classes around 32/16-bit MMIO register pointers; all peripheral drivers use these instead of raw pointer casts.
- **`bitfield` / `flag`** — typed bit-field and single-bit accessors on top of `registro`.
- **`Buffer`** — circular buffer used for UART RX/TX.
- **MCU-specific subdirs inside HAL** — `HAL/G031/`, `HAL/G070/`, `HAL/F767/` contain RCC and NVIC headers that differ per MCU; selected by the `TARGET_MCU` preprocessor define.
- **ISR linkage** — ISR functions use `extern "C"` and are defined in the target `.cpp` files; peripheral objects are exposed as global pointers (e.g., `g_uart1`, `tim2_ptr`) so ISRs can reach them.
- **Callbacks** — peripherals like NRF24 expose function-pointer callbacks (`rx_dr_callback`, `tx_ds_callback`) set by the application layer.

### `cmake_scripts/utils.cmake` — `build_target(TARGET_NAME)`
This function builds a `.elf` from `HAL` and `App` object libraries plus `<TARGET_NAME>.cpp`, then post-build generates `.hex` and `.bin` via `arm-none-eabi-objcopy`.

### Supported external hardware
- **MPU6050** — 6-axis IMU via I2C (`HAL/mpu6050.*`, `app/app_acelerometro.*`)
- **NRF24L01+** — 2.4 GHz wireless transceiver via SPI (`HAL/NRF24.*`, `app/app_nrf24.*`)

### Python tooling
`gui/main.py` — Tkinter serial monitor with PWM control panel, used for interactive testing. `test/` contains Python echo-test scripts.
