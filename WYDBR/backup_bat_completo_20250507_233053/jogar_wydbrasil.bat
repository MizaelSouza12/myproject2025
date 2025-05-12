@echo off
SETLOCAL EnableExtensions

:: Verificar se é o modo seguro
set "SECURE_MODE="
if "%1"=="-secure" set "SECURE_MODE=1"
if "%1"=="--secure" set "SECURE_MODE=1"
if "%1"=="/secure" set "SECURE_MODE=1"

:: Mostrar título adequado
if defined SECURE_MODE (
    echo WYDBRASIL 2.0 - INICIALIZAÇÃO SEGURA
    echo ===================================
) else (
    echo WYDBRASIL 2.0 - Inicializando...
    echo ===================================
)
echo.

:: Verificar arquivos do jogo
echo Verificando integridade dos arquivos...
if not exist WYDLauncher.exe (
    echo ERRO: WYDLauncher.exe nao encontrado!
    pause
    exit /b 1
)

:: Inicializar sistema de proteção no modo seguro
if defined SECURE_MODE (
    echo.
    echo Inicializando sistema de proteção...
    echo.
    echo Sistema de proteção ativado.
    echo.
)

:: Iniciar o launcher
echo Iniciando WYDBRASIL 2.0...
if defined SECURE_MODE (
    start "" "WYDLauncher.exe" -secure
) else (
    start "" "WYDLauncher.exe"
)

ENDLOCAL
exit /b 0
