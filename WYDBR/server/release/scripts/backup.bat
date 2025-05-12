@echo off
setlocal enabledelayedexpansion

REM Script de backup do servidor WYDBR para Windows
REM Realiza backups incrementais e completos com rotação e compressão inteligente

REM Configurações
set "SERVIDOR_ROOT=C:\WYDBR\server"
set "LOG_DIR=%SERVIDOR_ROOT%\logs"
set "CONFIG_DIR=%SERVIDOR_ROOT%\config"
set "TIMESTAMP=%date:~6,4%%date:~3,2%%date:~0,2%_%time:~0,2%%time:~3,2%%time:~6,2%"
set "TIMESTAMP=%TIMESTAMP: =0%"
set "LOG_FILE=%LOG_DIR%\backup_%TIMESTAMP%.log"

REM Diretórios de backup
set "BACKUP_ROOT=C:\WYDBR\backups"
set "DB_BACKUP_DIR=%BACKUP_ROOT%\database"
set "FILES_BACKUP_DIR=%BACKUP_ROOT%\files"
set "SCRIPTS_BACKUP_DIR=%BACKUP_ROOT%\scripts"
set "CONFIG_BACKUP_DIR=%BACKUP_ROOT%\configs"

REM Configurações de retenção
set "DAILY_RETENTION=7"
set "WEEKLY_RETENTION=4"
set "MONTHLY_RETENTION=6"

REM Configurações de compressão
set "COMPRESSION_TOOL=7z"
set "COMPRESSION_LEVEL=9"

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
call :log "Verificando e criando diretórios de backup..."

REM Cria diretórios principais se não existirem
for %%d in ("%LOG_DIR%" "%BACKUP_ROOT%" "%DB_BACKUP_DIR%" "%FILES_BACKUP_DIR%" "%SCRIPTS_BACKUP_DIR%" "%CONFIG_BACKUP_DIR%") do (
    if not exist %%d (
        mkdir %%d 2>nul
        call :log "Diretório criado: %%d"
    )
)

REM Cria diretórios para diferentes tipos de backup
for %%t in (diario semanal mensal) do (
    if not exist "%DB_BACKUP_DIR%\%%t" (
        mkdir "%DB_BACKUP_DIR%\%%t" 2>nul
        call :log "Diretório criado: %DB_BACKUP_DIR%\%%t"
    )
    if not exist "%FILES_BACKUP_DIR%\%%t" (
        mkdir "%FILES_BACKUP_DIR%\%%t" 2>nul
        call :log "Diretório criado: %FILES_BACKUP_DIR%\%%t"
    )
)

goto :eof

REM Função para verificar ferramentas necessárias
:verificar_ferramentas
call :log "Verificando ferramentas necessárias para backup..."

REM Verifica se o 7-Zip está instalado
where /q 7z
if errorlevel 1 (
    call :log "%YELLOW%AVISO: 7-Zip não encontrado. Usando compressão nativa do Windows%RESET%"
    set "COMPRESSION_TOOL=zip"
)

REM Verifica se o MySQL está instalado
where /q mysql
if errorlevel 1 (
    call :log "%YELLOW%AVISO: MySQL não encontrado. Verificando outras opções de banco de dados%RESET%"
    where /q psql
    if errorlevel 1 (
        call :log "%YELLOW%AVISO: PostgreSQL também não encontrado. Backup de banco de dados pode falhar%RESET%"
    )
)

goto :eof

REM Função para determinar o tipo de backup (diário, semanal ou mensal)
:determinar_tipo_backup
set "dia_semana=%date:~0,3%"
set "dia_mes=%date:~0,2%"

if "%dia_semana%"=="dom" (
    REM Domingo = backup semanal
    set "tipo_backup=semanal"
) else if "%dia_mes%"=="01" (
    REM Primeiro dia do mês = backup mensal
    set "tipo_backup=mensal"
) else (
    REM Outros dias = backup diário
    set "tipo_backup=diario"
)

call :log "Tipo de backup determinado: %tipo_backup%"
goto :eof

REM Função para backup do banco de dados
:backup_database
set "tipo=%~1"
set "backup_dir=%DB_BACKUP_DIR%\%tipo%"
set "backup_file=%backup_dir%\db_backup_%TIMESTAMP%.sql"

