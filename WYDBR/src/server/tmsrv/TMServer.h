/**
 * TMServer.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/TMServer.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef TMSERVER_H
#define TMSERVER_H

#include <vector>
#include <map>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <thread>
#include <chrono>
#include <atomic>

#include "../../core/WYDTypes.h"
#include "../common/ServerBase.h"
#include "TMConfig.h"
#include "../network/NetworkTypes.h"
#include "../network/SocketManager.h"
#include "../network/PacketHandler.h"
#include "../dbsrv/DBClient.h"
#include "world/World.h"
#include "player/Player.h"

namespace wydbr {


/**
 * @file TMServer.h
 * @brief Definição da classe principal do TM Server
 * 
 * Este arquivo contém a definição da classe CTMServer, que implementa o
 * servidor principal do WYD (Trade & Merchant Server), responsável por
 * gerenciar o mundo do jogo, jogadores, NPCs, itens, etc.
 * 
 * A implementação segue a estrutura original do WYD com os mesmos nomes
 * de classes e métodos, enquanto adiciona melhorias significativas
 * para corrigir bugs conhecidos e estender a funcionalidade.
 */

namespace wyd {
namespace server {

// Forward declarations
class CPlayer;
class CWorld;
class CZone;
class CItem;
class CDBClient;

/**
 * @brief Classe principal do TM Server (Trade & Merchant Server)
 * 
 * No WYD original, esta classe era chamada CTMServer e era responsável
 * por gerenciar o mundo do jogo, jogadores, NPCs, itens, etc.
 */
/**
 * Classe TMServer
 * 
 * Esta classe implementa a funcionalidade TMServer conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class CTMServer : public IServerBase {
public:
    /**
     * @brief Obtém a instância do singleton
     * @return Referência para a instância
     */
    static CTMServer& GetInstance();
    
    /**
     * @brief Inicializa o servidor
     * @param configFile Caminho para o arquivo de configuração
     * @return true se inicializado com sucesso, false caso contrário
     */
    virtual bool Initialize(const char* configFile = "TMServer.conf") override;
    
    /**
     * @brief Inicia o servidor
     * @return true se iniciado com sucesso, false caso contrário
     */
    virtual bool Start() override;
    
    /**
     * @brief Para o servidor
     * @return true se parado com sucesso, false caso contrário
     */
    virtual bool Stop() override;
    
    /**
     * @brief Processa um ciclo de atualização do servidor
     * @param deltaTime Tempo desde o último ciclo em milissegundos
     */
    virtual void Update(DWORD deltaTime) override;
    
    /**
     * @brief Verifica se o servidor está rodando
     * @return true se estiver rodando, false caso contrário
     */
    virtual bool IsRunning() const override {
        return isRunning_.load();
    }
    
    /**
     * @brief Obtém o nome do servidor
     * @return Nome do servidor
     */
    virtual const char* GetServerName() const override {
        return g_Config.szServerName;
    }
    
    /**
     * @brief Obtém a porta do servidor
     * @return Porta do servidor
     */
    virtual WORD GetServerPort() const override {
        return g_Config.wTMServerPort;
    }
    
    /**
     * @brief Obtém o tipo do servidor
     * @return Tipo do servidor
     */
    virtual ServerType GetServerType() const override {
        return ServerType::TM_SERVER;
    }
    
    /**
     * @brief Obtém o ID do servidor
     * @return ID do servidor
     */
    virtual BYTE GetServerID() const override {
        return g_Config.byServerNumber;
    }
    
    /**
     * @brief Obtém o tempo de atividade do servidor
     * @return Tempo de atividade em segundos
     */
    virtual DWORD GetUptime() const override;
    
    /**
     * @brief Obtém o número de jogadores online
     * @return Número de jogadores
     */
    virtual WORD GetOnlinePlayerCount() const override;
    
