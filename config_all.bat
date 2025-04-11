@echo off
rem Primero debes llamar update_path.bat
cd F767
mkdir build
cmake -S . -B build -G "MinGW Makefiles"
cd ..
cd G031
mkdir build
cmake -S . -B build -G "MinGW Makefiles"
cd ..
cd G070
mkdir build
cmake -S . -B build -G "MinGW Makefiles"
cd ..