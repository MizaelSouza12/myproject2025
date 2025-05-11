#ifndef WYDSTUDIO_GAME_CONTENT_GENERATOR_H
#define WYDSTUDIO_GAME_CONTENT_GENERATOR_H

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <optional>
#include <random>
#include <functional>
#include <variant>
#include "../binary/ItemListHandler.h" // Para definição de Item
#include "../core/MultitaskingCore.h"

namespace wydstudio {
namespace ai {

// Forward declarations
struct Monster;
struct Quest;
struct Dialogue;
struct DungeonLayout;
struct QuestStep;
struct DialogueNode;
struct Condition;
struct Reward;
struct NPCData;
struct Location;
struct ItemDropEntry;
class ContentModel;

/**
 * @brief Parâmetros para geração de itens
 */
struct ItemGenerationParameters {
    enum class GenerationMode {
        TEMPLATE_BASED,   // Baseado em templates existentes
        HYBRID,           // Combinação de templates com variações
        FULLY_PROCEDURAL  // Totalmente procedural
    };

    std::optional<binary::Item::Type> itemType;
    std::optional<binary::Item::Rarity> minRarity;
    std::optional<binary::Item::Rarity> maxRarity;
    std::optional<binary::Item::Element> element;
    std::optional<uint32_t> minLevel;
    std::optional<uint32_t> maxLevel;
    
    std::optional<std::vector<std::string>> themeTags;
    std::optional<bool> forceUnique;
    std::optional<uint32_t> attributeCount;
    std::optional<float> attributeQualityMultiplier;
    std::optional<uint32_t> templateItemId;
    
    GenerationMode mode = GenerationMode::HYBRID;
    float complexity = 0.5f; // 0.0 = simples, 1.0 = complexo
    float uniqueness = 0.5f; // 0.0 = comum, 1.0 = muito único
    float power = 0.5f;      // 0.0 = fraco, 1.0 = poderoso
    
    // Probabilidades de recursos especiais
    float socketProbability = 0.2f;
    float setItemProbability = 0.1f;
    float specialEffectProbability = 0.15f;
    
    // Controle de seed para geração determinística
    std::optional<uint64_t> seed;
    
    // Limitações específicas para classes
    std::optional<uint32_t> targetClassId;
    
    // Restrições de nomeação
    std::optional<std::string> namePrefix;
    std::optional<std::string> nameSuffix;
    std::optional<std::vector<std::string>> requiredNameWords;
    
    // Restrições de modelo e aparência
    std::optional<uint32_t> modelIdBase;
};

/**
 * @brief Parâmetros para geração de monstros
 */
struct MonsterGenerationParameters {
    enum class GenerationMode {
        TEMPLATE_BASED,   // Baseado em templates existentes
        VARIANT,          // Variação de monstros existentes
        FULLY_PROCEDURAL  // Totalmente procedural
    };
    
    enum class MonsterType {
        NORMAL,
        ELITE,
        BOSS,
        WORLD_BOSS,
        NPC
    };
    
    std::optional<uint32_t> level;
    std::optional<uint32_t> levelRange;  // Variação de nível (+-levelRange)
    std::optional<MonsterType> type;
    std::optional<binary::Item::Element> element;
    
    std::optional<std::vector<std::string>> themeTags;
    std::optional<std::string> locationTheme;
    std::optional<uint32_t> templateMonsterId;
    
    GenerationMode mode = GenerationMode::VARIANT;
    float difficulty = 0.5f;    // 0.0 = fácil, 1.0 = difícil
    float uniqueness = 0.5f;    // 0.0 = comum, 1.0 = muito único
    float complexity = 0.5f;    // 0.0 = simples, 1.0 = complexo
    
    // Drops
    std::optional<uint32_t> minDrops;
    std::optional<uint32_t> maxDrops;
    std::optional<float> dropQualityMultiplier;
    std::optional<uint32_t> guaranteedDropId;
    
    // Habilidades
    std::optional<uint32_t> minSkills;
    std::optional<uint32_t> maxSkills;
    std::optional<bool> usesElementalSkills;
    
    // Aparência
    std::optional<uint32_t> modelIdBase;
    std::optional<float> sizeMultiplier;
    
    // Comportamento
    std::optional<float> aggressiveness;   // 0.0 = passivo, 1.0 = agressivo
    std::optional<float> packBehavior;     // 0.0 = solitário, 1.0 = sempre em grupo
    std::optional<uint32_t> territorySize; // Tamanho do território em unidades do mundo
    
