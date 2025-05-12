@echo off
REM Script de build gerado automaticamente
cd ..
mkdir build
cd build
cmake ..
cmake --build . --config Release
