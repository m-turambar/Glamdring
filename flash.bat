@echo off
cmd /c A:\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\STM32_Programmer_CLI.exe -c port=SWD -w build\STM32G070.bin 0x08000000 -s