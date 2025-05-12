/**
 * PlayerQuest.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/quest/PlayerQuest.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef PLAYER_QUEST_H
#define PLAYER_QUEST_H

#include <string>
#include <vector>
#include <map>
#include <cstdint>
#include <ctime>

#include "src/server/tmsrv/quest/QuestDefinition.h"

namespace wydbr {


/**
 * Estados de uma Quest
 */
enum class QuestState : uint8_t {
    AVAILABLE,      // Disponível para aceitar
    ACTIVE,         // Ativa
    COMPLETE,       // Completa, mas recompensa não coletada
    FAILED,         // Falhou
    COMPLETED,      // Completa e recompensa coletada
    COOLDOWN,       // Em cooldown para repetir
    UNAVAILABLE     // Não disponível (requisitos não atendidos)
};

/**
 * Classe para rastrear o progresso de um jogador em uma quest
 */
/**
 * Classe WYDPlayerQuest
 * 
 * Esta classe implementa a funcionalidade WYDPlayerQuest conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
/**
 * Classe WYDPlayerQuest
 * 
 * Esta classe implementa a funcionalidade WYDPlayerQuest conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class PlayerQuest {
public:
    // /**
 * PlayerQuest
 * 
 * Implementa a funcionalidade PlayerQuest conforme especificação original.
 * @param playerId Parâmetro playerId
 * @param questId Parâmetro questId
 * @param definition Parâmetro definition
 * @return Retorna Construtor
 */
 Construtor
    PlayerQuest(uint32_t playerId, uint16_t questId, QuestDefinition* definition);
    
    // Destrutor
    ~PlayerQuest();
    
    // Getters básicos
    /**
 * getPlayerId
 * 
 * Implementa a funcionalidade getPlayerId conforme especificação original.
 * @return Retorna uint32_t
 */

    uint32_t getPlayerId() const{ return playerId_; }
    /**
 * getQuestId
 * 
 * Implementa a funcionalidade getQuestId conforme especificação original.
 * @return Retorna uint16_t
 */

    uint16_t getQuestId() const{ return questId_; }
    /**
 * getState
 * 
 * Implementa a funcionalidade getState conforme especificação original.
 * @return Retorna QuestState
 */

    QuestState getState() const{ return state_; }
    /**
 * getStartTime
 * 
 * Implementa a funcionalidade getStartTime conforme especificação original.
 * @return Retorna uint64_t
 */

    uint64_t getStartTime() const{ return startTime_; }
    /**
 * getEndTime
 * 
 * Implementa a funcionalidade getEndTime conforme especificação original.
 * @return Retorna uint64_t
 */

    uint64_t getEndTime() const{ return endTime_; }
    
    // Acesso ao progresso
    /**
 * getObjectiveProgress
 * 
 * Implementa a funcionalidade getObjectiveProgress conforme especificação original.
 * @param index Parâmetro index
 * @return Retorna int
 */

    int getObjectiveProgress(size_t index) const;
    /**
 * isObjectiveComplete
 * 
 * Implementa a funcionalidade isObjectiveComplete conforme especificação original.
 * @param index Parâmetro index
 * @return Retorna bool
 */

    bool isObjectiveComplete(size_t index) const;
    /**
 * getTotalProgress
 * 
 * Implementa a funcionalidade getTotalProgress conforme especificação original.
 * @return Retorna int
 */

    int getTotalProgress() const;
    /**
 * getMaxProgress
 * 
 * Implementa a funcionalidade getMaxProgress conforme especificação original.
 * @return Retorna int
 */

    int getMaxProgress() const;
    /**
 * getProgressPercentage
 * 
 * Implementa a funcionalidade getProgressPercentage conforme especificação original.
 * @return Retorna float
 */

    float getProgressPercentage() const;
    
    // Verificação de estado
    /**
 * isActive
 * 
 * Implementa a funcionalidade isActive conforme especificação original.
 * @return Retorna bool
 */

    bool isActive() const{ return state_ == QuestState::ACTIVE; }
    /**
 * isComplete
 * 
 * Implementa a funcionalidade isComplete conforme especificação original.
 * @return Retorna bool
 */

    bool isComplete() const{ return state_ == QuestState::COMPLETE; }
    /**
 * isCompleted
 * 
 * Implementa a funcionalidade isCompleted conforme especificação original.
 * @return Retorna bool
 */

    bool isCompleted() const{ return state_ == QuestState::COMPLETED; }
    /**
 * isFailed
 * 
 * Implementa a funcionalidade isFailed conforme especificação original.
 * @return Retorna bool
 */

    bool isFailed() const{ return state_ == QuestState::FAILED; }
    /**
 * isInCooldown
 * 
 * Implementa a funcionalidade isInCooldown conforme especificação original.
 * @return Retorna bool
 */

    bool isInCooldown() const{ return state_ == QuestState::COOLDOWN; }
    
    // Verificações de tempo
    /**
 * hasExpired
 * 
 * Implementa a funcionalidade hasExpired conforme especificação original.
 * @return Retorna bool
 */

    bool hasExpired() const;
    /**
 * getRemainingTime
 * 
 * Implementa a funcionalidade getRemainingTime conforme especificação original.
 * @return Retorna uint32_t
 */

    uint32_t getRemainingTime() const;
    /**
 * getCooldownRemaining
 * 
 * Implementa a funcionalidade getCooldownRemaining conforme especificação original.
 * @return Retorna uint32_t
 */

    uint32_t getCooldownRemaining() const;
    
    // Atualização de progresso
    /**
 * updateProgress
 * 
 * Implementa a funcionalidade updateProgress conforme especificação original.
 * @param objectiveIndex Parâmetro objectiveIndex
 * @param progress Parâmetro progress
 * @return Retorna bool
 */

    bool updateProgress(size_t objectiveIndex, int progress);
    /**
 * updateKillProgress
 * 
 * Implementa a funcionalidade updateKillProgress conforme especificação original.
 * @param mobId Parâmetro mobId
 * @param 1 Parâmetro 1
 * @return Retorna bool
 */

    bool updateKillProgress(uint16_t mobId, int count = 1);
    /**
 * updateGatherProgress
 * 
 * Implementa a funcionalidade updateGatherProgress conforme especificação original.
 * @param itemId Parâmetro itemId
 * @param 1 Parâmetro 1
 * @return Retorna bool
 */

    bool updateGatherProgress(uint16_t itemId, int count = 1);
    /**
 * updateDeliveryProgress
 * 
 * Implementa a funcionalidade updateDeliveryProgress conforme especificação original.
 * @param npcId Parâmetro npcId
 * @return Retorna bool
 */

    bool updateDeliveryProgress(uint16_t npcId);
    /**
 * updateLocationProgress
 * 
 * Implementa a funcionalidade updateLocationProgress conforme especificação original.
 * @param mapId Parâmetro mapId
 * @param x Parâmetro x
 * @param y Parâmetro y
 * @return Retorna bool
 */

    bool updateLocationProgress(uint16_t mapId, uint16_t x, uint16_t y);
    
    // Alteração de estado
    /**
 * start
 * 
 * Implementa a funcionalidade start conforme especificação original.
 * @param 0 Parâmetro 0
 */

    void start(uint64_t timestamp = 0);
    /**
 * complete
 * 
 * Implementa a funcionalidade complete conforme especificação original.
 * @param 0 Parâmetro 0
 */

    void complete(uint64_t timestamp = 0);
    /**
 * fail
 * 
 * Implementa a funcionalidade fail conforme especificação original.
 * @param "" Parâmetro ""
 */

    void fail(const std::string& reason = "");
    /**
 * reset
 * 
 * Implementa a funcionalidade reset conforme especificação original.
 */

    void reset();
    /**
 * setCompleted
 * 
 * Implementa a funcionalidade setCompleted conforme especificação original.
 * @param 0 Parâmetro 0
 */

    void setCompleted(uint64_t timestamp = 0);
    /**
 * setInCooldown
 * 
 * Implementa a funcionalidade setInCooldown conforme especificação original.
 * @param cooldownTime Parâmetro cooldownTime
 */

    void setInCooldown(uint32_t cooldownTime);
    
    // Dados adicionais
    /**
 * addExtraData
 * 
 * Implementa a funcionalidade addExtraData conforme especificação original.
 * @param key Parâmetro key
 * @param value Parâmetro value
 */

    void addExtraData(const std::string& key, const std::string& value);
    /**
 * getExtraData
 * 
 * Implementa a funcionalidade getExtraData conforme especificação original.
 * @param key Parâmetro key
 * @return Retorna std::string
 */

    std::string getExtraData(const std::string& key) const;
    
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
 * @param playerId Parâmetro playerId
 * @param data Parâmetro data
 * @param definition Parâmetro definition
 * @return Retorna PlayerQuest
 */

    static PlayerQuest* deserialize(uint32_t playerId, const std::string& data, QuestDefinition* definition);
    
    // Utilitários
    /**
 * getStateText
 * 
 * Implementa a funcionalidade getStateText conforme especificação original.
 * @return Retorna std::string
 */

    std::string getStateText() const;
    /**
 * getProgressText
 * 
 * Implementa a funcionalidade getProgressText conforme especificação original.
 * @return Retorna std::string
 */

    std::string getProgressText() const;
    /**
 * getFailReason
 * 
 * Implementa a funcionalidade getFailReason conforme especificação original.
 * @return Retorna std::string
 */

    std::string getFailReason() const{ return failReason_; }
    
    // Acesso à definição da quest
    /**
 * getDefinition
 * 
 * Implementa a funcionalidade getDefinition conforme especificação original.
 * @return Retorna QuestDefinition
 */

    QuestDefinition* getDefinition() const{ return definition_; }
    
private:
    // Identificação
    uint32_t playerId_;
    uint16_t questId_;
    QuestDefinition* definition_;
    
    // Estado
    QuestState state_;
    std::string failReason_;
    
    // Tempo
    uint64_t startTime_;       // Tempo de início (timestamp)
    uint64_t endTime_;         // Tempo de conclusão/falha (timestamp)
    uint64_t cooldownEndTime_; // Fim do período de cooldown (timestamp)
    
    // Progresso dos objetivos
    std::vector<int> objectiveProgress_;
    
    // Dados extras (para quests personalizadas ou tracking)
    std::map<std::string, std::string> extraData_;
    
    // Métodos auxiliares
    /**
 * initializeProgress
 * 
 * Implementa a funcionalidade initializeProgress conforme especificação original.
 */

    void initializeProgress();
    /**
 * checkCompletion
 * 
 * Implementa a funcionalidade checkCompletion conforme especificação original.
 * @return Retorna bool
 */

    bool checkCompletion();
    /**
 * getCurrentTime
 * 
 * Implementa a funcionalidade getCurrentTime conforme especificação original.
 * @return Retorna uint64_t
 */

    uint64_t getCurrentTime() const;
};

#endif // PLAYER_QUEST_H

} // namespace wydbr
