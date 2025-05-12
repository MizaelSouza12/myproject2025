#!/bin/bash
#
# Script de inicialização do servidor WYDBR
# Inicia todos os componentes do servidor em sequência com verificações de integridade

# Configurações
SERVIDOR_ROOT="/opt/WYDBR/server"
LOG_DIR="${SERVIDOR_ROOT}/logs"
TIMESTAMP=$(date +"%Y%m%d_%H%M%S")
LOG_FILE="${LOG_DIR}/startup_${TIMESTAMP}.log"
CONFIG_DIR="${SERVIDOR_ROOT}/config"
SERVERS=("DBSrv" "TMSrv" "WSKSrv" "NRSrv" "BISrv" "AgeSrv")
MAX_TENTATIVAS=3

# Cores para saída
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m' # Sem cor

# Função para exibir mensagem com timestamp
log() {
    echo -e "[$(date '+%Y-%m-%d %H:%M:%S')] $1"
}

# Função para verificar se um processo está rodando
is_running() {
    if [ -f "$1" ]; then
        local pid=$(cat "$1")
        if ps -p $pid > /dev/null; then
            return 0 # Está rodando
        fi
    fi
    return 1 # Não está rodando
}

# Função para iniciar um servidor específico com verificação de saúde
iniciar_servidor() {
    local servidor=$1
    local tentativa=1
    
    echo "[$(date +"%Y-%m-%d %H:%M:%S")] Iniciando $servidor..." | tee -a "$LOG_FILE"
    
    # Verifica se o binário existe
    if [ ! -f "${SERVIDOR_ROOT}/bin/$servidor" ]; then
        echo "[$(date +"%Y-%m-%d %H:%M:%S")] ERRO: Binário de $servidor não encontrado!" | tee -a "$LOG_FILE"
        return 1
    fi
    
    # Verifica configuração
    if [ ! -f "${CONFIG_DIR}/${servidor}.conf" ]; then
        echo "[$(date +"%Y-%m-%d %H:%M:%S")] AVISO: Arquivo de configuração ${servidor}.conf não encontrado. Usando configuração padrão." | tee -a "$LOG_FILE"
    else
        echo "[$(date +"%Y-%m-%d %H:%M:%S")] Usando configuração: ${CONFIG_DIR}/${servidor}.conf" | tee -a "$LOG_FILE"
    fi
    
    # Loop de tentativas para iniciar o servidor
    while [ $tentativa -le $MAX_TENTATIVAS ]; do
        echo "[$(date +"%Y-%m-%d %H:%M:%S")] Tentativa $tentativa de $MAX_TENTATIVAS para iniciar $servidor" | tee -a "$LOG_FILE"
        
        # Inicia o servidor em segundo plano
        cd "${SERVIDOR_ROOT}/bin"
        ./$servidor --config="${CONFIG_DIR}/${servidor}.conf" > "${LOG_DIR}/${servidor}_${TIMESTAMP}.log" 2>&1 &
        PID=$!
        
        # Aguarda para verificar se o servidor permanece em execução
        sleep 3
        if ps -p $PID > /dev/null; then
            echo "[$(date +"%Y-%m-%d %H:%M:%S")] $servidor iniciado com sucesso. PID: $PID" | tee -a "$LOG_FILE"
            
            # Registra o PID em um arquivo para referência
            echo $PID > "${SERVIDOR_ROOT}/pids/${servidor}.pid"
            return 0
        else
            echo "[$(date +"%Y-%m-%d %H:%M:%S")] Falha ao iniciar $servidor na tentativa $tentativa" | tee -a "$LOG_FILE"
            tentativa=$((tentativa+1))
            sleep 2
        fi
    done
    
    echo "[$(date +"%Y-%m-%d %H:%M:%S")] ERRO: Falha ao iniciar $servidor após $MAX_TENTATIVAS tentativas" | tee -a "$LOG_FILE"
    return 1
}

# Função para verificar dependências do sistema
verificar_dependencias() {
    echo "[$(date +"%Y-%m-%d %H:%M:%S")] Verificando dependências do sistema..." | tee -a "$LOG_FILE"
    
    # Lista de pacotes/bibliotecas necessárias
    DEPS=("libssl" "libcrypto" "libzmq" "libpq" "libmysqlclient")
    
    for dep in "${DEPS[@]}"; do
        if ! ldconfig -p | grep -q $dep; then
            echo "[$(date +"%Y-%m-%d %H:%M:%S")] AVISO: Dependência $dep não encontrada no sistema" | tee -a "$LOG_FILE"
        else
            echo "[$(date +"%Y-%m-%d %H:%M:%S")] Dependência $dep verificada com sucesso" | tee -a "$LOG_FILE"
        fi
    done
}

