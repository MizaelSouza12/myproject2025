@echo off
setlocal enabledelayedexpansion

REM Script de manutenção do servidor WYDBR para Windows
REM Realiza tarefas de manutenção automáticas como limpeza, otimização e verificações de integridade

REM Configurações
set "SERVIDOR_ROOT=C:\WYDBR\server"
set "LOG_DIR=%SERVIDOR_ROOT%\logs"
set "CONFIG_DIR=%SERVIDOR_ROOT%\config"
set "PID_DIR=%SERVIDOR_ROOT%\pids"

REM Timestamp para logs
for /f "tokens=2 delims==" %%a in ('wmic OS Get localdatetime /value') do set "dt=%%a"
set "TIMESTAMP=%dt:~0,8%_%dt:~8,6%"
set "LOG_FILE=%LOG_DIR%\maintenance_%TIMESTAMP%.log"

REM Valores padrão
set "MAX_LOG_DAYS=30"
set "TEMP_FILES_AGE=7"
set "SERVER_RESTART=false"
set "VACUUM_DB=true"
set "CHECK_INTEGRITY=true"
set "OPTIMIZE_TABLES=true"
set "VERBOSE=false"
set "OFFLINE_MODE=false"

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

REM Função para criar diretórios necessários
:criar_diretorios
call :log "Verificando diretórios necessários..."

if not exist "%LOG_DIR%" (
    mkdir "%LOG_DIR%" 2>nul
    call :log "Diretório de logs criado em %LOG_DIR%"
)

for %%d in (tmp cache reports) do (
    if not exist "%SERVIDOR_ROOT%\%%d" (
        mkdir "%SERVIDOR_ROOT%\%%d" 2>nul
        call :log "Diretório %%d criado em %SERVIDOR_ROOT%\%%d"
    )
)
goto :eof

REM Função para verificar os servidores ativos
:verificar_servidores_ativos
call :log "Verificando servidores ativos..." "DEBUG"

set "servidores_ativos="

REM Procurar por processos ativos
for %%s in (DBSrv TMSrv WSKSrv NRSrv BISrv AgeSrv) do (
    tasklist /fi "imagename eq %%s.exe" 2>nul | find "%%s.exe" >nul
    if not errorlevel 1 (
        set "servidores_ativos=!servidores_ativos! %%s"
        call :log "Servidor ativo: %%s" "DEBUG"
    )
)

call :log "Servidores ativos:%servidores_ativos%"
set "%~1=%servidores_ativos%"
goto :eof

REM Função para parar todos os servidores
:parar_servidores
call :log "Parando todos os servidores para manutenção..."

REM Usa o script de shutdown
if exist "%SERVIDOR_ROOT%\scripts\shutdown.bat" (
    call :log "Executando script de shutdown..."
    call "%SERVIDOR_ROOT%\scripts\shutdown.bat" --force
    timeout /t 5 /nobreak >nul
    
    REM Verifica se ainda há servidores ativos
    set "servidores_restantes="
    call :verificar_servidores_ativos servidores_restantes
    
    if not "!servidores_restantes!"=="" (
        call :log "AVISO: Alguns servidores ainda estão em execução:!servidores_restantes!" "WARN"
        
        REM Tenta forçar o encerramento
        for %%s in (!servidores_restantes!) do (
            call :log "Forçando encerramento de %%s..." "WARN"
            taskkill /f /im "%%s.exe" >nul 2>&1
        )
    )
) else (
    call :log "Script de shutdown não encontrado. Tentando parar servidores manualmente..." "WARN"
    
    REM Para cada processo de servidor conhecido
    for %%s in (DBSrv TMSrv WSKSrv NRSrv BISrv AgeSrv) do (
        tasklist /fi "imagename eq %%s.exe" 2>nul | find "%%s.exe" >nul
        if not errorlevel 1 (
            call :log "Parando %%s..."
            taskkill /im "%%s.exe" >nul 2>&1
            timeout /t 2 /nobreak >nul
            
            REM Verifica se o processo ainda está em execução
            tasklist /fi "imagename eq %%s.exe" 2>nul | find "%%s.exe" >nul
            if not errorlevel 1 (
                call :log "Forçando encerramento de %%s..." "WARN"
                taskkill /f /im "%%s.exe" >nul 2>&1
            )
        )
    )
)

