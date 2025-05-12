@echo off
setlocal enabledelayedexpansion

REM Script de reset de eventos do servidor WYDBR para Windows
REM Reinicia eventos do jogo e limpa caches relacionados

REM Configurações
set "SERVIDOR_ROOT=C:\WYDBR\server"
set "LOG_DIR=%SERVIDOR_ROOT%\logs"
set "CONFIG_DIR=%SERVIDOR_ROOT%\config"
set "PID_DIR=%SERVIDOR_ROOT%\pids"
set "DATA_DIR=%SERVIDOR_ROOT%\data"
set "EVENT_DIR=%DATA_DIR%\events"
set "EVENT_CACHE_DIR=%DATA_DIR%\cache\events"

REM Timestamp para logs
for /f "tokens=2 delims==" %%a in ('wmic OS Get localdatetime /value') do set "dt=%%a"
set "TIMESTAMP=%dt:~0,8%_%dt:~8,6%"
set "LOG_FILE=%LOG_DIR%\reset_events_%TIMESTAMP%.log"

REM Lista de eventos para reset
set "EVENTOS=CastleWar GuildWar BossRaid KingdomBattle DuelTournament DailyQuests WeeklyEvents SeasonalEvents RankingSystem RewardSystem"

REM Configurações padrão
set "RESET_ALL=false"
set "FORCE_RESET=false"
set "NOTIFY_PLAYERS=true"
set "BACKUP_EVENTS=true"
set "SHUTDOWN_REQUIRED=false"
set "NOTIFICATION_TIME=5"
set "VERBOSE=false"
set "SPECIFIED_EVENTS="

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
set "mensagem=%~1"
set "nivel=%~2"
if "%nivel%"=="" set "nivel=INFO"
echo [%date% %time%] [%nivel%] %mensagem% >> "%LOG_FILE%"
echo [%nivel%] %mensagem%
goto :eof

REM Função para verificar se o diretório de log existe
:verificar_log_dir
if not exist "%LOG_DIR%" (
    mkdir "%LOG_DIR%" 2>nul
    call :log "Diretório de logs criado em %LOG_DIR%"
)
goto :eof

REM Função para verificar se os servidores estão rodando
:verificar_servidores
call :log "Verificando servidores ativos..."
set "servidores_ativos="

REM Procurar por processos ativos
for %%s in (DBSrv TMSrv WSKSrv NRSrv BISrv AgeSrv) do (
    tasklist /fi "imagename eq %%s.exe" 2>nul | find "%%s.exe" >nul
    if not errorlevel 1 (
        set "servidores_ativos=!servidores_ativos! %%s"
        if "%VERBOSE%"=="true" call :log "Servidor ativo: %%s" "DEBUG"
    )
)

call :log "Servidores ativos:%servidores_ativos%"

REM Verifica se o TMSrv (servidor de jogo) está rodando
echo %servidores_ativos% | findstr /C:"TMSrv" >nul
if not errorlevel 1 (
    if "%FORCE_RESET%"=="true" (
        call :log "TMSrv está rodando, mas o reset forçado foi solicitado"
        set "SHUTDOWN_REQUIRED=true"
    ) else (
        call :log "TMSrv está rodando. Para resetar eventos, é necessário parar o servidor ou usar a opção --force" "ERROR"
        exit /b 1
    )
) else (
    call :log "TMSrv não está rodando, podemos prosseguir com o reset de eventos"
)
goto :eof

REM Função para notificar jogadores sobre o reset de eventos
:notificar_jogadores
if "%NOTIFY_PLAYERS%"=="false" goto :eof
if "%SHUTDOWN_REQUIRED%"=="false" goto :eof

call :log "Enviando notificação aos jogadores sobre o reset de eventos..."

REM Verifica se o TMSrv está rodando
tasklist /fi "imagename eq TMSrv.exe" 2>nul | find "TMSrv.exe" >nul
if errorlevel 1 (
    call :log "TMSrv não está em execução. Não é possível notificar jogadores." "WARN"
    goto :eof
)

REM Envia comando para o servidor notificar os jogadores
call :log "Enviando notificação: O servidor será reiniciado em %NOTIFICATION_TIME% minutos para reset de eventos"

REM Cria um arquivo de sinal que o TMSrv deve monitorar
if not exist "%SERVIDOR_ROOT%\signals" mkdir "%SERVIDOR_ROOT%\signals" 2>nul
echo SYSTEM_MSG^|O servidor será reiniciado em %NOTIFICATION_TIME% minutos para reset de eventos. Por favor, salve seu progresso e desconecte. > "%SERVIDOR_ROOT%\signals\notify_all.signal"

