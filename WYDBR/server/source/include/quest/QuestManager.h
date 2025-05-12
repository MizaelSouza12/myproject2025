/**
 * QuestManager.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/include/quest/QuestManager.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef _QUEST_MANAGER_H_
#define _QUEST_MANAGER_H_

/**
 * @file QuestManager.h
 * @brief Gerenciador de missões do WYD
 * 
 * Este arquivo contém o sistema de gerenciamento de missões do WYDBR,
 * responsável por controlar as missões dos jogadores,
 * mantendo compatibilidade binária com o cliente original.
 */

#include <stdint.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <memory>
#include <mutex>
#include <atomic>
#include <functional>

#include "../GlobalDef.h"
#include "../Basedef.h"

namespace wydbr {
namespace quest {

/**
 * @brief Estado da missão
 */
enum /**
 * Classe WYDQuestState
 * 
 * Esta classe implementa a funcionalidade WYDQuestState conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class QuestState {
    NONE = 0,                   // Nenhum
    AVAILABLE = 1,              // Disponível
    ACCEPTED = 2,               // Aceita
    IN_PROGRESS = 3,            // Em progresso
    COMPLETED = 4,              // Completa
    FAILED = 5,                 // Falha
    COMPLETED_NOT_REWARDED = 6, // Completa, não recompensada
    EXPIRED = 7,                // Expirada
    ABANDONED = 8,              // Abandonada
    LOCKED = 9,                 // Bloqueada
    HIDDEN = 10,                // Escondida
    CUSTOM_1 = 11,              // Personalizado 1
    CUSTOM_2 = 12,              // Personalizado 2
    CUSTOM_3 = 13,              // Personalizado 3
    UNKNOWN = 14                // Desconhecido
};

/**
 * @brief Tipo de missão
 */
enum /**
 * Classe WYDQuestType
 * 
 * Esta classe implementa a funcionalidade WYDQuestType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class QuestType {
    NONE = 0,                   // Nenhum
    MAIN = 1,                   // Principal
    SIDE = 2,                   // Secundária
    DAILY = 3,                  // Diária
    WEEKLY = 4,                 // Semanal
    REPEATABLE = 5,             // Repetível
    ACHIEVEMENT = 6,            // Conquista
    EVENT = 7,                  // Evento
    GUILD = 8,                  // Guilda
    STORY = 9,                  // História
    HIDDEN = 10,                // Escondida
    TUTORIAL = 11,              // Tutorial
    GROUP = 12,                 // Grupo
    SOLO = 13,                  // Solo
    DUNGEON = 14,               // Masmorra
    RAID = 15,                  // Raid
    WORLD = 16,                 // Mundo
    ELITE = 17,                 // Elite
    CUSTOM_1 = 18,              // Personalizado 1
    CUSTOM_2 = 19,              // Personalizado 2
    CUSTOM_3 = 20,              // Personalizado 3
    UNKNOWN = 21                // Desconhecido
};

/**
 * @brief Tipo de objetivo de missão
 */
enum /**
 * Classe WYDQuestObjectiveType
 * 
 * Esta classe implementa a funcionalidade WYDQuestObjectiveType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class QuestObjectiveType {
    NONE = 0,                   // Nenhum
    KILL = 1,                   // Matar
    COLLECT = 2,                // Coletar
    ESCORT = 3,                 // Escoltar
    DELIVER = 4,                // Entregar
    INTERACT = 5,               // Interagir
    EXPLORE = 6,                // Explorar
    USE_SKILL = 7,              // Usar habilidade
    USE_ITEM = 8,               // Usar item
    REACH_LEVEL = 9,            // Alcançar nível
    COMPLETE_QUEST = 10,        // Completar missão
    DEFEAT_BOSS = 11,           // Derrotar chefe
    REACH_DESTINATION = 12,     // Alcançar destino
    PROTECT = 13,               // Proteger
    SURVIVE = 14,               // Sobreviver
    WIN_PVP = 15,               // Vencer PvP
    TALK_TO_NPC = 16,           // Falar com NPC
    CRAFT_ITEM = 17,            // Fabricar item
    ENHANCE_ITEM = 18,          // Melhorar item
    JOIN_GUILD = 19,            // Entrar em guilda
    REACH_RANK = 20,            // Alcançar rank
    CUSTOM_1 = 21,              // Personalizado 1
    CUSTOM_2 = 22,              // Personalizado 2
    CUSTOM_3 = 23,              // Personalizado 3
    UNKNOWN = 24                // Desconhecido
};

/**
 * @brief Tipo de recompensa de missão
 */
enum /**
 * Classe WYDQuestRewardType
 * 
 * Esta classe implementa a funcionalidade WYDQuestRewardType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class QuestRewardType {
    NONE = 0,                   // Nenhum
    ITEM = 1,                   // Item
    GOLD = 2,                   // Ouro
    EXPERIENCE = 3,             // Experiência
    SKILL = 4,                  // Habilidade
    TITLE = 5,                  // Título
    REPUTATION = 6,             // Reputação
    HONOR = 7,                  // Honra
    TOKEN = 8,                  // Token
    ATTRIBUTE = 9,              // Atributo
    MOUNT = 10,                 // Montaria
    PET = 11,                   // Pet
    TELEPORT = 12,              // Teleporte
    RECIPE = 13,                // Receita
    CURRENCY = 14,              // Moeda
    BUFF = 15,                  // Buff
    HOUSE = 16,                 // Casa
    CUSTOM_1 = 17,              // Personalizado 1
    CUSTOM_2 = 18,              // Personalizado 2
    CUSTOM_3 = 19,              // Personalizado 3
    UNKNOWN = 20                // Desconhecido
};

/**
 * @brief Tipo de requerimento de missão
 */
enum /**
 * Classe WYDQuestRequirementType
 * 
 * Esta classe implementa a funcionalidade WYDQuestRequirementType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class QuestRequirementType {
    NONE = 0,                   // Nenhum
    LEVEL = 1,                  // Nível
    CLASS = 2,                  // Classe
    RACE = 3,                   // Raça
    REPUTATION = 4,             // Reputação
    SKILL = 5,                  // Habilidade
    ITEM = 6,                   // Item
    GOLD = 7,                   // Ouro
    GUILD = 8,                  // Guilda
    QUEST = 9,                  // Missão
    STAT = 10,                  // Atributo
    MAP = 11,                   // Mapa
    TIME = 12,                  // Tempo
    CUSTOM_1 = 13,              // Personalizado 1
    CUSTOM_2 = 14,              // Personalizado 2
    CUSTOM_3 = 15,              // Personalizado 3
    UNKNOWN = 16                // Desconhecido
};

/**
 * @brief Tipo de evento de missão
 */
enum /**
 * Classe WYDQuestEventType
 * 
 * Esta classe implementa a funcionalidade WYDQuestEventType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class QuestEventType {
    NONE = 0,                   // Nenhum
    ACCEPTED = 1,               // Aceita
    COMPLETED = 2,              // Completa
    FAILED = 3,                 // Falha
    ABANDONED = 4,              // Abandonada
    EXPIRED = 5,                // Expirada
    REWARDED = 6,               // Recompensada
    OBJECTIVE_UPDATED = 7,      // Objetivo atualizado
    OBJECTIVE_COMPLETED = 8,    // Objetivo completo
    UNLOCKED = 9,               // Desbloqueada
    RESET = 10,                 // Resetada
    TRACKED = 11,               // Rastreada
    UNTRACKED = 12,             // Não rastreada
    CUSTOM_1 = 13,              // Personalizado 1
    CUSTOM_2 = 14,              // Personalizado 2
    CUSTOM_3 = 15,              // Personalizado 3
    UNKNOWN = 16                // Desconhecido
};

/**
 * @brief Objetivo de missão
 */
struct QuestObjective {
    uint32_t ObjectiveID;       // ID do objetivo
    std::string Description;    // Descrição
    QuestObjectiveType Type;    // Tipo
    uint32_t TargetID;          // ID do alvo
    uint32_t Count;             // Contagem
    bool Required;              // Requerido
    bool Hidden;                // Escondido
    uint16_t MapID;             // ID do mapa
    uint16_t PosX;              // Posição X
    uint16_t PosY;              // Posição Y
    uint16_t Radius;            // Raio
    uint32_t TimeLimit;         // Limite de tempo
    std::string Script;         // Script
    