# Função para verificar sistema de arquivos
verificar_sistema_arquivos() {
    echo "[$(date +"%Y-%m-%d %H:%M:%S")] Verificando espaço em disco..." | tee -a "$LOG_FILE"
    
    # Verifica espaço em disco
    ESPACO_DISPONIVEL=$(df -h / | awk 'NR==2 {print $4}')
    echo "[$(date +"%Y-%m-%d %H:%M:%S")] Espaço disponível: $ESPACO_DISPONIVEL" | tee -a "$LOG_FILE"
    
    # Verifica permissões dos diretórios críticos
    echo "[$(date +"%Y-%m-%d %H:%M:%S")] Verificando permissões dos diretórios..." | tee -a "$LOG_FILE"
    for dir in "bin" "config" "data" "logs"; do
        if [ ! -w "${SERVIDOR_ROOT}/$dir" ]; then
            echo "[$(date +"%Y-%m-%d %H:%M:%S")] AVISO: Diretório ${SERVIDOR_ROOT}/$dir sem permissão de escrita" | tee -a "$LOG_FILE"
            chmod -R 755 "${SERVIDOR_ROOT}/$dir"
            echo "[$(date +"%Y-%m-%d %H:%M:%S")] Permissão corrigida para ${SERVIDOR_ROOT}/$dir" | tee -a "$LOG_FILE"
        fi
    done
}

# Função para criar diretório de PIDs se não existir
criar_diretorio_pids() {
    if [ ! -d "${SERVIDOR_ROOT}/pids" ]; then
        mkdir -p "${SERVIDOR_ROOT}/pids"
        echo "[$(date +"%Y-%m-%d %H:%M:%S")] Diretório de PIDs criado em ${SERVIDOR_ROOT}/pids" | tee -a "$LOG_FILE"
    else
        # Limpa arquivos de PID anteriores
        rm -f "${SERVIDOR_ROOT}/pids/*.pid"
        echo "[$(date +"%Y-%m-%d %H:%M:%S")] Arquivos de PID antigos removidos" | tee -a "$LOG_FILE"
    fi
}

# Função para verificar se outro servidor já está em execução
verificar_instancia_existente() {
    for servidor in "${SERVERS[@]}"; do
        if pgrep -x $servidor > /dev/null; then
            echo "[$(date +"%Y-%m-%d %H:%M:%S")] AVISO: Uma instância de $servidor já está em execução" | tee -a "$LOG_FILE"
            read -p "Deseja encerrar a instância existente e iniciar uma nova? (s/n): " resposta
            if [[ $resposta == "s" || $resposta == "S" ]]; then
                echo "[$(date +"%Y-%m-%d %H:%M:%S")] Encerrando instância existente de $servidor" | tee -a "$LOG_FILE"
                pkill -9 $servidor
                sleep 2
            else
                echo "[$(date +"%Y-%m-%d %H:%M:%S")] Mantendo instância existente de $servidor" | tee -a "$LOG_FILE"
            fi
        fi
    done
}

# Execução principal
echo "[$(date +"%Y-%m-%d %H:%M:%S")] Iniciando script de startup do WYDBR..." | tee -a "$LOG_FILE"
echo "[$(date +"%Y-%m-%d %H:%M:%S")] Versão do Sistema: $(uname -a)" | tee -a "$LOG_FILE"

# Criar diretório de PIDs
criar_diretorio_pids

# Verificações preliminares
verificar_sistema_arquivos
verificar_dependencias
verificar_instancia_existente

# Inicia os servidores na ordem correta
falhas=0

# Servidor de banco de dados primeiro
iniciar_servidor "DBSrv"
if [ $? -ne 0 ]; then
    echo "[$(date +"%Y-%m-%d %H:%M:%S")] ERRO CRÍTICO: Falha ao iniciar o servidor de banco de dados. Abortando inicialização!" | tee -a "$LOG_FILE"
    exit 1
fi

# Aguarda o banco de dados estabilizar
echo "[$(date +"%Y-%m-%d %H:%M:%S")] Aguardando estabilização do DBSrv..." | tee -a "$LOG_FILE"
sleep 5

# Inicia os demais servidores
for servidor in "TMSrv" "WSKSrv" "NRSrv" "BISrv" "AgeSrv"; do
    iniciar_servidor "$servidor"
    if [ $? -ne 0 ]; then
        falhas=$((falhas+1))
        echo "[$(date +"%Y-%m-%d %H:%M:%S")] AVISO: Falha ao iniciar $servidor" | tee -a "$LOG_FILE"
    else
        echo "[$(date +"%Y-%m-%d %H:%M:%S")] $servidor iniciado com sucesso" | tee -a "$LOG_FILE"
        # Pausa entre inicializações para evitar sobrecarga
        sleep 3
    fi
done

# Verifica status final
if [ $falhas -eq 0 ]; then
    echo "[$(date +"%Y-%m-%d %H:%M:%S")] Todos os servidores foram iniciados com sucesso" | tee -a "$LOG_FILE"
    exit 0
else
    echo "[$(date +"%Y-%m-%d %H:%M:%S")] Inicialização concluída com $falhas falhas. Verifique o log para detalhes: $LOG_FILE" | tee -a "$LOG_FILE"
    exit 1
fi