    // Controle de seed para geração determinística
    std::optional<uint64_t> seed;
    
    // Restrições de nomeação
    std::optional<std::string> namePrefix;
    std::optional<std::string> nameSuffix;
    std::optional<std::vector<std::string>> requiredNameWords;
};

/**
 * @brief Parâmetros para geração de quests
 */
struct QuestGenerationParameters {
    enum class QuestType {
        MAIN_STORY,
        SIDE_QUEST,
        DAILY,
        REPEATABLE,
        HIDDEN,
        EVENT,
        WORLD_EVENT,
        GUILD,
        ACHIEVEMENT
    };
    
    enum class QuestComplexity {
        SIMPLE,      // Um único objetivo
        MODERATE,    // 2-3 objetivos sequenciais
        COMPLEX,     // Múltiplos objetivos com condições
        BRANCHING,   // Objetivos com escolhas e ramificações
        EPIC         // História complexa com múltiplas fases
    };
    
    std::optional<QuestType> type;
    std::optional<QuestComplexity> complexity;
    std::optional<uint32_t> minLevel;
    std::optional<uint32_t> maxLevel;
    std::optional<uint32_t> levelRequirement;
    
    std::optional<std::vector<std::string>> themeTags;
    std::optional<std::string> locationName;
    std::optional<uint32_t> questGiverId;
    
    std::optional<uint32_t> minSteps;
    std::optional<uint32_t> maxSteps;
    
    float difficultyScale = 0.5f;  // 0.0 = fácil, 1.0 = difícil
    float rewardQuality = 0.5f;    // 0.0 = básico, 1.0 = excepcional
    float narrativeDepth = 0.5f;   // 0.0 = simples, 1.0 = rica em narrativa
    
    // Recompensas
    std::optional<uint32_t> guaranteedRewardId;
    std::optional<uint32_t> expReward;
    std::optional<uint32_t> goldReward;
    std::optional<bool> includeRandomRewards;
    
    // Relacionamentos
    std::optional<uint32_t> requiredPreviousQuestId;
    std::optional<uint32_t> chainQuestId; // Próxima quest na cadeia
    
    // Controle de seed para geração determinística
    std::optional<uint64_t> seed;
    
    // Restrições de narrativa
    std::optional<std::string> questTitle;
    std::optional<std::string> narrativeTheme;
    std::optional<std::vector<std::string>> requiredNarrativeElements;
    
    // Tipos de objetivos
    std::optional<bool> includeKillObjectives;
    std::optional<bool> includeCollectObjectives;
    std::optional<bool> includeTalkObjectives;
    std::optional<bool> includeDeliveryObjectives;
    std::optional<bool> includeExploreObjectives;
    std::optional<bool> includeEscortObjectives;
    std::optional<bool> includeDefendObjectives;
};

/**
 * @brief Parâmetros para geração de diálogos
 */
struct DialogueGenerationParameters {
    enum class DialogueType {
        NPC_CONVERSATION,
        QUEST_DIALOGUE,
        VENDOR_DIALOGUE,
        AMBIENT_CHATTER,
        TUTORIAL,
        STORY_EXPOSITION,
        CHARACTER_DEVELOPMENT,
        SPECIALIZED_FUNCTION
    };
    
    enum class ToneType {
        FRIENDLY,
        HOSTILE,
        NEUTRAL,
        FORMAL,
        INFORMAL,
        MYSTERIOUS,
        HUMOROUS,
        SERIOUS,
        EDUCATIONAL,
        SALES_ORIENTED
    };
    
    std::optional<DialogueType> type;
    std::optional<ToneType> tone;
    
    std::optional<uint32_t> npcId;
    std::optional<std::vector<std::string>> themeTags;
    std::optional<uint32_t> relatedQuestId;
    
    std::optional<uint32_t> minLength;
    std::optional<uint32_t> maxLength;
    std::optional<uint32_t> minChoices;
    std::optional<uint32_t> maxChoices;
    
    float complexity = 0.5f;   // 0.0 = simples, 1.0 = complexo
    float personality = 0.5f;  // 0.0 = genérico, 1.0 = personalidade distintiva
    
    bool allowBranching = true;
    bool usePlayerName = true;
    bool includeConditions = true;
    
    // Tópicos específicos
    std::optional<std::vector<std::string>> requiredTopics;
    std::optional<std::string> contextLocation;
    
