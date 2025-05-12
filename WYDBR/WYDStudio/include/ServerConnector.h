#pragma once

#include "GameStructs.h"
#include <string>
#include <functional>
#include <vector>
#include <memory>

namespace WYDStudio {

/**
 * @class ServerConnector
 * @brief Classe responsável por se conectar aos servidores do jogo
 * 
 * Esta classe permite que o WYDStudio se conecte diretamente aos servidores do jogo
 * usando o mesmo protocolo do cliente original, mas com privilégios administrativos.
 */
class ServerConnector {
public:
    /**
     * @brief Construtor padrão
     */
    ServerConnector();
    
    /**
     * @brief Destrutor virtual
     */
    virtual ~ServerConnector();
    
    /**
     * @brief Conecta ao servidor TMSrv (servidor principal do jogo)
     * @param ip Endereço IP do servidor
     * @param port Porta do servidor
     * @param adminUser Nome de usuário administrativo
     * @param adminPassword Senha administrativa
     * @return true se a conexão foi estabelecida com sucesso
     */
    bool ConnectToTMSrv(const std::string& ip, int port, 
                       const std::string& adminUser, 
                       const std::string& adminPassword);
    
    /**
     * @brief Conecta ao servidor DBSrv (servidor de banco de dados)
     * @param ip Endereço IP do servidor
     * @param port Porta do servidor
     * @param adminUser Nome de usuário administrativo
     * @param adminPassword Senha administrativa
     * @return true se a conexão foi estabelecida com sucesso
     */
    bool ConnectToDBSrv(const std::string& ip, int port, 
                       const std::string& adminUser, 
                       const std::string& adminPassword);
    
    /**
     * @brief Envia um comando administrativo para o servidor
     * @param command Comando a ser enviado
     * @return true se o comando foi enviado e executado com sucesso
     */
    bool SendAdminCommand(const std::string& command);
    
    /**
     * @brief Envia uma mensagem do sistema para todos os jogadores
     * @param message Mensagem a ser enviada
     * @return true se a mensagem foi enviada com sucesso
     */
    bool SendSystemMessage(const std::string& message);
    
    /**
     * @brief Atualiza os dados de um jogador no servidor
     * @param playerId ID do jogador
     * @param newData Novos dados do jogador
     * @return true se a atualização foi bem-sucedida
     */
    bool UpdatePlayerData(int playerId, const STRUCT_MOB& newData);
    
    /**
     * @brief Cria um NPC no mundo do jogo
     * @param npcId ID do NPC a ser criado
     * @param x Posição X no mundo
     * @param y Posição Y no mundo
     * @param count Quantidade de NPCs a serem criados
     * @return true se a criação foi bem-sucedida
     */
    bool SpawnNPC(int npcId, int x, int y, int count = 1);
    
    /**
     * @brief Modifica configurações do servidor em tempo real
     * @param settingName Nome da configuração
     * @param newValue Novo valor da configuração
     * @return true se a modificação foi bem-sucedida
     */
    bool ModifyServerSettings(const std::string& settingName, const std::string& newValue);
    
    /**
     * @brief Registra uma função de callback para eventos do servidor
     * @param callback Função a ser chamada quando um evento ocorrer
     */
    void RegisterEventCallback(std::function<void(const GameEvent&)> callback);
    
    /**
     * @brief Verifica o status da conexão com o servidor
     * @return true se está conectado
     */
    bool IsConnected() const;
    
    /**
     * @brief Obtém estatísticas do servidor
     * @return Vetor com pares de estatísticas (nome, valor)
     */
    std::vector<std::pair<std::string, std::string>> GetServerStats();
    
    /**
     * @brief Obtém lista de jogadores online
     * @return Lista de jogadores online
     */
    std::vector<PlayerData> GetOnlinePlayers();
    
    /**
     * @brief Reinicia o servidor
     * @param delaySeconds Tempo de espera em segundos antes do reinício
     * @param message Mensagem de aviso para os jogadores
     * @return true se o comando de reinício foi aceito
     */
    bool RestartServer(int delaySeconds, const std::string& message);
    
private:
    class Impl;
    std::unique_ptr<Impl> m_pImpl; // Implementação privada (Pimpl idiom)
};

} // namespace WYDStudio