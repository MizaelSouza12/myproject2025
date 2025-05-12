#!/bin/bash

# Script de manutenção do servidor WYDBR
# Realiza tarefas de manutenção automáticas como limpeza, otimização e verificações de integridade

# Configurações
SERVIDOR_ROOT="/opt/WYDBR/server"
LOG_DIR="${SERVIDOR_ROOT}/logs"
TIMESTAMP=$(date +"%Y%m%d_%H%M%S")
LOG_FILE="${LOG_DIR}/maintenance_${TIMESTAMP}.log"
CONFIG_DIR="${SERVIDOR_ROOT}/config"
PID_DIR="${SERVIDOR_ROOT}/pids"

# Valores padrão
MAX_LOG_DAYS=30           # Dias para manter logs antigos
TEMP_FILES_AGE=7          # Dias para considerar arquivos temporários como obsoletos
SERVER_RESTART=false      # Reiniciar servidores após manutenção
VACUUM_DB=true            # Realizar vacuum no banco de dados
CHECK_INTEGRITY=true      # Verificar integridade do banco de dados
OPTIMIZE_TABLES=true      # Otimizar tabelas do banco de dados
VERBOSE=false             # Modo verboso
OFFLINE_MODE=false        # Modo offline (desliga servidores durante manutenção)

# Função para registrar mensagens no log
log() {
    local mensagem="$1"
    local nivel="${2:-INFO}"
    echo "[$(date +"%Y-%m-%d %H:%M:%S")] [${nivel}] $mensagem" | tee -a "$LOG_FILE"
}

# Função para verificar e criar diretórios
verificar_diretorios() {
    log "Verificando diretórios necessários..."
    
    if [ ! -d "$LOG_DIR" ]; then
        mkdir -p "$LOG_DIR"
        log "Diretório de logs criado: $LOG_DIR"
    fi
    
    for dir in "tmp" "cache" "reports"; do
        if [ ! -d "${SERVIDOR_ROOT}/${dir}" ]; then
            mkdir -p "${SERVIDOR_ROOT}/${dir}"
            log "Diretório ${dir} criado: ${SERVIDOR_ROOT}/${dir}"
        fi
    done
}