    // Funcionalidades
    std::optional<bool> includeShop;
    std::optional<bool> includeQuestStart;
    std::optional<bool> includeQuestProgress;
    std::optional<bool> includeQuestCompletion;
    std::optional<bool> includeTraining;
    std::optional<bool> includeInformation;
    
    // Controle de seed para geração determinística
    std::optional<uint64_t> seed;
    
    // Personalização do NPC
    std::optional<std::string> npcPersonality;
    std::optional<std::string> npcBackground;
    std::optional<std::vector<std::string>> npcVocabulary;
};

/**
 * @brief Parâmetros para geração de dungeons
 */
struct DungeonGenerationParameters {
    enum class DungeonType {
        CAVE,
        RUINS,
        CASTLE,
        FOREST,
        CRYPT,
        MINE,
        LABORATORY,
        TEMPLE,
        OTHERWORLDLY,
        MECHANICAL
    };
    
    enum class GenerationAlgorithm {
        ROOMS_AND_CORRIDORS,
        CELLULAR_AUTOMATA,
        BSP_TREE,
        DELAUNAY_TRIANGULATION,
        VORONOI_DIAGRAM,
        WAVE_FUNCTION_COLLAPSE,
        CUSTOM_ALGORITHM
    };
    
    enum class DungeonPurpose {
        STANDARD_DUNGEON,
        BOSS_ENCOUNTER,
        PUZZLE_CHALLENGE,
        TREASURE_HUNT,
        WAVE_DEFENSE,
        STORY_PROGRESSION,
        TRAINING_GROUND,
        RESOURCE_GATHERING,
        PVP_ARENA,
        TIMED_CHALLENGE
    };
    
    std::optional<DungeonType> type;
    std::optional<GenerationAlgorithm> algorithm;
    std::optional<DungeonPurpose> purpose;
    
    std::optional<uint32_t> minLevel;
    std::optional<uint32_t> maxLevel;
    std::optional<uint32_t> levelRequirement;
    
    std::optional<uint32_t> width;
    std::optional<uint32_t> height;
    std::optional<uint32_t> floors;
    
    std::optional<uint32_t> minRooms;
    std::optional<uint32_t> maxRooms;
    std::optional<uint32_t> minCorridorLength;
    std::optional<uint32_t> maxCorridorLength;
    
    float complexity = 0.5f;       // 0.0 = simples, 1.0 = complexo
    float difficulty = 0.5f;       // 0.0 = fácil, 1.0 = difícil
    float rewardRichness = 0.5f;   // 0.0 = poucos recompensas, 1.0 = muitas recompensas
    float environmentDensity = 0.5f; // 0.0 = esparsa, 1.0 = densa
    
    // Conteúdo
    std::optional<uint32_t> minMonsters;
    std::optional<uint32_t> maxMonsters;
    std::optional<uint32_t> minElites;
    std::optional<uint32_t> maxElites;
    std::optional<uint32_t> bossCount;
    
    std::optional<uint32_t> minChests;
    std::optional<uint32_t> maxChests;
    std::optional<uint32_t> minCollectibles;
    std::optional<uint32_t> maxCollectibles;
    
    // Características especiais
    std::optional<bool> includePuzzles;
    std::optional<bool> includeTraps;
    std::optional<bool> includeHiddenAreas;
    std::optional<bool> includeShortcuts;
    std::optional<bool> includeUniqueEncounters;
    
    // Restrições temáticas
    std::optional<std::vector<std::string>> themeTags;
    std::optional<binary::Item::Element> elementTheme;
    
    // Conectividade
    std::optional<float> cycleRatio;  // 0.0 = árvore pura, 1.0 = muito cíclico
    
    // Estéticos
    std::optional<std::string> visualTheme;
    std::optional<std::string> soundTheme;
    std::optional<std::string> weatherEffect;
    std::optional<std::string> lightingEffect;
    
    // Controle de seed para geração determinística
    std::optional<uint64_t> seed;
};

/**
 * @brief Estrutura para representar monstros gerados
 */
struct Monster {
    uint32_t id;
    std::string name;
    uint32_t level;
    MonsterGenerationParameters::MonsterType type;
    binary::Item::Element element;
    
    struct Stats {
        uint32_t hp;
        uint32_t mp;
        uint32_t attack;
        uint32_t defense;
        uint32_t magicAttack;
        uint32_t magicDefense;
        uint32_t accuracy;
        uint32_t evasion;
        uint32_t critRate;
        uint32_t moveSpeed;
        uint32_t attackSpeed;
    } stats;
    