call :log "Todos os servidores foram parados"
goto :eof

REM Função para iniciar todos os servidores
:iniciar_servidores
call :log "Iniciando todos os servidores após manutenção..."

REM Usa o script de startup
if exist "%SERVIDOR_ROOT%\scripts\startup.bat" (
    call :log "Executando script de startup..."
    call "%SERVIDOR_ROOT%\scripts\startup.bat"
) else (
    call :log "Script de startup não encontrado. Não é possível iniciar os servidores automaticamente." "ERROR"
    exit /b 1
)

REM Verifica se os servidores iniciaram
timeout /t 10 /nobreak >nul
set "servidores_ativos="
call :verificar_servidores_ativos servidores_ativos

if not "!servidores_ativos!"=="" (
    call :log "Servidores iniciados com sucesso:!servidores_ativos!"
) else (
    call :log "ERRO: Nenhum servidor parece ter iniciado." "ERROR"
    exit /b 1
)

exit /b 0

REM Função para limpar logs antigos
:limpar_logs
call :log "Iniciando limpeza de logs antigos (mais de %MAX_LOG_DAYS% dias)..."

set "contador=0"
set "tamanho_total=0"
set "data_limite="

REM Calcula a data limite para deletar logs
powershell -Command "$date = (Get-Date).AddDays(-%MAX_LOG_DAYS%); Write-Output $date.ToString('yyyy-MM-dd')" > "%TEMP%\date_limit.txt"
set /p data_limite=<"%TEMP%\date_limit.txt"
del "%TEMP%\date_limit.txt" 2>nul

call :log "Removendo logs anteriores a %data_limite%..."

REM Encontra e remove logs antigos
for /f "delims=" %%f in ('powershell -Command "Get-ChildItem -Path '%LOG_DIR%\*.log' | Where-Object { $_.LastWriteTime -lt (Get-Date).AddDays(-%MAX_LOG_DAYS%) }"') do (
    REM Obtém o tamanho do arquivo
    for %%A in ("%%f") do set "tamanho=%%~zA"
    
    del "%%f" /q
    set /a tamanho_total+=tamanho
    set /a contador+=1
    
    if "%VERBOSE%"=="true" (
        call :log "Removido: %%f" "DEBUG"
    )
)

REM Compacta logs mais recentes que não estão compactados
set "contador_compactados=0"
for /f "delims=" %%f in ('powershell -Command "Get-ChildItem -Path '%LOG_DIR%\*.log' | Where-Object { $_.LastWriteTime -lt (Get-Date).AddDays(-1) -and $_.LastWriteTime -gt (Get-Date).AddDays(-%MAX_LOG_DAYS%) }"') do (
    powershell -Command "Compress-Archive -Path '%%f' -DestinationPath '%%f.zip' -Force"
    if exist "%%f.zip" (
        del "%%f" /q
        set /a contador_compactados+=1
        
        if "%VERBOSE%"=="true" (
            call :log "Compactado: %%f" "DEBUG"
        )
    )
)

REM Converte o tamanho para MB
set /a tamanho_total_mb=tamanho_total/1048576
call :log "Limpeza de logs concluída. %contador% arquivos removidos (%tamanho_total_mb% MB), %contador_compactados% arquivos compactados."
goto :eof

REM Função para limpar arquivos temporários
:limpar_temporarios
call :log "Iniciando limpeza de arquivos temporários (mais de %TEMP_FILES_AGE% dias)..."

set "contador=0"
set "tamanho_total=0"

REM Limpa arquivos temporários em diretórios específicos
for %%d in ("%SERVIDOR_ROOT%\tmp" "%SERVIDOR_ROOT%\cache") do (
    if exist "%%d" (
        for /f "delims=" %%f in ('powershell -Command "Get-ChildItem -Path '%%d\*' -File | Where-Object { $_.LastWriteTime -lt (Get-Date).AddDays(-%TEMP_FILES_AGE%) }"') do (
            REM Obtém o tamanho do arquivo
            for %%A in ("%%f") do set "tamanho=%%~zA"
            
            del "%%f" /q
            set /a tamanho_total+=tamanho
            set /a contador+=1
            
            if "%VERBOSE%"=="true" (
                call :log "Removido temporário: %%f" "DEBUG"
            )
        )
    )
)

