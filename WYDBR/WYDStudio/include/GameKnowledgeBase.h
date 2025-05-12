#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <functional>

namespace WYDStudio {

/**
 * @typedef LogCallback
 * @brief Tipo para callback de log
 */
using LogCallback = std::function<void(const std::string&)>;

/**
 * @enum GameItemType
 * @brief Tipos genéricos de itens de jogo
 */
enum class GameItemType {
    UNKNOWN,
    WEAPON,
    ARMOR,
    ACCESSORY,
    CONSUMABLE,
    MATERIAL,
    QUEST_ITEM,
    CURRENCY,
    COSMETIC,
    CONTAINER,
    KEY_ITEM
};

/**
 * @enum GameEntityRelation
 * @brief Tipos de relações entre entidades do jogo
 */
enum class GameEntityRelation {
    UNKNOWN,
    SELLS,          // NPC vende item
    BUYS,           // NPC compra item
    DROPS,          // Mob dropa item
    REQUIRES,       // Quest requer item
    TEACHES,        // NPC ensina habilidade
    SPAWNS_IN,      // Mob aparece em mapa
    GIVES_QUEST,    // NPC dá quest
    COMPLETES_QUEST // NPC completa quest
};

/**
 * @class GameKnowledgeBase
 * @brief Base de conhecimento genérica para jogos
 * 
 * Esta classe fornece uma base de conhecimento genérica
 * sobre conceitos de jogos que podem ser especializados
 * para jogos específicos.
 */
class GameKnowledgeBase {
public:
    GameKnowledgeBase();
    ~GameKnowledgeBase();
    
    /**
     * @brief Define callback para logs
     * @param callback Função de callback
     */
    void SetLogCallback(LogCallback callback);
    
    /**
     * @brief Registra um tipo de entidade
     * @param entityType Nome do tipo
     * @return True se registrado com sucesso
     */
    bool RegisterEntityType(const std::string& entityType);
    
    /**
     * @brief Registra um atributo para um tipo de entidade
     * @param entityType Tipo de entidade
     * @param attributeName Nome do atributo
     * @param attributeType Tipo do atributo (int, float, string, etc.)
     * @return True se registrado com sucesso
     */
    bool RegisterEntityAttribute(
        const std::string& entityType,
        const std::string& attributeName,
        const std::string& attributeType);
    
    /**
     * @brief Registra uma entidade
     * @param entityType Tipo da entidade
     * @param entityId ID da entidade
     * @param attributes Atributos da entidade
     * @return True se registrado com sucesso
     */
    bool RegisterEntity(
        const std::string& entityType,
        const std::string& entityId,
        const std::map<std::string, std::string>& attributes);
    
    /**
     * @brief Registra uma relação entre entidades
     * @param sourceType Tipo da entidade de origem
     * @param sourceId ID da entidade de origem
     * @param relation Tipo de relação
     * @param targetType Tipo da entidade de destino
     * @param targetId ID da entidade de destino
     * @param attributes Atributos adicionais da relação
     * @return True se registrado com sucesso
     */
    bool RegisterRelation(
        const std::string& sourceType,
        const std::string& sourceId,
        GameEntityRelation relation,
        const std::string& targetType,
        const std::string& targetId,
        const std::map<std::string, std::string>& attributes = {});
    
    /**
     * @brief Verifica se uma entidade existe
     * @param entityType Tipo da entidade
     * @param entityId ID da entidade
     * @return True se a entidade existe
     */
    bool EntityExists(
        const std::string& entityType,
        const std::string& entityId) const;
    
    /**
     * @brief Obtém atributos de uma entidade
     * @param entityType Tipo da entidade
     * @param entityId ID da entidade
     * @return Mapa com atributos da entidade
     */
    std::map<std::string, std::string> GetEntityAttributes(
        const std::string& entityType,
        const std::string& entityId) const;
    
    /**
     * @brief Busca entidades por atributos
     * @param entityType Tipo da entidade
     * @param attributes Atributos para filtrar
     * @return Lista de IDs de entidades que correspondem
     */
    std::vector<std::string> SearchEntities(
        const std::string& entityType,
        const std::map<std::string, std::string>& attributes) const;
    
    /**
     * @brief Busca relações para uma entidade
     * @param sourceType Tipo da entidade de origem
     * @param sourceId ID da entidade de origem
     * @param relation Tipo de relação (opcional)
     * @return Lista de pares (tipo da entidade de destino, ID da entidade de destino)
     */
    std::vector<std::pair<std::string, std::string>> GetRelations(
        const std::string& sourceType,
        const std::string& sourceId,
        GameEntityRelation relation = GameEntityRelation::UNKNOWN) const;
    
    /**
     * @brief Busca entidades relacionadas a outra
     * @param targetType Tipo da entidade de destino
     * @param targetId ID da entidade de destino
     * @param relation Tipo de relação (opcional)
     * @return Lista de pares (tipo da entidade de origem, ID da entidade de origem)
     */
    std::vector<std::pair<std::string, std::string>> GetRelatedEntities(
        const std::string& targetType,
        const std::string& targetId,
        GameEntityRelation relation = GameEntityRelation::UNKNOWN) const;
    
    /**
     * @brief Obtém atributos de uma relação
     * @param sourceType Tipo da entidade de origem
     * @param sourceId ID da entidade de origem
     * @param relation Tipo de relação
     * @param targetType Tipo da entidade de destino
     * @param targetId ID da entidade de destino
     * @return Mapa com atributos da relação
     */
    std::map<std::string, std::string> GetRelationAttributes(
        const std::string& sourceType,
        const std::string& sourceId,
        GameEntityRelation relation,
        const std::string& targetType,
        const std::string& targetId) const;
    
    /**
     * @brief Converte string de relação para enum
     * @param relationStr String de relação
     * @return Enum de relação
     */
    static GameEntityRelation StringToRelation(const std::string& relationStr);
    
    /**
     * @brief Converte enum de relação para string
     * @param relation Enum de relação
     * @return String representando a relação
     */
    static std::string RelationToString(GameEntityRelation relation);
    
    /**
     * @brief Converte string de tipo de item para enum
     * @param itemTypeStr String de tipo de item
     * @return Enum de tipo de item
     */
    static GameItemType StringToItemType(const std::string& itemTypeStr);
    
    /**
     * @brief Converte enum de tipo de item para string
     * @param itemType Enum de tipo de item
     * @return String representando o tipo de item
     */
    static std::string ItemTypeToString(GameItemType itemType);
    
private:
    // Classes internas
    struct EntityTypeInfo;
    struct EntityInstance;
    struct EntityRelation;
    
    // Armazenamento de dados
    std::map<std::string, std::shared_ptr<EntityTypeInfo>> m_entityTypes;
    std::map<std::string, std::map<std::string, std::shared_ptr<EntityInstance>>> m_entities;
    std::vector<std::shared_ptr<EntityRelation>> m_relations;
    
    // Callback de log
    LogCallback m_logCallback;
    
    // Implementações privadas
    void Log(const std::string& message);
    bool ValidateEntityAttributes(
        const std::string& entityType,
        const std::map<std::string, std::string>& attributes) const;
    std::string GenerateRelationKey(
        const std::string& sourceType,
        const std::string& sourceId,
        GameEntityRelation relation,
        const std::string& targetType,
        const std::string& targetId) const;
};

} // namespace WYDStudio