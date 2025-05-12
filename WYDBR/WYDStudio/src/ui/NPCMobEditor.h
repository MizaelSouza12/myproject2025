#ifndef WYDSTUDIO_NPC_MOB_EDITOR_H
#define WYDSTUDIO_NPC_MOB_EDITOR_H

#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <functional>
#include <map>
#include <variant>
#include <nlohmann/json.hpp>
#include "../core/WYDStudioFacade.h"

namespace wydstudio {
namespace ui {

using json = nlohmann::json;

/**
 * @brief Tipo de entidade (NPC ou Mob)
 */
enum class EntityType {
    NPC,    // Personagem não-jogável amigável
    MOB     // Monstro hostil
};

/**
 * @brief Tipo de comportamento de NPC
 */
enum class NPCBehaviorType {
    MERCHANT,           // Comerciante
    QUEST_GIVER,        // Fornecedor de quests
    BANKER,             // Bancário
    SKILL_TRAINER,      // Treinador de habilidades
    GUARD,              // Guarda
    TELEPORTER,         // Teleportador
    AUCTIONEER,         // Leiloeiro
    GUILD_MASTER,       // Mestre de guilda
    STORAGE_KEEPER,     // Guardião de armazenamento
    STABLE_MASTER,      // Mestre de estábulo
    BLACKSMITH,         // Ferreiro
    ENCHANTER,          // Encantador
    REPAIRMAN,          // Reparador
    STATIC,             // Estático (decorativo)
    PATROL,             // Patrulheiro
    DIALOG_ONLY,        // Apenas diálogo
    CUSTOM              // Personalizado
};

/**
 * @brief Tipo de comportamento de Mob
 */
enum class MobBehaviorType {
    AGGRESSIVE,         // Ataca ao ver jogador
    NEUTRAL,            // Ataca apenas se atacado
    PASSIVE,            // Não ataca, foge se atacado
    BOSS,               // Comportamento de chefe
    MINION,             // Subordinado a outro mob
    PATROL,             // Patrulha uma área
    TERRITORIAL,        // Defende território
    STATIC,             // Não se move
    WANDERER,           // Vagueia aleatoriamente
    AMBUSHER,           // Emboscador (escondido até ativar)
    SOCIAL,             // Chama outros mobs para ajudar
    COWARD,             // Foge quando com pouca vida
    BERSERKER,          // Fica mais forte com pouca vida
    CUSTOM              // Personalizado
};

/**
 * @brief Tipo de interação de NPC
 */
enum class NPCInteractionType {
    SHOP,               // Loja de itens
    DIALOG,             // Diálogo simples
    QUEST,              // Interação de quest
    SKILL_TRAINING,     // Treinamento de habilidades
    TELEPORT,           // Teleporte
    STORAGE,            // Armazenamento
    AUCTION,            // Leilão
    BANKING,            // Serviços bancários
    CRAFTING,           // Fabricação
    ENCHANTING,         // Encantamento
    REPAIR,             // Reparo
    GUILD_MANAGEMENT,   // Gerenciamento de guilda
    MOUNT_MANAGEMENT,   // Gerenciamento de montarias
    CUSTOM              // Personalizado
};

/**
 * @brief Tipo de ataque de mob
 */
enum class MobAttackType {
    MELEE,              // Corpo a corpo
    RANGED,             // A distância
    MAGIC,              // Mágico
    MIXED,              // Misto
    SPECIAL,            // Especial
    SUMMON,             // Invocação
    AOE,                // Área de efeito
    DOT,                // Dano ao longo do tempo
    DEBUFF,             // Debuff
    DRAIN,              // Drenagem
    CUSTOM              // Personalizado
};

/**
 * @brief Dados de loja de NPC
 */
struct NPCShopData {
    int32_t shopId;
    std::string name;
    
