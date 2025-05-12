#!/bin/bash

# Script de backup do servidor WYDBR
# Realiza backups incrementais e completos com rotação e compressão inteligente

# Configurações
SERVIDOR_ROOT="/opt/WYDBR/server"
LOG_DIR="${SERVIDOR_ROOT}/logs"
TIMESTAMP=$(date +"%Y%m%d_%H%M%S")
LOG_FILE="${LOG_DIR}/backup_${TIMESTAMP}.log"
BACKUP_ROOT="/opt/WYDBR/backups"
DB_BACKUP_DIR="${BACKUP_ROOT}/database"
FILES_BACKUP_DIR="${BACKUP_ROOT}/files"
SCRIPTS_BACKUP_DIR="${BACKUP_ROOT}/scripts"
CONFIG_BACKUP_DIR="${BACKUP_ROOT}/configs"

# Configurações de retenção
DAILY_RETENTION=7       # Dias para manter backups diários
WEEKLY_RETENTION=4      # Semanas para manter backups semanais
MONTHLY_RETENTION=6     # Meses para manter backups mensais

# Opções de compressão
COMPRESSION_LEVEL=9     # Nível de compressão (1-9)
COMPRESSION_PROGRAM="pigz"  # Usar pigz para compressão paralela se disponível
FALLBACK_COMPRESSION="gzip"

# Função para registrar mensagens no log
log() {
    local mensagem="$1"
    echo "[$(date +"%Y-%m-%d %H:%M:%S")] $mensagem" | tee -a "$LOG_FILE"
}

# Função para verificar e criar diretórios
verificar_diretorios() {
    log "Verificando e criando diretórios de backup..."
    
    for dir in "$LOG_DIR" "$BACKUP_ROOT" "$DB_BACKUP_DIR" "$FILES_BACKUP_DIR" "$SCRIPTS_BACKUP_DIR" "$CONFIG_BACKUP_DIR"; do
        if [ ! -d "$dir" ]; then
            mkdir -p "$dir"
            log "Diretório criado: $dir"
        fi
    done

    # Cria diretórios para backups diários, semanais e mensais
    for tipo in "diario" "semanal" "mensal"; do
        if [ ! -d "${DB_BACKUP_DIR}/${tipo}" ]; then
            mkdir -p "${DB_BACKUP_DIR}/${tipo}"
            log "Diretório criado: ${DB_BACKUP_DIR}/${tipo}"
        fi
        if [ ! -d "${FILES_BACKUP_DIR}/${tipo}" ]; then
            mkdir -p "${FILES_BACKUP_DIR}/${tipo}"
            log "Diretório criado: ${FILES_BACKUP_DIR}/${tipo}"
        fi
    done
}