    QuestObjective() : ObjectiveID(0), Type(QuestObjectiveType::NONE),
        TargetID(0), Count(0), Required(true), Hidden(false),
        MapID(0), PosX(0), PosY(0), Radius(0), TimeLimit(0) {}
};

/**
 * @brief Progresso de objetivo de missão
 */
struct QuestObjectiveProgress {
    uint32_t ObjectiveID;       // ID do objetivo
    uint32_t Progress;          // Progresso
    uint32_t StartTime;         // Tempo de início
    bool Completed;             // Completo
    
    QuestObjectiveProgress() : ObjectiveID(0), Progress(0),
        StartTime(0), Completed(false) {}
};

/**
 * @brief Recompensa de missão
 */
struct QuestReward {
    QuestRewardType Type;       // Tipo
    uint32_t ID;                // ID
    uint32_t Amount;            // Quantidade
    uint8_t Chance;             // Chance
    uint8_t Quality;            // Qualidade
    bool Optional;              // Opcional
    std::string Description;    // Descrição
    
    QuestReward() : Type(QuestRewardType::NONE), ID(0), Amount(0),
        Chance(100), Quality(0), Optional(false) {}
};

/**
 * @brief Requerimento de missão
 */
struct QuestRequirement {
    QuestRequirementType Type;  // Tipo
    uint32_t ID;                // ID
    uint32_t Value;             // Valor
    bool Required;              // Requerido
    std::string Description;    // Descrição
    