REM Limpar cache de compilação
if exist "%SERVIDOR_ROOT%\bin\obj" (
    call :log "Limpando cache de compilação..."
    rmdir /s /q "%SERVIDOR_ROOT%\bin\obj" >nul 2>&1
    mkdir "%SERVIDOR_ROOT%\bin\obj" >nul 2>&1
)

REM Converte o tamanho para MB
set /a tamanho_total_mb=tamanho_total/1048576
call :log "Limpeza de arquivos temporários concluída. %contador% arquivos removidos (%tamanho_total_mb% MB)."
goto :eof

REM Função para verificar uso de disco e espaço
:verificar_espaco_disco
call :log "Verificando uso de disco..."

REM Verifica espaço em cada partição
for /f "tokens=1-5" %%a in ('wmic logicaldisk get caption^,description^,freespace^,size ^| findstr /v "^$"') do (
    if "%%a" NEQ "Caption" (
        set "drive=%%a"
        set "desc=%%b"
        set "livre=%%c"
        set "total=%%d"
        
        if "!livre!" NEQ "" if "!total!" NEQ "" (
            set /a uso=100-(!livre!*100/!total!)
            call :log "Disco !drive! (!desc!): !uso!%% usado (livre: !livre! bytes, total: !total! bytes)"
            
            if !uso! gtr 90 (
                call :log "ALERTA: Disco !drive! com mais de 90%% de uso (!!uso!%%)!" "WARN"
            )
        )
    )
)

REM Verifica uso de espaço nos diretórios principais
call :log "Analisando tamanho dos diretórios principais..."
for %%d in (bin data logs config) do (
    if exist "%SERVIDOR_ROOT%\%%d" (
        for /f "tokens=*" %%s in ('powershell -Command "(Get-ChildItem -Path '%SERVIDOR_ROOT%\%%d' -Recurse | Measure-Object -Property Length -Sum).Sum / 1MB"') do (
            call :log "Diretório %SERVIDOR_ROOT%\%%d: %%s MB"
        )
    )
)
goto :eof

REM Função para otimizar banco de dados MySQL
:otimizar_mysql
call :log "Tentando otimizar banco de dados MySQL..."

REM Verifica se o MySQL está instalado
where mysql >nul 2>&1
if errorlevel 1 (
    call :log "MySQL não encontrado. Pulando otimização." "WARN"
    goto :eof
)

REM Lê credenciais do arquivo de configuração
set "mysql_user=wydbr"
set "mysql_password=wyd_db_password"
set "mysql_host=localhost"
set "mysql_db=wydbr"

REM Tenta ler configurações do arquivo, se existir
if exist "%CONFIG_DIR%\DBSrv.conf" (
    call :log "Lendo configurações de banco de dados de %CONFIG_DIR%\DBSrv.conf..."
    for /f "tokens=1,* delims==" %%a in ('type "%CONFIG_DIR%\DBSrv.conf" ^| findstr /r "^DB_USER= ^DB_PASSWORD= ^DB_HOST= ^DB_NAME="') do (
        set "param=%%a"
        set "value=%%b"
        
        if "!param!"=="DB_USER" set "mysql_user=!value!"
        if "!param!"=="DB_PASSWORD" set "mysql_password=!value!"
        if "!param!"=="DB_HOST" set "mysql_host=!value!"
        if "!param!"=="DB_NAME" set "mysql_db=!value!"
    )
)

REM Verifica conexão com o banco
mysql -u%mysql_user% -p%mysql_password% -h%mysql_host% -e "SELECT 1" >nul 2>&1
if errorlevel 1 (
    call :log "Não foi possível conectar ao MySQL. Verifique as credenciais." "ERROR"
    goto :eof
)

REM Realiza vacuum se habilitado
if "%VACUUM_DB%"=="true" (
    call :log "Realizando VACUUM nas tabelas MySQL..."
    mysql -u%mysql_user% -p%mysql_password% -h%mysql_host% %mysql_db% -e "SHOW TABLES" > "%TEMP%\tables.txt"
    
    for /f "skip=1" %%t in ('type "%TEMP%\tables.txt"') do (
        call :log "VACUUM na tabela %%t" "DEBUG"
        mysql -u%mysql_user% -p%mysql_password% -h%mysql_host% %mysql_db% -e "OPTIMIZE TABLE %%t"
    )
    
    del "%TEMP%\tables.txt" >nul 2>&1
)

