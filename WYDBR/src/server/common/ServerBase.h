/**
 * @file ServerBase.h
 * @brief Classe base para implementação de servidores
 * @author WYDBR Team
 */

#ifndef WYDBR_SERVERBASE_H
#define WYDBR_SERVERBASE_H

#include <string>
#include <memory>
#include <atomic>
#include <thread>
#include <functional>
#include <vector>
#include <unordered_map>
#include <chrono>

#include "../../core/Logger.h"
#include "../../core/Config.h"
#include "../../core/ThreadPool.h"
#include "CommonConfig.h"
#include "../network/NetworkTypes.h"

namespace wydbr {
namespace server {

/**
 * @brief Status do servidor
 */
enum class ServerStatus {
    STOPPED,        ///< Servidor parado
    INITIALIZING,   ///< Servidor em inicialização
    RUNNING,        ///< Servidor em execução
    PAUSING,        ///< Servidor entrando em modo de pausa
    PAUSED,         ///< Servidor pausado
    STOPPING,       ///< Servidor entrando em modo de parada
    ERROR           ///< Servidor em estado de erro
};

/**
 * @enum ServerType
 * @brief Tipos de servidores disponíveis
 */
enum class ServerType {
    DATABASE,   ///< Servidor de banco de dados
    NETWORKING, ///< Servidor de rede/roteamento
    BILLING,    ///< Servidor de billing/pagamentos
    GAME,       ///< Servidor de jogo
    WORLD,      ///< Servidor de mundo
    LOGIN,      ///< Servidor de login
    MONITOR,    ///< Servidor de monitoramento
    CUSTOM      ///< Servidor customizado
};

/**
 * @brief Evento do servidor
 */
struct ServerEvent {
    uint32_t id;           ///< ID do evento
    std::string name;      ///< Nome do evento
    std::string data;      ///< Dados associados ao evento
    std::chrono::system_clock::time_point timestamp; ///< Timestamp do evento
};

/**
 * @brief Estatísticas do servidor
 */
struct ServerStats {
    uint32_t connectionsActive;    ///< Conexões ativas
    uint32_t connectionsTotal;     ///< Total de conexões desde o início
    uint32_t packetsReceived;      ///< Pacotes recebidos
    uint32_t packetsSent;          ///< Pacotes enviados
    uint64_t bytesReceived;        ///< Bytes recebidos
    uint64_t bytesSent;            ///< Bytes enviados
    double cpuUsage;               ///< Uso de CPU em porcentagem
    size_t memoryUsage;            ///< Uso de memória em bytes
    uint32_t uptime;               ///< Tempo de atividade em segundos
    uint32_t errorCount;           ///< Contador de erros
    std::chrono::system_clock::time_point startTime; ///< Hora de início
};

/**
 * @class ServerBase
 * @brief Classe base para todos os servidores
 */
class ServerBase {
public:
    /**
     * @brief Construtor do servidor base
     * @param name Nome do servidor
     * @param type Tipo do servidor
     * @param configPath Caminho do arquivo de configuração
     */
    ServerBase(const std::string& name, ServerType type, const std::string& configPath)
        : m_name(name),
          m_type(type),
          m_configPath(configPath),
          m_status(ServerStatus::STOPPED),
          m_threadPool(nullptr),
          m_isShuttingDown(false) {
        
        // Inicializa estatísticas
        resetStats();
        
        LOG_INFO("Servidor " + name + " criado");
    }
    
    /**
     * @brief Destrutor virtual
     */
    virtual ~ServerBase() {
        shutdown();
        LOG_INFO("Servidor " + m_name + " destruído");
    }

