#pragma once

#include "../../include/BaseAgent.h"
#include "../../include/GameStructs.h"
#include "../../include/ServerConnector.h"
#include <memory>

namespace WYDStudio {

/**
 * @class AdminAgent
 * @brief Agente de IA para funções administrativas
 * 
 * Este agente é responsável por funções de alto nível como gerenciamento
 * de economia, balanceamento de jogo e configuração do servidor.
 */
class AdminAgent : public BaseAgent {
public:
    /**
     * @brief Construtor
     * @param serverConnector Ponteiro para o conector de servidor
     */
    AdminAgent(std::shared_ptr<ServerConnector> serverConnector);
    
    /**
     * @brief Destrutor virtual
     */
    virtual ~AdminAgent() override;
    
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
     * @brief Processa uma solicitação relacionada à economia do jogo
     * @param request Solicitação em linguagem natural
     * @return true se a solicitação foi processada com sucesso
     */
    bool HandleEconomyRequest(const std::string& request);
    
    /**
     * @brief Processa uma solicitação relacionada ao balanceamento do jogo
     * @param request Solicitação em linguagem natural
     * @return true se a solicitação foi processada com sucesso
     */
    bool HandleBalancingRequest(const std::string& request);
    
    /**
     * @brief Processa uma solicitação relacionada à configuração do servidor
     * @param request Solicitação em linguagem natural
     * @return true se a solicitação foi processada com sucesso
     */
    bool HandleServerConfigRequest(const std::string& request);
    
    /**
     * @brief Modifica propriedades de um item
     * @param itemId ID do item
     * @param property Nome da propriedade
     * @param value Novo valor
     * @return true se a modificação foi bem-sucedida
     */
    bool ModifyItemProperty(int itemId, const std::string& property, int value);
    
    /**
     * @brief Ajusta um parâmetro do jogo
     * @param parameter Nome do parâmetro
     * @param value Novo valor
     * @return true se o ajuste foi bem-sucedido
     */
    bool AdjustGameParameter(const std::string& parameter, double value);
    
    /**
     * @brief Agenda uma manutenção do servidor
     * @param startTime Horário de início
     * @param durationMinutes Duração em minutos
     * @return true se o agendamento foi bem-sucedido
     */
    bool ScheduleServerMaintenance(const time_t& startTime, int durationMinutes);
    
private:
    class Impl;
    std::unique_ptr<Impl> m_pImpl; // Implementação privada (Pimpl idiom)
    std::shared_ptr<ServerConnector> m_serverConnector;
};

} // namespace WYDStudio