    /**
     * @brief Obtém o número máximo de jogadores
     * @return Número máximo de jogadores
     */
    virtual WORD GetMaxPlayerCount() const override {
        return static_cast<WORD>(g_Config.dwMaxUsers);
    }
    
    /**
     * @brief Obtém o mundo
     * @return Ponteiro para o mundo
     */
    CWorld* GetWorld() {
        return pWorld_.get();
    }
    
    /**
     * @brief Obtém o mundo (const)
     * @return Ponteiro const para o mundo
     */
    const CWorld* GetWorld() const {
        return pWorld_.get();
    }
    
    /**
     * @brief Envia broadcast para todos os jogadores
     * @param packet Pacote a enviar
     * @param excludePlayer Jogador a excluir do broadcast (opcional)
     * @return Número de jogadores que receberam o broadcast
     */
    int BroadcastPacket(const Packet& packet, CPlayer* excludePlayer = nullptr);
    
    /**
     * @brief Envia broadcast para jogadores em uma área
     * @param packet Pacote a enviar
     * @param mapID ID do mapa
     * @param centerX Coordenada X central
     * @param centerY Coordenada Y central
     * @param radius Raio da área
     * @param excludePlayer Jogador a excluir do broadcast (opcional)
     * @return Número de jogadores que receberam o broadcast
     */
    int BroadcastPacketArea(const Packet& packet, WORD mapID, WORD centerX, WORD centerY, WORD radius, CPlayer* excludePlayer = nullptr);
    
    /**
     * @brief Envia broadcast para jogadores em um mapa
     * @param packet Pacote a enviar
     * @param mapID ID do mapa
     * @param excludePlayer Jogador a excluir do broadcast (opcional)
     * @return Número de jogadores que receberam o broadcast
     */
    int BroadcastPacketMap(const Packet& packet, WORD mapID, CPlayer* excludePlayer = nullptr);
    
    /**
     * @brief Adiciona um jogador ao servidor
     * @param player Ponteiro para o jogador
     * @return true se adicionado com sucesso, false caso contrário
     */
    bool AddPlayer(std::shared_ptr<CPlayer> player);
    
    /**
     * @brief Remove um jogador do servidor
     * @param player Ponteiro para o jogador
     * @return true se removido com sucesso, false caso contrário
     */
    bool RemovePlayer(CPlayer* player);
    
    /**
     * @brief Remove um jogador do servidor pelo ID
     * @param playerID ID do jogador
     * @return true se removido com sucesso, false caso contrário
     */
    bool RemovePlayerByID(DWORD playerID);
    
    /**
     * @brief Busca um jogador pelo ID
     * @param playerID ID do jogador
     * @return Ponteiro para o jogador (nullptr se não encontrado)
     */
    std::shared_ptr<CPlayer> GetPlayerByID(DWORD playerID);
    
    /**
     * @brief Busca um jogador pelo nome
     * @param playerName Nome do jogador
     * @return Ponteiro para o jogador (nullptr se não encontrado)
     */
    std::shared_ptr<CPlayer> GetPlayerByName(const char* playerName);
    
    /**
     * @brief Processa um pacote de rede
     * @param clientID ID do cliente
     * @param packet Pacote recebido
     */
    void ProcessPacket(SOCKET clientID, const Packet& packet);
    
    /**
     * @brief Conecta ao servidor de banco de dados
     * @return true se conectado com sucesso, false caso contrário
     */
    bool ConnectToDBServer();
    
    /**
     * @brief Obtém o cliente de banco de dados
     * @return Ponteiro para o cliente DB
     */
    CDBClient* GetDBClient() {
        return pDBClient_.get();
    }
    
    /**
     * @brief Salva todos os jogadores
     * @param force Força o salvamento mesmo que não seja necessário
     * @return Número de jogadores salvos
     */
    int SaveAllPlayers(bool force = false);
    
    /**
     * @brief Processa comandos de GM
     * @param player Jogador que enviou o comando
     * @param command Comando
     * @param params Parâmetros do comando
     * @return true se o comando foi processado, false caso contrário
     */
    bool ProcessGMCommand(CPlayer* player, const char* command, const char* params);
    