    /**
     * @brief Inicializa o servidor
     * @return true se inicializado com sucesso, false caso contrário
     */
    virtual bool initialize() {
        if (m_status != ServerStatus::STOPPED) {
            LOG_WARNING("Tentativa de inicializar servidor " + m_name + " que não está parado");
            return false;
        }
        
        m_status = ServerStatus::INITIALIZING;
        LOG_INFO("Inicializando servidor " + m_name);
        
        try {
            // Carrega configurações
            if (!loadConfig()) {
                m_status = ServerStatus::ERROR;
                LOG_ERROR("Falha ao carregar configurações para servidor " + m_name);
                return false;
            }
            
            // Inicializa thread pool
            uint16_t threadCount = m_config.getThreadCount();
            m_threadPool = std::make_unique<core::ThreadPool>(threadCount, m_name + "_pool");
            
            // Inicializa recursos específicos do servidor
            if (!onInitialize()) {
                m_status = ServerStatus::ERROR;
                LOG_ERROR("Falha ao inicializar recursos específicos para servidor " + m_name);
                return false;
            }
            
            // Registra horário de início
            m_stats.startTime = std::chrono::system_clock::now();
            
            LOG_INFO("Servidor " + m_name + " inicializado com sucesso");
            return true;
        } catch (const std::exception& e) {
            m_status = ServerStatus::ERROR;
            LOG_ERROR("Exceção ao inicializar servidor " + m_name + ": " + e.what());
            return false;
        } catch (...) {
            m_status = ServerStatus::ERROR;
            LOG_ERROR("Exceção desconhecida ao inicializar servidor " + m_name);
            return false;
        }
    }

    /**
     * @brief Inicia o servidor
     * @return true se iniciado com sucesso, false caso contrário
     */
    virtual bool start() {
        if (m_status != ServerStatus::INITIALIZING && m_status != ServerStatus::PAUSED) {
            LOG_WARNING("Tentativa de iniciar servidor " + m_name + " em estado inválido: " + 
                     getStatusString(m_status));
            return false;
        }
        
        LOG_INFO("Iniciando servidor " + m_name);
        
        try {
            // Inicia recursos específicos do servidor
            if (!onStart()) {
                m_status = ServerStatus::ERROR;
                LOG_ERROR("Falha ao iniciar recursos específicos para servidor " + m_name);
                return false;
            }
            
            // Inicia thread principal
            m_mainThread = std::thread(&ServerBase::mainLoop, this);
            
            m_status = ServerStatus::RUNNING;
            LOG_INFO("Servidor " + m_name + " iniciado com sucesso");
            return true;
        } catch (const std::exception& e) {
            m_status = ServerStatus::ERROR;
            LOG_ERROR("Exceção ao iniciar servidor " + m_name + ": " + e.what());
            return false;
        } catch (...) {
            m_status = ServerStatus::ERROR;
            LOG_ERROR("Exceção desconhecida ao iniciar servidor " + m_name);
            return false;
        }
    }

    /**
     * @brief Pausa o servidor
     * @return true se pausado com sucesso, false caso contrário
     */
    virtual bool pause() {
        if (m_status != ServerStatus::RUNNING) {
            LOG_WARNING("Tentativa de pausar servidor " + m_name + " que não está em execução");
            return false;
        }
        
        m_status = ServerStatus::PAUSING;
        LOG_INFO("Pausando servidor " + m_name);
        
        try {
            // Pausa recursos específicos do servidor
            if (!onPause()) {
                m_status = ServerStatus::ERROR;
                LOG_ERROR("Falha ao pausar recursos específicos para servidor " + m_name);
                return false;
            }
            
            m_status = ServerStatus::PAUSED;
            LOG_INFO("Servidor " + m_name + " pausado com sucesso");
            return true;
        } catch (const std::exception& e) {
            m_status = ServerStatus::ERROR;
            LOG_ERROR("Exceção ao pausar servidor " + m_name + ": " + e.what());
            return false;
        } catch (...) {
            m_status = ServerStatus::ERROR;
            LOG_ERROR("Exceção desconhecida ao pausar servidor " + m_name);
            return false;
        }
    }