    QuestRequirement() : Type(QuestRequirementType::NONE), ID(0),
        Value(0), Required(true) {}
};

/**
 * @brief NPC de missão
 */
struct QuestNPC {
    uint32_t NPCID;             // ID do NPC
    bool IsQuestGiver;          // É dador de missão
    bool IsQuestReceiver;       // É receptor de missão
    bool IsObjectiveTarget;     // É alvo de objetivo
    std::string DialogStart;    // Diálogo de início
    std::string DialogComplete; // Diálogo de conclusão
    std::string DialogInProgress; // Diálogo em progresso
    
    QuestNPC() : NPCID(0), IsQuestGiver(false), IsQuestReceiver(false),
        IsObjectiveTarget(false) {}
};

/**
 * @brief Configuração de missão
 */
struct QuestConfig {
    uint32_t QuestID;           // ID da missão
    std::string Name;           // Nome
    std::string Description;    // Descrição
    QuestType Type;             // Tipo
    uint8_t MinLevel;           // Nível mínimo
    uint8_t MaxLevel;           // Nível máximo
    uint8_t RecommendedLevel;   // Nível recomendado
    uint8_t RequiredPartySize;  // Tamanho de grupo requerido
    uint32_t ExpirationTime;    // Tempo de expiração
    uint32_t CooldownTime;      // Tempo de cooldown
    bool Repeatable;            // Repetível
    bool AutoAccept;            // Aceite automático
    bool AutoComplete;          // Conclusão automática
    bool AutoTrack;             // Rastreamento automático
    bool ShareableWithParty;    // Compartilhável com grupo
    std::vector<QuestObjective> Objectives; // Objetivos
    std::vector<QuestReward> Rewards; // Recompensas
    std::vector<QuestRequirement> Requirements; // Requerimentos
    std::vector<QuestNPC> NPCs; // NPCs
    std::vector<uint32_t> PrerequisiteQuests; // Missões pré-requisito
    std::vector<uint32_t> FollowupQuests; // Missões de continuação
    std::vector<uint32_t> ExclusiveQuests; // Missões exclusivas
    uint8_t RepeatLimit;        // Limite de repetição
    uint16_t QuestChainID;      // ID da cadeia de missões
    uint8_t QuestChainStep;     // Passo na cadeia de missões
    bool Enabled;               // Habilitada
    std::string Script;         // Script
    
    QuestConfig() : QuestID(0), Type(QuestType::NONE), MinLevel(0),
        MaxLevel(0), RecommendedLevel(0), RequiredPartySize(0),
        ExpirationTime(0), CooldownTime(0), Repeatable(false),
        AutoAccept(false), AutoComplete(false), AutoTrack(false),
        ShareableWithParty(false), RepeatLimit(0), QuestChainID(0),
        QuestChainStep(0), Enabled(true) {}
};

/**
 * @brief Instância de missão de jogador
 */
struct QuestInstance {
    uint32_t InstanceID;        // ID da instância
    uint32_t QuestID;           // ID da missão
    uint32_t CharacterID;       // ID do personagem
    QuestState State;           // Estado
    uint32_t AcceptTime;        // Tempo de aceitação
    uint32_t CompleteTime;      // Tempo de conclusão
    uint32_t ExpireTime;        // Tempo de expiração
    uint32_t FailTime;          // Tempo de falha
    uint32_t AbandonTime;       // Tempo de abandono
    uint32_t RewardTime;        // Tempo de recompensa
    uint8_t CompletionCount;    // Contagem de conclusão
    bool Tracked;               // Rastreada
    std::vector<QuestObjectiveProgress> ObjectiveProgress; // Progresso de objetivos
    std::vector<QuestReward> SelectedRewards; // Recompensas selecionadas
    