# Função para verificar os servidores ativos
verificar_servidores_ativos() {
    log "Verificando servidores ativos..." "DEBUG"
    
    local servidores_ativos=()
    
    for arquivo_pid in "${PID_DIR}"/*.pid; do
        if [ -f "$arquivo_pid" ]; then
            local nome_servidor=$(basename "$arquivo_pid" .pid)
            local pid=$(cat "$arquivo_pid")
            
            if ps -p $pid > /dev/null; then
                servidores_ativos+=("$nome_servidor")
                log "Servidor ativo: $nome_servidor (PID: $pid)" "DEBUG"
            else
                log "PID obsoleto encontrado para $nome_servidor. Removendo arquivo PID." "WARN"
                rm -f "$arquivo_pid"
            fi
        fi
    done
    
    log "Servidores ativos: ${servidores_ativos[*]}"
    echo "${servidores_ativos[@]}"
}

# Função para parar todos os servidores
parar_servidores() {
    log "Parando todos os servidores para manutenção..."
    
    # Usa o script de shutdown
    if [ -f "${SERVIDOR_ROOT}/scripts/shutdown.sh" ]; then
        log "Executando script de shutdown..."
        "${SERVIDOR_ROOT}/scripts/shutdown.sh" --force
        sleep 5
        
        # Verifica se ainda há servidores ativos
        local servidores_restantes=($(verificar_servidores_ativos))
        if [ ${#servidores_restantes[@]} -gt 0 ]; then
            log "AVISO: Alguns servidores ainda estão em execução: ${servidores_restantes[*]}" "WARN"
            
            # Tenta forçar o encerramento
            for servidor in "${servidores_restantes[@]}"; do
                log "Forçando encerramento de $servidor..." "WARN"
                pkill -9 -f "$servidor"
            done
        fi
    else
        log "Script de shutdown não encontrado. Tentando parar servidores manualmente..." "WARN"
        
        # Para cada arquivo PID, mata o processo
        for arquivo_pid in "${PID_DIR}"/*.pid; do
            if [ -f "$arquivo_pid" ]; then
                local nome_servidor=$(basename "$arquivo_pid" .pid)
                local pid=$(cat "$arquivo_pid")
                
                log "Parando $nome_servidor (PID: $pid)..."
                kill -15 $pid 2>/dev/null
                sleep 2
                
                # Verifica se o processo ainda está em execução
                if ps -p $pid > /dev/null; then
                    log "Forçando encerramento de $nome_servidor..." "WARN"
                    kill -9 $pid 2>/dev/null
                fi
                
                rm -f "$arquivo_pid"
            fi
        done
    fi
    
    log "Todos os servidores foram parados"
}

# Função para iniciar todos os servidores
iniciar_servidores() {
    log "Iniciando todos os servidores após manutenção..."
    
    # Usa o script de startup
    if [ -f "${SERVIDOR_ROOT}/scripts/startup.sh" ]; then
        log "Executando script de startup..."
        "${SERVIDOR_ROOT}/scripts/startup.sh"
    else
        log "Script de startup não encontrado. Não é possível iniciar os servidores automaticamente." "ERROR"
        return 1
    fi
    
    # Verifica se os servidores iniciaram
    sleep 10
    local servidores_ativos=($(verificar_servidores_ativos))
    if [ ${#servidores_ativos[@]} -gt 0 ]; then
        log "Servidores iniciados com sucesso: ${servidores_ativos[*]}"
    else
        log "ERRO: Nenhum servidor parece ter iniciado." "ERROR"
        return 1
    fi
    
    return 0
}

# Função para limpar logs antigos
limpar_logs() {
    log "Iniciando limpeza de logs antigos (mais de $MAX_LOG_DAYS dias)..."
    
    local contador=0
    local tamanho_total=0
    
    # Encontra e remove logs antigos
    while IFS= read -r arquivo; do
        local tamanho=$(stat -c %s "$arquivo" 2>/dev/null)
        rm -f "$arquivo"
        tamanho_total=$((tamanho_total + tamanho))
        contador=$((contador + 1))
        
        if $VERBOSE; then
            log "Removido: $arquivo ($(numfmt --to=iec-i --suffix=B $tamanho))" "DEBUG"
        fi
    done < <(find "$LOG_DIR" -name "*.log" -type f -mtime +$MAX_LOG_DAYS)
    
    # Compacta logs mais recentes que não estão compactados
    local contador_compactados=0
    while IFS= read -r arquivo; do
        if [[ "$arquivo" != *.gz && "$arquivo" != *.zip ]]; then
            gzip -9 "$arquivo"
            contador_compactados=$((contador_compactados + 1))
            
            if $VERBOSE; then
                log "Compactado: $arquivo" "DEBUG"
            fi
        fi
    done < <(find "$LOG_DIR" -name "*.log" -type f -mtime +1 -mtime -$MAX_LOG_DAYS)
    
    tamanho_total_mb=$(echo "scale=2; $tamanho_total / 1048576" | bc)
    log "Limpeza de logs concluída. $contador arquivos removidos (${tamanho_total_mb}MB), $contador_compactados arquivos compactados."
}

# Função para limpar arquivos temporários
limpar_temporarios() {
    log "Iniciando limpeza de arquivos temporários (mais de $TEMP_FILES_AGE dias)..."
    
    local temp_dirs=("${SERVIDOR_ROOT}/tmp" "${SERVIDOR_ROOT}/cache")
    local contador=0
    local tamanho_total=0
    
    for dir in "${temp_dirs[@]}"; do
        if [ -d "$dir" ]; then
            while IFS= read -r arquivo; do
                local tamanho=$(stat -c %s "$arquivo" 2>/dev/null)
                rm -f "$arquivo"
                tamanho_total=$((tamanho_total + tamanho))
                contador=$((contador + 1))
                
                if $VERBOSE; then
                    log "Removido temporário: $arquivo ($(numfmt --to=iec-i --suffix=B $tamanho))" "DEBUG"
                fi
            done < <(find "$dir" -type f -mtime +$TEMP_FILES_AGE)
        fi
    done
    
    # Limpar cache de compilação
    if [ -d "${SERVIDOR_ROOT}/bin/obj" ]; then
        log "Limpando cache de compilação..."
        rm -rf "${SERVIDOR_ROOT}/bin/obj/"*
    fi
    
    tamanho_total_mb=$(echo "scale=2; $tamanho_total / 1048576" | bc)
    log "Limpeza de arquivos temporários concluída. $contador arquivos removidos (${tamanho_total_mb}MB)."
}

# Função para verificar uso de disco e espaço
verificar_espaco_disco() {
    log "Verificando uso de disco..."
    
    # Verifica espaço em cada partição
    df -h | grep -v "tmpfs" | while read -r linha; do
        log "Uso de disco: $linha"
    done
    
    # Verifica uso de espaço nos diretórios principais
    log "Analisando tamanho dos diretórios principais..."
    for dir in "bin" "data" "logs" "config"; do
        if [ -d "${SERVIDOR_ROOT}/${dir}" ]; then
            local tamanho=$(du -sh "${SERVIDOR_ROOT}/${dir}" 2>/dev/null | cut -f1)
            log "Diretório ${SERVIDOR_ROOT}/${dir}: $tamanho"
        fi
    done
    
    # Verifica se há alertas de espaço
    local uso_raiz=$(df -h / | awk 'NR==2 {print $5}' | sed 's/%//')
    if [ "$uso_raiz" -gt 90 ]; then
        log "ALERTA: Partição raiz com mais de 90% de uso ($uso_raiz%)!" "WARN"
    fi
}

# Função para otimizar banco de dados MySQL
otimizar_mysql() {
    log "Tentando otimizar banco de dados MySQL..."
    
    # Verifica se o MySQL está instalado
    if ! command -v mysql &> /dev/null; then
        log "MySQL não encontrado. Pulando otimização." "WARN"
        return
    fi
    
    # Lê credenciais do arquivo de configuração
    local mysql_user="wydbr"
    local mysql_password="wyd_db_password"
    local mysql_host="localhost"
    local mysql_db="wydbr"
    
    # Tenta ler configurações do arquivo, se existir
    if [ -f "${CONFIG_DIR}/DBSrv.conf" ]; then
        log "Lendo configurações de banco de dados de ${CONFIG_DIR}/DBSrv.conf..."
        source <(grep -E "^DB_USER|^DB_PASSWORD|^DB_HOST|^DB_NAME" "${CONFIG_DIR}/DBSrv.conf")
        [ -n "$DB_USER" ] && mysql_user="$DB_USER"
        [ -n "$DB_PASSWORD" ] && mysql_password="$DB_PASSWORD"
        [ -n "$DB_HOST" ] && mysql_host="$DB_HOST"
        [ -n "$DB_NAME" ] && mysql_db="$DB_NAME"
    fi
    
    # Verifica conexão com o banco
    if ! mysql -u"$mysql_user" -p"$mysql_password" -h"$mysql_host" -e "SELECT 1" &>/dev/null; then
        log "Não foi possível conectar ao MySQL. Verifique as credenciais." "ERROR"
        return 1
    fi
    
    # Realiza vacuum se habilitado
    if $VACUUM_DB; then
        log "Realizando VACUUM nas tabelas..."
        mysql -u"$mysql_user" -p"$mysql_password" -h"$mysql_host" "$mysql_db" -e "
            SELECT CONCAT('VACUUM TABLE ', table_schema, '.', table_name, ';') 
            FROM information_schema.tables 
            WHERE table_schema = '$mysql_db';" | 
            grep -v "CONCAT" | 
            while read -r query; do
                log "Executando: $query" "DEBUG"
                mysql -u"$mysql_user" -p"$mysql_password" -h"$mysql_host" "$mysql_db" -e "$query"
            done
    fi
    
    # Realiza otimização de tabelas se habilitado
    if $OPTIMIZE_TABLES; then
        log "Otimizando tabelas MySQL..."
        mysql -u"$mysql_user" -p"$mysql_password" -h"$mysql_host" "$mysql_db" -e "
            SELECT CONCAT('OPTIMIZE TABLE ', table_schema, '.', table_name, ';') 
            FROM information_schema.tables 
            WHERE table_schema = '$mysql_db';" | 
            grep -v "CONCAT" | 
            while read -r query; do
                log "Executando: $query" "DEBUG"
                mysql -u"$mysql_user" -p"$mysql_password" -h"$mysql_host" "$mysql_db" -e "$query"
            done
    fi
    
    # Verifica integridade se habilitado
    if $CHECK_INTEGRITY; then
        log "Verificando integridade das tabelas MySQL..."
        mysql -u"$mysql_user" -p"$mysql_password" -h"$mysql_host" "$mysql_db" -e "
            SELECT CONCAT('CHECK TABLE ', table_schema, '.', table_name, ';') 
            FROM information_schema.tables 
            WHERE table_schema = '$mysql_db';" | 
            grep -v "CONCAT" | 
            while read -r query; do
                log "Executando: $query" "DEBUG"
                mysql -u"$mysql_user" -p"$mysql_password" -h"$mysql_host" "$mysql_db" -e "$query" > "${LOG_DIR}/mysql_check_${TIMESTAMP}.log"
            done
    fi
    
    log "Otimização MySQL concluída."
}

# Função para otimizar banco de dados PostgreSQL
otimizar_postgresql() {
    log "Tentando otimizar banco de dados PostgreSQL..."
    
    # Verifica se o PostgreSQL está instalado
    if ! command -v psql &> /dev/null; then
        log "PostgreSQL não encontrado. Pulando otimização." "WARN"
        return
    fi
    
    # Lê credenciais do arquivo de configuração
    local pg_user="wydbr"
    local pg_password="wyd_db_password"
    local pg_host="localhost"
    local pg_db="wydbr"
    
    # Tenta ler configurações do arquivo, se existir
    if [ -f "${CONFIG_DIR}/DBSrv.conf" ]; then
        log "Lendo configurações de banco de dados de ${CONFIG_DIR}/DBSrv.conf..."
        source <(grep -E "^DB_USER|^DB_PASSWORD|^DB_HOST|^DB_NAME" "${CONFIG_DIR}/DBSrv.conf")
        [ -n "$DB_USER" ] && pg_user="$DB_USER"
        [ -n "$DB_PASSWORD" ] && pg_password="$DB_PASSWORD"
        [ -n "$DB_HOST" ] && pg_host="$DB_HOST"
        [ -n "$DB_NAME" ] && pg_db="$DB_NAME"
    fi
    
    # Verifica conexão com o banco
    export PGPASSWORD="$pg_password"
    if ! psql -U "$pg_user" -h "$pg_host" -d "$pg_db" -c "SELECT 1" &>/dev/null; then
        log "Não foi possível conectar ao PostgreSQL. Verifique as credenciais." "ERROR"
        return 1
    fi
    
    # Realiza vacuum se habilitado
    if $VACUUM_DB; then
        log "Realizando VACUUM FULL no PostgreSQL..."
        psql -U "$pg_user" -h "$pg_host" -d "$pg_db" -c "VACUUM FULL;"
    fi
    
    # Realiza análise se habilitado
    if $OPTIMIZE_TABLES; then
        log "Analisando tabelas PostgreSQL..."
        psql -U "$pg_user" -h "$pg_host" -d "$pg_db" -c "ANALYZE;"
    fi
    
    # Verifica integridade se habilitado
    if $CHECK_INTEGRITY; then
        log "Verificando integridade do PostgreSQL..."
        psql -U "$pg_user" -h "$pg_host" -d "$pg_db" -c "SELECT * FROM pg_stat_database WHERE datname = '$pg_db';" > "${LOG_DIR}/pg_check_${TIMESTAMP}.log"
    fi
    
    log "Otimização PostgreSQL concluída."
}

# Função para verificar integridade do sistema de arquivos
verificar_arquivos() {
    log "Verificando integridade do sistema de arquivos..."
    
    # Verifica permissões dos arquivos executáveis
    log "Verificando permissões de arquivos executáveis..."
    find "${SERVIDOR_ROOT}/bin" -type f -executable -exec ls -la {} \; > "${LOG_DIR}/executables_permissions_${TIMESTAMP}.log"
    
    # Procura por arquivos corrompidos (tamanho zero onde não deveria)
    log "Procurando por arquivos corrompidos..."
    find "${SERVIDOR_ROOT}/data" -type f -size 0 -exec ls -la {} \; > "${LOG_DIR}/empty_files_${TIMESTAMP}.log"
    
    # Verifica arquivos de configuração
    log "Verificando arquivos de configuração..."
    for arquivo in "${CONFIG_DIR}"/*.conf; do
        if [ -f "$arquivo" ]; then
            if grep -q "ERROR\|FAILED\|CORRUPTED" "$arquivo"; then
                log "Possível problema no arquivo de configuração: $arquivo" "WARN"
            fi
        fi
    done
}

# Função para reparar permissões
reparar_permissoes() {
    log "Reparando permissões de diretórios e arquivos..."
    
    # Define permissões padrão
    find "${SERVIDOR_ROOT}/bin" -type f -exec chmod 755 {} \;
    find "${SERVIDOR_ROOT}/bin" -type d -exec chmod 755 {} \;
    
    find "${SERVIDOR_ROOT}/scripts" -type f -exec chmod 755 {} \;
    find "${SERVIDOR_ROOT}/scripts" -type d -exec chmod 755 {} \;
    
    find "${SERVIDOR_ROOT}/config" -type f -exec chmod 644 {} \;
    find "${SERVIDOR_ROOT}/config" -type d -exec chmod 755 {} \;
    
    find "${SERVIDOR_ROOT}/data" -type d -exec chmod 755 {} \;
    find "${SERVIDOR_ROOT}/data" -type f -exec chmod 644 {} \;
    
    log "Permissões reparadas"
}

# Função para gerar relatório de manutenção
gerar_relatorio() {
    log "Gerando relatório de manutenção..."
    local relatorio="${LOG_DIR}/maintenance_report_${TIMESTAMP}.html"
    
    cat > "$relatorio" << EOF
<!DOCTYPE html>
<html>
<head>
    <title>Relatório de Manutenção WYDBR - $(date +"%Y-%m-%d")</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; }
        h1 { color: #333366; }
        h2 { color: #336699; margin-top: 20px; }
        .success { color: green; }
        .warning { color: orange; }
        .error { color: red; }
        table { border-collapse: collapse; width: 100%; margin-top: 10px; }
        th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }
        th { background-color: #f2f2f2; }
    </style>
</head>
<body>
    <h1>Relatório de Manutenção WYDBR</h1>
    <p>Data: $(date +"%Y-%m-%d %H:%M:%S")</p>
    
    <h2>Resumo das Atividades</h2>
    <ul>
        <li>Limpeza de Logs: <span class="success">Concluída</span></li>
        <li>Limpeza de Arquivos Temporários: <span class="success">Concluída</span></li>
        <li>Verificação de Espaço em Disco: <span class="success">Concluída</span></li>
EOF
    
    if $VACUUM_DB || $OPTIMIZE_TABLES || $CHECK_INTEGRITY; then
        cat >> "$relatorio" << EOF
        <li>Otimização de Banco de Dados: <span class="success">Concluída</span></li>
EOF
    fi
    
    if $CHECK_INTEGRITY; then
        cat >> "$relatorio" << EOF
        <li>Verificação de Integridade: <span class="success">Concluída</span></li>
EOF
    fi
    
    cat >> "$relatorio" << EOF
    </ul>
    
    <h2>Uso de Disco</h2>
    <pre>$(df -h)</pre>
    
    <h2>Diretórios Principais</h2>
    <table>
        <tr>
            <th>Diretório</th>
            <th>Tamanho</th>
        </tr>
EOF
    
    for dir in "bin" "data" "logs" "config"; do
        if [ -d "${SERVIDOR_ROOT}/${dir}" ]; then
            local tamanho=$(du -sh "${SERVIDOR_ROOT}/${dir}" 2>/dev/null | cut -f1)
            echo "<tr><td>${SERVIDOR_ROOT}/${dir}</td><td>$tamanho</td></tr>" >> "$relatorio"
        fi
    done
    
    cat >> "$relatorio" << EOF
    </table>
    
    <h2>Banco de Dados</h2>
    <p>Último backup: $(ls -lt "${SERVIDOR_ROOT}/backups/database/" | head -2 | tail -1)</p>
    
    <h2>Status do Sistema</h2>
    <pre>$(uptime)</pre>
    <pre>$(free -h)</pre>
    
    <p>Relatório gerado automaticamente pelo script de manutenção do servidor WYDBR.</p>
</body>
</html>
EOF
    
    log "Relatório HTML gerado: $relatorio"
}

# Função principal
main() {
    # Inicialização
    verificar_diretorios
    
    log "=== Iniciando manutenção do servidor WYDBR ==="
    log "Versão do Sistema: $(uname -a)"
    
    # Processa parâmetros da linha de comando
    while [[ $# -gt 0 ]]; do
        key="$1"
        case $key in
            --log-days=*)
                MAX_LOG_DAYS="${key#*=}"
                shift
                ;;
            --temp-days=*)
                TEMP_FILES_AGE="${key#*=}"
                shift
                ;;
            --restart)
                SERVER_RESTART=true
                shift
                ;;
            --no-vacuum)
                VACUUM_DB=false
                shift
                ;;
            --no-integrity)
                CHECK_INTEGRITY=false
                shift
                ;;
            --no-optimize)
                OPTIMIZE_TABLES=false
                shift
                ;;
            --verbose|-v)
                VERBOSE=true
                shift
                ;;
            --offline)
                OFFLINE_MODE=true
                shift
                ;;
            --help|-h)
                echo "Uso: $0 [opções]"
                echo "Opções:"
                echo "  --log-days=N        Define dias para manter logs (padrão: $MAX_LOG_DAYS)"
                echo "  --temp-days=N       Define dias para manter arquivos temporários (padrão: $TEMP_FILES_AGE)"
                echo "  --restart           Reinicia servidores após manutenção"
                echo "  --no-vacuum         Desativa vacuum no banco de dados"
                echo "  --no-integrity      Desativa verificação de integridade"
                echo "  --no-optimize       Desativa otimização de tabelas"
                echo "  --verbose, -v       Modo verboso"
                echo "  --offline           Modo offline (desliga servidores durante manutenção)"
                exit 0
                ;;
            *)
                log "AVISO: Opção desconhecida: $key" "WARN"
                shift
                ;;
        esac
    done
    
    # Verificar servidores ativos
    local servidores_antes=($(verificar_servidores_ativos))
    
    # Se modo offline ativado, para os servidores
    local precisa_reiniciar=false
    if $OFFLINE_MODE && [ ${#servidores_antes[@]} -gt 0 ]; then
        parar_servidores
        precisa_reiniciar=true
    fi
    
    # Executar tarefas de manutenção
    log "Iniciando tarefas de manutenção..."
    
    # Limpeza de logs
    limpar_logs
    
    # Limpeza de arquivos temporários
    limpar_temporarios
    
    # Verificação de espaço em disco
    verificar_espaco_disco
    
    # Se tiver banco de dados MySQL
    otimizar_mysql
    
    # Se tiver banco de dados PostgreSQL
    otimizar_postgresql
    
    # Verificação de integridade de arquivos
    verificar_arquivos
    
    # Reparo de permissões
    reparar_permissoes
    
    # Gera relatório
    gerar_relatorio
    
    # Reinicia servidores se necessário
    if ( $precisa_reiniciar || $SERVER_RESTART ) && [ ${#servidores_antes[@]} -gt 0 ]; then
        log "Reiniciando servidores..."
        iniciar_servidores
        
        # Verifica se os servidores foram reiniciados corretamente
        local servidores_depois=($(verificar_servidores_ativos))
        if [ ${#servidores_depois[@]} -lt ${#servidores_antes[@]} ]; then
            log "ALERTA: Nem todos os servidores foram reiniciados corretamente!" "WARN"
            log "Servidores antes: ${servidores_antes[*]}"
            log "Servidores depois: ${servidores_depois[*]}"
        else
            log "Todos os servidores foram reiniciados corretamente"
        fi
    fi
    
    log "=== Manutenção do servidor WYDBR concluída ==="
}

# Executa a função principal
main "$@"

exit 0 