    struct ShopItem {
        int32_t itemId;
        std::string itemName;
        int32_t price;
        int32_t quantity;           // -1 = ilimitado
        int32_t restockTime;        // em segundos, 0 = sem restock
        bool limitedStock;
        int32_t requiredReputation; // 0 = sem requisito
    };
    
    std::vector<ShopItem> items;
    
    int32_t restockInterval;  // Tempo global de restock
    float priceMultiplier;    // Multiplicador de preços
    bool acceptsSpecialCurrency;
    std::string specialCurrencyName;
};

/**
 * @brief Dados de diálogo de NPC
 */
struct NPCDialogData {
    int32_t dialogId;
    std::string greeting;
    
    struct DialogOption {
        int32_t id;
        std::string text;
        std::string response;
        std::vector<int32_t> nextOptions;
        
        struct Condition {
            std::string type;     // quest, item, level, etc.
            std::string value;    // ID ou valor específico
        };
        
        std::vector<Condition> conditions;
        
        struct Action {
            std::string type;     // startQuest, giveItem, etc.
            std::string value;    // ID ou valor específico
        };
        
        std::vector<Action> actions;
    };
    
    std::vector<DialogOption> options;
    
    bool randomGreeting;
    std::vector<std::string> alternativeGreetings;
    std::map<std::string, std::string> specialGreetings;  // Por condição
};

/**
 * @brief Dados de drops de mob
 */
struct MobDropData {
    int32_t dropTableId;
    
    struct DropItem {
        int32_t itemId;
        std::string itemName;
        float dropChance;         // 0.0 a 1.0
        int32_t minQuantity;
        int32_t maxQuantity;
        bool questItem;
        bool uniqueDrop;          // Apenas um jogador recebe
        std::vector<std::string> conditions;  // Condições para o drop
    };
    
    std::vector<DropItem> items;
    
    float goldDropChance;         // 0.0 a 1.0
    int32_t minGold;
    int32_t maxGold;
    
    float specialCurrencyChance;  // 0.0 a 1.0
    int32_t minSpecialCurrency;
    int32_t maxSpecialCurrency;
    std::string specialCurrencyType;
};

/**
 * @brief Dados de comportamento de mob
 */
struct MobBehaviorData {
    MobBehaviorType primaryBehavior;
    std::vector<MobBehaviorType> secondaryBehaviors;
    
    float aggroRange;            // Distância de agressão
    float leashRange;            // Distância máxima de perseguição
    float fleeHealthPercentage;  // % de vida para fugir
    int32_t callForHelpRange;    // Distância para chamar ajuda
    bool ambushOnProximity;      // Embosca ao aproximar
    
    struct MovementPattern {
        std::string type;        // random, patrol, static, etc.
        std::vector<std::pair<int32_t, int32_t>> waypoints;
        float movementSpeed;
        float pauseDuration;     // Tempo de pausa entre movimentos
    };
    
    MovementPattern movement;
    
    struct CombatBehavior {
        std::vector<MobAttackType> attackTypes;
        float preferredAttackRange;
        float meleeRange;
        float rangedRange;
        int32_t attackSpeed;     // ms entre ataques
        bool usesSkills;
        std::vector<int32_t> skillIds;
        std::map<std::string, float> skillUsageChances;
        int32_t specialAttackHealthThreshold;
    };
    
    CombatBehavior combat;
    
    struct SpecialTriggers {
        std::vector<std::string> onHealthPercentage;
        std::vector<std::string> onPlayerProximity;
        std::vector<std::string> onTimeElapsed;
        std::vector<std::string> onMinionDeath;
        std::vector<std::string> onEnvironmentChange;
    };
    
    SpecialTriggers triggers;
};

/**
 * @brief Dados de habilidades de mob
 */
struct MobSkillData {
    int32_t skillId;
    std::string name;
    std::string description;
    
    struct SkillEffect {
        std::string type;        // damage, heal, buff, debuff, etc.
        std::string target;      // self, enemy, area, etc.
        float range;
        float value;
        float duration;          // em segundos
        std::string animation;
        std::string particleEffect;
        std::vector<std::string> additionalEffects;
    };
    
