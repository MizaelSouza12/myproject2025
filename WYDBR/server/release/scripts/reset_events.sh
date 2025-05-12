#!/bin/bash

# Script de reset de eventos do servidor WYDBR
# Reinicia eventos do jogo e limpa caches relacionados

# Configurações
SERVIDOR_ROOT="/opt/WYDBR/server"
LOG_DIR="${SERVIDOR_ROOT}/logs"
TIMESTAMP=$(date +"%Y%m%d_%H%M%S")
LOG_FILE="${LOG_DIR}/reset_events_${TIMESTAMP}.log"
CONFIG_DIR="${SERVIDOR_ROOT}/config"
PID_DIR="${SERVIDOR_ROOT}/pids"
DATA_DIR="${SERVIDOR_ROOT}/data"
EVENT_DIR="${DATA_DIR}/events"
EVENT_CACHE_DIR="${DATA_DIR}/cache/events"

# Lista de eventos para reset
EVENTOS=(
    "CastleWar"
    "GuildWar"
    "BossRaid"
    "KingdomBattle"
    "DuelTournament"
    "DailyQuests"
    "WeeklyEvents"
    "SeasonalEvents"
    "RankingSystem"
    "RewardSystem"
)

# Configurações padrão
RESET_ALL=false
FORCE_RESET=false
NOTIFY_PLAYERS=true
BACKUP_EVENTS=true
SHUTDOWN_REQUIRED=false
NOTIFICATION_TIME=5  # minutos
VERBOSE=false
SPECIFIED_EVENTS=()

# Função para registrar mensagens no log
log() {
    local mensagem="$1"
    local nivel="${2:-INFO}"
    echo "[$(date +"%Y-%m-%d %H:%M:%S")] [${nivel}] $mensagem" | tee -a "$LOG_FILE"
}

# Função para verificar se o diretório de log existe
verificar_log_dir() {
    if [ ! -d "$LOG_DIR" ]; then
        mkdir -p "$LOG_DIR"
        log "Diretório de logs criado em $LOG_DIR"
    fi
}

