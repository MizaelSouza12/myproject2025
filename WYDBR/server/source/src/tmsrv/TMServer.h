/**
 * TMServer.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/src/tmsrv/TMServer.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef WYDBR_TMSERVER_H
#define WYDBR_TMSERVER_H

#include <string>
#include <memory>
#include <thread>
#include <map>
#include <mutex>
#include <atomic>
#include <chrono>

#include "../../include/Basedef.h"
#include "config/TMConfig.h"
#include "player/WYDPlayer.h"
#include "world/World.h"

// Namespaces antecipados
namespace wydbr {
namespace core {
class EventBus;
}
namespace network {
class NetworkAdapter;
}
namespace database {
class DatabaseClient;
}
}

#define SERVER_VERSION "1.0.0"
#define TM_CONNECTION_PORT 8281
#define DROP_DECAY_TIME 120000  // 2 minutos em milissegundos

namespace wydbr {
namespace tmsrv {

/**
 * @class TMServer
 * @brief Classe principal do servidor Trade & Message (TMSrv)
 * 
 * Gerencia todos os aspectos do servidor, incluindo rede, banco de dados,
 * gerenciamento de jogadores, mundo, eventos, etc.
 */
/**
 * Classe TMServer
 * 
 * Esta classe implementa a funcionalidade TMServer conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class TMServer {
public:
    /**
     * @brief Constrói um novo servidor
     * @param configPath Caminho para o arquivo de configuração
     */
    explicit TMServer(const std::string& configPath);
    
    /**
     * @brief Destrutor
     */
    ~TMServer();
    
    /**
     * @brief Inicializa o servidor
     * @return true se a inicialização foi bem-sucedida
     */
    bool initialize();
    
    /**
     * @brief Inicia o servidor
     * @return true se o servidor foi iniciado com sucesso
     */
    bool start();
    
    /**
     * @brief Desliga o servidor
     */
    void shutdown();
    
    /**
     * @brief Verifica se o servidor está em execução
     * @return true se o servidor estiver em execução
     */
    bool isRunning() const;
    
    /**
     * @brief Obtém estatísticas do servidor
     * @return Mapa com estatísticas (chave-valor)
     */
    std::map<std::string, std::string> getStats() const;
    
    /**
     * @brief Realiza manutenção no servidor
     * @param fullMaintenance true para manutenção completa
     */
    void performMaintenance(bool fullMaintenance = false);
    
    /**
     * @brief Obtém um jogador pelo ID
     * @param playerID ID do jogador
     * @return Ponteiro para o jogador ou nullptr
     */
    std::shared_ptr<WYDPlayer> getPlayer(uint32_t playerID) {
        std::lock_guard<std::mutex> lock(m_playersMutex);
        auto it = m_players.find(playerID);
        return (it != m_players.end()) ? it->second : nullptr;
    }
    
    /**
     * @brief Autentica um jogador
     * @param playerID ID do jogador
     * @param authToken Token de autenticação
     * @return true se a autenticação foi bem-sucedida
     */
    bool authenticatePlayer(uint32_t playerID, const std::string& authToken);

private:
    /**
     * @brief Carrega configurações do servidor
     * @param configPath Caminho para o arquivo de configuração
     * @return true se as configurações foram carregadas com sucesso
     */
    bool loadConfig(const std::string& configPath);
    
    /**
     * @brief Inicializa subsistemas do servidor
     * @return true se todos os subsistemas foram inicializados com sucesso
     */
    bool initializeSubsystems();
    
    /**
     * @brief Loop principal do servidor
     */
    void serverMainLoop();
    
    /**
     * @brief Processa conexões de entrada
     */
    void processIncomingConnections();
    
    /**
     * @brief Atualiza os sistemas do jogo
     * @param deltaTime Tempo desde a última atualização em segundos
     */
    void updateGameSystems(float deltaTime);
    
    /**
     * @brief Trata desconexão de um jogador
     * @param playerID ID do jogador desconectado
     */
    void handlePlayerDisconnect(uint32_t playerID);
    
    /**
     * @brief Salva dados de todos os jogadores
     */
    void saveAllPlayers();
    
    /**
     * @brief Registra um evento no log
     * @param severity Severidade (0-3)
     * @param message Mensagem
     */
    void logEvent(int severity, const std::string& message);

private:
    std::unique_ptr<TMConfig> m_config;                    // Configurações do servidor
    std::unique_ptr<core::EventBus> m_eventBus;            // Barramento de eventos
    std::unique_ptr<network::NetworkAdapter> m_networkAdapter; // Adaptador de rede
    std::unique_ptr<database::DatabaseClient> m_databaseClient; // Cliente de banco de dados
    std::unique_ptr<World> m_world;                        // Mundo do jogo
    
    std::atomic<bool> m_running;                           // Indica se o servidor está em execução
    std::atomic<bool> m_shuttingDown;                      // Indica se o servidor está desligando
    
    std::unique_ptr<std::thread> m_mainThread;             // Thread principal
    std::unique_ptr<std::thread> m_networkThread;          // Thread de rede
    
    std::chrono::system_clock::time_point m_startTime;     // Timestamp de início
    
    std::mutex m_playersMutex;                             // Mutex para acesso à lista de jogadores
    std::map<uint32_t, std::shared_ptr<WYDPlayer>> m_players; // Jogadores por ID
    
    std::atomic<uint64_t> m_connectionCounter;             // Contador de conexões totais
    std::atomic<uint32_t> m_activeConnections;             // Conexões ativas atualmente
    std::atomic<uint64_t> m_bytesReceived;                 // Bytes recebidos
    std::atomic<uint64_t> m_bytesSent;                     // Bytes enviados
    std::atomic<uint64_t> m_packetsProcessed;              // Pacotes processados
};

} // namespace tmsrv
} // namespace wydbr

#endif // WYDBR_TMSERVER_H