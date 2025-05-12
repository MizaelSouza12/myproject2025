#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <functional>

namespace WYDStudio {

// Forward declaration
class GameKnowledgeBase;

/**
 * @typedef ProgressCallback
 * @brief Tipo para callback de progresso
 */
using ProgressCallback = std::function<void(float progress, const std::string& status)>;

/**
 * @typedef LogCallback
 * @brief Tipo para callback de log
 */
using LogCallback = std::function<void(const std::string&)>;

/**
 * @class WYDSpecificKnowledge
 * @brief Base de conhecimento específica do jogo WYD
 * 
 * Esta classe encapsula o conhecimento específico sobre o jogo WYD,
 * incluindo itens, monstros, mapas, quests, etc. Ela é usada pelo
 * agente administrativo para tomar decisões informadas.
 */
class WYDSpecificKnowledge {
public:
    /**
     * @brief Construtor
     * @param baseKnowledge Base de conhecimento genérica de jogos
     */
    WYDSpecificKnowledge(std::shared_ptr<GameKnowledgeBase> baseKnowledge);
    
    ~WYDSpecificKnowledge();
    
    /**
     * @brief Inicializa a base de conhecimento
     * @param basePath Caminho base para arquivos de dados
     * @return True se inicializado com sucesso
     */
    bool Initialize(const std::string& basePath);
    
    /**
     * @brief Define callback para logs
     * @param callback Função de callback
     */
    void SetLogCallback(LogCallback callback);
    
    /**
     * @brief Carrega dados do jogo
     * @param progressCallback Callback opcional para progresso
     * @return True se carregado com sucesso
     */
    bool LoadGameData(ProgressCallback progressCallback = nullptr);
    
    /**
     * @brief Responde a uma pergunta sobre o jogo
     * @param question Pergunta em linguagem natural
     * @return Resposta à pergunta
     */
    std::string AnswerGameQuestion(const std::string& question);
    
    /**
     * @brief Verifica se um item existe
     * @param itemName Nome do item
     * @return True se o item existe
     */
    bool ItemExists(const std::string& itemName) const;
    
    /**
     * @brief Verifica se um jogador existe
     * @param playerName Nome do jogador
     * @return True se o jogador existe
     */
    bool PlayerExists(const std::string& playerName) const;
    
    /**
     * @brief Verifica se um mapa existe
     * @param mapName Nome do mapa
     * @return True se o mapa existe
     */
    bool MapExists(const std::string& mapName) const;
    
    /**
     * @brief Verifica se uma guilda existe
     * @param guildName Nome da guilda
     * @return True se a guilda existe
     */
    bool GuildExists(const std::string& guildName) const;
    
    /**
     * @brief Obtém informações sobre um item
     * @param itemName Nome do item
     * @return Mapa com informações do item
     */
    std::map<std::string, std::string> GetItemInfo(const std::string& itemName) const;
    
    /**
     * @brief Obtém informações sobre um jogador
     * @param playerName Nome do jogador
     * @return Mapa com informações do jogador
     */
    std::map<std::string, std::string> GetPlayerInfo(const std::string& playerName) const;
    
    /**
     * @brief Obtém informações sobre um mapa
     * @param mapName Nome do mapa
     * @return Mapa com informações do mapa
     */
    std::map<std::string, std::string> GetMapInfo(const std::string& mapName) const;
    
    /**
     * @brief Obtém informações sobre uma guilda
     * @param guildName Nome da guilda
     * @return Mapa com informações da guilda
     */
    std::map<std::string, std::string> GetGuildInfo(const std::string& guildName) const;
    
    /**
     * @brief Busca itens por critérios
     * @param criteria Critérios de busca (pares chave-valor)
     * @return Lista de nomes de itens que correspondem aos critérios
     */
    std::vector<std::string> SearchItems(const std::map<std::string, std::string>& criteria) const;
    
    /**
     * @brief Busca jogadores por critérios
     * @param criteria Critérios de busca (pares chave-valor)
     * @return Lista de nomes de jogadores que correspondem aos critérios
     */
    std::vector<std::string> SearchPlayers(const std::map<std::string, std::string>& criteria) const;
    
    /**
     * @brief Busca mapas por critérios
     * @param criteria Critérios de busca (pares chave-valor)
     * @return Lista de nomes de mapas que correspondem aos critérios
     */
    std::vector<std::string> SearchMaps(const std::map<std::string, std::string>& criteria) const;
    
    /**
     * @brief Verifica a validade de uma ação de evento
     * @param eventType Tipo de evento
     * @param eventParams Parâmetros do evento
     * @return True se a ação é válida
     */
    bool ValidateEventAction(const std::string& eventType, const std::map<std::string, std::string>& eventParams) const;
    
    /**
     * @brief Verifica a validade de uma ação de economia
     * @param economyAction Ação de economia
     * @param actionParams Parâmetros da ação
     * @return True se a ação é válida
     */
    bool ValidateEconomyAction(const std::string& economyAction, const std::map<std::string, std::string>& actionParams) const;
    
    /**
     * @brief Obtém o valor recomendado para um parâmetro de sistema
     * @param paramName Nome do parâmetro
     * @return Valor recomendado ou string vazia se desconhecido
     */
    std::string GetRecommendedSystemParam(const std::string& paramName) const;
    
private:
    // Classes internas
    struct ItemDatabase;
    struct PlayerDatabase;
    struct MapDatabase;
    struct GuildDatabase;
    struct QuestDatabase;
    struct EventDatabase;
    struct SystemParameters;
    
    // Componentes
    std::shared_ptr<GameKnowledgeBase> m_baseKnowledge;
    std::string m_basePath;
    LogCallback m_logCallback;
    
    // Bases de dados específicas
    std::shared_ptr<ItemDatabase> m_itemDb;
    std::shared_ptr<PlayerDatabase> m_playerDb;
    std::shared_ptr<MapDatabase> m_mapDb;
    std::shared_ptr<GuildDatabase> m_guildDb;
    std::shared_ptr<QuestDatabase> m_questDb;
    std::shared_ptr<EventDatabase> m_eventDb;
    std::shared_ptr<SystemParameters> m_sysParams;
    
    // Implementações privadas
    void Log(const std::string& message);
    bool LoadItems(ProgressCallback callback);
    bool LoadPlayers(ProgressCallback callback);
    bool LoadMaps(ProgressCallback callback);
    bool LoadGuilds(ProgressCallback callback);
    bool LoadQuests(ProgressCallback callback);
    bool LoadEvents(ProgressCallback callback);
    bool LoadSystemParameters(ProgressCallback callback);
    std::string ProcessQueryForDomain(const std::string& question, const std::string& domain);
};

} // namespace WYDStudio