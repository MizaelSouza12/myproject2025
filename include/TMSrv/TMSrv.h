
/**
 * @file TMSrv.h
 * @brief Servidor de jogo TMSrv para WYDBR 2.0
 */

#ifndef WYDBR_TMSRV_H
#define WYDBR_TMSRV_H

#include <memory>
#include <string>
#include <vector>
#include <cstdint>

namespace WYDBR {

// Forward declarations
class NetworkManager;
class MobManager;
class ItemManager;
class PlayerManager;
class MapManager;
class QuestManager;
class EventManager;

/**
 * Enum para status do servidor
 */
enum class ServerStatus {
    OFFLINE,
    STARTING,
    RUNNING,
    STOPPING,
    ERROR
};

/**
 * Classe principal do TMSrv
 */
class TMSrv {
public:
    TMSrv();
    ~TMSrv();
    
    /**
     * Inicializa o servidor
     * @param configPath Caminho para o arquivo de configuração
     * @return true se a inicialização foi bem-sucedida, false caso contrário
     */
    bool Initialize(const std::string& configPath = "");
    
    /**
     * Inicia o servidor
     * @param port Porta para escutar (0 = usar valor da configuração)
     * @return true se o início foi bem-sucedido, false caso contrário
     */
    bool Start(uint16_t port = 0);
    
    /**
     * Encerra o servidor
     */
    void Shutdown();
    
    /**
     * Processa a lógica do servidor
     * @param deltaTime Tempo desde o último update em milissegundos
     */
    void Update(float deltaTime);
    
    /**
     * Obtém o status atual do servidor
     * @return Status atual
     */
    ServerStatus GetStatus() const;
    
    /**
     * Obtém informações sobre os clientes conectados
     * @return Vetor com informações dos clientes
     */
    const std::vector<Network::ClientInfo>& GetConnectedClients() const;
    
    // Getters para gerenciadores
    MobManager* GetMobManager() const;
    ItemManager* GetItemManager() const;
    PlayerManager* GetPlayerManager() const;
    MapManager* GetMapManager() const;
    QuestManager* GetQuestManager() const;
    EventManager* GetEventManager() const;
    
private:
    // Evitar cópias
    TMSrv(const TMSrv&) = delete;
    TMSrv& operator=(const TMSrv&) = delete;
    
    // Componentes internos
    std::unique_ptr<NetworkManager> m_networkManager;
    std::unique_ptr<MobManager> m_mobManager;
    std::unique_ptr<ItemManager> m_itemManager;
    std::unique_ptr<PlayerManager> m_playerManager;
    std::unique_ptr<MapManager> m_mapManager;
    std::unique_ptr<QuestManager> m_questManager;
    std::unique_ptr<EventManager> m_eventManager;
    
    // Estado
    ServerStatus m_status;
    std::vector<Network::ClientInfo> m_clients;
    std::string m_configPath;
    uint16_t m_port;
    
    // Métodos internos
    bool LoadConfig();
    bool InitializeManagers();
    void ProcessCommands();
};

} // namespace WYDBR

#endif // WYDBR_TMSRV_H