    struct Skill {
        uint32_t id;
        std::string name;
        uint32_t damage;
        uint32_t cooldown;
        float range;
        binary::Item::Element element;
        std::string effectDescription;
    };
    
    struct BehaviorPattern {
        float aggressiveness;
        float territorialRange;
        bool isRanged;
        bool usesFleeing;
        bool summonsMinionss;
        uint32_t groupSize;
        float patrolRadius;
        std::vector<std::string> specialBehaviors;
    } behavior;
    
    std::vector<Skill> skills;
    std::vector<ItemDropEntry> drops;
    
    uint32_t expReward;
    uint32_t goldReward;
    
    uint32_t modelId;
    float scale;
    std::string textureVariant;
    
    std::vector<std::string> tags;
    std::string description;
};

/**
 * @brief Estrutura para item drop
 */
struct ItemDropEntry {
    uint32_t itemId;
    float dropRate;
    uint32_t minQuantity;
    uint32_t maxQuantity;
    bool isGuaranteed;
    std::vector<std::string> dropConditions;
};

/**
 * @brief Recompensa de quest
 */
struct Reward {
    enum class Type {
        ITEM,
        GOLD,
        EXPERIENCE,
        REPUTATION,
        SKILL_POINT,
        STAT_POINT,
        SPECIAL_CURRENCY,
        CUSTOM
    };
    
    Type type;
    uint32_t amount;
    uint32_t itemId;  // Se for um item
    std::string customRewardId; // Para recompensas customizadas
    std::string description;
    bool isOptional;
    bool isHidden;
    std::vector<std::string> tags;
};

/**
 * @brief Condição para objetivos, diálogos, etc.
 */
struct Condition {
    enum class Type {
        LEVEL,
        QUEST_COMPLETED,
        QUEST_ACTIVE,
        ITEM_OWNED,
        STAT_VALUE,
        REPUTATION,
        LOCATION_VISITED,
        MONSTER_KILLED,
        TIME_OF_DAY,
        PLAYER_CLASS,
        CUSTOM
    };
    
    Type type;
    std::string target;  // O alvo da condição (ID de quest, item, etc)
    std::string operator_;  // ==, >, <, >=, <=, !=
    std::string value;   // Valor a ser comparado
    bool isNegated;      // Se a condição é negada (NOT)
    std::string customConditionId; // Para condições customizadas
    std::string description;
};

/**
 * @brief Passo de uma quest
 */
struct QuestStep {
    enum class Type {
        KILL_MONSTER,
        COLLECT_ITEM,
        TALK_TO_NPC,
        DELIVER_ITEM,
        ESCORT_NPC,
        EXPLORE_LOCATION,
        DEFEND_LOCATION,
        USE_SKILL,
        CRAFT_ITEM,
        CUSTOM_OBJECTIVE
    };
    
    uint32_t id;
    Type type;
    std::string description;
    std::string shortDescription;
    
    uint32_t targetId;    // ID do monstro, item, NPC, etc
    uint32_t quantity;    // Quantidade necessária
    std::string locationName; // Nome da localização
    
    std::vector<Condition> conditions;
    bool isOptional;
    bool isHidden;
    float completionPercentage; // Para objetivos parciais
    std::string customObjectiveId; // Para objetivos customizados
    
    std::vector<Reward> stepRewards; // Recompensas específicas deste passo
    
    std::vector<uint32_t> prerequisiteStepIds; // IDs de passos que precisam ser completados antes
    std::vector<uint32_t> nextStepIds; // IDs dos próximos passos possíveis
};

/**
 * @brief Estrutura para representar quests geradas
 */
struct Quest {
    uint32_t id;
    std::string title;
    std::string description;
    
    QuestGenerationParameters::QuestType type;
    uint32_t levelRequirement;
    bool repeatable;
    
    uint32_t questGiverId; // NPC que dá a quest
    uint32_t questTurnInId; // NPC para entregar a quest
    
    std::vector<QuestStep> steps;
    std::vector<Reward> rewards;
    std::vector<Condition> requirements;
    
    uint32_t previousQuestId; // Quest anterior na cadeia
    uint32_t nextQuestId;     // Próxima quest na cadeia
    
    std::vector<std::string> tags;
    std::string narrativeText;
    std::string completeText;
};

/**
 * @brief Nó de diálogo para conversas dinâmicas
 */
struct DialogueNode {
    uint32_t id;
    std::string speakerName;
    std::string text;
    bool isPlayerSpeaking;
    