REM Usa um método alternativo para notificar o servidor (já que não podemos enviar sinais diretamente no Windows)
REM Geralmente há um arquivo de controle ou socket que o servidor monitora
if exist "%SERVIDOR_ROOT%\bin\TMSrv_control.exe" (
    "%SERVIDOR_ROOT%\bin\TMSrv_control.exe" --notify-all "O servidor será reiniciado em %NOTIFICATION_TIME% minutos para reset de eventos."
)

REM Aguarda o tempo especificado
call :log "Aguardando %NOTIFICATION_TIME% minutos antes de prosseguir..."
for /l %%i in (%NOTIFICATION_TIME%,-1,1) do (
    if %%i LEQ 5 (
        if %%i EQU 1 (
            echo SYSTEM_MSG^|ATENÇÃO: O servidor será reiniciado em 1 minuto para reset de eventos! > "%SERVIDOR_ROOT%\signals\notify_all.signal"
        ) else (
            echo SYSTEM_MSG^|ATENÇÃO: O servidor será reiniciado em %%i minutos para reset de eventos! > "%SERVIDOR_ROOT%\signals\notify_all.signal"
        )
        if exist "%SERVIDOR_ROOT%\bin\TMSrv_control.exe" (
            "%SERVIDOR_ROOT%\bin\TMSrv_control.exe" --notify-all "O servidor será reiniciado em %%i minutos para reset de eventos."
        )
    )
    timeout /t 60 /nobreak >nul
)

REM Notificação final
echo SYSTEM_MSG^|O servidor está sendo reiniciado agora para reset de eventos. Reconecte em alguns minutos. > "%SERVIDOR_ROOT%\signals\notify_all.signal"
if exist "%SERVIDOR_ROOT%\bin\TMSrv_control.exe" (
    "%SERVIDOR_ROOT%\bin\TMSrv_control.exe" --notify-all "O servidor está sendo reiniciado agora para reset de eventos."
)
timeout /t 5 /nobreak >nul

goto :eof

REM Função para parar o servidor de jogo
:parar_servidor
if "%SHUTDOWN_REQUIRED%"=="false" goto :eof

call :log "Parando o servidor de jogo para reset de eventos..."

REM Usa o script de shutdown se existir
if exist "%SERVIDOR_ROOT%\scripts\shutdown.bat" (
    call :log "Executando script de shutdown..."
    call "%SERVIDOR_ROOT%\scripts\shutdown.bat" --force
    timeout /t 5 /nobreak >nul
) else (
    call :log "Script de shutdown não encontrado. Tentando parar TMSrv manualmente..." "WARN"
    
    REM Tenta parar o TMSrv manualmente
    taskkill /im TMSrv.exe /f >nul 2>&1
    timeout /t 3 /nobreak >nul
)

REM Verifica se o TMSrv realmente parou
tasklist /fi "imagename eq TMSrv.exe" 2>nul | find "TMSrv.exe" >nul
if not errorlevel 1 (
    call :log "ERRO: Não foi possível parar o TMSrv. Abortando reset de eventos." "ERROR"
    exit /b 1
)

call :log "Servidor de jogo parado com sucesso"
goto :eof

REM Função para fazer backup dos dados de eventos
:backup_eventos
if "%BACKUP_EVENTS%"=="false" (
    call :log "Backup de eventos desativado, pulando..."
    goto :eof
)

call :log "Criando backup dos dados de eventos..."

REM Cria diretório de backup se não existir
set "backup_dir=%SERVIDOR_ROOT%\backups\events\%TIMESTAMP%"
if not exist "%backup_dir%" mkdir "%backup_dir%" 2>nul

REM Copia todos os arquivos de eventos para o backup
if exist "%EVENT_DIR%" (
    xcopy "%EVENT_DIR%\*.*" "%backup_dir%\" /E /I /H /Y >nul
    call :log "Backup dos dados de eventos criado em %backup_dir%"
) else (
    call :log "Diretório de eventos não encontrado: %EVENT_DIR%" "WARN"
)

REM Backup do cache de eventos
if exist "%EVENT_CACHE_DIR%" (
    mkdir "%backup_dir%\cache" 2>nul
    xcopy "%EVENT_CACHE_DIR%\*.*" "%backup_dir%\cache\" /E /I /H /Y >nul
    call :log "Backup do cache de eventos criado em %backup_dir%\cache"
)

REM Backup das configurações de eventos
if exist "%CONFIG_DIR%\events" (
    mkdir "%backup_dir%\config" 2>nul
    xcopy "%CONFIG_DIR%\events\*.*" "%backup_dir%\config\" /E /I /H /Y >nul
    call :log "Backup das configurações de eventos criado em %backup_dir%\config"
)