call :log "Iniciando backup da base de dados (tipo: %tipo%)..."

REM Tenta MySQL primeiro
mysql --version >nul 2>&1
if not errorlevel 1 (
    call :log "Usando MySQL para backup"
    set "MYSQL_PWD=wyd_db_password"
    mysqldump -h localhost -u wydbr --all-databases > "%backup_file%"
) else (
    REM Tenta PostgreSQL
    psql --version >nul 2>&1
    if not errorlevel 1 (
        call :log "Usando PostgreSQL para backup"
        set "PGPASSWORD=wyd_db_password"
        pg_dump -h localhost -U wydbr -d wydbr > "%backup_file%"
    ) else (
        call :log "%RED%ERRO: Nenhum sistema de banco de dados compatível encontrado%RESET%"
        goto :database_backup_error
    )
)

if not exist "%backup_file%" (
    call :log "%RED%ERRO: Falha ao criar arquivo de backup do banco de dados%RESET%"
    goto :database_backup_error
)

REM Comprime o arquivo de backup
if "%COMPRESSION_TOOL%"=="7z" (
    call :log "Comprimindo backup do banco de dados com 7z..."
    7z a -tzip -mx=%COMPRESSION_LEVEL% "%backup_file%.zip" "%backup_file%" >nul 2>&1
    if exist "%backup_file%.zip" del "%backup_file%"
    set "backup_file=%backup_file%.zip"
) else (
    call :log "Comprimindo backup do banco de dados com compressão nativa..."
    powershell -Command "Compress-Archive -Path '%backup_file%' -DestinationPath '%backup_file%.zip' -Force"
    if exist "%backup_file%.zip" del "%backup_file%"
    set "backup_file=%backup_file%.zip"
)

for %%F in ("%backup_file%") do set "file_size=%%~zF"
set "file_size_mb=%file_size:~0,-6%.%file_size:~-6,2% MB"
if %file_size% LSS 1000000 set "file_size_mb=%file_size% bytes"

call :log "Backup de banco de dados concluído: %backup_file% (%file_size_mb%)"
exit /b 0

:database_backup_error
call :log "%RED%ERRO: Backup de banco de dados falhou%RESET%"
exit /b 1

REM Função para backup de arquivos
:backup_files
set "tipo=%~1"
set "backup_dir=%FILES_BACKUP_DIR%\%tipo%"
set "backup_file=%backup_dir%\files_backup_%TIMESTAMP%.zip"

call :log "Iniciando backup de arquivos (tipo: %tipo%)..."

REM Define diretórios a serem incluídos e excluídos
set "INCLUDE_DIRS=%SERVIDOR_ROOT%\data %SERVIDOR_ROOT%\config %SERVIDOR_ROOT%\scripts"
set "EXCLUDE_PATTERNS=*.tmp *.log *.bak tmp\* cache\*"

REM Cria arquivo com padrões de exclusão
set "exclude_file=%TEMP%\exclude_patterns.txt"
echo. > "%exclude_file%"
for %%p in (%EXCLUDE_PATTERNS%) do (
    echo %%p >> "%exclude_file%"
)