    struct Choice {
        uint32_t id;
        std::string text;
        uint32_t nextNodeId;
        std::vector<Condition> conditions;
        std::vector<std::string> tags;
        bool isExit;
    };
    
    std::vector<Choice> choices;
    std::vector<Condition> conditions;
    
    struct Action {
        enum class Type {
            START_QUEST,
            COMPLETE_QUEST_STEP,
            GIVE_ITEM,
            TAKE_ITEM,
            GIVE_GOLD,
            TAKE_GOLD,
            OPEN_SHOP,
            OPEN_BANK,
            OPEN_STORAGE,
            TELEPORT,
            CUSTOM_ACTION
        };
        
        Type type;
        std::string targetId;  // ID da quest, item, etc
        uint32_t amount;       // Quantidade para dar/tomar
        std::string customActionId; // Para ações customizadas
    };
    
    std::vector<Action> actions;
    
    std::vector<std::string> tags;
    std::string mood;  // Tom emocional deste nó
};

/**
 * @brief Estrutura para representar diálogos gerados
 */
struct Dialogue {
    uint32_t id;
    std::string title;
    DialogueGenerationParameters::DialogueType type;
    
    uint32_t npcId;
    std::vector<DialogueNode> nodes;
    uint32_t startNodeId;
    
    std::vector<std::string> tags;
    std::string description;
    
    // Metadados para processamento
    std::vector<uint32_t> relatedQuestIds;
    bool hasShopFunctionality;
    bool hasBankFunctionality;
    bool hasTrainingFunctionality;
};

/**
 * @brief Sala em uma dungeon
 */
struct Room {
    uint32_t id;
    int x, y;          // Posição
    int width, height; // Dimensões
    
    enum class Type {
        STANDARD,
        ENTRANCE,
        EXIT,
        BOSS,
        TREASURE,
        PUZZLE,
        TRAP,
        SANCTUARY,
        SHOP,
        AMBUSH,
        SECRET
    } type;
    
    std::vector<uint32_t> connectedRoomIds;
    std::vector<Monster> monsters;
    std::vector<ItemDropEntry> treasures;
    
    struct SpecialFeature {
        std::string type;  // "trap", "puzzle", "altar", etc
        std::string description;
        std::vector<Condition> activationConditions;
        std::vector<Reward> rewards;
    };
    
    std::vector<SpecialFeature> specialFeatures;
    std::string theme;
    std::string description;
    
    // Características ambientais
    std::string lighting;
    std::string soundEffect;
    std::string backgroundMusic;
    std::vector<std::string> environmentalEffects;
};

/**
 * @brief Corredor em uma dungeon
 */
struct Corridor {
    uint32_t id;
    uint32_t room1Id;
    uint32_t room2Id;
    
    enum class Type {
        STANDARD,
        HIDDEN,
        LOCKED,
        TRAPPED,
        ONE_WAY,
        DESTRUCTIBLE,
        SPECIAL
    } type;
    
    std::vector<std::pair<int, int>> path; // Pontos no caminho
    
    std::vector<Monster> monsters;
    std::vector<ItemDropEntry> treasures;
    std::vector<Room::SpecialFeature> specialFeatures;
    
    std::string description;
    std::vector<Condition> accessConditions;
};

/**
 * @brief Estrutura para representar layouts de dungeon gerados
 */
struct DungeonLayout {
    uint32_t id;
    std::string name;
    std::string description;
    DungeonGenerationParameters::DungeonType type;
    
    uint32_t width, height;
    uint32_t floors;
    
    struct Floor {
        uint32_t level;
        std::vector<Room> rooms;
        std::vector<Corridor> corridors;
        std::vector<std::pair<int, int>> entranceCoordinates;
        std::vector<std::pair<int, int>> exitCoordinates;
        std::vector<Monster> patrollingMonsters;
        std::string theme;
        
        // Representação em grid para facilitar renderização
        std::vector<std::vector<int>> gridMap;  // -1=parede, 0=vazio, >0=ID da sala
    };
    
    std::vector<Floor> floorLayouts;
    
    uint32_t minLevel, maxLevel;
    Monster boss;
    std::vector<Quest> associatedQuests;
    std::vector<std::string> tags;
    
    struct Navigation {
        std::vector<std::pair<uint32_t, uint32_t>> floorConnections; // Pares de (floor1, floor2)
        std::vector<std::pair<int, int>> shortcutLocations;
        std::vector<std::pair<int, int>> checkpointLocations;
    } navigation;
    