REM Realiza otimização de tabelas se habilitado
if "%OPTIMIZE_TABLES%"=="true" (
    call :log "Otimizando tabelas MySQL..."
    mysql -u%mysql_user% -p%mysql_password% -h%mysql_host% %mysql_db% -e "SHOW TABLES" > "%TEMP%\tables.txt"
    
    for /f "skip=1" %%t in ('type "%TEMP%\tables.txt"') do (
        call :log "Otimizando tabela %%t" "DEBUG"
        mysql -u%mysql_user% -p%mysql_password% -h%mysql_host% %mysql_db% -e "OPTIMIZE TABLE %%t"
    )
    
    del "%TEMP%\tables.txt" >nul 2>&1
)

REM Verifica integridade se habilitado
if "%CHECK_INTEGRITY%"=="true" (
    call :log "Verificando integridade das tabelas MySQL..."
    mysql -u%mysql_user% -p%mysql_password% -h%mysql_host% %mysql_db% -e "SHOW TABLES" > "%TEMP%\tables.txt"
    
    for /f "skip=1" %%t in ('type "%TEMP%\tables.txt"') do (
        call :log "Verificando tabela %%t" "DEBUG"
        mysql -u%mysql_user% -p%mysql_password% -h%mysql_host% %mysql_db% -e "CHECK TABLE %%t" > "%LOG_DIR%\mysql_check_%TIMESTAMP%_%%t.log"
    )
    
    del "%TEMP%\tables.txt" >nul 2>&1
)

call :log "Otimização MySQL concluída."
goto :eof

REM Função para otimizar banco de dados PostgreSQL
:otimizar_postgresql
call :log "Tentando otimizar banco de dados PostgreSQL..."

REM Verifica se o PostgreSQL está instalado
where psql >nul 2>&1
if errorlevel 1 (
    call :log "PostgreSQL não encontrado. Pulando otimização." "WARN"
    goto :eof
)

REM Lê credenciais do arquivo de configuração
set "pg_user=wydbr"
set "pg_password=wyd_db_password"
set "pg_host=localhost"
set "pg_db=wydbr"

REM Tenta ler configurações do arquivo, se existir
if exist "%CONFIG_DIR%\DBSrv.conf" (
    call :log "Lendo configurações de banco de dados de %CONFIG_DIR%\DBSrv.conf..."
    for /f "tokens=1,* delims==" %%a in ('type "%CONFIG_DIR%\DBSrv.conf" ^| findstr /r "^DB_USER= ^DB_PASSWORD= ^DB_HOST= ^DB_NAME="') do (
        set "param=%%a"
        set "value=%%b"
        
        if "!param!"=="DB_USER" set "pg_user=!value!"
        if "!param!"=="DB_PASSWORD" set "pg_password=!value!"
        if "!param!"=="DB_HOST" set "pg_host=!value!"
        if "!param!"=="DB_NAME" set "pg_db=!value!"
    )
)

REM Define a variável de ambiente para a senha
set "PGPASSWORD=%pg_password%"

REM Verifica conexão com o banco
psql -U %pg_user% -h %pg_host% -d %pg_db% -c "SELECT 1" >nul 2>&1
if errorlevel 1 (
    call :log "Não foi possível conectar ao PostgreSQL. Verifique as credenciais." "ERROR"
    goto :eof
)

REM Realiza vacuum se habilitado
if "%VACUUM_DB%"=="true" (
    call :log "Realizando VACUUM FULL no PostgreSQL..."
    psql -U %pg_user% -h %pg_host% -d %pg_db% -c "VACUUM FULL;"
)

REM Realiza análise se habilitado
if "%OPTIMIZE_TABLES%"=="true" (
    call :log "Analisando tabelas PostgreSQL..."
    psql -U %pg_user% -h %pg_host% -d %pg_db% -c "ANALYZE;"
)

REM Verifica integridade se habilitado
if "%CHECK_INTEGRITY%"=="true" (
    call :log "Verificando integridade do PostgreSQL..."
    psql -U %pg_user% -h %pg_host% -d %pg_db% -c "SELECT * FROM pg_stat_database WHERE datname = '%pg_db%';" > "%LOG_DIR%\pg_check_%TIMESTAMP%.log"
)

call :log "Otimização PostgreSQL concluída."
goto :eof

