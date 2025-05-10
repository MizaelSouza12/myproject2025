@echo off
REM Script de compilação para Windows - WYDBR 2.0

echo ===============================================
echo       WYDBR 2.0 - Script de Compilacao
echo ===============================================

REM Verificar requisitos
echo Verificando requisitos...

REM Verificar CMake
where cmake >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo [ERRO] CMake nao encontrado! Por favor, instale o CMake.
    exit /b 1
)
echo [OK] CMake encontrado

REM Verificar Visual Studio
reg query "HKEY_CLASSES_ROOT\VisualStudio.DTE" >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo [AVISO] Visual Studio pode nao estar instalado.
    echo         O script continuara, mas a compilacao pode falhar.
)

REM Criar diretório de build se não existir
echo Criando diretorio de build...
if not exist build mkdir build
cd build

REM Configurar CMake
echo Configurando CMake...
cmake .. -DCMAKE_BUILD_TYPE=Release -G "Visual Studio 17 2022" -A x64

if %ERRORLEVEL% NEQ 0 (
    echo [ERRO] Falha na configuracao do CMake!
    cd ..
    exit /b 1
)

REM Compilar
echo Compilando...
cmake --build . --config Release

if %ERRORLEVEL% NEQ 0 (
    echo [ERRO] Erro durante a compilacao!
    cd ..
    exit /b 1
) else (
    echo [SUCESSO] Compilacao concluida com sucesso!
    echo Os binarios estao disponiveis em: %cd%\bin\Release
)

REM Voltar para o diretório original
cd ..

echo Processo concluido!