    /**
     * @brief Retoma o servidor após pausa
     * @return true se retomado com sucesso, false caso contrário
     */
    virtual bool resume() {
        if (m_status != ServerStatus::PAUSED) {
            LOG_WARNING("Tentativa de retomar servidor " + m_name + " que não está pausado");
            return false;
        }
        
        LOG_INFO("Retomando servidor " + m_name);
        
        try {
            // Retoma recursos específicos do servidor
            if (!onResume()) {
                m_status = ServerStatus::ERROR;
                LOG_ERROR("Falha ao retomar recursos específicos para servidor " + m_name);
                return false;
            }
            
            m_status = ServerStatus::RUNNING;
            LOG_INFO("Servidor " + m_name + " retomado com sucesso");
            return true;
        } catch (const std::exception& e) {
            m_status = ServerStatus::ERROR;
            LOG_ERROR("Exceção ao retomar servidor " + m_name + ": " + e.what());
            return false;
        } catch (...) {
            m_status = ServerStatus::ERROR;
            LOG_ERROR("Exceção desconhecida ao retomar servidor " + m_name);
            return false;
        }
    }

    /**
     * @brief Para o servidor
     * @return true se parado com sucesso, false caso contrário
     */
    virtual bool stop() {
        if (m_status == ServerStatus::STOPPED) {
            LOG_WARNING("Tentativa de parar servidor " + m_name + " que já está parado");
            return true;
        }
        
        m_status = ServerStatus::STOPPING;
        LOG_INFO("Parando servidor " + m_name);
        
        try {
            m_isShuttingDown = true;
            
            // Finaliza thread principal
            if (m_mainThread.joinable()) {
                m_mainThread.join();
            }
            
            // Para recursos específicos do servidor
            if (!onStop()) {
                m_status = ServerStatus::ERROR;
                LOG_ERROR("Falha ao parar recursos específicos para servidor " + m_name);
                return false;
            }
            
            m_status = ServerStatus::STOPPED;
            LOG_INFO("Servidor " + m_name + " parado com sucesso");
            return true;
        } catch (const std::exception& e) {
            m_status = ServerStatus::ERROR;
            LOG_ERROR("Exceção ao parar servidor " + m_name + ": " + e.what());
            return false;
        } catch (...) {
            m_status = ServerStatus::ERROR;
            LOG_ERROR("Exceção desconhecida ao parar servidor " + m_name);
            return false;
        }
    }

    /**
     * @brief Finaliza e desaloca os recursos do servidor
     * @return true se finalizado com sucesso, false caso contrário
     */
    virtual bool shutdown() {
        if (m_status != ServerStatus::STOPPED) {
            if (!stop()) {
                LOG_ERROR("Falha ao parar servidor " + m_name + " durante shutdown");
                return false;
            }
        }
        
        LOG_INFO("Finalizando servidor " + m_name);
        
        try {
            // Finaliza thread pool
            if (m_threadPool) {
                m_threadPool->shutdown();
                m_threadPool.reset();
            }
            
            // Finaliza recursos específicos do servidor
            if (!onShutdown()) {
                LOG_ERROR("Falha ao finalizar recursos específicos para servidor " + m_name);
                return false;
            }
            
            LOG_INFO("Servidor " + m_name + " finalizado com sucesso");
            return true;
        } catch (const std::exception& e) {
            LOG_ERROR("Exceção ao finalizar servidor " + m_name + ": " + e.what());
            return false;
        } catch (...) {
            LOG_ERROR("Exceção desconhecida ao finalizar servidor " + m_name);
            return false;
        }
    }