    QuestInstance() : InstanceID(0), QuestID(0), CharacterID(0),
        State(QuestState::NONE), AcceptTime(0), CompleteTime(0),
        ExpireTime(0), FailTime(0), AbandonTime(0), RewardTime(0),
        CompletionCount(0), Tracked(false) {}
};

/**
 * @brief Log de missão
 */
struct QuestLog {
    uint32_t LogID;             // ID do log
    uint32_t InstanceID;        // ID da instância
    QuestEventType EventType;   // Tipo de evento
    uint32_t Timestamp;         // Timestamp
    std::string Message;        // Mensagem
    
    QuestLog() : LogID(0), InstanceID(0), EventType(QuestEventType::NONE),
        Timestamp(0) {}
};

/**
 * @brief Evento de missão
 */
struct QuestEvent {
    uint32_t EventID;           // ID do evento
    QuestEventType Type;        // Tipo
    uint32_t QuestID;           // ID da missão
    uint32_t CharacterID;       // ID do personagem
    uint32_t InstanceID;        // ID da instância
    uint32_t ObjectiveID;       // ID do objetivo
    uint32_t Progress;          // Progresso
    uint32_t Timestamp;         // Timestamp
    
    QuestEvent() : EventID(0), Type(QuestEventType::NONE), QuestID(0),
        CharacterID(0), InstanceID(0), ObjectiveID(0), Progress(0),
        Timestamp(0) {}
    
    QuestEvent(QuestEventType _type, uint32_t _questID, uint32_t _characterID,
              uint32_t _instanceID = 0, uint32_t _objectiveID = 0, uint32_t _progress = 0)
        : EventID(0), Type(_type), QuestID(_questID), CharacterID(_characterID),
          InstanceID(_instanceID), ObjectiveID(_objectiveID), Progress(_progress),
          Timestamp(static_cast<uint32_t>(time(nullptr))) {}
};

/**
 * @brief Cadeia de missões
 */
struct QuestChain {
    uint16_t ChainID;           // ID da cadeia
    std::string Name;           // Nome
    std::string Description;    // Descrição
    std::vector<uint32_t> Quests; // Missões
    
    QuestChain() /**
 * ChainID
 * 
 * Implementa a funcionalidade ChainID conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna :
 */
 : ChainID(0){}
};

/**
 * @brief Tipo de callback de missão
 */
using QuestCallback = std::function<void(const QuestEvent&)>;

/**
 * @brief Tipo de callback de objetivo
 */
using ObjectiveCallback = std::function<bool(const QuestObjective&, const QuestObjectiveProgress&, uint32_t)>;

/**
 * @brief Tipo de callback de recompensa
 */
using RewardCallback = std::function<bool(const QuestReward&, uint32_t)>;

/**
 * @brief Gerenciador de missões
 * 
 * Esta classe é responsável por gerenciar as missões dos jogadores.
 */
/**
 * Classe WYDQuestManager
 * 
 * Esta classe implementa a funcionalidade WYDQuestManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class QuestManager {
public:
    /**
     * @brief Obtém a instância única do gerenciador
     * @return Instância
     */
    static QuestManager& getInstance() {
        static QuestManager instance;
        return instance;
    }
    
    /**
     * @brief Inicializa o gerenciador
     * @return true se inicializado com sucesso
     */
    bool initialize();
    
    /**
     * @brief Finaliza o gerenciador
     */
    void shutdown();
    
    /**
     * @brief Atualiza o gerenciador
     * @param deltaTime Tempo decorrido em milissegundos
     */
    void update(uint32_t deltaTime);
    
    /**
     * @brief Carrega a configuração de missões
     * @return true se carregado com sucesso
     */
    bool loadQuestConfig();
    
    /**
     * @brief Cria uma missão
     * @param config Configuração
     * @return ID da missão ou 0 em caso de erro
     */
    uint32_t createQuest(const QuestConfig& config);
    
    /**
     * @brief Remove uma missão
     * @param questID ID da missão
     * @return true se removido com sucesso
     */
    bool removeQuest(uint32_t questID);
    
    /**
     * @brief Atualiza uma missão
     * @param config Configuração
     * @return true se atualizado com sucesso
     */
    bool updateQuest(const QuestConfig& config);
    