    std::vector<SkillEffect> effects;
    
    int32_t cooldown;           // em segundos
    int32_t castTime;           // em ms
    bool interruptible;
    float useChance;            // 0.0 a 1.0
    int32_t healthThreshold;    // % de vida para usar
    std::vector<std::string> conditions;
};

/**
 * @brief Dados de aparência de entidade
 */
struct EntityAppearanceData {
    int32_t modelId;
    float scale;
    
    struct {
        int32_t textureId;
        std::string colorPalette;
        bool useAlternateTexture;
        int32_t textureVariation;
    } texture;
    
    struct {
        bool hasEquipment;
        int32_t headItem;
        int32_t bodyItem;
        int32_t handItem;
        int32_t feetItem;
        int32_t weaponItem;
        int32_t offhandItem;
        int32_t accessoryItem;
    } equipment;
    
    struct {
        std::string idleAnimation;
        std::string walkAnimation;
        std::string runAnimation;
        std::string attackAnimation;
        std::string hitAnimation;
        std::string deathAnimation;
        std::string specialAnimation1;
        std::string specialAnimation2;
        std::map<std::string, std::string> customAnimations;
    } animations;
    
    struct {
        std::string ambientSound;
        std::string attackSound;
        std::string hitSound;
        std::string deathSound;
        std::string specialSound1;
        std::string specialSound2;
        std::map<std::string, std::string> customSounds;
    } sounds;
    
    struct {
        std::string idleEffect;
        std::string movementEffect;
        std::string attackEffect;
        std::string deathEffect;
        std::string specialEffect;
        std::map<std::string, std::string> customEffects;
    } effects;
};

/**
 * @brief Representação básica de NPC ou Mob para a interface
 */
struct EntityUIModel {
    int32_t id;
    std::string name;
    EntityType type;
    
    // Informações gerais
    struct {
        std::string displayName;
        std::string description;
        int32_t level;
        std::string faction;
        std::string category;
        bool unique;
        bool respawnable;
        int32_t respawnTime;
    } info;
    
    // Dados de atributos
    struct {
        int32_t hp;
        int32_t mp;
        int32_t str;
        int32_t intl;
        int32_t dex;
        int32_t def;
        int32_t mdef;
        int32_t agi;
        int32_t hit;
        int32_t dodge;
        std::map<std::string, int32_t> customAttributes;
    } attributes;
    
    // Localização padrão
    struct {
        int32_t mapId;
        std::string mapName;
        int32_t x;
        int32_t y;
        float direction;
        int32_t spawnRadius;
        int32_t maxInstances;
    } location;
    
    // Resistências
    struct {
        int32_t fire;
        int32_t water;
        int32_t earth;
        int32_t wind;
        int32_t light;
        int32_t dark;
        int32_t poison;
        int32_t stun;
        int32_t sleep;
        int32_t paralyze;
        std::map<std::string, int32_t> customResistances;
    } resistances;
    
    // Dados específicos por tipo
    struct {
        // Para NPCs
        std::optional<NPCBehaviorType> npcBehavior;
        std::vector<NPCInteractionType> npcInteractions;
        std::optional<NPCShopData> shopData;
        std::optional<NPCDialogData> dialogData;
        
        // Para Mobs
        std::optional<MobBehaviorType> mobBehavior;
        std::vector<MobAttackType> mobAttackTypes;
        std::optional<MobDropData> dropData;
        std::optional<MobBehaviorData> behaviorData;
        std::vector<MobSkillData> skills;
        
        // EXP ao matar (só para mobs)
        int32_t expReward;
    } specifics;
    
    // Aparência
    EntityAppearanceData appearance;
    
    // Dados de IA
    struct {
        std::string aiScript;
        std::map<std::string, std::string> aiParameters;
        int32_t intelligenceLevel;  // 0-100
        bool usesPathfinding;
    } ai;
    
