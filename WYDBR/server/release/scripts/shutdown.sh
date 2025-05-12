#!/bin/bash
#
# Script de encerramento do servidor WYDBR
# Encerra todos os componentes do servidor em sequência adequada
#

# Configurações
SERVIDOR_ROOT="/opt/WYDBR/server"
LOG_DIR="${SERVIDOR_ROOT}/logs"
TIMESTAMP=$(date +"%Y%m%d_%H%M%S")
LOG_FILE="${LOG_DIR}/shutdown_${TIMESTAMP}.log"
PID_DIR="${SERVIDOR_ROOT}/pids"

# Lista de servidores na ordem reversa de encerramento (oposta à ordem de inicialização)
SERVERS=("AgeSrv" "BISrv" "NRSrv" "WSKSrv" "TMSrv" "DBSrv")

# Tempo de espera em segundos para encerramento normal antes de forçar
GRACEFUL_TIMEOUT=30

# Cores para saída
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m' # Sem cor

# Função para registrar mensagens no log
log() {
    local mensagem="$1"
    echo "[$(date +"%Y-%m-%d %H:%M:%S")] $mensagem" | tee -a "$LOG_FILE"
}

# Função para verificar se o diretório de log existe, senão o cria
verificar_log_dir() {
    if [ ! -d "$LOG_DIR" ]; then
        mkdir -p "$LOG_DIR"
        log "Diretório de logs criado em $LOG_DIR"
    fi
}

# Função para enviar sinal de salvamento de dados antes do encerramento
salvar_dados() {
    local servidor=$1
    local pid=$2
    
    log "Enviando sinal de salvamento para $servidor (PID: $pid)..."
    
    # Envia SIGUSR1 que deve ser capturado pelo servidor para salvar dados
    if kill -SIGUSR1 $pid 2>/dev/null; then
        log "Sinal de salvamento enviado para $servidor"
        # Aguarda um momento para o servidor salvar os dados
        sleep 5
        return 0
    else
        log "Falha ao enviar sinal de salvamento para $servidor"
        return 1
    fi
}

# Função para encerrar um servidor com espera por encerramento normal
encerrar_servidor() {
    local servidor=$1
    local pid_file="${PID_DIR}/${servidor}.pid"
    local modo=$2  # "normal" ou "forçado"
    
    # Verifica se o arquivo PID existe
    if [ ! -f "$pid_file" ]; then
        log "$servidor não está em execução (arquivo PID não encontrado)"
        return 0
    fi
    
    # Obtém o PID do arquivo
    local pid=$(cat "$pid_file")
    
    # Verifica se o processo está rodando
    if ! ps -p $pid > /dev/null; then
        log "$servidor não está em execução (PID $pid não encontrado)"
        rm -f "$pid_file"
        return 0
    fi
    
    log "Encerrando $servidor (PID: $pid)..."
    
    if [ "$modo" == "normal" ]; then
        # Primeiro tenta um encerramento normal com SIGTERM
        salvar_dados "$servidor" "$pid"
        kill -TERM $pid 2>/dev/null
        
        # Aguarda o encerramento normal
        local contador=0
        while ps -p $pid > /dev/null && [ $contador -lt $GRACEFUL_TIMEOUT ]; do
            log "Aguardando encerramento normal de $servidor... ($contador/$GRACEFUL_TIMEOUT)"
            sleep 1
            contador=$((contador+1))
        done
        
        # Verifica se ainda está rodando
        if ps -p $pid > /dev/null; then
            log "Timeout atingido. Forçando encerramento de $servidor..."
            kill -9 $pid 2>/dev/null
            sleep 1
        fi
    else
        # Encerramento forçado imediato
        log "Forçando encerramento de $servidor..."
        kill -9 $pid 2>/dev/null
    fi
    
    # Verifica se o processo foi encerrado
    if ! ps -p $pid > /dev/null; then
        log "$servidor encerrado com sucesso"
        rm -f "$pid_file"
        return 0
    else
        log "ERRO: Não foi possível encerrar $servidor"
        return 1
    fi
}

# Função para verificar se existem processos órfãos
verificar_orfaos() {
    log "Verificando processos órfãos..."
    
    for servidor in "${SERVERS[@]}"; do
        # Verifica processos em execução pelo nome
        local pids=$(pgrep -x $servidor)
        
        if [ -n "$pids" ]; then
            log "Encontrado processo órfão de $servidor. PIDs: $pids"
            for pid in $pids; do
                log "Encerrando processo órfão $servidor (PID: $pid)..."
                kill -9 $pid 2>/dev/null
            done
        fi
    done
}