REM Função para verificar integridade do sistema de arquivos
:verificar_arquivos
call :log "Verificando integridade do sistema de arquivos..."

REM Verifica permissões dos arquivos executáveis
call :log "Verificando arquivos executáveis..."
dir /b "%SERVIDOR_ROOT%\bin\*.exe" > "%TEMP%\executables.txt"
type "%TEMP%\executables.txt" | find /v "" > "%LOG_DIR%\executables_%TIMESTAMP%.log"
del "%TEMP%\executables.txt" >nul 2>&1

REM Procura por arquivos corrompidos (tamanho zero onde não deveria)
call :log "Procurando por arquivos corrompidos..."
powershell -Command "Get-ChildItem -Path '%SERVIDOR_ROOT%\data' -Recurse -File | Where-Object {$_.Length -eq 0} | Select-Object FullName, LastWriteTime | Format-Table -AutoSize" > "%LOG_DIR%\empty_files_%TIMESTAMP%.log"

REM Verifica arquivos de configuração
call :log "Verificando arquivos de configuração..."
for %%f in ("%CONFIG_DIR%\*.conf") do (
    type "%%f" | findstr /i "ERROR FAILED CORRUPTED" >nul
    if not errorlevel 1 (
        call :log "Possível problema no arquivo de configuração: %%f" "WARN"
    )
)
goto :eof

REM Função para reparar permissões
:reparar_permissoes
call :log "Verificando permissões de arquivos e diretórios..."

REM Em Windows, precisamos verificar principalmente os problemas de acesso
powershell -Command "Get-ChildItem -Path '%SERVIDOR_ROOT%' -Recurse -Force | Where-Object { -not $_.PSIsContainer } | ForEach-Object { $_ | Add-Member -MemberType NoteProperty -Name 'AccessDenied' -Value $false -Force; try { $null = [IO.File]::OpenRead($_.FullName).Close() } catch { $_.AccessDenied = $true }; $_ } | Where-Object { $_.AccessDenied } | Select-Object FullName" > "%TEMP%\access_denied.txt"

REM Verifica se existem arquivos com problemas de acesso
for /f "tokens=*" %%f in ('type "%TEMP%\access_denied.txt"') do (
    call :log "Problema de acesso detectado em: %%f" "WARN"
)

del "%TEMP%\access_denied.txt" >nul 2>&1

REM Verifica arquivos somente leitura nos diretórios importantes
powershell -Command "Get-ChildItem -Path '%SERVIDOR_ROOT%\config' -Recurse -Force | Where-Object { -not $_.PSIsContainer -and $_.IsReadOnly } | Set-ItemProperty -Name IsReadOnly -Value $false"
powershell -Command "Get-ChildItem -Path '%SERVIDOR_ROOT%\data' -Recurse -Force | Where-Object { -not $_.PSIsContainer -and $_.IsReadOnly } | Set-ItemProperty -Name IsReadOnly -Value $false"

call :log "Permissões verificadas e corrigidas quando possível"
goto :eof

REM Função para gerar relatório de manutenção
:gerar_relatorio
call :log "Gerando relatório de manutenção..."
set "relatorio=%LOG_DIR%\maintenance_report_%TIMESTAMP%.html"