    // Flags especiais
    struct {
        bool invulnerable;
        bool invisible;
        bool noCollision;
        bool noTargeting;
        bool permanent;
        bool questRelated;
        bool eventOnly;
        bool noExp;
        bool noLoot;
        std::map<std::string, bool> customFlags;
    } flags;
    
    // Metadados
    struct {
        int32_t createdTimestamp;
        int32_t lastModifiedTimestamp;
        std::string author;
        std::string version;
        std::map<std::string, std::string> extraInfo;
    } metadata;
};

/**
 * @brief Resultado de operações relacionadas a NPCs/Mobs
 */
struct EntityOperationResult {
    bool success;
    std::string message;
    std::optional<EntityUIModel> entity;
};

/**
 * @brief Filtros de pesquisa para NPCs/Mobs
 */
struct EntityFilter {
    std::optional<EntityType> type;
    std::optional<std::string> nameContains;
    std::optional<int32_t> minLevel;
    std::optional<int32_t> maxLevel;
    std::optional<std::string> faction;
    std::optional<std::string> category;
    std::optional<NPCBehaviorType> npcBehaviorType;
    std::optional<MobBehaviorType> mobBehaviorType;
    std::optional<bool> isUnique;
    std::optional<bool> drops; // tem item específico
    std::optional<int32_t> mapId;
};

/**
 * @brief Editor de NPCs e Mobs do WYDStudio
 */
class NPCMobEditor {
public:
    /**
     * @brief Construtor
     * @param facade Referência ao facade principal do WYDStudio
     */
    NPCMobEditor(const std::shared_ptr<core::WYDStudioFacade>& facade);
    
    /**
     * @brief Destrutor
     */
    ~NPCMobEditor();
    
    /**
     * @brief Inicializa o editor
     * @return true se inicializado com sucesso
     */
    bool initialize();
    
    /**
     * @brief Carrega lista de entidades do servidor
     * @return true se carregado com sucesso
     */
    bool loadEntities();
    
    /**
     * @brief Obtém entidade pelo ID
     * @param entityId ID da entidade
     * @return Modelo da entidade ou nulo se não encontrada
     */
    std::optional<EntityUIModel> getEntityById(int32_t entityId);
    
    /**
     * @brief Busca entidades com filtragem
     * @param filter Filtros de busca
     * @return Lista de entidades que correspondem aos filtros
     */
    std::vector<EntityUIModel> searchEntities(const EntityFilter& filter);
    
    /**
     * @brief Cria uma nova entidade com valores padrão
     * @param name Nome da nova entidade
     * @param type Tipo da entidade
     * @return Resultado da operação contendo a nova entidade
     */
    EntityOperationResult createNewEntity(const std::string& name, EntityType type);
    
    /**
     * @brief Cria uma nova entidade a partir de um template
     * @param name Nome da nova entidade
     * @param templateId ID do template a usar
     * @return Resultado da operação contendo a nova entidade
     */
    EntityOperationResult createEntityFromTemplate(const std::string& name, int32_t templateId);
    
    /**
     * @brief Salva alterações em uma entidade
     * @param entity Modelo da entidade modificada
     * @return Resultado da operação
     */
    EntityOperationResult saveEntity(const EntityUIModel& entity);
    
    /**
     * @brief Duplica uma entidade existente
     * @param entityId ID da entidade a ser duplicada
     * @param newName Nome da entidade duplicada
     * @return Resultado da operação contendo a entidade duplicada
     */
    EntityOperationResult duplicateEntity(int32_t entityId, const std::string& newName);
    
    /**
     * @brief Remove uma entidade
     * @param entityId ID da entidade a ser removida
     * @return Resultado da operação
     */
    EntityOperationResult removeEntity(int32_t entityId);
    
    /**
     * @brief Configura loja de NPC
     * @param entityId ID do NPC
     * @param shopData Dados da loja
     * @return Resultado da operação
     */
    EntityOperationResult setNPCShop(int32_t entityId, const NPCShopData& shopData);
    