# Função para verificar se os servidores estão rodando
verificar_servidores() {
    log "Verificando servidores ativos..."
    local servidores_ativos=()
    
    for arquivo_pid in "${PID_DIR}"/*.pid; do
        if [ -f "$arquivo_pid" ]; then
            local nome_servidor=$(basename "$arquivo_pid" .pid)
            local pid=$(cat "$arquivo_pid")
            
            if ps -p $pid > /dev/null; then
                servidores_ativos+=("$nome_servidor")
                log "Servidor ativo: $nome_servidor (PID: $pid)" "DEBUG"
            fi
        fi
    done
    
    log "Servidores ativos: ${servidores_ativos[*]}"
    
    # Verifica se o TMSrv (servidor de jogo) está rodando
    if [[ " ${servidores_ativos[*]} " == *" TMSrv "* ]]; then
        if [ "$FORCE_RESET" = true ]; then
            log "TMSrv está rodando, mas o reset forçado foi solicitado"
            SHUTDOWN_REQUIRED=true
        else
            log "TMSrv está rodando. Para resetar eventos, é necessário parar o servidor ou usar a opção --force" "ERROR"
            exit 1
        fi
    else
        log "TMSrv não está rodando, podemos prosseguir com o reset de eventos"
    fi
}

# Função para notificar jogadores sobre o reset de eventos
notificar_jogadores() {
    if [ "$NOTIFY_PLAYERS" = false ] || [ "$SHUTDOWN_REQUIRED" = false ]; then
        return 0
    fi
    
    log "Enviando notificação aos jogadores sobre o reset de eventos..."
    
    # Verifica se o TMSrv está rodando
    local pid_file="${PID_DIR}/TMSrv.pid"
    if [ ! -f "$pid_file" ]; then
        log "TMSrv não está em execução. Não é possível notificar jogadores." "WARN"
        return 1
    fi
    
    # Obtém o PID do TMSrv
    local pid=$(cat "$pid_file")
    
    # Envia comando para o servidor notificar os jogadores
    log "Enviando notificação: O servidor será reiniciado em $NOTIFICATION_TIME minutos para reset de eventos"
    
    # Cria um arquivo de sinal que o TMSrv deve monitorar
    echo "SYSTEM_MSG|O servidor será reiniciado em $NOTIFICATION_TIME minutos para reset de eventos. Por favor, salve seu progresso e desconecte." > "${SERVIDOR_ROOT}/signals/notify_all.signal"
    
    # Envia sinal SIGUSR2 para o TMSrv processar a notificação
    kill -SIGUSR2 $pid
    
    # Aguarda o tempo especificado
    log "Aguardando $NOTIFICATION_TIME minutos antes de prosseguir..."
    for ((i=NOTIFICATION_TIME; i>0; i--)); do
        if ((i % 1 == 0)); then
            if ((i == 1)); then
                echo "SYSTEM_MSG|ATENÇÃO: O servidor será reiniciado em 1 minuto para reset de eventos!" > "${SERVIDOR_ROOT}/signals/notify_all.signal"
            else
                echo "SYSTEM_MSG|ATENÇÃO: O servidor será reiniciado em $i minutos para reset de eventos!" > "${SERVIDOR_ROOT}/signals/notify_all.signal"
            fi
            kill -SIGUSR2 $pid
        fi
        sleep 60
    done
    
    # Notificação final
    echo "SYSTEM_MSG|O servidor está sendo reiniciado agora para reset de eventos. Reconecte em alguns minutos." > "${SERVIDOR_ROOT}/signals/notify_all.signal"
    kill -SIGUSR2 $pid
    sleep 5
    
    return 0
}

# Função para parar o servidor de jogo
parar_servidor() {
    if [ "$SHUTDOWN_REQUIRED" = false ]; then
        return 0
    fi
    
    log "Parando o servidor de jogo para reset de eventos..."
    
    # Usa o script de shutdown se existir
    if [ -f "${SERVIDOR_ROOT}/scripts/shutdown.sh" ]; then
        log "Executando script de shutdown..."
        "${SERVIDOR_ROOT}/scripts/shutdown.sh" --force
        sleep 5
    else
        log "Script de shutdown não encontrado. Tentando parar TMSrv manualmente..." "WARN"
        
        # Tenta parar o TMSrv manualmente
        local pid_file="${PID_DIR}/TMSrv.pid"
        if [ -f "$pid_file" ]; then
            local pid=$(cat "$pid_file")
            
            log "Parando TMSrv (PID: $pid)..."
            kill -15 $pid 2>/dev/null
            sleep 3
            
            # Verifica se o processo ainda está em execução
            if ps -p $pid > /dev/null; then
                log "Forçando encerramento de TMSrv..." "WARN"
                kill -9 $pid 2>/dev/null
                sleep 2
            fi
            
            rm -f "$pid_file"
        fi
    fi
    
    # Verifica se o TMSrv realmente parou
    if pgrep -x TMSrv > /dev/null; then
        log "ERRO: Não foi possível parar o TMSrv. Abortando reset de eventos." "ERROR"
        exit 1
    fi
    
    log "Servidor de jogo parado com sucesso"
    return 0
}

# Função para fazer backup dos dados de eventos
backup_eventos() {
    if [ "$BACKUP_EVENTS" = false ]; then
        log "Backup de eventos desativado, pulando..."
        return 0
    fi
    
    log "Criando backup dos dados de eventos..."
    
    # Cria diretório de backup se não existir
    local backup_dir="${SERVIDOR_ROOT}/backups/events/${TIMESTAMP}"
    mkdir -p "$backup_dir"
    
    # Copia todos os arquivos de eventos para o backup
    if [ -d "$EVENT_DIR" ]; then
        cp -r "$EVENT_DIR"/* "$backup_dir/" 2>/dev/null
        log "Backup dos dados de eventos criado em $backup_dir"
    else
        log "Diretório de eventos não encontrado: $EVENT_DIR" "WARN"
    fi
    
    # Backup do cache de eventos
    if [ -d "$EVENT_CACHE_DIR" ]; then
        mkdir -p "${backup_dir}/cache"
        cp -r "$EVENT_CACHE_DIR"/* "${backup_dir}/cache/" 2>/dev/null
        log "Backup do cache de eventos criado em ${backup_dir}/cache"
    fi
    
    # Backup das configurações de eventos
    if [ -d "$CONFIG_DIR/events" ]; then
        mkdir -p "${backup_dir}/config"
        cp -r "$CONFIG_DIR/events"/* "${backup_dir}/config/" 2>/dev/null
        log "Backup das configurações de eventos criado em ${backup_dir}/config"
    fi
    
    return 0
}

# Função para resetar um evento específico
resetar_evento() {
    local evento=$1
    log "Resetando evento: $evento"
    
    # Verifica se o diretório do evento existe
    local evento_dir="${EVENT_DIR}/${evento}"
    if [ ! -d "$evento_dir" ] && [ ! -f "${EVENT_DIR}/${evento}.dat" ]; then
        log "Diretório ou arquivo de evento não encontrado para $evento" "WARN"
        return 1
    fi
    
    # Reseta o evento baseado no tipo
    case "$evento" in
        "CastleWar")
            # Reseta dados de guerra de castelo
            if [ -f "${EVENT_DIR}/CastleWar.dat" ]; then
                log "Resetando dados de Guerra de Castelo..."
                cp "${DATA_DIR}/templates/CastleWar.template" "${EVENT_DIR}/CastleWar.dat"
            fi
            # Limpa cache de guerra de castelo
            rm -f "${EVENT_CACHE_DIR}/castle_war_*" 2>/dev/null
            ;;
            
        "GuildWar")
            # Reseta dados de guerra de guild
            if [ -d "${EVENT_DIR}/GuildWar" ]; then
                log "Resetando dados de Guerra de Guild..."
                rm -f "${EVENT_DIR}/GuildWar/scores.dat" 2>/dev/null
                rm -f "${EVENT_DIR}/GuildWar/matches.dat" 2>/dev/null
                cp "${DATA_DIR}/templates/GuildWar/scores.template" "${EVENT_DIR}/GuildWar/scores.dat" 2>/dev/null
                cp "${DATA_DIR}/templates/GuildWar/matches.template" "${EVENT_DIR}/GuildWar/matches.dat" 2>/dev/null
            fi
            # Limpa cache de guerra de guild
            rm -f "${EVENT_CACHE_DIR}/guild_war_*" 2>/dev/null
            ;;
            
        "BossRaid")
            # Reseta dados de raid de boss
            if [ -d "${EVENT_DIR}/BossRaid" ]; then
                log "Resetando dados de Raid de Boss..."
                rm -f "${EVENT_DIR}/BossRaid/boss_status.dat" 2>/dev/null
                rm -f "${EVENT_DIR}/BossRaid/last_spawn.dat" 2>/dev/null
                cp "${DATA_DIR}/templates/BossRaid/boss_status.template" "${EVENT_DIR}/BossRaid/boss_status.dat" 2>/dev/null
            fi
            # Limpa cache de raid de boss
            rm -f "${EVENT_CACHE_DIR}/boss_raid_*" 2>/dev/null
            ;;
            
        "KingdomBattle")
            # Reseta dados de batalha de reinos
            if [ -d "${EVENT_DIR}/KingdomBattle" ]; then
                log "Resetando dados de Batalha de Reinos..."
                rm -f "${EVENT_DIR}/KingdomBattle/kingdom_status.dat" 2>/dev/null
                rm -f "${EVENT_DIR}/KingdomBattle/scores.dat" 2>/dev/null
                cp "${DATA_DIR}/templates/KingdomBattle/kingdom_status.template" "${EVENT_DIR}/KingdomBattle/kingdom_status.dat" 2>/dev/null
                cp "${DATA_DIR}/templates/KingdomBattle/scores.template" "${EVENT_DIR}/KingdomBattle/scores.dat" 2>/dev/null
            fi
            # Limpa cache de batalha de reinos
            rm -f "${EVENT_CACHE_DIR}/kingdom_*" 2>/dev/null
            ;;
            
        "DuelTournament")
            # Reseta dados de torneio de duelos
            if [ -d "${EVENT_DIR}/DuelTournament" ]; then
                log "Resetando dados de Torneio de Duelos..."
                rm -f "${EVENT_DIR}/DuelTournament/brackets.dat" 2>/dev/null
                rm -f "${EVENT_DIR}/DuelTournament/participants.dat" 2>/dev/null
                cp "${DATA_DIR}/templates/DuelTournament/brackets.template" "${EVENT_DIR}/DuelTournament/brackets.dat" 2>/dev/null
            fi
            # Limpa cache de torneio de duelos
            rm -f "${EVENT_CACHE_DIR}/duel_*" 2>/dev/null
            ;;
            
        "DailyQuests")
            # Reseta missões diárias
            if [ -d "${EVENT_DIR}/DailyQuests" ]; then
                log "Resetando Missões Diárias..."
                rm -f "${EVENT_DIR}/DailyQuests/daily_pool.dat" 2>/dev/null
                cp "${DATA_DIR}/templates/DailyQuests/daily_pool.template" "${EVENT_DIR}/DailyQuests/daily_pool.dat" 2>/dev/null
            fi
            # Limpa cache de missões diárias
            rm -f "${EVENT_CACHE_DIR}/daily_quest_*" 2>/dev/null
            ;;
            
        "WeeklyEvents")
            # Reseta eventos semanais
            if [ -d "${EVENT_DIR}/WeeklyEvents" ]; then
                log "Resetando Eventos Semanais..."
                rm -f "${EVENT_DIR}/WeeklyEvents/weekly_status.dat" 2>/dev/null
                cp "${DATA_DIR}/templates/WeeklyEvents/weekly_status.template" "${EVENT_DIR}/WeeklyEvents/weekly_status.dat" 2>/dev/null
            fi
            # Limpa cache de eventos semanais
            rm -f "${EVENT_CACHE_DIR}/weekly_*" 2>/dev/null
            ;;
            
        "SeasonalEvents")
            # Reseta eventos sazonais
            if [ -d "${EVENT_DIR}/SeasonalEvents" ]; then
                log "Resetando Eventos Sazonais..."
                rm -f "${EVENT_DIR}/SeasonalEvents/seasonal_status.dat" 2>/dev/null
                cp "${DATA_DIR}/templates/SeasonalEvents/seasonal_status.template" "${EVENT_DIR}/SeasonalEvents/seasonal_status.dat" 2>/dev/null
            fi
            # Limpa cache de eventos sazonais
            rm -f "${EVENT_CACHE_DIR}/seasonal_*" 2>/dev/null
            ;;
            
        "RankingSystem")
            # Reseta sistema de ranking
            if [ -d "${EVENT_DIR}/RankingSystem" ]; then
                log "Resetando Sistema de Ranking..."
                # Faz backup dos rankings antigos com timestamp
                if [ -f "${EVENT_DIR}/RankingSystem/rankings.dat" ]; then
                    cp "${EVENT_DIR}/RankingSystem/rankings.dat" "${EVENT_DIR}/RankingSystem/rankings_${TIMESTAMP}.bak" 2>/dev/null
                fi
                # Reseta apenas rankings específicos, mantendo histórico
                cp "${DATA_DIR}/templates/RankingSystem/rankings.template" "${EVENT_DIR}/RankingSystem/rankings.dat" 2>/dev/null
            fi
            # Limpa cache de rankings
            rm -f "${EVENT_CACHE_DIR}/rank_*" 2>/dev/null
            ;;
            
        "RewardSystem")
            # Reseta sistema de recompensas
            if [ -d "${EVENT_DIR}/RewardSystem" ]; then
                log "Resetando Sistema de Recompensas..."
                rm -f "${EVENT_DIR}/RewardSystem/pending_rewards.dat" 2>/dev/null
                cp "${DATA_DIR}/templates/RewardSystem/pending_rewards.template" "${EVENT_DIR}/RewardSystem/pending_rewards.dat" 2>/dev/null
            fi
            # Limpa cache de recompensas
            rm -f "${EVENT_CACHE_DIR}/reward_*" 2>/dev/null
            ;;
            
        *)
            # Evento desconhecido
            log "Evento desconhecido: $evento, pulando..." "WARN"
            return 1
            ;;
    esac
    
    log "Evento $evento resetado com sucesso"
    return 0
}

# Função para limpar caches de eventos
limpar_caches() {
    log "Limpando caches de eventos..."
    
    # Cria diretório de cache se não existir
    if [ ! -d "$EVENT_CACHE_DIR" ]; then
        mkdir -p "$EVENT_CACHE_DIR"
        log "Diretório de cache de eventos criado: $EVENT_CACHE_DIR"
        return 0
    fi
    
    # Remove todos os arquivos de cache
    rm -f "${EVENT_CACHE_DIR}"/* 2>/dev/null
    
    log "Caches de eventos limpos com sucesso"
    return 0
}

# Função para iniciar o servidor após o reset
iniciar_servidor() {
    if [ "$SHUTDOWN_REQUIRED" = false ]; then
        return 0
    fi
    
    log "Iniciando o servidor após reset de eventos..."
    
    # Usa o script de startup se existir
    if [ -f "${SERVIDOR_ROOT}/scripts/startup.sh" ]; then
        log "Executando script de startup..."
        "${SERVIDOR_ROOT}/scripts/startup.sh"
        sleep 5
    else
        log "Script de startup não encontrado. Não é possível iniciar o servidor automaticamente." "WARN"
        return 1
    fi
    
    # Verifica se o TMSrv iniciou
    if pgrep -x TMSrv > /dev/null; then
        log "Servidor de jogo iniciado com sucesso"
        return 0
    else
        log "AVISO: Não foi possível confirmar se o servidor de jogo iniciou corretamente" "WARN"
        return 1
    fi
}

# Função para exibir ajuda
exibir_ajuda() {
    echo "Uso: $0 [opções] [eventos]"
    echo "Reseta eventos do servidor WYDBR e limpa caches relacionados."
    echo
    echo "Opções:"
    echo "  --all                   Reseta todos os eventos disponíveis"
    echo "  --force                 Força o reset mesmo se o servidor estiver rodando"
    echo "  --no-notify             Não notifica os jogadores antes de resetar eventos"
    echo "  --no-backup             Não cria backup dos dados de eventos antes do reset"
    echo "  --notification-time N   Define o tempo de notificação em minutos (padrão: 5)"
    echo "  --verbose               Modo verboso com mais detalhes"
    echo "  --help                  Exibe esta ajuda"
    echo
    echo "Eventos disponíveis:"
    for evento in "${EVENTOS[@]}"; do
        echo "  $evento"
    done
    echo
    echo "Exemplos:"
    echo "  $0 --all                           # Reseta todos os eventos"
    echo "  $0 CastleWar GuildWar              # Reseta apenas os eventos especificados"
    echo "  $0 --force --no-notify BossRaid    # Força reset do evento BossRaid sem notificar"
    echo
}

# Função principal
main() {
    # Inicializa log
    verificar_log_dir
    
    log "=== Iniciando script de reset de eventos do WYDBR ==="
    log "Versão do Sistema: $(uname -a)"
    
    # Processa parâmetros da linha de comando
    while [[ $# -gt 0 ]]; do
        key="$1"
        case $key in
            --all)
                RESET_ALL=true
                shift
                ;;
            --force)
                FORCE_RESET=true
                shift
                ;;
            --no-notify)
                NOTIFY_PLAYERS=false
                shift
                ;;
            --no-backup)
                BACKUP_EVENTS=false
                shift
                ;;
            --notification-time=*)
                NOTIFICATION_TIME="${key#*=}"
                shift
                ;;
            --notification-time)
                NOTIFICATION_TIME="$2"
                shift 2
                ;;
            --verbose)
                VERBOSE=true
                shift
                ;;
            --help|-h)
                exibir_ajuda
                exit 0
                ;;
            -*)
                log "AVISO: Opção desconhecida: $key" "WARN"
                shift
                ;;
            *)
                # Verifica se o evento especificado existe na lista
                evento_valido=false
                for evento in "${EVENTOS[@]}"; do
                    if [ "$evento" = "$key" ]; then
                        evento_valido=true
                        SPECIFIED_EVENTS+=("$key")
                        break
                    fi
                done
                
                if [ "$evento_valido" = false ]; then
                    log "AVISO: Evento desconhecido: $key" "WARN"
                fi
                
                shift
                ;;
        esac
    done
    
    # Verifica se pelo menos um evento foi especificado ou --all foi usado
    if [ "$RESET_ALL" = false ] && [ ${#SPECIFIED_EVENTS[@]} -eq 0 ]; then
        log "Nenhum evento especificado para reset. Use --all ou especifique eventos." "ERROR"
        exibir_ajuda
        exit 1
    fi
    
    # Verifica se os servidores estão rodando
    verificar_servidores
    
    # Notifica jogadores se necessário
    if [ "$SHUTDOWN_REQUIRED" = true ] && [ "$NOTIFY_PLAYERS" = true ]; then
        notificar_jogadores
    fi
    
    # Para o servidor se necessário
    if [ "$SHUTDOWN_REQUIRED" = true ]; then
        parar_servidor
    fi
    
    # Faz backup dos dados de eventos
    if [ "$BACKUP_EVENTS" = true ]; then
        backup_eventos
    fi
    
    # Reseta eventos
    if [ "$RESET_ALL" = true ]; then
        log "Resetando todos os eventos..."
        for evento in "${EVENTOS[@]}"; do
            resetar_evento "$evento"
        done
    else
        log "Resetando eventos especificados..."
        for evento in "${SPECIFIED_EVENTS[@]}"; do
            resetar_evento "$evento"
        done
    fi
    
    # Limpa caches
    limpar_caches
    
    # Inicia o servidor se foi parado
    if [ "$SHUTDOWN_REQUIRED" = true ]; then
        iniciar_servidor
    fi
    
    log "=== Reset de eventos concluído ==="
}

# Executa a função principal
main "$@"

exit 0 