    // Metadados
    std::string generationAlgorithm;
    uint64_t seed;
    float cycleRatio;
};

/**
 * @brief Conjunto de conteúdos gerados
 */
struct GeneratedContent {
    std::vector<binary::Item> items;
    std::vector<Monster> monsters;
    std::vector<Quest> quests;
    std::vector<Dialogue> dialogues;
    std::vector<DungeonLayout> dungeons;
    
    std::string name;
    std::string description;
    std::vector<std::string> tags;
    uint64_t seed;
    std::string generationParameters;
};

/**
 * @brief Descrição das capacidades de geração
 */
struct GenerationCapabilities {
    bool canGenerateItems;
    bool canGenerateMonsters;
    bool canGenerateQuests;
    bool canGenerateDialogues;
    bool canGenerateDungeons;
    
    std::vector<binary::Item::Type> supportedItemTypes;
    std::vector<MonsterGenerationParameters::MonsterType> supportedMonsterTypes;
    std::vector<QuestGenerationParameters::QuestType> supportedQuestTypes;
    std::vector<DialogueGenerationParameters::DialogueType> supportedDialogueTypes;
    std::vector<DungeonGenerationParameters::DungeonType> supportedDungeonTypes;
    
    std::vector<std::string> supportedThemes;
    std::vector<binary::Item::Element> supportedElements;
    
    uint32_t maxContentComplexity;
    bool supportsConditionalGeneration;
    bool supportsDeterministicGeneration;
    bool supportsThematicCoherence;
    bool supportsNarrativeIntegration;
};

/**
 * @brief Contexto para geração de conteúdo
 */
struct ContentGenerationContext {
    // Modelos de referência
    std::unordered_map<uint32_t, binary::Item> referenceItems;
    std::unordered_map<uint32_t, Monster> referenceMonsters;
    std::unordered_map<uint32_t, Quest> referenceQuests;
    std::unordered_map<uint32_t, Dialogue> referenceDialogues;
    std::unordered_map<uint32_t, DungeonLayout> referenceDungeons;
    
    // Configurações do mundo de jogo
    std::unordered_map<std::string, std::vector<std::string>> themeWords;
    std::unordered_map<binary::Item::Element, std::vector<std::string>> elementalThemes;
    
    // Unificação de IDs
    std::unordered_map<std::string, uint32_t> namedLocationsToIds;
    std::unordered_map<std::string, uint32_t> namedNpcsToIds;
    
    // Dados de balanceamento
    struct BalanceData {
        std::unordered_map<uint32_t, float> levelPowerCurve;
        std::unordered_map<binary::Item::Rarity, float> rarityMultipliers;
        std::unordered_map<MonsterGenerationParameters::MonsterType, float> monsterTypeMultipliers;
        std::unordered_map<uint32_t, float> questRewardScale;
    } balanceData;
    
    // Gerador de números aleatórios
    std::mt19937_64 rng;
    
    // Estado global da geração
    uint64_t masterSeed;
    std::unordered_map<std::string, std::any> persistentState;
    
    // Retroalimentação de conteúdo gerado
    std::vector<GeneratedContent> previouslyGeneratedContent;
    
    // Dicionários de nomes e frases
    std::unordered_map<std::string, std::vector<std::string>> nameParts;
    std::unordered_map<std::string, std::vector<std::string>> dialogueTemplates;
    std::unordered_map<std::string, std::vector<std::string>> questTemplates;
    
    // Inicializador
    void initializeFromGameData();
    void setSeed(uint64_t seed);
    
    // Utilitários
    std::string generateName(const std::vector<std::string>& namePrefixes,
                            const std::vector<std::string>& nameSuffixes);
    
    std::vector<std::string> getRandomThemeWords(const std::string& theme, int count);
    
    template<typename T>
    T getRandomValue(const std::vector<T>& values) {
        if (values.empty()) return T();
        std::uniform_int_distribution<size_t> dist(0, values.size() - 1);
        return values[dist(rng)];
    }
};

/**
 * @brief Modelo para geração de conteúdo
 */
class ContentModel {
public:
    enum class Type {
        ITEM_MODEL,
        MONSTER_MODEL,
        QUEST_MODEL,
        DIALOGUE_MODEL,
        DUNGEON_MODEL
    };
    
    ContentModel(Type type, const std::string& name);
    virtual ~ContentModel() = default;
    
    Type getType() const { return m_type; }
    std::string getName() const { return m_name; }
    
    virtual bool load(const std::string& modelPath) = 0;
    virtual bool initialize() = 0;
    
