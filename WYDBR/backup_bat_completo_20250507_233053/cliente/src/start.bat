@echo off
REM Script de inicialização do WYDBRASIL
REM Gerado automaticamente por verificar_e_aplicar_recursos_wydbr.sh

echo Iniciando WYDBRASIL...
echo.

REM Verificar presença de DLLs necessárias
if not exist "bin\d3d9.dll" echo AVISO: d3d9.dll não encontrado.
if not exist "bin\d3dx9_43.dll" echo AVISO: d3dx9_43.dll não encontrado.

REM Iniciar o jogo
cd bin
start WYDLauncher.exe

echo.
echo Script de inicialização concluído.
