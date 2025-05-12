/**
 * QuestDefinition.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/quest/QuestDefinition.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef QUEST_DEFINITION_H
#define QUEST_DEFINITION_H

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <cstdint>

namespace wydbr {


/**
 * Tipos de Quests
 */
enum class QuestType : uint8_t {
    MAIN_STORY,     // Quest da história principal
    SIDE_STORY,     // Quest da história secundária
    DAILY,          // Quest diária
    WEEKLY,         // Quest semanal
    GUILD,          // Quest de guilda
    EVENT,          // Quest de evento
    HIDDEN,         // Quest secreta
    ACHIEVEMENT,    // Conquista
    TUTORIAL        // Tutorial
};

/**
 * Tipos de Objetivos
 */
enum class ObjectiveType : uint8_t {
    KILL_MONSTERS,  // Matar monstros
    GATHER_ITEMS,   // Coletar itens
    DELIVER_ITEMS,  // Entregar itens
    TALK_TO_NPC,    // Falar com NPC
    VISIT_LOCATION, // Visitar local
    ESCORT_NPC,     // Escoltar NPC
    DEFEND_AREA,    // Defender área
    USE_SKILL,      // Usar skill
    CRAFT_ITEM,     // Criar item
    COMPLETE_QUEST, // Completar outra quest
    CUSTOM          // Objetivo personalizado
};

/**
 * Definição de um objetivo de Quest
 */
struct QuestObjective {
    ObjectiveType type;
    std::string description;
    
    // Parâmetros específicos para cada tipo de objetivo
    std::vector<uint16_t> targetIds;     // IDs de mobs, itens, NPCs, etc.
    std::vector<uint16_t> quantities;    // Quantidades necessárias
    
    // Localização (para objetivos de visita)
    uint16_t mapId;
    uint16_t x;
    uint16_t y;
    uint16_t radius;
    
    // Parâmetros adicionais para objetivos especiais
    std::map<std::string, std::string> parameters;
    
    // Valores para exibição ao cliente
    std::string shortText;      // Texto resumido para lista de objetivos
    std::string completeText;   // Texto exibido ao completar objetivo
    
    // Configurações especiais
    bool isOptional;            // Se é um objetivo opcional
    bool isHidden;              // Se é um objetivo escondido
    
    QuestObjective() : type(ObjectiveType::KILL_MONSTERS), mapId(0), x(0), y(0), 
                      radius(0), isOptional(false), isHidden(false) {}
};

/**
 * Definição de uma Quest
 */
/**
 * Classe WYDQuestDefinition
 * 
 * Esta classe implementa a funcionalidade WYDQuestDefinition conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class QuestDefinition {
public:
    // Métodos bá/**
 * QuestDefinition
 * 
 * Implementa a funcionalidade QuestDefinition conforme especificação original.
 * @param id Parâmetro id
 * @param name Parâmetro name
 * @return Retorna sicos
 */