    /**
     * @brief Habilita ou desabilita uma missão
     * @param questID ID da missão
     * @param enabled Habilitada
     * @return true se alterado com sucesso
     */
    bool enableQuest(uint32_t questID, bool enabled);
    
    /**
     * @brief Obtém a configuração de uma missão
     * @param questID ID da missão
     * @return Configuração
     */
    QuestConfig getQuestConfig(uint32_t questID) const;
    
    /**
     * @brief Aceita uma missão
     * @param characterID ID do personagem
     * @param questID ID da missão
     * @return ID da instância ou 0 em caso de erro
     */
    uint32_t acceptQuest(uint32_t characterID, uint32_t questID);
    
    /**
     * @brief Completa uma missão
     * @param characterID ID do personagem
     * @param questID ID da missão
     * @return true se completado com sucesso
     */
    bool completeQuest(uint32_t characterID, uint32_t questID);
    
    /**
     * @brief Falha uma missão
     * @param characterID ID do personagem
     * @param questID ID da missão
     * @return true se falhado com sucesso
     */
    bool failQuest(uint32_t characterID, uint32_t questID);
    
    /**
     * @brief Abandona uma missão
     * @param characterID ID do personagem
     * @param questID ID da missão
     * @return true se abandonado com sucesso
     */
    bool abandonQuest(uint32_t characterID, uint32_t questID);
    
    /**
     * @brief Recompensa uma missão
     * @param characterID ID do personagem
     * @param questID ID da missão
     * @param selectedRewards Recompensas selecionadas
     * @return true se recompensado com sucesso
     */
    bool rewardQuest(uint32_t characterID, uint32_t questID, const std::vector<QuestReward>& selectedRewards);
    
    /**
     * @brief Reseta uma missão
     * @param characterID ID do personagem
     * @param questID ID da missão
     * @return true se resetado com sucesso
     */
    bool resetQuest(uint32_t characterID, uint32_t questID);
    
    /**
     * @brief Atualiza o progresso de um objetivo
     * @param characterID ID do personagem
     * @param questID ID da missão
     * @param objectiveID ID do objetivo
     * @param progress Progresso
     * @param forceUpdate Forçar atualização
     * @return true se atualizado com sucesso
     */
    bool updateObjectiveProgress(uint32_t characterID, uint32_t questID, uint32_t objectiveID, uint32_t progress, bool forceUpdate = false);
    
    /**
     * @brief Incrementa o progresso de um objetivo por tipo
     * @param characterID ID do personagem
     * @param type Tipo de objetivo
     * @param targetID ID do alvo
     * @param amount Quantidade
     * @return Número de objetivos atualizados
     */
    uint32_t incrementObjectiveProgress(uint32_t characterID, QuestObjectiveType type, uint32_t targetID, uint32_t amount = 1);
    
    /**
     * @brief Completa um objetivo
     * @param characterID ID do personagem
     * @param questID ID da missão
     * @param objectiveID ID do objetivo
     * @return true se completado com sucesso
     */
    bool completeObjective(uint32_t characterID, uint32_t questID, uint32_t objectiveID);
    
    /**
     * @brief Marca ou desmarca uma missão para rastreamento
     * @param characterID ID do personagem
     * @param questID ID da missão
     * @param tracked Rastreada
     * @return true se alterado com sucesso
     */
    bool trackQuest(uint32_t characterID, uint32_t questID, bool tracked);
    
    /**
     * @brief Obtém as missões disponíveis para um personagem
     * @param characterID ID do personagem
     * @return Lista de IDs de missões
     */
    std::vector<uint32_t> getAvailableQuests(uint32_t characterID);
    
    /**
     * @brief Obtém as missões ativas de um personagem
     * @param characterID ID do personagem
     * @return Lista de IDs de missões
     */
    std::vector<uint32_t> getActiveQuests(uint32_t characterID);
    
    /**
     * @brief Obtém as missões completas de um personagem
     * @param characterID ID do personagem
     * @return Lista de IDs de missões
     */
    std::vector<uint32_t> getCompletedQuests(uint32_t characterID);
    
    /**
     * @brief Obtém as missões rastreadas de um personagem
     * @param characterID ID do personagem
     * @return Lista de IDs de missões
     */
    std::vector<uint32_t> getTrackedQuests(uint32_t characterID);
    
    /**
     * @brief Obtém uma instância de missão
     * @param characterID ID do personagem
     * @param questID ID da missão
     * @return Instância
     */
    QuestInstance getQuestInstance(uint32_t characterID, uint32_t questID) const;
    