    /**
     * @brief Reinicia o servidor
     * @return true se reiniciado com sucesso, false caso contrário
     */
    bool restart() {
        LOG_INFO("Reiniciando servidor " + m_name);
        
        if (!stop()) {
            LOG_ERROR("Falha ao parar servidor " + m_name + " para reinício");
            return false;
        }
        
        if (!initialize()) {
            LOG_ERROR("Falha ao inicializar servidor " + m_name + " para reinício");
            return false;
        }
        
        if (!start()) {
            LOG_ERROR("Falha ao iniciar servidor " + m_name + " para reinício");
            return false;
        }
        
        LOG_INFO("Servidor " + m_name + " reiniciado com sucesso");
        return true;
    }

    /**
     * @brief Obtém o nome do servidor
     * @return Nome do servidor
     */
    std::string getName() const {
        return m_name;
    }

    /**
     * @brief Obtém o tipo do servidor
     * @return Tipo do servidor
     */
    ServerType getType() const {
        return m_type;
    }

    /**
     * @brief Obtém o status atual do servidor
     * @return Status do servidor
     */
    ServerStatus getStatus() const {
        return m_status;
    }

    /**
     * @brief Obtém estatísticas do servidor
     * @return Estrutura com estatísticas
     */
    ServerStats getStats() const {
        return m_stats;
    }
    
    /**
     * @brief Verifica se o servidor está em execução
     * @return true se em execução, false caso contrário
     */
    bool isRunning() const {
        return m_status == ServerStatus::RUNNING;
    }

    /**
     * @brief Adiciona um evento de servidor
     * @param name Nome do evento
     * @param data Dados associados ao evento
     * @return ID do evento registrado
     */
    uint32_t addEvent(const std::string& name, const std::string& data = "") {
        std::lock_guard<std::mutex> lock(m_eventsMutex);
        
        static uint32_t nextId = 1;
        uint32_t eventId = nextId++;
        
        ServerEvent event;
        event.id = eventId;
        event.name = name;
        event.data = data;
        event.timestamp = std::chrono::system_clock::now();
        
        m_events.push_back(event);
        
        // Limita o histórico para evitar uso excessivo de memória
        if (m_events.size() > m_config.getMaxEventHistory()) {
            m_events.erase(m_events.begin());
        }
        
        return eventId;
    }

    /**
     * @brief Obtém lista de eventos recentes
     * @param count Número de eventos a obter (0 = todos)
     * @return Vetor de eventos
     */
    std::vector<ServerEvent> getEvents(size_t count = 0) const {
        std::lock_guard<std::mutex> lock(m_eventsMutex);
        
        if (count == 0 || count >= m_events.size()) {
            return m_events;
        }
        
        return std::vector<ServerEvent>(m_events.end() - count, m_events.end());
    }

    /**
     * @brief Limpa o histórico de eventos
     */
    void clearEvents() {
        std::lock_guard<std::mutex> lock(m_eventsMutex);
        m_events.clear();
    }

    /**
     * @brief Obtém configuração do servidor
     * @return Configuração do servidor
     */
    const CommonServerConfig& getConfig() const {
        return m_config;
    }

    /**
     * @brief Registra uma função de callback para execução periódica
     * @param name Nome do callback
     * @param callback Função a ser executada
     * @param intervalMs Intervalo de execução em milissegundos
     * @return true se registrado com sucesso, false caso contrário
     */
    bool registerCallback(const std::string& name, 
                         std::function<void()> callback, 
                         uint32_t intervalMs) {
        if (name.empty() || !callback || intervalMs == 0) {
            return false;
        }
        
        std::lock_guard<std::mutex> lock(m_callbacksMutex);
        
        CallbackInfo info;
        info.name = name;
        info.callback = callback;
        info.intervalMs = intervalMs;
        info.nextExecution = std::chrono::steady_clock::now() + 
                             std::chrono::milliseconds(intervalMs);
        
        m_callbacks[name] = info;
        return true;
    }

    /**
     * @brief Remove um callback registrado
     * @param name Nome do callback
     * @return true se removido com sucesso, false caso contrário
     */
    bool unregisterCallback(const std::string& name) {
        std::lock_guard<std::mutex> lock(m_callbacksMutex);
        return m_callbacks.erase(name) > 0;
    }

protected:
    /**
     * @brief Função para inicialização específica da classe derivada
     * @return true se inicializado com sucesso, false caso contrário
     */
    virtual bool onInitialize() = 0;
    