    /**
     * @brief Configura diálogo de NPC
     * @param entityId ID do NPC
     * @param dialogData Dados do diálogo
     * @return Resultado da operação
     */
    EntityOperationResult setNPCDialog(int32_t entityId, const NPCDialogData& dialogData);
    
    /**
     * @brief Configura drops de mob
     * @param entityId ID do mob
     * @param dropData Dados de drops
     * @return Resultado da operação
     */
    EntityOperationResult setMobDrops(int32_t entityId, const MobDropData& dropData);
    
    /**
     * @brief Configura comportamento de mob
     * @param entityId ID do mob
     * @param behaviorData Dados de comportamento
     * @return Resultado da operação
     */
    EntityOperationResult setMobBehavior(int32_t entityId, const MobBehaviorData& behaviorData);
    
    /**
     * @brief Adiciona habilidade a um mob
     * @param entityId ID do mob
     * @param skillData Dados da habilidade
     * @return Resultado da operação
     */
    EntityOperationResult addMobSkill(int32_t entityId, const MobSkillData& skillData);
    
    /**
     * @brief Remove habilidade de um mob
     * @param entityId ID do mob
     * @param skillId ID da habilidade
     * @return Resultado da operação
     */
    EntityOperationResult removeMobSkill(int32_t entityId, int32_t skillId);
    
    /**
     * @brief Configura aparência de entidade
     * @param entityId ID da entidade
     * @param appearanceData Dados de aparência
     * @return Resultado da operação
     */
    EntityOperationResult setEntityAppearance(int32_t entityId, const EntityAppearanceData& appearanceData);
    
    /**
     * @brief Fornece uma pré-visualização da entidade no jogo
     * @param entity Modelo da entidade a ser visualizada
     * @return Dados de visualização (imagem, modelo 3D, etc.)
     */
    std::vector<uint8_t> generateEntityPreview(const EntityUIModel& entity);
    
    /**
     * @brief Simula comportamento da entidade
     * @param entityId ID da entidade
     * @param scenario Cenário de simulação
     * @return Resultados da simulação
     */
    std::string simulateEntityBehavior(int32_t entityId, const std::string& scenario);
    
    /**
     * @brief Exporta entidades para um arquivo
     * @param entityIds Lista de IDs das entidades a exportar
     * @param exportPath Caminho do arquivo de exportação
     * @return Resultado da operação
     */
    EntityOperationResult exportEntities(const std::vector<int32_t>& entityIds, const std::string& exportPath);
    
    /**
     * @brief Importa entidades de um arquivo
     * @param importPath Caminho do arquivo de importação
     * @return Resultado da operação
     */
    EntityOperationResult importEntities(const std::string& importPath);
    
    /**
     * @brief Gera script de IA para entidade
     * @param entityId ID da entidade
     * @param complexity Nível de complexidade (1-5)
     * @return Script gerado
     */
    std::string generateAIScript(int32_t entityId, int32_t complexity);
    
    /**
     * @brief Verifica a integridade de uma entidade
     * @param entityId ID da entidade a verificar
     * @return Lista de problemas encontrados (vazia se OK)
     */
    std::vector<std::string> validateEntity(int32_t entityId);
    
    /**
     * @brief Sugere balanceamento para um mob
     * @param entityId ID do mob
     * @param targetLevel Nível alvo
     * @return Atributos balanceados
     */
    std::map<std::string, int32_t> suggestMobBalance(int32_t entityId, int32_t targetLevel);
    
    /**
     * @brief Obtém a lista de todas as categorias de entidade disponíveis
     * @param type Tipo de entidade
     * @return Lista de categorias
     */
    std::vector<std::string> getEntityCategories(EntityType type);
    
    /**
     * @brief Obtém a lista de todas as facções disponíveis
     * @return Lista de facções
     */
    std::vector<std::string> getFactions();
    
