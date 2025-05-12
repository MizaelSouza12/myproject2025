@echo off
setlocal enabledelayedexpansion

REM Script de inicialização do servidor WYDBR para Windows
REM Inicia todos os componentes do servidor em sequência com verificações de integridade

REM Configurações
set "SERVIDOR_ROOT=C:\WYDBR\server"
set "LOG_DIR=%SERVIDOR_ROOT%\logs"
set "CONFIG_DIR=%SERVIDOR_ROOT%\config"
set "PID_DIR=%SERVIDOR_ROOT%\pids"

REM Timestamp para logs
for /f "tokens=2 delims==" %%a in ('wmic OS Get localdatetime /value') do set "dt=%%a"
set "TIMESTAMP=%dt:~0,8%_%dt:~8,6%"
set "LOG_FILE=%LOG_DIR%\startup_%TIMESTAMP%.log"

REM Lista de servidores
set "SERVERS=DBSrv TMSrv WSKSrv NRSrv BISrv AgeSrv"
set "MAX_TENTATIVAS=3"

REM Cores para console (requere ANSI)
set "RED=[91m"
set "GREEN=[92m"
set "YELLOW=[93m"
set "BLUE=[94m"
set "MAGENTA=[95m"
set "CYAN=[96m"
set "WHITE=[97m"
set "RESET=[0m"

REM Função para logar mensagens
:log
echo [%date% %time%] %~1 >> "%LOG_FILE%"
echo %~1
goto :eof

REM Função para criar diretórios necessários
:criar_diretorios
if not exist "%LOG_DIR%" (
    mkdir "%LOG_DIR%" 2>nul
    call :log "Diretório de logs criado em %LOG_DIR%"
)

if not exist "%PID_DIR%" (
    mkdir "%PID_DIR%" 2>nul
    call :log "Diretório de PIDs criado em %PID_DIR%"
) else (
    del /q "%PID_DIR%\*.pid" 2>nul
    call :log "Arquivos de PID antigos removidos"
)
goto :eof

REM Função para verificar sistema
:verificar_sistema
call :log "Verificando sistema..."

REM Verificar espaço em disco
for /f "tokens=3" %%a in ('dir /s /-c C:\ ^| findstr "bytes free"') do set "ESPACO_LIVRE=%%a"
call :log "Espaço livre em disco: !ESPACO_LIVRE! bytes"

REM Verificar permissões de diretórios
for %%d in (bin config data logs) do (
    if not exist "%SERVIDOR_ROOT%\%%d" (
        mkdir "%SERVIDOR_ROOT%\%%d" 2>nul
        call :log "Diretório %SERVIDOR_ROOT%\%%d criado"
    )
)

REM Verificar conexão de rede
ping -n 1 127.0.0.1 >nul
if %errorlevel% neq 0 (
    call :log "%RED%ALERTA: Problemas com a interface de rede local%RESET%"
)

goto :eof

REM Função para verificar se um processo está rodando
:verificar_processo
set "nome_processo=%~1"
set "encontrado=0"

for /f "tokens=1" %%p in ('tasklist /fi "imagename eq %nome_processo%.exe" /nh ^| findstr /i %nome_processo%') do (
    set "encontrado=1"
)

exit /b !encontrado!

REM Função para verificar instâncias existentes
:verificar_instancias
call :log "Verificando instâncias existentes..."

for %%s in (%SERVERS%) do (
    call :verificar_processo "%%s"
    if !errorlevel! equ 1 (
        call :log "%YELLOW%AVISO: Instância de %%s já está em execução%RESET%"
        choice /c SN /m "Deseja encerrar a instância existente e iniciar uma nova? (S/N)"
        if !errorlevel! equ 1 (
            call :log "Encerrando instância existente de %%s"
            taskkill /f /im "%%s.exe" >nul 2>&1
            timeout /t 2 /nobreak >nul
        ) else (
            call :log "Mantendo instância existente de %%s"
        )
    )
)
goto :eof

REM Função para iniciar um servidor
:iniciar_servidor
set "servidor=%~1"
set "tentativa=1"

call :log "Iniciando %servidor%..."

