@echo off
REM Script de backup gerado automaticamente
cd ..
mkdir build
cd build
cmake ..
cmake --build . --config Release
