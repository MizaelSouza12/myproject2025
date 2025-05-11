#pragma once

#include "../../include/BaseAgent.h"
#include "../../include/GameStructs.h"
#include "../../include/ServerConnector.h"
#include <memory>

namespace WYDStudio {

/**
 * @class GameMasterAgent
 * @brief Agente de IA para funções de Game Master
 * 
 * Este agente é responsável por funções de moderação, gerenciamento de
 * eventos e distribuição de recompensas.
 */
class GameMasterAgent : public BaseAgent {
public:
    /**
     * @brief Construtor
     * @param serverConnector Ponteiro para o conector de servidor
     */
    GameMasterAgent(std::shared_ptr<ServerConnector> serverConnector);
    
    /**
     * @brief Destrutor virtual
     */
    virtual ~GameMasterAgent() override;
    
    /**
     * @brief Inicializa o agente com modelos de IA
     * @param modelsPath Caminho para os modelos de IA
     * @return true se inicializado com sucesso
     */
    virtual bool Initialize(const std::string& modelsPath) override;
    
    /**
     * @brief Processa uma consulta e retorna uma resposta
     * @param query Consulta em linguagem natural
     * @return Resposta gerada pelo agente
     */
    virtual std::string ProcessQuery(const std::string& query) override;
    
    /**
     * @brief Executa uma ação com base na intenção do usuário
     * @param intent Intenção interpretada da consulta do usuário
     * @param params Parâmetros extraídos da consulta
     * @return true se a ação foi executada com sucesso
     */
    virtual bool ExecuteAction(const std::string& intent, 
                              const std::vector<std::string>& params) override;
    
    /**
     * @brief Verifica se o agente pode lidar com determinada intenção
     * @param intent A intenção a ser verificada
     * @return true se o agente pode lidar com a intenção
     */
    virtual bool CanHandleIntent(const std::string& intent) const override;
    
    /**
     * @brief Obtém o tipo do agente
     * @return Tipo do agente
     */
    virtual AgentType GetAgentType() const override;
    
    /**
     * @brief Processa uma denúncia de jogador
     * @param reportId ID da denúncia
     * @return true se a denúncia foi processada com sucesso
     */
    bool HandlePlayerReport(int reportId);
    
    /**
     * @brief Processa uma solicitação de gerenciamento de evento
     * @param request Solicitação em linguagem natural
     * @return true se a solicitação foi processada com sucesso
     */
    bool HandleEventManagement(const std::string& request);
    
    /**
     * @brief Processa uma solicitação de distribuição de recompensas
     * @param request Solicitação em linguagem natural
     * @return true se a solicitação foi processada com sucesso
     */
    bool HandleRewardDistribution(const std::string& request);
    
    /**
     * @brief Aplica banimento a um jogador
     * @param playerId ID do jogador
     * @param reason Motivo do banimento
     * @param durationHours Duração do banimento em horas (0 para permanente)
     * @return true se o banimento foi aplicado com sucesso
     */
    bool BanPlayer(int playerId, const std::string& reason, int durationHours = 0);
    
    /**
     * @brief Cria um evento de jogo
     * @param eventTemplate Template do evento
     * @return true se o evento foi criado com sucesso
     */
    bool CreateGameEvent(const GameEventTemplate& eventTemplate);
    
    /**
     * @brief Concede um item a um jogador
     * @param playerId ID do jogador
     * @param itemId ID do item
     * @param quantity Quantidade
     * @return true se o item foi concedido com sucesso
     */
    bool AwardItemToPlayer(int playerId, int itemId, int quantity = 1);
    
    /**
     * @brief Envia uma mensagem para todos os jogadores online
     * @param message Mensagem a ser enviada
     * @return true se a mensagem foi enviada com sucesso
     */
    bool BroadcastMessage(const std::string& message);
    
    /**
     * @brief Teleporta um jogador para uma localização específica
     * @param playerId ID do jogador
     * @param x Coordenada X
     * @param y Coordenada Y
     * @return true se o teleporte foi bem-sucedido
     */
    bool TeleportPlayer(int playerId, int x, int y);
    
    /**
     * @brief Controla o clima no jogo
     * @param weatherType Tipo de clima
     * @param duration Duração em minutos
     * @return true se o clima foi alterado com sucesso
     */
    bool SetWeather(int weatherType, int duration);
    
    /**
     * @brief Inicia uma invasão de monstros
     * @param mobId ID do monstro
     * @param location Local da invasão
     * @param count Quantidade de monstros
     * @param level Nível dos monstros
     * @return true se a invasão foi iniciada com sucesso
     */
    bool StartMobInvasion(int mobId, const std::string& location, int count, int level = 0);
    
private:
    class Impl;
    std::unique_ptr<Impl> m_pImpl; // Implementação privada (Pimpl idiom)
    std::shared_ptr<ServerConnector> m_serverConnector;
};

} // namespace WYDStudio