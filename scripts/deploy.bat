@echo off
REM Script de deploy gerado automaticamente
cd ..
mkdir build
cd build
cmake ..
cmake --build . --config Release