    /**
     * @brief Envia mensagem global para todos os jogadores
     * @param message Mensagem
     * @param type Tipo de mensagem
     * @return Número de jogadores que receberam a mensagem
     */
    int SendGlobalMessage(const char* message, BYTE type = 0);
    
    /**
     * @brief Registra estatísticas do servidor
     * @param fileName Nome do arquivo de log
     * @return true se registrado com sucesso, false caso contrário
     */
    bool LogServerStats(const char* fileName = "TMServer_Stats.log");
    
    /**
     * @brief Obtém o handler de pacotes
     * @return Referência para o handler de pacotes
     */
    CPacketHandler& GetPacketHandler() {
        return packetHandler_;
    }
    
private:
    /**
     * @brief Construtor privado (singleton)
     */
    CTMServer();
    
    /**
     * @brief Destrutor
     */
    virtual ~CTMServer();
    
    /**
     * @brief Desabilita o construtor de cópia
     */
    CTMServer(const CTMServer&) = delete;
    
    /**
     * @brief Desabilita o operador de atribuição
     */
    CTMServer& operator=(const CTMServer&) = delete;
    
    /**
     * @brief Inicializa o tratador de pacotes
     */
    void InitializePacketHandler();
    
    /**
     * @brief Thread de aceitação de conexões
     */
    void AcceptThread();
    
    /**
     * @brief Thread de atualização
     */
    void UpdateThread();
    
    /**
     * @brief Thread de salvamento periódico
     */
    void SaveThread();
    
    /**
     * @brief Verifica conexões inativas e timeout
     */
    void CheckInactiveConnections();
    
    /**
     * @brief Fecha uma conexão
     * @param clientID ID do cliente
     */
    void CloseConnection(SOCKET clientID);
    
    // Atributos principais
    std::atomic<bool> isRunning_;               // Flag de execução
    std::atomic<bool> isInitialized_;           // Flag de inicialização
    std::chrono::steady_clock::time_point startTime_; // Tempo de início
    
    // Socket e conexões
    CSocketManager sockManager_;                  // Gerenciador de sockets
    std::map<SOCKET, std::shared_ptr<CPlayer>> players_; // Jogadores conectados
    std::unordered_map<std::string, DWORD> playerNameMap_; // Mapa de nomes para IDs
    std::mutex playerMutex_;                     // Mutex para thread safety
    
    // Threads
    std::thread acceptThread_;                   // Thread de aceitação
    std::thread updateThread_;                   // Thread de atualização
    std::thread saveThread_;                     // Thread de salvamento
    
    // Componentes do servidor
    std::unique_ptr<CWorld> pWorld_;             // Mundo do jogo
    std::unique_ptr<CDBClient> pDBClient_;       // Cliente de banco de dados
    CPacketHandler packetHandler_;               // Handler de pacotes
    
    // Estatísticas e monitoramento
    DWORD tickCount_;                           // Contador de ticks
    DWORD packetCount_;                         // Contador de pacotes
    WORD activeConnectionCount_;                // Contador de conexões ativas
    std::atomic<DWORD> totalPacketsProcessed_;  // Total de pacotes processados
    
    // Tabelas de referência
    std::vector<WORD> expTable_;                // Tabela de experiência
    std::vector<WORD> statPointsTable_;         // Tabela de pontos de atributos
    
    // Constantes
    static const DWORD DEFAULT_SAVE_INTERVAL = 300000; // 5 minutos
    static const DWORD TICK_RATE = 50;                // 20 ticks por segundo
    static const DWORD MAX_INACTIVE_TIME = 30000;     // 30 segundos de inatividade
};

// Definição global para facilitar o acesso
#define g_TMSrv CTMServer::GetInstance()

} // namespace server
} // namespace wyd

#endif // TMSERVER_H

} // namespace wydbr