    /**
     * @brief Função para início específico da classe derivada
     * @return true se iniciado com sucesso, false caso contrário
     */
    virtual bool onStart() = 0;
    
    /**
     * @brief Função para pausa específica da classe derivada
     * @return true se pausado com sucesso, false caso contrário
     */
    virtual bool onPause() = 0;
    
    /**
     * @brief Função para retomada específica da classe derivada
     * @return true se retomado com sucesso, false caso contrário
     */
    virtual bool onResume() = 0;
    
    /**
     * @brief Função para parada específica da classe derivada
     * @return true se parado com sucesso, false caso contrário
     */
    virtual bool onStop() = 0;
    
    /**
     * @brief Função para finalização específica da classe derivada
     * @return true se finalizado com sucesso, false caso contrário
     */
    virtual bool onShutdown() = 0;
    
    /**
     * @brief Função para atualização específica da classe derivada
     * @param deltaTime Tempo desde a última atualização em segundos
     */
    virtual void onUpdate(float deltaTime) = 0;

private:
    /**
     * @brief Estrutura para callbacks registrados
     */
    struct CallbackInfo {
        std::string name;
        std::function<void()> callback;
        uint32_t intervalMs;
        std::chrono::steady_clock::time_point nextExecution;
    };

    /**
     * @brief Carrega configuração do servidor
     * @return true se carregado com sucesso, false caso contrário
     */
    bool loadConfig() {
        LOG_INFO("Carregando configuração para servidor " + m_name + " de " + m_configPath);
        
        if (!m_config.load(m_configPath)) {
            LOG_ERROR("Falha ao carregar configuração para servidor " + m_name);
            return false;
        }
        
        LOG_DEBUG("Configuração carregada com sucesso para servidor " + m_name);
        return true;
    }

    /**
     * @brief Reset de estatísticas do servidor
     */
    void resetStats() {
        m_stats.connectionsActive = 0;
        m_stats.connectionsTotal = 0;
        m_stats.packetsReceived = 0;
        m_stats.packetsSent = 0;
        m_stats.bytesReceived = 0;
        m_stats.bytesSent = 0;
        m_stats.cpuUsage = 0.0;
        m_stats.memoryUsage = 0;
        m_stats.uptime = 0;
        m_stats.errorCount = 0;
        m_stats.startTime = std::chrono::system_clock::now();
    }

    /**
     * @brief Atualiza estatísticas do servidor
     */
    void updateStats() {
        // Calcula uptime
        auto now = std::chrono::system_clock::now();
        auto uptime = std::chrono::duration_cast<std::chrono::seconds>(
            now - m_stats.startTime).count();
        m_stats.uptime = static_cast<uint32_t>(uptime);
        
        // Outras estatísticas são atualizadas pelos métodos específicos da aplicação
    }

    /**
     * @brief Loop principal do servidor
     */
    void mainLoop() {
        LOG_INFO("Iniciando loop principal para servidor " + m_name);
        
        auto lastTime = std::chrono::steady_clock::now();
        auto lastStatsUpdate = std::chrono::steady_clock::now();
        
        // Intervalo de atualização de estatísticas (1 segundo)
        const auto statsUpdateInterval = std::chrono::seconds(1);
        
        while (!m_isShuttingDown) {
            auto currentTime = std::chrono::steady_clock::now();
            float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
            lastTime = currentTime;
            
            // Verifica se é hora de atualizar estatísticas
            if (currentTime - lastStatsUpdate >= statsUpdateInterval) {
                updateStats();
                lastStatsUpdate = currentTime;
            }
            
            // Atualiza lógica específica do servidor
            if (m_status == ServerStatus::RUNNING) {
                try {
                    onUpdate(deltaTime);
                } catch (const std::exception& e) {
                    m_stats.errorCount++;
                    LOG_ERROR("Exceção durante atualização do servidor " + m_name + ": " + e.what());
                } catch (...) {
                    m_stats.errorCount++;
                    LOG_ERROR("Exceção desconhecida durante atualização do servidor " + m_name);
                }
                
                // Executa callbacks registrados
                executeCallbacks();
            }
            
            // Pequena pausa para evitar uso excessivo de CPU
            if (m_config.getTickRateMs() > 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(m_config.getTickRateMs()));
            }
        }
        
