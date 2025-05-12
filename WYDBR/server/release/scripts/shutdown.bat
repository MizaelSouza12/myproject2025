@echo off
setlocal enabledelayedexpansion

REM Script de encerramento do servidor WYDBR para Windows
REM Encerra todos os componentes do servidor em sequência adequada

REM Configurações
set "SERVIDOR_ROOT=C:\WYDBR\server"
set "LOG_DIR=%SERVIDOR_ROOT%\logs"
set "CONFIG_DIR=%SERVIDOR_ROOT%\config"
set "PID_DIR=%SERVIDOR_ROOT%\pids"

REM Timestamp para logs
for /f "tokens=2 delims==" %%a in ('wmic OS Get localdatetime /value') do set "dt=%%a"
set "TIMESTAMP=%dt:~0,8%_%dt:~8,6%"
set "LOG_FILE=%LOG_DIR%\shutdown_%TIMESTAMP%.log"

REM Lista de servidores na ordem reversa de encerramento
set "SERVERS=AgeSrv BISrv NRSrv WSKSrv TMSrv DBSrv"

REM Tempo de espera em segundos para encerramento normal antes de forçar
set "GRACEFUL_TIMEOUT=30"

REM Cores para console (requer ANSI)
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

if not exist "%LOG_DIR%\stats" (
    mkdir "%LOG_DIR%\stats" 2>nul
    call :log "Diretório de estatísticas criado em %LOG_DIR%\stats"
)
goto :eof

REM Função para verificar e encerrar um servidor
:encerrar_servidor
set "servidor=%~1"
set "modo=%~2"
set "pid_file=%PID_DIR%\%servidor%.pid"

REM Verifica se o arquivo PID existe
if not exist "%pid_file%" (
    call :log "%servidor% não está em execução (arquivo PID não encontrado)"
    goto :eof
)

REM Obtém o PID do arquivo
set /p PID=<"%pid_file%"

REM Verifica se o processo está rodando
tasklist /fi "pid eq %PID%" 2>nul | find "%PID%" >nul
if errorlevel 1 (
    call :log "%servidor% não está em execução (PID %PID% não encontrado)"
    del "%pid_file%" 2>nul
    goto :eof
)

call :log "Encerrando %servidor% (PID: %PID%)..."

REM Envia sinal de salvamento (simulado com arquivo)
call :log "Enviando sinal de salvamento para %servidor%..."
echo "SAVE_DATA" > "%SERVIDOR_ROOT%\signals\%servidor%_save.signal"
timeout /t 5 /nobreak >nul

if "%modo%"=="normal" (
    REM Tenta encerramento normal primeiro
    call :log "Tentando encerramento normal de %servidor%..."
    taskkill /PID %PID% /T >nul 2>&1
    
    REM Aguarda o encerramento normal
    set "contador=0"
    :loop_espera
    if %contador% geq %GRACEFUL_TIMEOUT% goto :timeout_atingido
    
    REM Verifica se o processo ainda está rodando
    tasklist /fi "pid eq %PID%" 2>nul | find "%PID%" >nul
    if errorlevel 1 (
        call :log "%GREEN%%servidor% encerrado com sucesso%RESET%"
        del "%pid_file%" 2>nul
        goto :eof
    )
    
    timeout /t 1 /nobreak >nul
    set /a contador+=1
    call :log "Aguardando encerramento normal de %servidor%... (!contador!/%GRACEFUL_TIMEOUT%)"
    goto :loop_espera
    
    :timeout_atingido
    call :log "%YELLOW%Timeout atingido. Forçando encerramento de %servidor%...%RESET%"
    taskkill /F /PID %PID% /T >nul 2>&1
) else (
    REM Encerramento forçado imediato
    call :log "Forçando encerramento de %servidor%..."
    taskkill /F /PID %PID% /T >nul 2>&1
)

REM Verificação final
timeout /t 2 /nobreak >nul
tasklist /fi "pid eq %PID%" 2>nul | find "%PID%" >nul
if errorlevel 1 (
    call :log "%GREEN%%servidor% encerrado com sucesso%RESET%"
    del "%pid_file%" 2>nul
) else (
    call :log "%RED%ERRO: Falha ao encerrar %servidor% (PID: %PID%)%RESET%"
)

goto :eof

REM Função para verificar processos órfãos
:verificar_orfaos
call :log "Verificando processos órfãos..."