    virtual std::variant<binary::Item, Monster, Quest, Dialogue, DungeonLayout>
    generate(const std::variant<ItemGenerationParameters, 
                               MonsterGenerationParameters,
                               QuestGenerationParameters,
                               DialogueGenerationParameters,
                               DungeonGenerationParameters>& params,
             ContentGenerationContext& context) = 0;
             
    virtual float evaluateQuality(const std::variant<binary::Item, 
                                                   Monster, 
                                                   Quest, 
                                                   Dialogue, 
                                                   DungeonLayout>& content) = 0;
                                                   
    virtual std::string getModelInfo() const = 0;
    
protected:
    Type m_type;
    std::string m_name;
    std::string m_description;
    std::string m_version;
    bool m_initialized;
};

/**
 * @brief Configuração para geração de conteúdo
 */
struct GenerationConfiguration {
    std::string modelsDirectory;
    bool enableCaching;
    bool validateOutput;
    bool enableLogging;
    uint32_t generationTimeout;
    
    // Recursos específicos
    bool useRandomCraftingTrees;
    bool useDynamicMonsterScaling;
    bool useAdvancedQuestLogic;
    bool useComplexDialogueBranching;
    bool useDynamicDungeonGeneration;
    
    // Limites de recursos
    uint32_t maxGeneratedItemsPerBatch;
    uint32_t maxGeneratedMonstersPerBatch;
    uint32_t maxQuestComplexity;
    uint32_t maxDialogueNodes;
    uint32_t maxDungeonSize;
    
    // Balanceamento global
    float globalPowerScale;
    float globalComplexityScale;
    
    // Callbacks
    std::function<void(const std::string&)> logCallback;
    std::function<bool(const GeneratedContent&)> validationCallback;
    
    GenerationConfiguration() 
        : modelsDirectory("models"), 
          enableCaching(true),
          validateOutput(true),
          enableLogging(true),
          generationTimeout(30000),
          useRandomCraftingTrees(false),
          useDynamicMonsterScaling(true),
          useAdvancedQuestLogic(true),
          useComplexDialogueBranching(true),
          useDynamicDungeonGeneration(true),
          maxGeneratedItemsPerBatch(100),
          maxGeneratedMonstersPerBatch(50),
          maxQuestComplexity(10),
          maxDialogueNodes(50),
          maxDungeonSize(1000),
          globalPowerScale(1.0f),
          globalComplexityScale(1.0f) {}
};

/**
 * @brief Gerador de conteúdo procedural para o jogo
 */
class GameContentGenerator {
public:
    GameContentGenerator(const GenerationConfiguration& config);
    ~GameContentGenerator();
    
    // Inicialização
    bool initialize();
    bool loadModels(const std::string& directory);
    
    // Geração de item individual
    binary::Item generateItem(const ItemGenerationParameters& params);
    
    // Geração de monstro individual
    Monster generateMonster(const MonsterGenerationParameters& params);
    
    // Geração de quest individual
    Quest generateQuest(const QuestGenerationParameters& params);
    
    // Geração de diálogo individual
    Dialogue generateDialogue(const DialogueGenerationParameters& params);
    
    // Geração de dungeon
    DungeonLayout generateDungeon(const DungeonGenerationParameters& params);
    
    // Geração em lote
    std::vector<binary::Item> generateItemSet(uint32_t count, const ItemGenerationParameters& params);
    std::vector<Monster> generateMonsterGroup(uint32_t count, const MonsterGenerationParameters& params);
    std::vector<Quest> generateQuestChain(uint32_t count, const QuestGenerationParameters& params);
    std::vector<Dialogue> generateDialogueSet(uint32_t count, const DialogueGenerationParameters& params);
    
    // Geração temática coerente
    GeneratedContent generateThematicContent(const std::string& theme, uint32_t minLevel, uint32_t maxLevel);
    
    // Utilitários
    void setMasterSeed(uint64_t seed);
    uint64_t getMasterSeed() const;
    void randomizeSeed();
    
    // Exportação de conteúdo gerado
    bool exportGeneratedContent(const GeneratedContent& content, const std::string& outputPath);
    bool exportItem(const binary::Item& item, const std::string& outputPath);
    bool exportMonster(const Monster& monster, const std::string& outputPath);
    bool exportQuest(const Quest& quest, const std::string& outputPath);
    bool exportDialogue(const Dialogue& dialogue, const std::string& outputPath);
    bool exportDungeon(const DungeonLayout& dungeon, const std::string& outputPath);
    