# Função para notificar jogadores sobre o shutdown
notificar_jogadores() {
    local mensagem="$1"
    local tempo="$2"  # em minutos
    
    log "Enviando notificação de encerramento para jogadores: $mensagem"
    
    # Verifica se o TMSrv está rodando
    local pid_file="${PID_DIR}/TMSrv.pid"
    if [ ! -f "$pid_file" ]; then
        log "TMSrv não está em execução. Não é possível notificar jogadores."
        return 1
    fi
    
    # Obtém o PID do TMSrv
    local pid=$(cat "$pid_file")
    
    # Lógica para enviar comando de notificação
    # Isto depende da implementação específica do servidor
    log "Enviando notificação: $mensagem (Tempo: $tempo minutos)"
    
    # Simulando envio de comando para o servidor (esta parte precisa ser adaptada)
    echo "$mensagem" > "${SERVIDOR_ROOT}/messages/shutdown_notice.txt"
    
    log "Notificação enviada. Aguardando $tempo minutos..."
    tempo_segundos=$((tempo * 60))
    
    # Contagem regressiva
    for ((i=tempo_segundos; i>0; i--)); do
        if ((i % 60 == 0)); then
            minutos=$((i / 60))
            log "Encerramento em $minutos minutos..."
        fi
        sleep 1
    done
}

# Função para salvar logs de estatísticas antes do encerramento
salvar_estatisticas() {
    log "Coletando estatísticas do servidor antes do encerramento..."
    
    # Diretório para estatísticas
    local stats_dir="${LOG_DIR}/stats"
    mkdir -p "$stats_dir"
    
    # Coleta estatísticas de uso de CPU e memória
    log "Coletando estatísticas de uso de recursos..."
    ps aux | grep -E $(printf "%s|" "${SERVERS[@]}" | sed 's/|$//') > "${stats_dir}/proc_stats_${TIMESTAMP}.txt"
    
    # Coleta estatísticas do sistema
    log "Coletando estatísticas do sistema..."
    uptime > "${stats_dir}/uptime_${TIMESTAMP}.txt"
    free -m > "${stats_dir}/memory_${TIMESTAMP}.txt"
    df -h > "${stats_dir}/disk_${TIMESTAMP}.txt"
    
    log "Estatísticas salvas em ${stats_dir}"
}

# Função principal
main() {
    # Inicialização
    verificar_log_dir
    
    log "=== Iniciando encerramento do servidor WYDBR ==="
    log "Versão do Sistema: $(uname -a)"
    
    # Verifica se está sendo executado como root
    if [ "$(id -u)" -ne 0 ]; then
        log "AVISO: Este script não está sendo executado como root, o que pode limitar algumas funcionalidades"
    fi
    
    # Salva estatísticas antes do encerramento
    salvar_estatisticas
    
    # Verifica parâmetros
    MODO_ENCERRAMENTO="normal"
    NOTIFICAR=0
    TEMPO_ESPERA=0
    
    for arg in "$@"; do
        case $arg in
            --force|-f)
                MODO_ENCERRAMENTO="forçado"
                log "Modo de encerramento forçado ativado"
                ;;
            --notify|-n)
                NOTIFICAR=1
                ;;
            --time=*|-t=*)
                TEMPO_ESPERA="${arg#*=}"
                ;;
        esac
    done
    
    # Notifica jogadores se necessário
    if [ $NOTIFICAR -eq 1 ] && [ $TEMPO_ESPERA -gt 0 ]; then
        notificar_jogadores "ATENÇÃO: O servidor será encerrado para manutenção." $TEMPO_ESPERA
    fi
    
    # Encerra servidores na ordem reversa
    log "Encerrando servidores na ordem reversa..."
    for servidor in "${SERVERS[@]}"; do
        encerrar_servidor "$servidor" "$MODO_ENCERRAMENTO"
        # Aguarda um momento entre encerramentos para evitar problemas de dependência
        sleep 2
    done
    
    # Verifica processos órfãos
    verificar_orfaos
    
    # Conclusão
    log "=== Encerramento do servidor WYDBR concluído ==="
}

# Executa a função principal
main "$@"

exit 0