for %%s in (%SERVERS%) do (
    for /f "tokens=2" %%p in ('tasklist /fi "imagename eq %%s.exe" /nh ^| findstr /i %%s') do (
        REM Verifica se este PID não está em nenhum arquivo .pid
        if not exist "%PID_DIR%\%%s.pid" (
            call :log "%YELLOW%Processo órfão encontrado: %%s (PID: %%p)%RESET%"
            call :log "Encerrando processo órfão..."
            taskkill /F /PID %%p /T >nul 2>&1
        ) else (
            set /p SAVED_PID=<"%PID_DIR%\%%s.pid"
            if not "!SAVED_PID!"=="%%p" (
                call :log "%YELLOW%Processo órfão encontrado: %%s (PID: %%p)%RESET%"
                call :log "Encerrando processo órfão..."
                taskkill /F /PID %%p /T >nul 2>&1
            )
        )
    )
)
goto :eof

REM Função para salvar estatísticas
:salvar_estatisticas
call :log "Coletando estatísticas do servidor antes do encerramento..."

set "STATS_DIR=%LOG_DIR%\stats"
if not exist "%STATS_DIR%" mkdir "%STATS_DIR%" 2>nul

REM Coleta estatísticas de uso de CPU e memória
call :log "Coletando estatísticas de uso de recursos..."
tasklist /v > "%STATS_DIR%\proc_stats_%TIMESTAMP%.txt"

REM Coleta estatísticas do sistema
call :log "Coletando estatísticas do sistema..."
systeminfo | findstr /C:"OS" /C:"Memory" > "%STATS_DIR%\system_%TIMESTAMP%.txt"
wmic logicaldisk get caption,description,freespace,size > "%STATS_DIR%\disk_%TIMESTAMP%.txt"

call :log "Estatísticas salvas em %STATS_DIR%"
goto :eof

REM Função para notificar jogadores
:notificar_jogadores
set "mensagem=%~1"
set "tempo=%~2"

call :log "Enviando notificação de encerramento para jogadores: %mensagem%"

REM Verifica se o TMSrv está rodando
if not exist "%PID_DIR%\TMSrv.pid" (
    call :log "TMSrv não está em execução. Não é possível notificar jogadores."
    goto :eof
)

REM Simulando envio de comando para o servidor
if not exist "%SERVIDOR_ROOT%\messages" mkdir "%SERVIDOR_ROOT%\messages" 2>nul
echo %mensagem% > "%SERVIDOR_ROOT%\messages\shutdown_notice.txt"

call :log "Notificação enviada. Aguardando %tempo% minutos..."
set /a tempo_segundos=tempo*60

REM Contagem regressiva
for /l %%i in (%tempo_segundos%,-1,1) do (
    set /a minutos=%%i / 60
    set /a segundos=%%i %% 60
    
    if !segundos! equ 0 (
        call :log "Encerramento em !minutos! minutos..."
    )
    
    timeout /t 1 /nobreak >nul
)

goto :eof

REM Configuração inicial
title WYDBR Server Shutdown
echo %CYAN%=============================================================%RESET%
echo %CYAN%        WYDBR - Sistema de Encerramento do Servidor         %RESET%
echo %CYAN%=============================================================%RESET%
echo.

REM Criar diretórios necessários
call :criar_diretorios

REM Inicializa log
call :log "Iniciando script de encerramento do WYDBR..."
call :log "Versão do Windows: %OS%"

REM Salva estatísticas antes do encerramento
call :salvar_estatisticas

REM Verifica parâmetros
set "MODO_ENCERRAMENTO=normal"
set "NOTIFICAR=0"
set "TEMPO_ESPERA=0"

REM Processa parâmetros da linha de comando
if "%1"=="--force" set "MODO_ENCERRAMENTO=forçado"
if "%1"=="-f" set "MODO_ENCERRAMENTO=forçado"
if "%MODO_ENCERRAMENTO%"=="forçado" call :log "Modo de encerramento forçado ativado"

if "%1"=="--notify" set "NOTIFICAR=1"
if "%1"=="-n" set "NOTIFICAR=1"
if "%2" NEQ "" set "TEMPO_ESPERA=%2"

REM Notifica jogadores se necessário
if %NOTIFICAR%==1 if %TEMPO_ESPERA% gtr 0 (
    call :notificar_jogadores "ATENÇÃO: O servidor será encerrado para manutenção." %TEMPO_ESPERA%
)

REM Encerra servidores na ordem reversa
call :log "Encerrando servidores na ordem reversa..."
for %%s in (%SERVERS%) do (
    call :encerrar_servidor "%%s" "%MODO_ENCERRAMENTO%"
    REM Aguarda um momento entre encerramentos
    timeout /t 2 /nobreak >nul
)

REM Verifica processos órfãos
call :verificar_orfaos

REM Conclusão
call :log "=== Encerramento do servidor WYDBR concluído ==="

if "%3"=="-nopause" goto :end
pause
:end
endlocal 