goto :eof

REM Função para resetar um evento específico
:resetar_evento
set "evento=%~1"
call :log "Resetando evento: %evento%"

REM Verifica se o diretório do evento existe
set "evento_dir=%EVENT_DIR%\%evento%"
if not exist "%evento_dir%" (
    if not exist "%EVENT_DIR%\%evento%.dat" (
        call :log "Diretório ou arquivo de evento não encontrado para %evento%" "WARN"
        goto :eof
    )
)

REM Reseta o evento baseado no tipo
if "%evento%"=="CastleWar" (
    REM Reseta dados de guerra de castelo
    if exist "%EVENT_DIR%\CastleWar.dat" (
        call :log "Resetando dados de Guerra de Castelo..."
        copy "%DATA_DIR%\templates\CastleWar.template" "%EVENT_DIR%\CastleWar.dat" /Y >nul
    )
    REM Limpa cache de guerra de castelo
    del "%EVENT_CACHE_DIR%\castle_war_*" 2>nul
) else if "%evento%"=="GuildWar" (
    REM Reseta dados de guerra de guild
    if exist "%EVENT_DIR%\GuildWar" (
        call :log "Resetando dados de Guerra de Guild..."
        del "%EVENT_DIR%\GuildWar\scores.dat" 2>nul
        del "%EVENT_DIR%\GuildWar\matches.dat" 2>nul
        copy "%DATA_DIR%\templates\GuildWar\scores.template" "%EVENT_DIR%\GuildWar\scores.dat" /Y >nul 2>nul
        copy "%DATA_DIR%\templates\GuildWar\matches.template" "%EVENT_DIR%\GuildWar\matches.dat" /Y >nul 2>nul
    )
    REM Limpa cache de guerra de guild
    del "%EVENT_CACHE_DIR%\guild_war_*" 2>nul
) else if "%evento%"=="BossRaid" (
    REM Reseta dados de raid de boss
    if exist "%EVENT_DIR%\BossRaid" (
        call :log "Resetando dados de Raid de Boss..."
        del "%EVENT_DIR%\BossRaid\boss_status.dat" 2>nul
        del "%EVENT_DIR%\BossRaid\last_spawn.dat" 2>nul
        copy "%DATA_DIR%\templates\BossRaid\boss_status.template" "%EVENT_DIR%\BossRaid\boss_status.dat" /Y >nul 2>nul
    )
    REM Limpa cache de raid de boss
    del "%EVENT_CACHE_DIR%\boss_raid_*" 2>nul
) else if "%evento%"=="KingdomBattle" (
    REM Reseta dados de batalha de reinos
    if exist "%EVENT_DIR%\KingdomBattle" (
        call :log "Resetando dados de Batalha de Reinos..."
        del "%EVENT_DIR%\KingdomBattle\kingdom_status.dat" 2>nul
        del "%EVENT_DIR%\KingdomBattle\scores.dat" 2>nul
        copy "%DATA_DIR%\templates\KingdomBattle\kingdom_status.template" "%EVENT_DIR%\KingdomBattle\kingdom_status.dat" /Y >nul 2>nul
        copy "%DATA_DIR%\templates\KingdomBattle\scores.template" "%EVENT_DIR%\KingdomBattle\scores.dat" /Y >nul 2>nul
    )
    REM Limpa cache de batalha de reinos
    del "%EVENT_CACHE_DIR%\kingdom_*" 2>nul
) else if "%evento%"=="DuelTournament" (
    REM Reseta dados de torneio de duelos
    if exist "%EVENT_DIR%\DuelTournament" (
        call :log "Resetando dados de Torneio de Duelos..."
        del "%EVENT_DIR%\DuelTournament\brackets.dat" 2>nul
        del "%EVENT_DIR%\DuelTournament\participants.dat" 2>nul
        copy "%DATA_DIR%\templates\DuelTournament\brackets.template" "%EVENT_DIR%\DuelTournament\brackets.dat" /Y >nul 2>nul
    )
    REM Limpa cache de torneio de duelos
    del "%EVENT_CACHE_DIR%\duel_*" 2>nul
) else if "%evento%"=="DailyQuests" (
    REM Reseta missões diárias
    if exist "%EVENT_DIR%\DailyQuests" (
        call :log "Resetando Missões Diárias..."
        del "%EVENT_DIR%\DailyQuests\daily_pool.dat" 2>nul
        copy "%DATA_DIR%\templates\DailyQuests\daily_pool.template" "%EVENT_DIR%\DailyQuests\daily_pool.dat" /Y >nul 2>nul
    )
    REM Limpa cache de missões diárias
    del "%EVENT_CACHE_DIR%\daily_quest_*" 2>nul
) else if "%evento%"=="WeeklyEvents" (
    REM Reseta eventos semanais
    if exist "%EVENT_DIR%\WeeklyEvents" (
        call :log "Resetando Eventos Semanais..."
        del "%EVENT_DIR%\WeeklyEvents\weekly_status.dat" 2>nul
        copy "%DATA_DIR%\templates\WeeklyEvents\weekly_status.template" "%EVENT_DIR%\WeeklyEvents\weekly_status.dat" /Y >nul 2>nul
    )
    REM Limpa cache de eventos semanais
    del "%EVENT_CACHE_DIR%\weekly_*" 2>nul
) else if "%evento%"=="SeasonalEvents" (
    REM Reseta eventos sazonais
    if exist "%EVENT_DIR%\SeasonalEvents" (
        call :log "Resetando Eventos Sazonais..."
        del "%EVENT_DIR%\SeasonalEvents\seasonal_status.dat" 2>nul
        copy "%DATA_DIR%\templates\SeasonalEvents\seasonal_status.template" "%EVENT_DIR%\SeasonalEvents\seasonal_status.dat" /Y >nul 2>nul
    )
    REM Limpa cache de eventos sazonais
    del "%EVENT_CACHE_DIR%\seasonal_*" 2>nul
) else if "%evento%"=="RankingSystem" (
    REM Reseta sistema de ranking
    if exist "%EVENT_DIR%\RankingSystem" (
        call :log "Resetando Sistema de Ranking..."
        REM Faz backup dos rankings antigos com timestamp
        if exist "%EVENT_DIR%\RankingSystem\rankings.dat" (
            copy "%EVENT_DIR%\RankingSystem\rankings.dat" "%EVENT_DIR%\RankingSystem\rankings_%TIMESTAMP%.bak" /Y >nul 2>nul
        )
        REM Reseta apenas rankings específicos, mantendo histórico
        copy "%DATA_DIR%\templates\RankingSystem\rankings.template" "%EVENT_DIR%\RankingSystem\rankings.dat" /Y >nul 2>nul
    )
    REM Limpa cache de rankings
    del "%EVENT_CACHE_DIR%\rank_*" 2>nul
) else if "%evento%"=="RewardSystem" (
    REM Reseta sistema de recompensas
    if exist "%EVENT_DIR%\RewardSystem" (
        call :log "Resetando Sistema de Recompensas..."
        del "%EVENT_DIR%\RewardSystem\pending_rewards.dat" 2>nul
        copy "%DATA_DIR%\templates\RewardSystem\pending_rewards.template" "%EVENT_DIR%\RewardSystem\pending_rewards.dat" /Y >nul 2>nul
    )
    REM Limpa cache de recompensas
    del "%EVENT_CACHE_DIR%\reward_*" 2>nul
) else (
    REM Evento desconhecido
    call :log "Evento desconhecido: %evento%, pulando..." "WARN"
    goto :eof
)