REM Realiza backup dependendo do tipo
if "%tipo%"=="diario" (
    REM Se for backup incremental e existir um backup mensal para referência
    if exist "%FILES_BACKUP_DIR%\mensal\*.zip" (
        for /f "delims=" %%f in ('dir /b /o-d "%FILES_BACKUP_DIR%\mensal\*.zip" 2^>nul') do (
            set "last_full=%%f"
            goto :got_last_full
        )
        :got_last_full
        
        if defined last_full (
            call :log "Usando backup completo como referência: %last_full%"
            
            REM Cria lista dos arquivos no último backup completo
            set "ref_list=%TEMP%\ref_list.txt"
            7z l "%FILES_BACKUP_DIR%\mensal\%last_full%" > "%ref_list%"
            
            REM Realiza backup incremental (apenas arquivos modificados após o último completo)
            if "%COMPRESSION_TOOL%"=="7z" (
                call :log "Criando backup incremental com 7z..."
                7z u -tzip -mx=%COMPRESSION_LEVEL% "%backup_file%" %INCLUDE_DIRS% -xr@"%exclude_file%" >nul 2>&1
            ) else (
                call :log "Criando backup incremental com PowerShell..."
                powershell -Command "$files = Get-ChildItem -Path %INCLUDE_DIRS% -Recurse | Where-Object {$_.LastWriteTime -gt (Get-Date).AddDays(-30)}; Compress-Archive -Path $files.FullName -DestinationPath '%backup_file%' -Force"
            )
        ) else (
            goto :full_backup
        )
    ) else (
        :full_backup
        call :log "Nenhum backup completo encontrado. Criando backup completo..."
        if "%COMPRESSION_TOOL%"=="7z" (
            7z a -tzip -mx=%COMPRESSION_LEVEL% "%backup_file%" %INCLUDE_DIRS% -xr@"%exclude_file%" >nul 2>&1
        ) else (
            powershell -Command "Compress-Archive -Path %INCLUDE_DIRS% -DestinationPath '%backup_file%' -Force"
        )
    )
) else (
    REM Backup completo para semanal ou mensal
    call :log "Criando backup completo de arquivos..."
    if "%COMPRESSION_TOOL%"=="7z" (
        7z a -tzip -mx=%COMPRESSION_LEVEL% "%backup_file%" %INCLUDE_DIRS% -xr@"%exclude_file%" >nul 2>&1
    ) else (
        powershell -Command "Compress-Archive -Path %INCLUDE_DIRS% -DestinationPath '%backup_file%' -Force"
    )
)

REM Limpa arquivos temporários
if exist "%exclude_file%" del "%exclude_file%"
if exist "%ref_list%" del "%ref_list%"

if not exist "%backup_file%" (
    call :log "%RED%ERRO: Falha ao criar arquivo de backup dos arquivos%RESET%"
    exit /b 1
)

for %%F in ("%backup_file%") do set "file_size=%%~zF"
set "file_size_mb=%file_size:~0,-6%.%file_size:~-6,2% MB"
if %file_size% LSS 1000000 set "file_size_mb=%file_size% bytes"

call :log "Backup de arquivos concluído: %backup_file% (%file_size_mb%)"
exit /b 0

REM Função para backup de configurações
:backup_configs
set "backup_file=%CONFIG_BACKUP_DIR%\configs_%TIMESTAMP%.zip"

call :log "Iniciando backup de configurações..."

REM Comprime todos os arquivos de configuração
if "%COMPRESSION_TOOL%"=="7z" (
    7z a -tzip -mx=%COMPRESSION_LEVEL% "%backup_file%" "%CONFIG_DIR%\*" >nul 2>&1
) else (
    powershell -Command "Compress-Archive -Path '%CONFIG_DIR%\*' -DestinationPath '%backup_file%' -Force"
)

if not exist "%backup_file%" (
    call :log "%RED%ERRO: Falha ao criar backup das configurações%RESET%"
    exit /b 1
)

call :log "Backup de configurações concluído: %backup_file%"
exit /b 0

REM Função para backup de scripts
:backup_scripts
set "backup_file=%SCRIPTS_BACKUP_DIR%\scripts_%TIMESTAMP%.zip"

call :log "Iniciando backup de scripts..."

REM Comprime todos os scripts
if "%COMPRESSION_TOOL%"=="7z" (
    7z a -tzip -mx=%COMPRESSION_LEVEL% "%backup_file%" "%SERVIDOR_ROOT%\scripts\*" >nul 2>&1
) else (
    powershell -Command "Compress-Archive -Path '%SERVIDOR_ROOT%\scripts\*' -DestinationPath '%backup_file%' -Force"
)

if not exist "%backup_file%" (
    call :log "%RED%ERRO: Falha ao criar backup dos scripts%RESET%"
    exit /b 1
)

call :log "Backup de scripts concluído: %backup_file%"
exit /b 0

REM Função para limpar backups antigos
:limpar_backups_antigos
call :log "Iniciando limpeza de backups antigos..."

