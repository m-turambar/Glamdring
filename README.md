# Glamdring
Experiments with STM32 microcontrollers.
Focusing on the NUCLEO-G070RB Cortex-M0+ development unit.

Developing code to practice device driver writing, and some libraries to interact with sensors.

Libraries developed so far:
HAL substitute for I2C peripheral.
MPU6050 accelerometer library.

15/Feb/2020:
Figured out a way to compile, link, and flash the NUCLEO-F767ZI (or any other NUCLEO board for that matter) with a single keystroke in NP++.
STM32CubeProgrammer must be downloaded, for its CLI will be used to flash the targets.
NppExec must be added as a plugin, and the following script should be added, and of course modify it according to your needs, and PATH:

NPP_SAVE
cd $(CURRENT_DIRECTORY)\..
mingw32-make -j4
A:\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\STM32_Programmer_CLI.exe -c port=SWD -w build\STM32F767ZI.bin 0x08000000 -s

Once you have saved this script, map it to a hotkey and voilà.

Extremely useful since this will cut down development time and eliminate developer attention loss by not having to exit NP++.

May/2020:
We (I) now develop using Clion.

This is far superior to NP++, since debugging is possible with register viewing. It is the best embedded environment I have developed in so far.