REM Verifica se o binário existe
if not exist "%SERVIDOR_ROOT%\bin\%servidor%.exe" (
    call :log "%RED%ERRO: Binário de %servidor% não encontrado!%RESET%"
    exit /b 1
)

REM Verifica configuração
if not exist "%CONFIG_DIR%\%servidor%.conf" (
    call :log "%YELLOW%AVISO: Arquivo de configuração %servidor%.conf não encontrado. Usando configuração padrão.%RESET%"
) else (
    call :log "Usando configuração: %CONFIG_DIR%\%servidor%.conf"
)

:loop_tentativas
if %tentativa% gtr %MAX_TENTATIVAS% (
    call :log "%RED%ERRO: Falha ao iniciar %servidor% após %MAX_TENTATIVAS% tentativas%RESET%"
    exit /b 1
)

call :log "Tentativa %tentativa% de %MAX_TENTATIVAS% para iniciar %servidor%"

REM Inicia o servidor em segundo plano
cd /d "%SERVIDOR_ROOT%\bin"
start "" /b "%servidor%" "%servidor%.exe" --config="%CONFIG_DIR%\%servidor%.conf" > "%LOG_DIR%\%servidor%_%TIMESTAMP%.log" 2>&1

REM Obtém o PID do processo
for /f "tokens=2" %%p in ('tasklist /fi "imagename eq %servidor%.exe" /nh ^| findstr /i %servidor%') do (
    set "PID=%%p"
)

REM Aguarda para verificar se o servidor permanece em execução
timeout /t 3 /nobreak >nul
call :verificar_processo "%servidor%"
if !errorlevel! equ 1 (
    call :log "%GREEN%%servidor% iniciado com sucesso. PID: !PID!%RESET%"
    echo !PID! > "%PID_DIR%\%servidor%.pid"
    exit /b 0
) else (
    call :log "%RED%Falha ao iniciar %servidor% na tentativa %tentativa%%RESET%"
    set /a tentativa+=1
    timeout /t 2 /nobreak >nul
    goto :loop_tentativas
)
goto :eof

REM Configuração inicial
title WYDBR Server Startup
echo %CYAN%=============================================================%RESET%
echo %CYAN%          WYDBR - Sistema de Inicialização do Servidor       %RESET%
echo %CYAN%=============================================================%RESET%
echo.

REM Inicializa log
call :log "Iniciando script de startup do WYDBR..."
call :log "Versão do Windows: %OS%"

REM Cria diretórios necessários
call :criar_diretorios

REM Verifica o sistema
call :verificar_sistema

REM Verifica instâncias existentes
call :verificar_instancias

REM Inicializa variável de contagem de falhas
set "falhas=0"

REM Inicia o servidor de banco de dados primeiro
call :log "%CYAN%Iniciando servidor de banco de dados...%RESET%"
call :iniciar_servidor "DBSrv"
if %errorlevel% neq 0 (
    call :log "%RED%ERRO CRÍTICO: Falha ao iniciar o servidor de banco de dados. Abortando inicialização!%RESET%"
    pause
    exit /b 1
)

REM Aguarda o banco de dados estabilizar
call :log "Aguardando estabilização do DBSrv..."
timeout /t 5 /nobreak >nul

REM Inicia os demais servidores
for %%s in (TMSrv WSKSrv NRSrv BISrv AgeSrv) do (
    call :iniciar_servidor "%%s"
    if !errorlevel! neq 0 (
        set /a falhas+=1
        call :log "%YELLOW%AVISO: Falha ao iniciar %%s%RESET%"
    ) else (
        call :log "%GREEN%%%s iniciado com sucesso%RESET%"
        REM Pausa entre inicializações para evitar sobrecarga
        timeout /t 3 /nobreak >nul
    )
)

REM Verifica status final
if !falhas! equ 0 (
    call :log "%GREEN%Todos os servidores foram iniciados com sucesso%RESET%"
    exit /b 0
) else (
    call :log "%YELLOW%Inicialização concluída com !falhas! falhas. Verifique o log para detalhes: %LOG_FILE%%RESET%"
    exit /b 1
)

endlocal 