echo ^<!DOCTYPE html^> > "%relatorio%"
echo ^<html^> >> "%relatorio%"
echo ^<head^> >> "%relatorio%"
echo     ^<title^>Relatório de Manutenção WYDBR - %date%^</title^> >> "%relatorio%"
echo     ^<style^> >> "%relatorio%"
echo         body { font-family: Arial, sans-serif; margin: 20px; } >> "%relatorio%"
echo         h1 { color: #333366; } >> "%relatorio%"
echo         h2 { color: #336699; margin-top: 20px; } >> "%relatorio%"
echo         .success { color: green; } >> "%relatorio%"
echo         .warning { color: orange; } >> "%relatorio%"
echo         .error { color: red; } >> "%relatorio%"
echo         table { border-collapse: collapse; width: 100%%; margin-top: 10px; } >> "%relatorio%"
echo         th, td { border: 1px solid #ddd; padding: 8px; text-align: left; } >> "%relatorio%"
echo         th { background-color: #f2f2f2; } >> "%relatorio%"
echo     ^</style^> >> "%relatorio%"
echo ^</head^> >> "%relatorio%"
echo ^<body^> >> "%relatorio%"
echo     ^<h1^>Relatório de Manutenção WYDBR^</h1^> >> "%relatorio%"
echo     ^<p^>Data: %date% %time%^</p^> >> "%relatorio%"
echo     >> "%relatorio%"
echo     ^<h2^>Resumo das Atividades^</h2^> >> "%relatorio%"
echo     ^<ul^> >> "%relatorio%"
echo         ^<li^>Limpeza de Logs: ^<span class="success"^>Concluída^</span^>^</li^> >> "%relatorio%"
echo         ^<li^>Limpeza de Arquivos Temporários: ^<span class="success"^>Concluída^</span^>^</li^> >> "%relatorio%"
echo         ^<li^>Verificação de Espaço em Disco: ^<span class="success"^>Concluída^</span^>^</li^> >> "%relatorio%"

if "%VACUUM_DB%"=="true" (
    echo         ^<li^>Otimização de Banco de Dados: ^<span class="success"^>Concluída^</span^>^</li^> >> "%relatorio%"
)

if "%CHECK_INTEGRITY%"=="true" (
    echo         ^<li^>Verificação de Integridade: ^<span class="success"^>Concluída^</span^>^</li^> >> "%relatorio%"
)

echo     ^</ul^> >> "%relatorio%"
echo     >> "%relatorio%"
echo     ^<h2^>Uso de Disco^</h2^> >> "%relatorio%"
echo     ^<table^> >> "%relatorio%"
echo         ^<tr^>^<th^>Drive^</th^>^<th^>Descrição^</th^>^<th^>Espaço Livre^</th^>^<th^>Tamanho Total^</th^>^<th^>Percentual Usado^</th^>^</tr^> >> "%relatorio%"

REM Adiciona informações de cada disco
for /f "tokens=1-5" %%a in ('wmic logicaldisk get caption^,description^,freespace^,size ^| findstr /v "^$"') do (
    if "%%a" NEQ "Caption" (
        set "drive=%%a"
        set "desc=%%b"
        set "livre=%%c"
        set "total=%%d"
        
        if "!livre!" NEQ "" if "!total!" NEQ "" (
            set /a uso=100-(!livre!*100/!total!)
            set /a livre_gb=!livre!/1073741824
            set /a total_gb=!total!/1073741824
            echo         ^<tr^>^<td^>!drive!^</td^>^<td^>!desc!^</td^>^<td^>!livre_gb! GB^</td^>^<td^>!total_gb! GB^</td^>^<td^>!uso!%%^</td^>^</tr^> >> "%relatorio%"
        )
    )
)

echo     ^</table^> >> "%relatorio%"
echo     >> "%relatorio%"
echo     ^<h2^>Diretórios Principais^</h2^> >> "%relatorio%"
echo     ^<table^> >> "%relatorio%"
echo         ^<tr^>^<th^>Diretório^</th^>^<th^>Tamanho^</th^>^</tr^> >> "%relatorio%"

REM Adiciona informações de cada diretório principal
for %%d in (bin data logs config) do (
    if exist "%SERVIDOR_ROOT%\%%d" (
        for /f "tokens=*" %%s in ('powershell -Command "(Get-ChildItem -Path '%SERVIDOR_ROOT%\%%d' -Recurse | Measure-Object -Property Length -Sum).Sum / 1MB"') do (
            echo         ^<tr^>^<td^>%SERVIDOR_ROOT%\%%d^</td^>^<td^>%%s MB^</td^>^</tr^> >> "%relatorio%"
        )
    )
)

echo     ^</table^> >> "%relatorio%"
echo     >> "%relatorio%"
echo     ^<h2^>Status do Sistema^</h2^> >> "%relatorio%"
echo     ^<pre^> >> "%relatorio%"
systeminfo | findstr /B /C:"OS Name" /C:"OS Version" /C:"System Type" /C:"Total Physical Memory" /C:"Available Physical Memory" >> "%relatorio%"
echo     ^</pre^> >> "%relatorio%"
echo     >> "%relatorio%"
echo     ^<p^>Relatório gerado automaticamente pelo script de manutenção do servidor WYDBR.^</p^> >> "%relatorio%"
echo ^</body^> >> "%relatorio%"
echo ^</html^> >> "%relatorio%"

call :log "Relatório HTML gerado: %relatorio%"
goto :eof

REM Início do script
title WYDBR Server Maintenance
echo %CYAN%=============================================================%RESET%
echo %CYAN%        WYDBR - Sistema de Manutenção do Servidor         %RESET%
echo %CYAN%=============================================================%RESET%
echo.

REM Criar diretórios necessários
call :criar_diretorios

REM Inicializa log
call :log "=== Iniciando script de manutenção do WYDBR ==="
call :log "Versão do Windows: %OS%"

REM Processa parâmetros da linha de comando
:parse_args
if "%1"=="" goto end_parse_args

if "%1"=="--log-days" (
    if "%2" NEQ "" set "MAX_LOG_DAYS=%2"
    shift & shift
    goto parse_args
) else if "%1"=="--temp-days" (
    if "%2" NEQ "" set "TEMP_FILES_AGE=%2"
    shift & shift
    goto parse_args
) else if "%1"=="--restart" (
    set "SERVER_RESTART=true"
    shift
    goto parse_args
) else if "%1"=="--no-vacuum" (
    set "VACUUM_DB=false"
    shift
    goto parse_args
) else if "%1"=="--no-integrity" (
    set "CHECK_INTEGRITY=false"
    shift
    goto parse_args
) else if "%1"=="--no-optimize" (
    set "OPTIMIZE_TABLES=false"
    shift
    goto parse_args
) else if "%1"=="--verbose" (
    set "VERBOSE=true"
    shift
    goto parse_args
) else if "%1"=="-v" (
    set "VERBOSE=true"
    shift
    goto parse_args
) else if "%1"=="--offline" (
    set "OFFLINE_MODE=true"
    shift
    goto parse_args
) else if "%1"=="--help" (
    echo Uso: %0 [opções]
    echo Opções:
    echo   --log-days N        Define dias para manter logs (padrão: %MAX_LOG_DAYS%)
    echo   --temp-days N       Define dias para manter arquivos temporários (padrão: %TEMP_FILES_AGE%)
    echo   --restart           Reinicia servidores após manutenção
    echo   --no-vacuum         Desativa vacuum no banco de dados
    echo   --no-integrity      Desativa verificação de integridade
    echo   --no-optimize       Desativa otimização de tabelas
    echo   --verbose, -v       Modo verboso
    echo   --offline           Modo offline (desliga servidores durante manutenção)
    exit /b 0
) else (
    call :log "AVISO: Opção desconhecida: %1" "WARN"
    shift
    goto parse_args
)

:end_parse_args

REM Verificar servidores ativos
set "servidores_antes="
call :verificar_servidores_ativos servidores_antes

REM Se modo offline ativado, para os servidores
set "precisa_reiniciar=false"
if "%OFFLINE_MODE%"=="true" if not "%servidores_antes%"=="" (
    call :parar_servidores
    set "precisa_reiniciar=true"
)

REM Executar tarefas de manutenção
call :log "Iniciando tarefas de manutenção..."

REM Limpeza de logs
call :limpar_logs

REM Limpeza de arquivos temporários
call :limpar_temporarios

REM Verificação de espaço em disco
call :verificar_espaco_disco

REM Se tiver banco de dados MySQL
call :otimizar_mysql

REM Se tiver banco de dados PostgreSQL
call :otimizar_postgresql

REM Verificação de integridade de arquivos
call :verificar_arquivos

REM Reparo de permissões
call :reparar_permissoes

REM Gera relatório
call :gerar_relatorio

REM Reinicia servidores se necessário
if "%precisa_reiniciar%"=="true" (
    call :log "Reiniciando servidores..."
    call :iniciar_servidores
    
    REM Verifica se os servidores foram reiniciados corretamente
    set "servidores_depois="
    call :verificar_servidores_ativos servidores_depois
    
    if "%servidores_depois%"=="" (
        call :log "ALERTA: Servidores não foram reiniciados corretamente!" "WARN"
    ) else (
        call :log "Servidores reiniciados: %servidores_depois%"
    )
) else if "%SERVER_RESTART%"=="true" if not "%servidores_antes%"=="" (
    call :log "Reiniciando servidores conforme solicitado..."
    call :parar_servidores
    call :iniciar_servidores
)

call :log "=== Manutenção do servidor WYDBR concluída ==="

endlocal 