REM Remove backups diários antigos
call :log "Removendo backups diários com mais de %DAILY_RETENTION% dias..."
powershell -Command "$cutoffDate = (Get-Date).AddDays(-%DAILY_RETENTION%); Get-ChildItem -Path '%DB_BACKUP_DIR%\diario\*' -File | Where-Object { $_.LastWriteTime -lt $cutoffDate } | Remove-Item -Force"
powershell -Command "$cutoffDate = (Get-Date).AddDays(-%DAILY_RETENTION%); Get-ChildItem -Path '%FILES_BACKUP_DIR%\diario\*' -File | Where-Object { $_.LastWriteTime -lt $cutoffDate } | Remove-Item -Force"

REM Remove backups semanais antigos
call :log "Removendo backups semanais com mais de %WEEKLY_RETENTION% semanas..."
set /a weekly_days=%WEEKLY_RETENTION% * 7
powershell -Command "$cutoffDate = (Get-Date).AddDays(-%weekly_days%); Get-ChildItem -Path '%DB_BACKUP_DIR%\semanal\*' -File | Where-Object { $_.LastWriteTime -lt $cutoffDate } | Remove-Item -Force"
powershell -Command "$cutoffDate = (Get-Date).AddDays(-%weekly_days%); Get-ChildItem -Path '%FILES_BACKUP_DIR%\semanal\*' -File | Where-Object { $_.LastWriteTime -lt $cutoffDate } | Remove-Item -Force"

REM Remove backups mensais antigos
call :log "Removendo backups mensais com mais de %MONTHLY_RETENTION% meses..."
set /a monthly_days=%MONTHLY_RETENTION% * 30
powershell -Command "$cutoffDate = (Get-Date).AddDays(-%monthly_days%); Get-ChildItem -Path '%DB_BACKUP_DIR%\mensal\*' -File | Where-Object { $_.LastWriteTime -lt $cutoffDate } | Remove-Item -Force"
powershell -Command "$cutoffDate = (Get-Date).AddDays(-%monthly_days%); Get-ChildItem -Path '%FILES_BACKUP_DIR%\mensal\*' -File | Where-Object { $_.LastWriteTime -lt $cutoffDate } | Remove-Item -Force"

call :log "Limpeza de backups antigos concluída"
goto :eof

REM Função para verificar integridade dos backups
:verificar_backups
call :log "Verificando integridade dos backups recentes..."
set "falhas=0"

REM Verifica os backups de banco de dados
for %%t in (diario semanal mensal) do (
    for /f "delims=" %%f in ('dir /b /o-d "%DB_BACKUP_DIR%\%%t\*.zip" 2^>nul ^| findstr /v "^$" ^| head -1') do (
        call :log "Verificando backup de banco de dados: %%f"
        if "%COMPRESSION_TOOL%"=="7z" (
            7z t "%DB_BACKUP_DIR%\%%t\%%f" >nul 2>&1
            if errorlevel 1 (
                call :log "%RED%ERRO: Backup de banco de dados corrompido: %%f%RESET%"
                set /a falhas+=1
            )
        ) else (
            powershell -Command "try { $null = [System.IO.Compression.ZipFile]::OpenRead('%DB_BACKUP_DIR%\%%t\%%f'); Write-Output 'OK' } catch { Write-Error $_.Exception.Message; exit 1 }" >nul 2>&1
            if errorlevel 1 (
                call :log "%RED%ERRO: Backup de banco de dados corrompido: %%f%RESET%"
                set /a falhas+=1
            )
        )
    )
)

REM Verifica os backups de arquivos
for %%t in (diario semanal mensal) do (
    for /f "delims=" %%f in ('dir /b /o-d "%FILES_BACKUP_DIR%\%%t\*.zip" 2^>nul ^| findstr /v "^$" ^| head -1') do (
        call :log "Verificando backup de arquivos: %%f"
        if "%COMPRESSION_TOOL%"=="7z" (
            7z t "%FILES_BACKUP_DIR%\%%t\%%f" >nul 2>&1
            if errorlevel 1 (
                call :log "%RED%ERRO: Backup de arquivos corrompido: %%f%RESET%"
                set /a falhas+=1
            )
        ) else (
            powershell -Command "try { $null = [System.IO.Compression.ZipFile]::OpenRead('%FILES_BACKUP_DIR%\%%t\%%f'); Write-Output 'OK' } catch { Write-Error $_.Exception.Message; exit 1 }" >nul 2>&1
            if errorlevel 1 (
                call :log "%RED%ERRO: Backup de arquivos corrompido: %%f%RESET%"
                set /a falhas+=1
            )
        )
    )
)