# Função para verificar dependências do sistema
verificar_dependencias() {
    log "Verificando dependências do sistema para backup..."
    
    # Lista de programas necessários
    local DEPS=("rsync" "mysqldump" "tar" "$COMPRESSION_PROGRAM")
    local MISSING_DEPS=()
    
    for dep in "${DEPS[@]}"; do
        if ! command -v "$dep" &> /dev/null; then
            if [ "$dep" == "$COMPRESSION_PROGRAM" ]; then
                log "AVISO: $COMPRESSION_PROGRAM não encontrado, usando $FALLBACK_COMPRESSION como alternativa"
                COMPRESSION_PROGRAM="$FALLBACK_COMPRESSION"
            else
                MISSING_DEPS+=("$dep")
                log "ERRO: Dependência necessária não encontrada: $dep"
            fi
        fi
    done
    
    if [ ${#MISSING_DEPS[@]} -gt 0 ]; then
        log "ERRO: Faltam dependências necessárias para executar o backup. Por favor, instale: ${MISSING_DEPS[*]}"
        return 1
    fi
    
    return 0
}

# Função para determinar o tipo de backup (diário, semanal ou mensal)
determinar_tipo_backup() {
    local dia_semana=$(date +"%u")  # 1-7 (segunda-domingo)
    local dia_mes=$(date +"%d")     # 01-31
    
    if [ "$dia_semana" -eq 7 ]; then
        # Domingo = backup semanal
        echo "semanal"
    elif [ "$dia_mes" -eq 1 ]; then
        # Primeiro dia do mês = backup mensal
        echo "mensal"
    else
        # Outros dias = backup diário
        echo "diario"
    fi
}

# Função para criar backup da base de dados
backup_database() {
    local tipo=$1
    local backup_dir="${DB_BACKUP_DIR}/${tipo}"
    local backup_file="${backup_dir}/db_backup_${TIMESTAMP}.sql.gz"
    
    log "Iniciando backup da base de dados (tipo: $tipo)..."
    
    # Verifica se o servidor de banco de dados está online
    if ! nc -z localhost 3306; then
        log "AVISO: Servidor MySQL não está acessível. Verificando PostgreSQL..."
        if ! nc -z localhost 5432; then
            log "ERRO: Nenhum servidor de banco de dados está acessível"
            return 1
        else
            log "Usando PostgreSQL para backup"
            PGPASSWORD="wyd_db_password" pg_dump -h localhost -U wydbr -d wydbr | $COMPRESSION_PROGRAM -$COMPRESSION_LEVEL > "$backup_file"
        fi
    else
        log "Usando MySQL/MariaDB para backup"
        mysqldump -h localhost -u wydbr -p"wyd_db_password" --all-databases | $COMPRESSION_PROGRAM -$COMPRESSION_LEVEL > "$backup_file"
    fi
    
    if [ $? -ne 0 ]; then
        log "ERRO: Falha ao realizar backup do banco de dados"
        return 1
    fi
    
    log "Backup de banco de dados concluído: $backup_file ($(du -h "$backup_file" | cut -f1))"
    return 0
}

# Função para criar backup de arquivos
backup_files() {
    local tipo=$1
    local backup_dir="${FILES_BACKUP_DIR}/${tipo}"
    local backup_file="${backup_dir}/files_backup_${TIMESTAMP}.tar.gz"
    
    log "Iniciando backup de arquivos (tipo: $tipo)..."
    
    # Define a lista de diretórios a serem incluídos e excluídos
    local INCLUDE_DIRS=("${SERVIDOR_ROOT}/data" "${SERVIDOR_ROOT}/config" "${SERVIDOR_ROOT}/scripts")
    local EXCLUDE_PATTERNS=("*.tmp" "*.log" "*.bak" "tmp/*" "cache/*")
    
    # Constrói a string de exclusão
    local exclude_str=""
    for pattern in "${EXCLUDE_PATTERNS[@]}"; do
        exclude_str="$exclude_str --exclude='$pattern'"
    done
    
    # Se for backup incremental (diário), verifica se existe um backup completo anterior
    if [ "$tipo" == "diario" ] && ls "${FILES_BACKUP_DIR}/mensal/"*.tar.gz >/dev/null 2>&1; then
        local ultimo_completo=$(ls -t "${FILES_BACKUP_DIR}/mensal/"*.tar.gz | head -1)
        log "Usando backup completo como referência: $(basename "$ultimo_completo")"
        
        # Realiza backup incremental baseado no último backup completo
        tar --listed-incremental="${backup_dir}/snapshot.snar" $exclude_str -czf "$backup_file" "${INCLUDE_DIRS[@]}"
    else
        # Realiza backup completo
        tar $exclude_str -czf "$backup_file" "${INCLUDE_DIRS[@]}"
    fi
    
    if [ $? -ne 0 ]; then
        log "ERRO: Falha ao realizar backup de arquivos"
        return 1
    fi
    
    log "Backup de arquivos concluído: $backup_file ($(du -h "$backup_file" | cut -f1))"
    return 0
}

# Função para backup de configurações
backup_configs() {
    local backup_file="${CONFIG_BACKUP_DIR}/configs_${TIMESTAMP}.tar.gz"
    
    log "Iniciando backup de configurações..."
    
    # Cria um arquivo tar com todos os arquivos de configuração
    tar -czf "$backup_file" -C "${SERVIDOR_ROOT}" config
    
    if [ $? -ne 0 ]; then
        log "ERRO: Falha ao realizar backup das configurações"
        return 1
    fi
    
    log "Backup de configurações concluído: $backup_file"
    return 0
}

# Função para backup de scripts
backup_scripts() {
    local backup_file="${SCRIPTS_BACKUP_DIR}/scripts_${TIMESTAMP}.tar.gz"
    
    log "Iniciando backup de scripts..."
    
    # Cria um arquivo tar com todos os scripts
    tar -czf "$backup_file" -C "${SERVIDOR_ROOT}" scripts
    
    if [ $? -ne 0 ]; then
        log "ERRO: Falha ao realizar backup dos scripts"
        return 1
    fi
    
    log "Backup de scripts concluído: $backup_file"
    return 0
}

# Função para limpar backups antigos
limpar_backups_antigos() {
    log "Iniciando limpeza de backups antigos..."
    
    # Remove backups diários antigos
    log "Removendo backups diários com mais de $DAILY_RETENTION dias..."
    find "${DB_BACKUP_DIR}/diario" -name "*.sql.gz" -type f -mtime +$DAILY_RETENTION -delete
    find "${FILES_BACKUP_DIR}/diario" -name "*.tar.gz" -type f -mtime +$DAILY_RETENTION -delete
    
    # Remove backups semanais antigos
    # Aqui multiplicamos por 7 para converter semanas em dias
    log "Removendo backups semanais com mais de $WEEKLY_RETENTION semanas..."
    local weekly_days=$((WEEKLY_RETENTION * 7))
    find "${DB_BACKUP_DIR}/semanal" -name "*.sql.gz" -type f -mtime +$weekly_days -delete
    find "${FILES_BACKUP_DIR}/semanal" -name "*.tar.gz" -type f -mtime +$weekly_days -delete
    
    # Remove backups mensais antigos
    # Aqui multiplicamos por 30 para converter meses em dias (aproximadamente)
    log "Removendo backups mensais com mais de $MONTHLY_RETENTION meses..."
    local monthly_days=$((MONTHLY_RETENTION * 30))
    find "${DB_BACKUP_DIR}/mensal" -name "*.sql.gz" -type f -mtime +$monthly_days -delete
    find "${FILES_BACKUP_DIR}/mensal" -name "*.tar.gz" -type f -mtime +$monthly_days -delete
    
    log "Limpeza de backups antigos concluída"
}

# Função para verificar integridade dos backups
verificar_backups() {
    log "Verificando integridade dos backups recentes..."
    local falhas=0
    
    # Verifica o último backup de banco de dados
    for tipo in "diario" "semanal" "mensal"; do
        if ls "${DB_BACKUP_DIR}/${tipo}/"*".sql.gz" > /dev/null 2>&1; then
            local ultimo_db=$(ls -t "${DB_BACKUP_DIR}/${tipo}/"*".sql.gz" | head -1)
            log "Verificando backup de banco de dados: $(basename "$ultimo_db")"
            
            # Testa o arquivo comprimido
            $COMPRESSION_PROGRAM -t "$ultimo_db" > /dev/null 2>&1
            if [ $? -ne 0 ]; then
                log "ERRO: Backup de banco de dados corrompido: $ultimo_db"
                falhas=$((falhas+1))
            fi
        fi
    done
    
    # Verifica o último backup de arquivos
    for tipo in "diario" "semanal" "mensal"; do
        if ls "${FILES_BACKUP_DIR}/${tipo}/"*".tar.gz" > /dev/null 2>&1; then
            local ultimo_files=$(ls -t "${FILES_BACKUP_DIR}/${tipo}/"*".tar.gz" | head -1)
            log "Verificando backup de arquivos: $(basename "$ultimo_files")"
            
            # Testa o arquivo tar.gz
            tar -tzf "$ultimo_files" > /dev/null 2>&1
            if [ $? -ne 0 ]; then
                log "ERRO: Backup de arquivos corrompido: $ultimo_files"
                falhas=$((falhas+1))
            fi
        fi
    done
    
    if [ $falhas -eq 0 ]; then
        log "Todos os backups recentes passaram na verificação de integridade"
        return 0
    else
        log "AVISO: $falhas backups falharam na verificação de integridade"
        return 1
    fi
}

# Função para gerar relatório de backup
gerar_relatorio() {
    local backup_status=$1
    local tipo_backup=$2
    local relatorio="${LOG_DIR}/backup_report_${TIMESTAMP}.txt"
    
    log "Gerando relatório de backup..."
    
    echo "=======================================================" > "$relatorio"
    echo "  RELATÓRIO DE BACKUP DO SERVIDOR WYDBR" >> "$relatorio"
    echo "  Data: $(date +"%Y-%m-%d %H:%M:%S")" >> "$relatorio"
    echo "  Tipo: $tipo_backup" >> "$relatorio"
    echo "=======================================================" >> "$relatorio"
    echo "" >> "$relatorio"
    
    echo "RESUMO:" >> "$relatorio"
    if [ $backup_status -eq 0 ]; then
        echo "  Status: SUCESSO" >> "$relatorio"
    else
        echo "  Status: FALHA" >> "$relatorio"
    fi
    
    echo "" >> "$relatorio"
    echo "DETALHES DOS BACKUPS:" >> "$relatorio"
    
    # Adiciona informações sobre os backups de BD
    echo "  Database Backups:" >> "$relatorio"
    if [ -f "${DB_BACKUP_DIR}/${tipo_backup}/db_backup_${TIMESTAMP}.sql.gz" ]; then
        local db_size=$(du -h "${DB_BACKUP_DIR}/${tipo_backup}/db_backup_${TIMESTAMP}.sql.gz" | cut -f1)
        echo "    - ${DB_BACKUP_DIR}/${tipo_backup}/db_backup_${TIMESTAMP}.sql.gz ($db_size)" >> "$relatorio"
    else
        echo "    - Nenhum backup de banco de dados gerado" >> "$relatorio"
    fi
    
    # Adiciona informações sobre os backups de arquivos
    echo "  File Backups:" >> "$relatorio"
    if [ -f "${FILES_BACKUP_DIR}/${tipo_backup}/files_backup_${TIMESTAMP}.tar.gz" ]; then
        local files_size=$(du -h "${FILES_BACKUP_DIR}/${tipo_backup}/files_backup_${TIMESTAMP}.tar.gz" | cut -f1)
        echo "    - ${FILES_BACKUP_DIR}/${tipo_backup}/files_backup_${TIMESTAMP}.tar.gz ($files_size)" >> "$relatorio"
    else
        echo "    - Nenhum backup de arquivos gerado" >> "$relatorio"
    fi
    
    # Adiciona informações sobre espaço em disco
    echo "" >> "$relatorio"
    echo "ESPAÇO EM DISCO:" >> "$relatorio"
    df -h | grep -v "tmpfs\|udev" >> "$relatorio"
    
    # Adiciona histórico de backups
    echo "" >> "$relatorio"
    echo "HISTÓRICO DE BACKUPS RECENTES:" >> "$relatorio"
    echo "  Diários:" >> "$relatorio"
    ls -la "${DB_BACKUP_DIR}/diario" | head -10 >> "$relatorio"
    
    echo "" >> "$relatorio"
    echo "=======================================================" >> "$relatorio"
    
    log "Relatório de backup gerado: $relatorio"
    
    # Envia o relatório por email se configurado
    if [ -n "$EMAIL_NOTIFY" ]; then
        log "Enviando relatório por email para $EMAIL_NOTIFY..."
        cat "$relatorio" | mail -s "Relatório de Backup WYDBR - $(date +"%Y-%m-%d")" "$EMAIL_NOTIFY"
    fi
}

# Função para armazenar backup externo (se configurado)
backup_externo() {
    local backup_status=$1
    
    # Verifica se o backup remoto está configurado
    if [ -z "$REMOTE_BACKUP_PATH" ]; then
        log "Backup remoto não configurado"
        return 0
    fi
    
    log "Iniciando cópia de backup para destino externo: $REMOTE_BACKUP_PATH"
    
    # Verifica se o destino é acessível
    if [[ $REMOTE_BACKUP_PATH == *":"* ]]; then
        # Destino é via SSH
        local remote_host=$(echo $REMOTE_BACKUP_PATH | cut -d: -f1)
        
        if ! ping -c 1 $remote_host &>/dev/null; then
            log "ERRO: Host remoto não acessível: $remote_host"
            return 1
        fi
        
        # Copia os backups mais recentes para o destino remoto
        rsync -avz --progress "${BACKUP_ROOT}/" "${REMOTE_BACKUP_PATH}/"
    else
        # Destino é um caminho local (possivelmente um disco externo)
        if [ ! -d "$REMOTE_BACKUP_PATH" ]; then
            log "ERRO: Caminho de backup externo não existe: $REMOTE_BACKUP_PATH"
            return 1
        fi
        
        # Copia os backups mais recentes para o destino local
        rsync -av --progress "${BACKUP_ROOT}/" "${REMOTE_BACKUP_PATH}/"
    fi
    
    if [ $? -ne 0 ]; then
        log "ERRO: Falha ao copiar para destino externo"
        return 1
    fi
    
    log "Backup externo concluído com sucesso"
    return 0
}

# Função principal
main() {
    # Inicializa log
    if [ ! -d "$LOG_DIR" ]; then
        mkdir -p "$LOG_DIR"
    fi
    
    log "=== Iniciando script de backup do WYDBR ==="
    log "Versão do Sistema: $(uname -a)"
    
    # Processa parâmetros da linha de comando
    TIPO_BACKUP=""
    EMAIL_NOTIFY=""
    REMOTE_BACKUP_PATH=""
    
    while [[ $# -gt 0 ]]; do
        key="$1"
        case $key in
            --type=*|--tipo=*)
                TIPO_BACKUP="${key#*=}"
                shift
                ;;
            --email=*)
                EMAIL_NOTIFY="${key#*=}"
                shift
                ;;
            --remote=*|--remoto=*)
                REMOTE_BACKUP_PATH="${key#*=}"
                shift
                ;;
            --help|-h)
                echo "Uso: $0 [opções]"
                echo "Opções:"
                echo "  --tipo=TIPO    Especifica o tipo de backup (diario, semanal, mensal)"
                echo "  --email=EMAIL  Envia relatório para o email especificado"
                echo "  --remoto=PATH  Define um destino remoto para cópia de segurança"
                exit 0
                ;;
            *)
                log "AVISO: Opção desconhecida: $key"
                shift
                ;;
        esac
    done
    
    # Se o tipo não for especificado, determina automaticamente
    if [ -z "$TIPO_BACKUP" ]; then
        TIPO_BACKUP=$(determinar_tipo_backup)
    fi
    
    log "Tipo de backup: $TIPO_BACKUP"
    
    # Verifica diretórios
    verificar_diretorios
    
    # Verifica dependências
    verificar_dependencias
    if [ $? -ne 0 ]; then
        log "ERRO: Dependências necessárias não encontradas, abortando backup"
        exit 1
    fi
    
    # Executa os backups
    backup_status=0
    
    # Backup do banco de dados
    backup_database "$TIPO_BACKUP"
    if [ $? -ne 0 ]; then backup_status=1; fi
    
    # Backup de arquivos
    backup_files "$TIPO_BACKUP"
    if [ $? -ne 0 ]; then backup_status=1; fi
    
    # Backup de configurações (sempre)
    backup_configs
    if [ $? -ne 0 ]; then backup_status=1; fi
    
    # Backup de scripts (sempre)
    backup_scripts
    if [ $? -ne 0 ]; then backup_status=1; fi
    
    # Limpa backups antigos
    limpar_backups_antigos
    
    # Verifica integridade dos backups
    verificar_backups
    if [ $? -ne 0 ]; then backup_status=1; fi
    
    # Gera relatório
    gerar_relatorio $backup_status "$TIPO_BACKUP"
    
    # Copia para backup externo se configurado
    if [ -n "$REMOTE_BACKUP_PATH" ]; then
        backup_externo $backup_status
    fi
    
    # Finaliza
    if [ $backup_status -eq 0 ]; then
        log "=== Backup concluído com sucesso ==="
        exit 0
    else
        log "=== Backup concluído com erros ==="
        exit 1
    fi
}

# Executa a função principal
main "$@" 