    // Informações sobre capacidades
    std::vector<std::string> getSupportedContentTypes() const;
    GenerationCapabilities getCapabilities() const;
    
    // Validação de conteúdo
    bool validateGeneratedContent(const GeneratedContent& content, std::string& error);
    bool validateItem(const binary::Item& item, std::string& error);
    bool validateMonster(const Monster& monster, std::string& error);
    bool validateQuest(const Quest& quest, std::string& error);
    bool validateDialogue(const Dialogue& dialogue, std::string& error);
    bool validateDungeon(const DungeonLayout& dungeon, std::string& error);
    
    // Acesso ao contexto
    ContentGenerationContext& getContext() { return m_context; }
    const ContentGenerationContext& getContext() const { return m_context; }
    
    // Estado e configuração
    const GenerationConfiguration& getConfiguration() const { return m_config; }
    void updateConfiguration(const GenerationConfiguration& config);
    std::string getLastError() const { return m_lastError; }
    
private:
    GenerationConfiguration m_config;
    ContentGenerationContext m_context;
    std::string m_lastError;
    
    std::unordered_map<std::string, std::shared_ptr<ContentModel>> m_models;
    bool m_initialized;
    
    // Modelos organizados por tipo
    std::vector<std::shared_ptr<ContentModel>> m_itemModels;
    std::vector<std::shared_ptr<ContentModel>> m_monsterModels;
    std::vector<std::shared_ptr<ContentModel>> m_questModels;
    std::vector<std::shared_ptr<ContentModel>> m_dialogueModels;
    std::vector<std::shared_ptr<ContentModel>> m_dungeonModels;
    
    // Cache de conteúdo
    struct ContentCache {
        std::unordered_map<std::string, binary::Item> items;
        std::unordered_map<std::string, Monster> monsters;
        std::unordered_map<std::string, Quest> quests;
        std::unordered_map<std::string, Dialogue> dialogues;
        std::unordered_map<std::string, DungeonLayout> dungeons;
        
        void clear() {
            items.clear();
            monsters.clear();
            quests.clear();
            dialogues.clear();
            dungeons.clear();
        }
    } m_cache;
    
    // Métodos para gestão de modelos
    bool loadContentModel(const std::string& modelPath);
    void categorizeModels();
    
    // Seleção de modelo
    std::shared_ptr<ContentModel> selectBestModelForItem(const ItemGenerationParameters& params);
    std::shared_ptr<ContentModel> selectBestModelForMonster(const MonsterGenerationParameters& params);
    std::shared_ptr<ContentModel> selectBestModelForQuest(const QuestGenerationParameters& params);
    std::shared_ptr<ContentModel> selectBestModelForDialogue(const DialogueGenerationParameters& params);
    std::shared_ptr<ContentModel> selectBestModelForDungeon(const DungeonGenerationParameters& params);
    
    // Validação de parâmetros
    bool validateItemParameters(const ItemGenerationParameters& params, std::string& error);
    bool validateMonsterParameters(const MonsterGenerationParameters& params, std::string& error);
    bool validateQuestParameters(const QuestGenerationParameters& params, std::string& error);
    bool validateDialogueParameters(const DialogueGenerationParameters& params, std::string& error);
    bool validateDungeonParameters(const DungeonGenerationParameters& params, std::string& error);
    
    // Processamento pós-geração
    binary::Item postProcessItem(binary::Item item);
    Monster postProcessMonster(Monster monster);
    Quest postProcessQuest(Quest quest);
    Dialogue postProcessDialogue(Dialogue dialogue);
    DungeonLayout postProcessDungeon(DungeonLayout dungeon);
    
    // Métodos específicos de geração
    std::vector<ItemDropEntry> generateDropsForMonster(const Monster& monster);
    std::vector<QuestStep> generateQuestSteps(const QuestGenerationParameters& params, uint32_t stepCount);
    std::vector<DialogueNode> generateDialogueTree(const DialogueGenerationParameters& params);
    std::vector<Room> generateRoomsForDungeon(const DungeonGenerationParameters& params);
    
    // Utilitários internos
    std::string generateHash(const std::string& input) const;
    uint32_t generateUniqueId(const std::string& prefix) const;
    void logGenerationEvent(const std::string& message);
    void validateOutputConsistency(const GeneratedContent& content);
};

} // namespace ai
} // namespace wydstudio

#endif // WYDSTUDIO_GAME_CONTENT_GENERATOR_H