sicos
    QuestDefinition(uint16_t id, const std::string& name);
    ~QuestDefinition();
    
    // Getters
    /**
 * getId
 * 
 * Implementa a funcionalidade getId conforme especificação original.
 * @return Retorna uint16_t
 */

    uint16_t getId() const{ return id_; }
    const std::string& getName() const { return name_; }
    const std::string& getDescription() const { return description_; }
    /**
 * getType
 * 
 * Implementa a funcionalidade getType conforme especificação original.
 * @return Retorna QuestType
 */

    QuestType getType() const{ return type_; }
    /**
 * getMinLevel
 * 
 * Implementa a funcionalidade getMinLevel conforme especificação original.
 * @return Retorna uint16_t
 */

    uint16_t getMinLevel() const{ return minLevel_; }
    /**
 * getMaxLevel
 * 
 * Implementa a funcionalidade getMaxLevel conforme especificação original.
 * @return Retorna uint16_t
 */

    uint16_t getMaxLevel() const{ return maxLevel_; }
    /**
 * getStartNpcId
 * 
 * Implementa a funcionalidade getStartNpcId conforme especificação original.
 * @return Retorna uint16_t
 */

    uint16_t getStartNpcId() const{ return startNpcId_; }
    /**
 * getEndNpcId
 * 
 * Implementa a funcionalidade getEndNpcId conforme especificação original.
 * @return Retorna uint16_t
 */

    uint16_t getEndNpcId() const{ return endNpcId_; }
    /**
 * getMapId
 * 
 * Implementa a funcionalidade getMapId conforme especificação original.
 * @return Retorna uint16_t
 */

    uint16_t getMapId() const{ return mapId_; }
    
    // Getters para objetivos e requisitos
    const std::vector<QuestObjective>& getObjectives() const { return objectives_; }
    const std::vector<uint16_t>& getPrerequisiteQuests() const { return prerequisiteQuests_; }
    const std::vector<uint16_t>& getExclusiveQuests() const { return exclusiveQuests_; }
    
    // Getters para texto e diálogo
    const std::string& getStartDialogue() const { return startDialogue_; }
    const std::string& getProgressDialogue() const { return progressDialogue_; }
    const std::string& getCompleteDialogue() const { return completeDialogue_; }
    
    // Configuração
    /**
 * setDescription
 * 
 * Implementa a funcionalidade setDescription conforme especificação original.
 * @param desc Parâmetro desc
 */

    void setDescription(const std::string& desc){ description_ = desc; }
    /**
 * setType
 * 
 * Implementa a funcionalidade setType conforme especificação original.
 * @param type Parâmetro type
 */

    void setType(QuestType type){ type_ = type; }
    /**
 * setLevel
 * 
 * Implementa a funcionalidade setLevel conforme especificação original.
 * @param min Parâmetro min
 * @param max Parâmetro max
 */

    void setLevel(uint16_t min, uint16_t max){ minLevel_ = min; maxLevel_ = max; }
    /**
 * setNPCs
 * 
 * Implementa a funcionalidade setNPCs conforme especificação original.
 * @param startNpc Parâmetro startNpc
 * @param endNpc Parâmetro endNpc
 */

    void setNPCs(uint16_t startNpc, uint16_t endNpc){ startNpcId_ = startNpc; endNpcId_ = endNpc; }
    /**
 * setMapId
 * 
 * Implementa a funcionalidade setMapId conforme especificação original.
 * @param mapId Parâmetro mapId
 */

    void setMapId(uint16_t mapId){ mapId_ = mapId; }
    
    // Adicionar objetivos e requisitos
    /**
 * addObjective
 * 
 * Implementa a funcionalidade addObjective conforme especificação original.
 * @param objective Parâmetro objective
 */

    void addObjective(const QuestObjective& objective){ objectives_.push_back(objective); }
    /**
 * addPrerequisiteQuest
 * 
 * Implementa a funcionalidade addPrerequisiteQuest conforme especificação original.
 * @param questId Parâmetro questId
 */

    void addPrerequisiteQuest(uint16_t questId){ prerequisiteQuests_.push_back(questId); }
    /**
 * addExclusiveQuest
 * 
 * Implementa a funcionalidade addExclusiveQuest conforme especificação original.
 * @param questId Parâmetro questId
 */

    void addExclusiveQuest(uint16_t questId){ exclusiveQuests_.push_back(questId); }
    
    // Configuração de diálogos
    /**
 * setStartDialogue
 * 
 * Implementa a funcionalidade setStartDialogue conforme especificação original.
 * @param dialogue Parâmetro dialogue
 */

    void setStartDialogue(const std::string& dialogue){ startDialogue_ = dialogue; }
    /**
 * setProgressDialogue
 * 
 * Implementa a funcionalidade setProgressDialogue conforme especificação original.
 * @param dialogue Parâmetro dialogue
 */

    void setProgressDialogue(const std::string& dialogue){ progressDialogue_ = dialogue; }
    /**
 * setCompleteDialogue
 * 
 * Implementa a funcionalidade setCompleteDialogue conforme especificação original.
 * @param dialogue Parâmetro dialogue
 */

    void setCompleteDialogue(const std::string& dialogue){ completeDialogue_ = dialogue; }
    
    // Configuração temporal
    /**
 * setTimeLimit
 * 
 * Implementa a funcionalidade setTimeLimit conforme especificação original.
 * @param seconds Parâmetro seconds
 */

    void setTimeLimit(uint32_t seconds){ timeLimit_ = seconds; }
    /**
 * setRepeatType
 * 
 * Implementa a funcionalidade setRepeatType conforme especificação original.
 * @param type Parâmetro type
 * @param cooldown Parâmetro cooldown
 */

    void setRepeatType(uint8_t type, uint32_t cooldown){ repeatType_ = type; repeatCooldown_ = cooldown; }
    
    // Flags e propriedades especiais
    /**
 * setShareable
 * 
 * Implementa a funcionalidade setShareable conforme especificação original.
 * @param value Parâmetro value
 */

    void setShareable(bool value){ isShareable_ = value; }
    /**
 * setAutoComplete
 * 
 * Implementa a funcionalidade setAutoComplete conforme especificação original.
 * @param value Parâmetro value
 */

    void setAutoComplete(bool value){ isAutoComplete_ = value; }
    /**
 * setFailOnDeath
 * 
 * Implementa a funcionalidade setFailOnDeath conforme especificação original.
 * @param value Parâmetro value
 */

    void setFailOnDeath(bool value){ failOnDeath_ = value; }
    
    // Configurações de rastreamento
    /**
 * isShareable
 * 
 * Implementa a funcionalidade isShareable conforme especificação original.
 * @return Retorna bool
 */

    bool isShareable() const{ return isShareable_; }
    /**
 * isAutoComplete
 * 
 * Implementa a funcionalidade isAutoComplete conforme especificação original.
 * @return Retorna bool
 */

    bool isAutoComplete() const{ return isAutoComplete_; }
    /**
 * hasTimeLimit
 * 
 * Implementa a funcionalidade hasTimeLimit conforme especificação original.
 * @return Retorna bool
 */

    bool hasTimeLimit() const{ return timeLimit_ > 0; }
    /**
 * getTimeLimit
 * 
 * Implementa a funcionalidade getTimeLimit conforme especificação original.
 * @return Retorna uint32_t
 */

    uint32_t getTimeLimit() const{ return timeLimit_; }
    
    // Verificações de repetição
    /**
 * isRepeatable
 * 
 * Implementa a funcionalidade isRepeatable conforme especificação original.
 * @return Retorna bool
 */

    bool isRepeatable() const{ return repeatType_ > 0; }
    /**
 * getRepeatType
 * 
 * Implementa a funcionalidade getRepeatType conforme especificação original.
 * @return Retorna uint8_t
 */

    uint8_t getRepeatType() const{ return repeatType_; }
    /**
 * getRepeatCooldown
 * 
 * Implementa a funcionalidade getRepeatCooldown conforme especificação original.
 * @return Retorna uint32_t
 */

    uint32_t getRepeatCooldown() const{ return repeatCooldown_; }
    
    // Serialização
    /**
 * serialize
 * 
 * Implementa a funcionalidade serialize conforme especificação original.
 * @return Retorna std::string
 */

    std::string serialize() const;
    /**
 * deserialize
 * 
 * Implementa a funcionalidade deserialize conforme especificação original.
 * @param data Parâmetro data
 * @return Retorna QuestDefinition
 */

    static QuestDefinition* deserialize(const std::string& data);
    