        LOG_INFO("Loop principal finalizado para servidor " + m_name);
    }

    /**
     * @brief Executa callbacks programados
     */
    void executeCallbacks() {
        auto currentTime = std::chrono::steady_clock::now();
        std::vector<std::string> callbacksToExecute;
        
        // Identifica callbacks a serem executados
        {
            std::lock_guard<std::mutex> lock(m_callbacksMutex);
            
            for (auto& pair : m_callbacks) {
                CallbackInfo& info = pair.second;
                
                if (currentTime >= info.nextExecution) {
                    callbacksToExecute.push_back(pair.first);
                    
                    // Atualiza próxima execução
                    info.nextExecution = currentTime + std::chrono::milliseconds(info.intervalMs);
                }
            }
        }
        
        // Executa callbacks identificados
        for (const auto& name : callbacksToExecute) {
            std::function<void()> callback;
            
            {
                std::lock_guard<std::mutex> lock(m_callbacksMutex);
                auto it = m_callbacks.find(name);
                
                if (it != m_callbacks.end()) {
                    callback = it->second.callback;
                }
            }
            
            if (callback) {
                try {
                    callback();
                } catch (const std::exception& e) {
                    m_stats.errorCount++;
                    LOG_ERROR("Exceção durante execução do callback '" + name + "': " + e.what());
                } catch (...) {
                    m_stats.errorCount++;
                    LOG_ERROR("Exceção desconhecida durante execução do callback '" + name + "'");
                }
            }
        }
    }

    /**
     * @brief Converte status para string
     * @param status Status a ser convertido
     * @return String representando o status
     */
    static std::string getStatusString(ServerStatus status) {
        switch (status) {
            case ServerStatus::STOPPED: return "STOPPED";
            case ServerStatus::INITIALIZING: return "INITIALIZING";
            case ServerStatus::RUNNING: return "RUNNING";
            case ServerStatus::PAUSING: return "PAUSING";
            case ServerStatus::PAUSED: return "PAUSED";
            case ServerStatus::STOPPING: return "STOPPING";
            case ServerStatus::ERROR: return "ERROR";
            default: return "UNKNOWN";
        }
    }

private:
    std::string m_name;                   ///< Nome do servidor
    ServerType m_type;                    ///< Tipo do servidor
    std::string m_configPath;             ///< Caminho do arquivo de configuração
    ServerStatus m_status;                ///< Status atual do servidor
    CommonServerConfig m_config;          ///< Configuração do servidor
    ServerStats m_stats;                  ///< Estatísticas do servidor
    
    std::unique_ptr<core::ThreadPool> m_threadPool;  ///< Pool de threads do servidor
    std::thread m_mainThread;             ///< Thread principal do servidor
    std::atomic<bool> m_isShuttingDown;   ///< Flag de encerramento
    
    std::vector<ServerEvent> m_events;    ///< Registro de eventos do servidor
    mutable std::mutex m_eventsMutex;     ///< Mutex para proteção de eventos
    
    std::unordered_map<std::string, CallbackInfo> m_callbacks; ///< Callbacks registrados
    mutable std::mutex m_callbacksMutex;  ///< Mutex para proteção de callbacks
};

} // namespace server
} // namespace wydbr

#endif // WYDBR_SERVERBASE_H 