    /**
     * @brief Obtém o progresso de um objetivo
     * @param characterID ID do personagem
     * @param questID ID da missão
     * @param objectiveID ID do objetivo
     * @return Progresso
     */
    QuestObjectiveProgress getObjectiveProgress(uint32_t characterID, uint32_t questID, uint32_t objectiveID) const;
    
    /**
     * @brief Obtém o estado de uma missão
     * @param characterID ID do personagem
     * @param questID ID da missão
     * @return Estado
     */
    QuestState getQuestState(uint32_t characterID, uint32_t questID) const;
    
    /**
     * @brief Verifica se uma missão está completa
     * @param characterID ID do personagem
     * @param questID ID da missão
     * @return true se estiver completa
     */
    bool isQuestComplete(uint32_t characterID, uint32_t questID) const;
    
    /**
     * @brief Verifica se uma missão está disponível
     * @param characterID ID do personagem
     * @param questID ID da missão
     * @return true se estiver disponível
     */
    bool isQuestAvailable(uint32_t characterID, uint32_t questID) const;
    
    /**
     * @brief Verifica se uma missão foi completada
     * @param characterID ID do personagem
     * @param questID ID da missão
     * @return true se tiver sido completada
     */
    bool hasCompletedQuest(uint32_t characterID, uint32_t questID) const;
    
    /**
     * @brief Verifica se um personagem atende aos requisitos de uma missão
     * @param characterID ID do personagem
     * @param questID ID da missão
     * @return true se atender
     */
    bool meetsQuestRequirements(uint32_t characterID, uint32_t questID) const;
    
    /**
     * @brief Verifica se uma missão é repetível para um personagem
     * @param characterID ID do personagem
     * @param questID ID da missão
     * @return true se for repetível
     */
    bool isQuestRepeatable(uint32_t characterID, uint32_t questID) const;
    
    /**
     * @brief Obtém a contagem de conclusão de uma missão
     * @param characterID ID do personagem
     * @param questID ID da missão
     * @return Contagem de conclusão
     */
    uint8_t getQuestCompletionCount(uint32_t characterID, uint32_t questID) const;
    
    /**
     * @brief Obtém os logs de uma missão
     * @param characterID ID do personagem
     * @param questID ID da missão
     * @return Lista de logs
     */
    std::vector<QuestLog> getQuestLogs(uint32_t characterID, uint32_t questID) const;
    
    /**
     * @brief Obtém as recompensas de uma missão
     * @param questID ID da missão
     * @return Lista de recompensas
     */
    std::vector<QuestReward> getQuestRewards(uint32_t questID) const;
    
    /**
     * @brief Obtém os objetivos de uma missão
     * @param questID ID da missão
     * @return Lista de objetivos
     */
    std::vector<QuestObjective> getQuestObjectives(uint32_t questID) const;
    
    /**
     * @brief Obtém os requisitos de uma missão
     * @param questID ID da missão
     * @return Lista de requisitos
     */
    std::vector<QuestRequirement> getQuestRequirements(uint32_t questID) const;
    
    /**
     * @brief Obtém os NPCs de uma missão
     * @param questID ID da missão
     * @return Lista de NPCs
     */
    std::vector<QuestNPC> getQuestNPCs(uint32_t questID) const;
    
    /**
     * @brief Obtém as missões que um NPC oferece
     * @param npcID ID do NPC
     * @return Lista de IDs de missões
     */
    std::vector<uint32_t> getQuestsByNPC(uint32_t npcID) const;
    
    /**
     * @brief Obtém as missões por tipo
     * @param type Tipo
     * @return Lista de IDs de missões
     */
    std::vector<uint32_t> getQuestsByType(QuestType type) const;
    
    /**
     * @brief Obtém as missões por nível
     * @param minLevel Nível mínimo
     * @param maxLevel Nível máximo
     * @return Lista de IDs de missões
     */
    std::vector<uint32_t> getQuestsByLevel(uint8_t minLevel, uint8_t maxLevel) const;
    
    /**
     * @brief Obtém as missões por cadeia
     * @param chainID ID da cadeia
     * @return Lista de IDs de missões
     */
    std::vector<uint32_t> getQuestsByChain(uint16_t chainID) const;
    
    /**
     * @brief Cria uma cadeia de missões
     * @param name Nome
     * @param description Descrição
     * @param quests Missões
     * @return ID da cadeia ou 0 em caso de erro
     */
    uint16_t createQuestChain(const std::string& name, const std::string& description, const std::vector<uint32_t>& quests);
    