private:
    // Identificação
    uint16_t id_;
    std::string name_;
    std::string description_;
    QuestType type_;
    
    // Nível e requisitos
    uint16_t minLevel_;
    uint16_t maxLevel_;
    std::vector<uint16_t> prerequisiteQuests_;
    std::vector<uint16_t> exclusiveQuests_;
    
    // NPCs e localização
    uint16_t startNpcId_;
    uint16_t endNpcId_;
    uint16_t mapId_;
    
    // Objetivos
    std::vector<QuestObjective> objectives_;
    
    // Diálogos
    std::string startDialogue_;
    std::string progressDialogue_;
    std::string completeDialogue_;
    
    // Tempo e repetição
    uint32_t timeLimit_;        // Limite de tempo em segundos (0 = sem limite)
    uint8_t repeatType_;        // 0 = não repetível, 1 = diária, 2 = semanal, 3 = mensal, 4 = custom
    uint32_t repeatCooldown_;   // Cooldown em segundos para repetição custom
    
    // Flags
    bool isShareable_;          // Se pode ser compartilhada com grupo
    bool isAutoComplete_;       // Se completa automaticamente ao cumprir objetivos
    bool failOnDeath_;          // Se falha ao morrer
    
    // Serialização auxiliar
    /**
 * serializeObjective
 * 
 * Implementa a funcionalidade serializeObjective conforme especificação original.
 * @param ss Parâmetro ss
 * @param obj Parâmetro obj
 */

    void serializeObjective(std::stringstream& ss, const QuestObjective& obj) const;
    /**
 * deserializeObjective
 * 
 * Implementa a funcionalidade deserializeObjective conforme especificação original.
 * @param data Parâmetro data
 * @return Retorna QuestObjective
 */

    static QuestObjective deserializeObjective(const std::string& data);
};

#endif // QUEST_DEFINITION_H

} // namespace wydbr