if %falhas%==0 (
    call :log "%GREEN%Todos os backups recentes passaram na verificação de integridade%RESET%"
    exit /b 0
) else (
    call :log "%YELLOW%AVISO: %falhas% backups falharam na verificação de integridade%RESET%"
    exit /b 1
)

REM Função para gerar relatório de backup
:gerar_relatorio
set "backup_status=%~1"
set "tipo_backup=%~2"
set "relatorio=%LOG_DIR%\backup_report_%TIMESTAMP%.txt"

call :log "Gerando relatório de backup..."

echo ======================================================= > "%relatorio%"
echo   RELATORIO DE BACKUP DO SERVIDOR WYDBR >> "%relatorio%"
echo   Data: %date% %time% >> "%relatorio%"
echo   Tipo: %tipo_backup% >> "%relatorio%"
echo ======================================================= >> "%relatorio%"
echo. >> "%relatorio%"

echo RESUMO: >> "%relatorio%"
if %backup_status%==0 (
    echo   Status: SUCESSO >> "%relatorio%"
) else (
    echo   Status: FALHA >> "%relatorio%"
)

echo. >> "%relatorio%"
echo DETALHES DOS BACKUPS: >> "%relatorio%"

REM Adiciona informações sobre os backups de BD
echo   Database Backups: >> "%relatorio%"
for /f "delims=" %%f in ('dir /b /o-d "%DB_BACKUP_DIR%\%tipo_backup%\*.zip" 2^>nul ^| findstr /v "^$" ^| head -1') do (
    for %%F in ("%DB_BACKUP_DIR%\%tipo_backup%\%%f") do set "file_size=%%~zF"
    set "file_size_kb=!file_size:~0,-3!.!file_size:~-3! KB"
    echo     - %DB_BACKUP_DIR%\%tipo_backup%\%%f (!file_size_kb!) >> "%relatorio%"
)

REM Adiciona informações sobre os backups de arquivos
echo   File Backups: >> "%relatorio%"
for /f "delims=" %%f in ('dir /b /o-d "%FILES_BACKUP_DIR%\%tipo_backup%\*.zip" 2^>nul ^| findstr /v "^$" ^| head -1') do (
    for %%F in ("%FILES_BACKUP_DIR%\%tipo_backup%\%%f") do set "file_size=%%~zF"
    set "file_size_kb=!file_size:~0,-3!.!file_size:~-3! KB"
    echo     - %FILES_BACKUP_DIR%\%tipo_backup%\%%f (!file_size_kb!) >> "%relatorio%"
)

REM Adiciona informações sobre espaço em disco
echo. >> "%relatorio%"
echo ESPAÇO EM DISCO: >> "%relatorio%"
wmic logicaldisk get caption,description,freespace,size >> "%relatorio%"

REM Adiciona histórico de backups
echo. >> "%relatorio%"
echo HISTÓRICO DE BACKUPS RECENTES: >> "%relatorio%"
echo   Diários: >> "%relatorio%"
dir "%DB_BACKUP_DIR%\diario" >> "%relatorio%"

echo. >> "%relatorio%"
echo ======================================================= >> "%relatorio%"

call :log "Relatório de backup gerado: %relatorio%"

REM Envia o relatório por email se configurado
if defined EMAIL_NOTIFY (
    call :log "Enviando relatório por email para %EMAIL_NOTIFY%..."
    powershell -Command "Send-MailMessage -From 'backup@wydbr.com' -To '%EMAIL_NOTIFY%' -Subject 'Relatório de Backup WYDBR - %date%' -Body (Get-Content -Path '%relatorio%' | Out-String) -SmtpServer 'smtp.wydbr.com'"
)

goto :eof

REM Função para backup externo (se configurado)
:backup_externo
set "backup_status=%~1"