    /**
     * @brief Remove uma cadeia de missões
     * @param chainID ID da cadeia
     * @return true se removido com sucesso
     */
    bool removeQuestChain(uint16_t chainID);
    
    /**
     * @brief Atualiza uma cadeia de missões
     * @param chainID ID da cadeia
     * @param name Nome
     * @param description Descrição
     * @param quests Missões
     * @return true se atualizado com sucesso
     */
    bool updateQuestChain(uint16_t chainID, const std::string& name, const std::string& description, const std::vector<uint32_t>& quests);
    
    /**
     * @brief Obtém uma cadeia de missões
     * @param chainID ID da cadeia
     * @return Cadeia
     */
    QuestChain getQuestChain(uint16_t chainID) const;
    
    /**
     * @brief Registra um callback de missão
     * @param questID ID da missão
     * @param callback Função de callback
     * @return ID do callback ou 0 em caso de erro
     */
    uint32_t registerQuestCallback(uint32_t questID, QuestCallback callback);
    
    /**
     * @brief Remove um callback de missão
     * @param callbackID ID do callback
     * @return true se removido com sucesso
     */
    bool unregisterQuestCallback(uint32_t callbackID);
    
    /**
     * @brief Registra um callback de evento
     * @param eventType Tipo de evento
     * @param callback Função de callback
     * @return ID do callback ou 0 em caso de erro
     */
    uint32_t registerEventCallback(QuestEventType eventType, QuestCallback callback);
    
    /**
     * @brief Remove um callback de evento
     * @param callbackID ID do callback
     * @return true se removido com sucesso
     */
    bool unregisterEventCallback(uint32_t callbackID);
    
    /**
     * @brief Registra um callback de objetivo
     * @param objectiveType Tipo de objetivo
     * @param callback Função de callback
     * @return ID do callback ou 0 em caso de erro
     */
    uint32_t registerObjectiveCallback(QuestObjectiveType objectiveType, ObjectiveCallback callback);
    
    /**
     * @brief Remove um callback de objetivo
     * @param callbackID ID do callback
     * @return true se removido com sucesso
     */
    bool unregisterObjectiveCallback(uint32_t callbackID);
    
    /**
     * @brief Registra um callback de recompensa
     * @param rewardType Tipo de recompensa
     * @param callback Função de callback
     * @return ID do callback ou 0 em caso de erro
     */
    uint32_t registerRewardCallback(QuestRewardType rewardType, RewardCallback callback);
    
    /**
     * @brief Remove um callback de recompensa
     * @param callbackID ID do callback
     * @return true se removido com sucesso
     */
    bool unregisterRewardCallback(uint32_t callbackID);
    
    /**
     * @brief Loga um evento
     * @param event Evento
     * @return ID do log ou 0 em caso de erro
     */
    uint32_t logEvent(const QuestEvent& event);
    
    /**
     * @brief Loga uma mensagem
     * @param characterID ID do personagem
     * @param questID ID da missão
     * @param message Mensagem
     * @param eventType Tipo de evento
     * @return ID do log ou 0 em caso de erro
     */
    uint32_t logMessage(uint32_t characterID, uint32_t questID, const std::string& message, QuestEventType eventType = QuestEventType::NONE);
    
    /**
     * @brief Define o modo de debug
     * @param debug Modo de debug
     */
    void setDebugMode(bool debug);
    
    /**
     * @brief Verifica se o modo de debug está ativado
     * @return true se estiver ativado
     */
    bool isDebugMode() const;
    
    /**
     * @brief Define o modo de log
     * @param logging Modo de log
     */
    void setLoggingEnabled(bool logging);
    
    /**
     * @brief Verifica se o modo de log está ativado
     * @return true se estiver ativado
     */
    bool isLoggingEnabled() const;
    
private:
    /**
     * @brief Construtor privado
     */
    QuestManager();
    
    /**
     * @brief Destrutor privado
     */
    ~QuestManager();
    
    /**
     * @brief Construtor de cópia privado
     */
    QuestManager(const QuestManager&) = delete;
    
    /**
     * @brief Operador de atribuição privado
     */
    QuestManager& operator=(const QuestManager&) = delete;
    
    // Missões
    std::unordered_map<uint32_t, QuestConfig> _quests;
    std::atomic<uint32_t> _nextQuestID;
    
    // Instâncias
    std::unordered_map<uint32_t, std::unordered_map<uint32_t, QuestInstance>> _questInstances;
    std::atomic<uint32_t> _nextInstanceID;
    