call :log "Evento %evento% resetado com sucesso"
goto :eof

REM Função para limpar caches de eventos
:limpar_caches
call :log "Limpando caches de eventos..."

REM Cria diretório de cache se não existir
if not exist "%EVENT_CACHE_DIR%" (
    mkdir "%EVENT_CACHE_DIR%" 2>nul
    call :log "Diretório de cache de eventos criado: %EVENT_CACHE_DIR%"
    goto :eof
)

REM Remove todos os arquivos de cache
del "%EVENT_CACHE_DIR%\*.*" /Q 2>nul

call :log "Caches de eventos limpos com sucesso"
goto :eof

REM Função para iniciar o servidor após o reset
:iniciar_servidor
if "%SHUTDOWN_REQUIRED%"=="false" goto :eof

call :log "Iniciando o servidor após reset de eventos..."

REM Usa o script de startup se existir
if exist "%SERVIDOR_ROOT%\scripts\startup.bat" (
    call :log "Executando script de startup..."
    call "%SERVIDOR_ROOT%\scripts\startup.bat"
    timeout /t 5 /nobreak >nul
) else (
    call :log "Script de startup não encontrado. Não é possível iniciar o servidor automaticamente." "WARN"
    goto :eof
)

REM Verifica se o TMSrv iniciou
tasklist /fi "imagename eq TMSrv.exe" 2>nul | find "TMSrv.exe" >nul
if not errorlevel 1 (
    call :log "Servidor de jogo iniciado com sucesso"
) else (
    call :log "AVISO: Não foi possível confirmar se o servidor de jogo iniciou corretamente" "WARN"
)

