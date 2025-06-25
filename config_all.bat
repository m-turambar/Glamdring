@echo off
rem Primero debes llamar update_path.bat
mkdir build\F767
cmake -S F767 -B build\F767 -G "MinGW Makefiles"
mkdir build\G031
cmake -S G031 -B build\G031 -G "MinGW Makefiles"
mkdir build\G070
cmake -S G070 -B build\G070 -G "MinGW Makefiles"