    // Logs
    std::unordered_map<uint32_t, std::vector<QuestLog>> _questLogs;
    std::atomic<uint32_t> _nextLogID;
    
    // Cadeias
    std::unordered_map<uint16_t, QuestChain> _questChains;
    std::atomic<uint16_t> _nextChainID;
    
    // NPCs
    std::unordered_map<uint32_t, std::vector<uint32_t>> _npcQuests;
    
    // Tipos
    std::unordered_map<QuestType, std::vector<uint32_t>> _questsByType;
    
    // Callbacks
    std::unordered_map<uint32_t, QuestCallback> _questCallbacks;
    std::unordered_map<uint32_t, std::vector<uint32_t>> _questCallbacksByQuest;
    std::unordered_map<uint32_t, QuestCallback> _eventCallbacks;
    std::unordered_map<QuestEventType, std::vector<uint32_t>> _eventCallbacksByType;
    std::unordered_map<uint32_t, ObjectiveCallback> _objectiveCallbacks;
    std::unordered_map<QuestObjectiveType, std::vector<uint32_t>> _objectiveCallbacksByType;
    std::unordered_map<uint32_t, RewardCallback> _rewardCallbacks;
    std::unordered_map<QuestRewardType, std::vector<uint32_t>> _rewardCallbacksByType;
    std::atomic<uint32_t> _nextCallbackID;
    
    // Thread safety
    mutable std::mutex _questMutex;
    mutable std::mutex _instanceMutex;
    mutable std::mutex _logMutex;
    mutable std::mutex _chainMutex;
    mutable std::mutex _callbackMutex;
    
    // Flags
    std::atomic<bool> _initialized;
    std::atomic<bool> _debugMode;
    std::atomic<bool> _loggingEnabled;
    
    /**
     * @brief Verifica missões para expiração
     * @param currentTime Tempo atual
     */
    void checkQuestsForExpiration(uint32_t currentTime);
    
    /**
     * @brief Verifica se todos os objetivos de uma missão estão completos
     * @param characterID ID do personagem
     * @param questID ID da missão
     * @return true se todos estiverem completos
     */
    bool checkQuestCompletion(uint32_t characterID, uint32_t questID);
    
    /**
     * @brief Executa callbacks de evento
     * @param event Evento
     */
    void executeEventCallbacks(const QuestEvent& event);
    
    /**
     * @brief Executa callbacks de objetivo
     * @param objective Objetivo
     * @param progress Progresso
     * @param characterID ID do personagem
     * @return true se processado com sucesso
     */
    bool executeObjectiveCallbacks(const QuestObjective& objective, const QuestObjectiveProgress& progress, uint32_t characterID);
    
    /**
     * @brief Executa callbacks de recompensa
     * @param reward Recompensa
     * @param characterID ID do personagem
     * @return true se processado com sucesso
     */
    bool executeRewardCallbacks(const QuestReward& reward, uint32_t characterID);
    
    /**
     * @brief Gera uma instância de missão
     * @param characterID ID do personagem
     * @param questID ID da missão
     * @return Instância
     */
    QuestInstance createQuestInstance(uint32_t characterID, uint32_t questID);
    
    /**
     * @brief Processa recompensas de missão
     * @param characterID ID do personagem
     * @param questID ID da missão
     * @param selectedRewards Recompensas selecionadas
     * @return true se processado com sucesso
     */
    bool processRewards(uint32_t characterID, uint32_t questID, const std::vector<QuestReward>& selectedRewards);
    
    /**
     * @brief Verifica e atualiza objetivos dependentes
     * @param characterID ID do personagem
     * @param objectiveType Tipo de objetivo
     * @param targetID ID do alvo
     * @param amount Quantidade
     */
    void updateDependentObjectives(uint32_t characterID, QuestObjectiveType objectiveType, uint32_t targetID, uint32_t amount);
    
    /**
     * @brief Verifica e desbloqueia missões dependentes
     * @param characterID ID do personagem
     * @param questID ID da missão
     */
    void unlockDependentQuests(uint32_t characterID, uint32_t questID);
    
    /**
     * @brief Carrega missões do banco de dados
     * @return true se carregado com sucesso
     */
    bool loadQuestsFromDatabase();
    
    /**
     * @brief Salva missões no banco de dados
     * @return true se salvo com sucesso
     */
    bool saveQuestsToDatabase();
};

} // namespace quest
} // namespace wydbr

#endif // _QUEST_MANAGER_H_