REM Verifica se o backup externo está configurado
if not defined REMOTE_BACKUP_PATH (
    call :log "Backup remoto não configurado"
    goto :eof
)

call :log "Iniciando cópia de backup para destino externo: %REMOTE_BACKUP_PATH%"

REM Verifica se o destino está acessível
if exist "%REMOTE_BACKUP_PATH%\" (
    REM Copia os backups mais recentes para o destino
    robocopy "%BACKUP_ROOT%" "%REMOTE_BACKUP_PATH%" /MIR /R:3 /W:5 /MT:8 /LOG:"%LOG_DIR%\robocopy_%TIMESTAMP%.log"
    
    if errorlevel 8 (
        call :log "%RED%ERRO: Falha ao copiar para destino externo%RESET%"
        exit /b 1
    ) else (
        call :log "%GREEN%Backup externo concluído com sucesso%RESET%"
        exit /b 0
    )
) else (
    call :log "%YELLOW%AVISO: Caminho de backup externo não existe: %REMOTE_BACKUP_PATH%%RESET%"
    exit /b 1
)

REM Início do script
title WYDBR Server Backup
echo %CYAN%=============================================================%RESET%
echo %CYAN%          WYDBR - Sistema de Backup do Servidor           %RESET%
echo %CYAN%=============================================================%RESET%
echo.

REM Verifica se o diretório de log existe
if not exist "%LOG_DIR%" mkdir "%LOG_DIR%" 2>nul

REM Inicializa log
call :log "=== Iniciando script de backup do WYDBR ==="
call :log "Versão do Windows: %OS%"

REM Processa parâmetros da linha de comando
set "TIPO_BACKUP="
set "EMAIL_NOTIFY="
set "REMOTE_BACKUP_PATH="

:parse_args
if "%1"=="" goto end_parse_args

if "%1"=="--tipo" (
    set "TIPO_BACKUP=%2"
    shift & shift
    goto parse_args
) else if "%1"=="--email" (
    set "EMAIL_NOTIFY=%2"
    shift & shift
    goto parse_args
) else if "%1"=="--remoto" (
    set "REMOTE_BACKUP_PATH=%2"
    shift & shift
    goto parse_args
) else if "%1"=="--help" (
    echo Uso: %0 [opções]
    echo Opções:
    echo   --tipo TIPO    Especifica o tipo de backup (diario, semanal, mensal)
    echo   --email EMAIL  Envia relatório para o email especificado
    echo   --remoto PATH  Define um destino remoto para cópia de segurança
    exit /b 0
) else (
    call :log "%YELLOW%AVISO: Opção desconhecida: %1%RESET%"
    shift
    goto parse_args
)

:end_parse_args

REM Se o tipo não for especificado, determina automaticamente
if not defined TIPO_BACKUP call :determinar_tipo_backup

REM Cria diretórios necessários
call :criar_diretorios

REM Verifica dependências
call :verificar_ferramentas

REM Executa os backups
set "backup_status=0"

REM Backup do banco de dados
call :backup_database "%TIPO_BACKUP%"
if errorlevel 1 set "backup_status=1"

REM Backup de arquivos
call :backup_files "%TIPO_BACKUP%"
if errorlevel 1 set "backup_status=1"

REM Backup de configurações (sempre)
call :backup_configs
if errorlevel 1 set "backup_status=1"

REM Backup de scripts (sempre)
call :backup_scripts
if errorlevel 1 set "backup_status=1"

REM Limpa backups antigos
call :limpar_backups_antigos

REM Verifica integridade dos backups
call :verificar_backups
if errorlevel 1 set "backup_status=1"

REM Gera relatório
call :gerar_relatorio %backup_status% "%TIPO_BACKUP%"

REM Backup externo se configurado
if defined REMOTE_BACKUP_PATH (
    call :backup_externo %backup_status%
)

REM Finaliza
if %backup_status%==0 (
    call :log "%GREEN%=== Backup concluído com sucesso ===%RESET%"
    exit /b 0
) else (
    call :log "%YELLOW%=== Backup concluído com erros ===%RESET%"
    exit /b 1
)

endlocal 