goto :eof

REM Função para exibir ajuda
:exibir_ajuda
echo Uso: %0 [opções] [eventos]
echo Reseta eventos do servidor WYDBR e limpa caches relacionados.
echo.
echo Opções:
echo   --all                   Reseta todos os eventos disponíveis
echo   --force                 Força o reset mesmo se o servidor estiver rodando
echo   --no-notify             Não notifica os jogadores antes de resetar eventos
echo   --no-backup             Não cria backup dos dados de eventos antes do reset
echo   --notification-time N   Define o tempo de notificação em minutos (padrão: 5)
echo   --verbose               Modo verboso com mais detalhes
echo   --help                  Exibe esta ajuda
echo.
echo Eventos disponíveis:
for %%e in (%EVENTOS%) do (
    echo   %%e
)
echo.
echo Exemplos:
echo   %0 --all                           # Reseta todos os eventos
echo   %0 CastleWar GuildWar              # Reseta apenas os eventos especificados
echo   %0 --force --no-notify BossRaid    # Força reset do evento BossRaid sem notificar
echo.
goto :eof

REM Início do script
title WYDBR Event Reset
echo %CYAN%=============================================================%RESET%
echo %CYAN%          WYDBR - Sistema de Reset de Eventos             %RESET%
echo %CYAN%=============================================================%RESET%
echo.

REM Verificar diretório de log
call :verificar_log_dir

REM Inicializa log
call :log "=== Iniciando script de reset de eventos do WYDBR ==="
call :log "Versão do Windows: %OS%"

REM Processa parâmetros da linha de comando
:parse_args
if "%1"=="" goto end_parse_args

if "%1"=="--all" (
    set "RESET_ALL=true"
    shift
    goto parse_args
) else if "%1"=="--force" (
    set "FORCE_RESET=true"
    shift
    goto parse_args
) else if "%1"=="--no-notify" (
    set "NOTIFY_PLAYERS=false"
    shift
    goto parse_args
) else if "%1"=="--no-backup" (
    set "BACKUP_EVENTS=false"
    shift
    goto parse_args
) else if "%1"=="--notification-time" (
    if "%2" NEQ "" set "NOTIFICATION_TIME=%2"
    shift & shift
    goto parse_args
) else if "%1"=="--verbose" (
    set "VERBOSE=true"
    shift
    goto parse_args
) else if "%1"=="--help" (
    call :exibir_ajuda
    exit /b 0
) else if "%1:~0,1%"=="-" (
    call :log "AVISO: Opção desconhecida: %1" "WARN"
    shift
    goto parse_args
) else (
    REM Verifica se o evento especificado existe na lista
    set "evento_valido=false"
    for %%e in (%EVENTOS%) do (
        if "%%e"=="%1" (
            set "evento_valido=true"
            set "SPECIFIED_EVENTS=!SPECIFIED_EVENTS! %1"
        )
    )
    
    if "%evento_valido%"=="false" (
        call :log "AVISO: Evento desconhecido: %1" "WARN"
    )
    
    shift
    goto parse_args
)

:end_parse_args

REM Verifica se pelo menos um evento foi especificado ou --all foi usado
if "%RESET_ALL%"=="false" if "%SPECIFIED_EVENTS%"=="" (
    call :log "Nenhum evento especificado para reset. Use --all ou especifique eventos." "ERROR"
    call :exibir_ajuda
    exit /b 1
)

REM Verifica se os servidores estão rodando
call :verificar_servidores

REM Notifica jogadores se necessário
if "%SHUTDOWN_REQUIRED%"=="true" if "%NOTIFY_PLAYERS%"=="true" (
    call :notificar_jogadores
)

REM Para o servidor se necessário
if "%SHUTDOWN_REQUIRED%"=="true" (
    call :parar_servidor
)

REM Faz backup dos dados de eventos
if "%BACKUP_EVENTS%"=="true" (
    call :backup_eventos
)

REM Reseta eventos
if "%RESET_ALL%"=="true" (
    call :log "Resetando todos os eventos..."
    for %%e in (%EVENTOS%) do (
        call :resetar_evento "%%e"
    )
) else (
    call :log "Resetando eventos especificados..."
    for %%e in (%SPECIFIED_EVENTS%) do (
        call :resetar_evento "%%e"
    )
)

REM Limpa caches
call :limpar_caches

REM Inicia o servidor se foi parado
if "%SHUTDOWN_REQUIRED%"=="true" (
    call :iniciar_servidor
)

call :log "=== Reset de eventos concluído ==="

endlocal 