    /**
     * @brief Obtém a lista de todos os tipos de comportamento de NPC disponíveis
     * @return Lista de tipos de comportamento de NPC
     */
    std::vector<std::pair<NPCBehaviorType, std::string>> getNPCBehaviorTypes();
    
    /**
     * @brief Obtém a lista de todos os tipos de comportamento de mob disponíveis
     * @return Lista de tipos de comportamento de mob
     */
    std::vector<std::pair<MobBehaviorType, std::string>> getMobBehaviorTypes();
    
    /**
     * @brief Obtém a lista de todos os tipos de interação de NPC disponíveis
     * @return Lista de tipos de interação de NPC
     */
    std::vector<std::pair<NPCInteractionType, std::string>> getNPCInteractionTypes();
    
    /**
     * @brief Obtém a lista de todos os tipos de ataque de mob disponíveis
     * @return Lista de tipos de ataque de mob
     */
    std::vector<std::pair<MobAttackType, std::string>> getMobAttackTypes();
    
    /**
     * @brief Obtém a lista de todos os modelos de entidade disponíveis
     * @param type Tipo de entidade
     * @return Lista de modelos
     */
    std::vector<std::pair<int32_t, std::string>> getEntityModels(EntityType type);
    
    /**
     * @brief Recarrega todas as entidades do disco
     * @return true se recarregado com sucesso
     */
    bool reloadEntities();
    
    /**
     * @brief Aplica mudanças ao servidor sem reiniciar
     * @return true se aplicado com sucesso
     */
    bool applyChangesToLiveServer();
    
    /**
     * @brief Registra callback para notificações de alterações em entidades
     * @param callback Função de callback
     * @return ID de registro (para cancelamento posterior)
     */
    uint32_t registerChangeCallback(std::function<void(const EntityUIModel&)> callback);
    
    /**
     * @brief Cancela registro de callback
     * @param callbackId ID do callback a cancelar
     */
    void unregisterChangeCallback(uint32_t callbackId);
    
    /**
     * @brief Obtém o histórico de operações na entidade (para undo/redo)
     * @param entityId ID da entidade
     * @return Lista de operações, da mais recente para a mais antiga
     */
    std::vector<std::string> getEntityEditHistory(int32_t entityId);
    
    /**
     * @brief Desfaz a última operação na entidade
     * @param entityId ID da entidade
     * @return Resultado da operação
     */
    EntityOperationResult undoLastEntityOperation(int32_t entityId);
    
    /**
     * @brief Refaz a última operação desfeita
     * @param entityId ID da entidade
     * @return Resultado da operação
     */
    EntityOperationResult redoEntityOperation(int32_t entityId);

private:
    // Implementação interna
    class Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Referência ao facade principal
    std::shared_ptr<core::WYDStudioFacade> m_facade;
    
    // Utilitários de conversão
    EntityUIModel convertInternalToUIModel(const std::any& internalData);
    std::any convertUIModelToInternal(const EntityUIModel& uiModel);
    
    // Cache de entidades
    std::map<int32_t, EntityUIModel> m_entityCache;
    
    // Histórico de operações por entidade
    std::map<int32_t, std::vector<std::string>> m_entityHistory;
    std::map<int32_t, int32_t> m_historyPosition; // Posição atual para redo
    
    // Validação de entidades
    bool validateEntityIntegrity(const EntityUIModel& entity);
    
    // Gerenciamento de arquivos
    std::string getEntitiesFilePath();
    
    // Utilitários de serialização
    json serializeEntityToJson(const EntityUIModel& entity);
    EntityUIModel deserializeEntityFromJson(const json& json);
    
    // Geração de IDs únicos
    int32_t generateUniqueEntityId();
    int32_t generateUniqueShopId();
    int32_t generateUniqueDialogId();
    int32_t generateUniqueDropTableId();
    int32_t generateUniqueSkillId();
};

}} // namespace wydstudio::ui

#endif // WYDSTUDIO_NPC_MOB_EDITOR_H