@echo off
setlocal enabledelayedexpansion

:: Configurações
set BUILD_DIR=build
set INSTALL_DIR=dist
set CONFIG=Release
set PLATFORM=x64

:: Verificar se o Visual Studio está instalado
where cl >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo Visual Studio não encontrado. Por favor, instale o Visual Studio 2019 ou superior.
    exit /b 1
)

:: Criar diretórios
if not exist %BUILD_DIR% mkdir %BUILD_DIR%
if not exist %INSTALL_DIR% mkdir %INSTALL_DIR%

:: Configurar ambiente do Visual Studio
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"

:: Compilar WYDBR.exe
echo Compilando WYDBR.exe...
cd %BUILD_DIR%
cmake -G "Visual Studio 16 2019" -A %PLATFORM% -DCMAKE_BUILD_TYPE=%CONFIG% -DBUILD_CLIENT=ON -DBUILD_SERVER=OFF -DBUILD_STUDIO=OFF ..
cmake --build . --config %CONFIG% --target WYDBR
if %ERRORLEVEL% neq 0 (
    echo Erro ao compilar WYDBR.exe
    exit /b 1
)
copy %CONFIG%\WYDBR.exe ..\%INSTALL_DIR%\
cd ..

:: Compilar WYDLauncher.exe
echo Compilando WYDLauncher.exe...
cd %BUILD_DIR%
cmake -G "Visual Studio 16 2019" -A %PLATFORM% -DCMAKE_BUILD_TYPE=%CONFIG% -DBUILD_CLIENT=OFF -DBUILD_SERVER=OFF -DBUILD_STUDIO=OFF -DBUILD_LAUNCHER=ON ..
cmake --build . --config %CONFIG% --target WYDLauncher
if %ERRORLEVEL% neq 0 (
    echo Erro ao compilar WYDLauncher.exe
    exit /b 1
)
copy %CONFIG%\WYDLauncher.exe ..\%INSTALL_DIR%\
cd ..

:: Compilar WYDStudio.exe
echo Compilando WYDStudio.exe...
cd %BUILD_DIR%
cmake -G "Visual Studio 16 2019" -A %PLATFORM% -DCMAKE_BUILD_TYPE=%CONFIG% -DBUILD_CLIENT=OFF -DBUILD_SERVER=ON -DBUILD_STUDIO=ON ..
cmake --build . --config %CONFIG% --target WYDStudio
if %ERRORLEVEL% neq 0 (
    echo Erro ao compilar WYDStudio.exe
    exit /b 1
)
copy %CONFIG%\WYDStudio.exe ..\%INSTALL_DIR%\
cd ..

:: Copiar arquivos necessários
echo Copiando arquivos necessários...
copy config\*.* %INSTALL_DIR%\
copy data\*.* %INSTALL_DIR%\
copy docs\*.* %INSTALL_DIR%\

:: Criar arquivo de versão
echo Criando arquivo de versão...
echo WYDBR 2.0 > %INSTALL_DIR%\version.txt
echo Build: %date% %time% >> %INSTALL_DIR%\version.txt

:: Verificar dependências
echo Verificando dependências...
where ffmpeg >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo Aviso: FFmpeg não encontrado. O sistema de VoIP pode não funcionar corretamente.
)

:: Finalizar
echo Build concluído com sucesso!
echo Os executáveis foram gerados em: %INSTALL_DIR%

endlocal
