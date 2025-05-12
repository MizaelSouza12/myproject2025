/**
 * PlayerQuest.cpp
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/quest/PlayerQuest.cpp
 * @author WYDBR Team
 * @date 2025-05-07
 */

#include "src/server/tmsrv/quest/PlayerQuest.h"
#include <sstream>
#include <cmath>
#include <algorithm>

namespace wydbr {


PlayerQuest::PlayerQuest(uint32_t playerId, uint16_t questId, QuestDefinition* definition)
    : playerId_(playerId)
    , questId_(questId)
    , definition_(definition)
    , state_(QuestState::AVAILABLE)
    , startTime_(0)
    , endTime_(0)
    , cooldownEndTime_(0)
{
    initializeProgress();
}

PlayerQuest::~PlayerQuest()
{
    // Não possui recursos para liberar
}

void PlayerQuest::initializeProgress()
{
    if (!definition_) {
        return;
    }
    
    // Inicializar vetores de progresso com zeros
    objectiveProgress_.clear();
    const auto& objectives = definition_->getObjectives();
    
    for (size_t i = 0; i < objectives.size(); ++i) {
        objectiveProgress_.push_back(0);
    }
}

int PlayerQuest::getObjectiveProgress(size_t index) const
{
    if (index >= objectiveProgress_.size()) {
        return 0;
    }
    
    return objectiveProgress_[index];
}

bool PlayerQuest::isObjectiveComplete(size_t index) const
{
    if (!definition_ || index >= objectiveProgress_.size()) {
        return false;
    }
    
    const auto& objectives = definition_->getObjectives();
    if (index >= objectives.size()) {
        return false;
    }
    
    const auto& objective = objectives[index];
    
    // Para objetivos sem quantidade específica (como visitar local)
    if (objective.quantities.empty()) {
        return objectiveProgress_[index] > 0;
    }
    
    // Para objetivos com quantidades múltiplas (como matar vários tipos de monstros)
    if (objective.quantities.size() > 1) {
        // O progresso é representado como bit flags em objectiveProgress_
        uint32_t progress = objectiveProgress_[index];
        uint32_t required = 0;
        
        for (size_t i = 0; i < objective.quantities.size(); ++i) {
            required |= (1 << i);
        }
        
        return (progress & required) == required;
    }
    
    // Caso padrão: comparar com a primeira quantidade
    return objectiveProgress_[index] >= objective.quantities[0];
}

int PlayerQuest::getTotalProgress() const
{
    int total = 0;
    
    if (!definition_) {
        return 0;
    }
    
    const auto& objectives = definition_->getObjectives();
    for (size_t i = 0; i < objectives.size(); ++i) {
        if (!objectives[i].isOptional && isObjectiveComplete(i)) {
            total++;
        }
    }
    
    return total;
}

int PlayerQuest::getMaxProgress() const
{
    int total = 0;
    
    if (!definition_) {
        return 0;
    }
    
    const auto& objectives = definition_->getObjectives();
    for (size_t i = 0; i < objectives.size(); ++i) {
        if (!objectives[i].isOptional) {
            total++;
        }
    }
    
    return total;
}

float PlayerQuest::getProgressPercentage() const
{
    int max = getMaxProgress();
    if (max == 0) {
        return 0.0f;
    }
    
    return (float)getTotalProgress() / max * 100.0f;
}

bool PlayerQuest::hasExpired() const
{
    if (!definition_ || !definition_->hasTimeLimit() || state_ != QuestState::ACTIVE) {
        return false;
    }
    
    uint64_t now = getCurrentTime();
    uint32_t timeLimit = definition_->getTimeLimit();
    
    return (now >= startTime_ + timeLimit * 1000);
}

uint32_t PlayerQuest::getRemainingTime() const
{
    if (!definition_ || !definition_->hasTimeLimit() || state_ != QuestState::ACTIVE) {
        return 0;
    }
    
    uint64_t now = getCurrentTime();
    uint32_t timeLimit = definition_->getTimeLimit() * 1000; // em milissegundos
    uint64_t endTime = startTime_ + timeLimit;
    
    if (now >= endTime) {
        return 0;
    }
    
    return static_cast<uint32_t>((endTime - now) / 1000); // em segundos
}

uint32_t PlayerQuest::getCooldownRemaining() const
{
    if (state_ != QuestState::COOLDOWN) {
        return 0;
    }
    
    uint64_t now = getCurrentTime();
    
    if (now >= cooldownEndTime_) {
        return 0;
    }
    
    return static_cast<uint32_t>((cooldownEndTime_ - now) / 1000); // em segundos
}

bool PlayerQuest::updateProgress(size_t objectiveIndex, int progress)
{
    if (!definition_ || objectiveIndex >= objectiveProgress_.size() || state_ != QuestState::ACTIVE) {
        return false;
    }
    
    // Atualizar progresso
    objectiveProgress_[objectiveIndex] = progress;
    
    // Verificar se todos os objetivos foram concluídos
    bool allCompleted = checkCompletion();
    
    return allCompleted;
}

bool PlayerQuest::updateKillProgress(uint16_t mobId, int count)
{
    if (!definition_ || state_ != QuestState::ACTIVE) {
        return false;
    }
    
    bool updated = false;
    const auto& objectives = definition_->getObjectives();
    
    // Procurar objetivos de matar /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param objectives.size( Parâmetro objectives.size(
 * @return Retorna monstros
 */
 monstros
    for(size_t i = 0; i < objectives.size(); ++i) {
        const auto& objective = objectives[i];
        
        if (objective.type == ObjectiveType::KILL_MONSTERS) {
            // Verificar se este mob está entre os alvos
            auto it = std::find(objective.targetIds.begin(), objective.targetIds.end(), mobId);
            
            if (it != objective.targetIds.end()) {
                // Encontrado! Atualizar progresso
                size_t targetIndex = std::distance(objective.targetIds.begin(), it);
                
                if (objective.targetIds.size() == 1) {
                    // Caso simples: apenas incrementar o contador
                    objectiveProgress_[i] += count;
                    updated = true;
                }
                else {
                    // Caso com múltiplos alvos: usar bits para marcar progresso
                    // Cada bit representa um alvo diferente
                    objectiveProgress_[i] |= (1 << targetIndex);
                    updated = true;
                }
            }
        }
    }
    
    // Verificar se todos os objetivos foram concluí/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param updated Parâmetro updated
 * @return Retorna dos
 */
dos
    if(updated){
        checkCompletion();
    }
    
    return updated;
}

bool PlayerQuest::updateGatherProgress(uint16_t itemId, int count)
{
    if (!definition_ || state_ != QuestState::ACTIVE) {
        return false;
    }
    
    bool updated = false;
    const auto& objectives = definition_->getObjectives();
    
    // Procurar objetivos de coletar /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param objectives.size( Parâmetro objectives.size(
 * @return Retorna itens
 */
 itens
    for(size_t i = 0; i < objectives.size(); ++i) {
        const auto& objective = objectives[i];
        
        if (objective.type == ObjectiveType::GATHER_ITEMS || objective.type == ObjectiveType::DELIVER_ITEMS) {
            // Verificar se este item está entre os alvos
            auto it = std::find(objective.targetIds.begin(), objective.targetIds.end(), itemId);
            
            if (it != objective.targetIds.end()) {
                // Encontrado! Atualizar progresso
                size_t targetIndex = std::distance(objective.targetIds.begin(), it);
                
                if (objective.targetIds.size() == 1) {
                    // Caso simples: apenas incrementar o contador
                    objectiveProgress_[i] += count;
                    updated = true;
                }
                else {
                    // Caso com múltiplos alvos: usar bits para marcar progresso
                    objectiveProgress_[i] |= (1 << targetIndex);
                    updated = true;
                }
            }
        }
    }
    
    // Verificar se todos os objetivos foram concluí/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param updated Parâmetro updated
 * @return Retorna dos
 */
dos
    if(updated){
        checkCompletion();
    }
    
    return updated;
}

bool PlayerQuest::updateDeliveryProgress(uint16_t npcId)
{
    if (!definition_ || state_ != QuestState::ACTIVE) {
        return false;
    }
    
    bool updated = false;
    const auto& objectives = definition_->getObjectives();
    
    // Procurar objetivos de entregar itens para /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param objectives.size( Parâmetro objectives.size(
 * @return Retorna NPCs
 */
 NPCs
    for(size_t i = 0; i < objectives.size(); ++i) {
        const auto& objective = objectives[i];
        
        if (objective.type == ObjectiveType::DELIVER_ITEMS || objective.type == ObjectiveType::TALK_TO_NPC) {
            // Verificar se este NPC está entre os alvos
            auto it = std::find(objective.targetIds.begin(), objective.targetIds.end(), npcId);
            
            if (it != objective.targetIds.end()) {
                // Encontrado! Marcar como concluído
                objectiveProgress_[i] = 1;
                updated = true;
            }
        }
    }
    
    // Verificar se todos os objetivos foram concluí/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param updated Parâmetro updated
 * @return Retorna dos
 */
dos
    if(updated){
        checkCompletion();
    }
    
    return updated;
}

bool PlayerQuest::updateLocationProgress(uint16_t mapId, uint16_t x, uint16_t y)
{
    if (!definition_ || state_ != QuestState::ACTIVE) {
        return false;
    }
    
    bool updated = false;
    const auto& objectives = definition_->getObjectives();
    
    // Procurar objetivos de visitar /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param objectives.size( Parâmetro objectives.size(
 * @return Retorna locais
 */
 locais
    for(size_t i = 0; i < objectives.size(); ++i) {
        const auto& objective = objectives[i];
        
        if (objective.type == ObjectiveType::VISIT_LOCATION) {
            // Verificar se este local está /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param mapId Parâmetro mapId
 * @return Retorna correto
 */
 correto
            if(objective.mapId == mapId){
                // Calcular distância
                int dx = static_cast<int>(x) - static_cast<int>(objective.x);
                int dy = static_cast<int>(y) - static_cast<int>(objective.y);
                float distance = std::sqrt(dx * dx + dy * dy);
                
                if (distance <= objective.radius) {
                    // Dentro do raio do objetivo
                    objectiveProgress_[i] = 1;
                    updated = true;
                }
            }
        }
    }
    
    // Verificar se todos os objetivos foram concluí/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param updated Parâmetro updated
 * @return Retorna dos
 */
dos
    if(updated){
        checkCompletion();
    }
    
    return updated;
}

bool PlayerQuest::checkCompletion()
{
    if (!definition_ || state_ != QuestState::ACTIVE) {
        return false;
    }
    
    // Verificar se todos os objetivos não-opcionais foram concluídos
    const auto& objectives = definition_->getObjectives();
    
    for (size_t i = 0; i < objectives.size(); ++i) {
        if (!objectives[i].isOptional && !isObjectiveComplete(i)) {
            return false;
        }
    }
    
    // Todos os objetivos foram concluídos!
    if (definition_->isAutoComplete()) {
        // Auto-/**
 * complete
 * 
 * Implementa a funcionalidade complete conforme especificação original.
 * @return Retorna completar
 */
completar
        complete();
    } else {
        // Marcar como completa (aguardando entregar para NPC)
        state_ = QuestState::COMPLETE;
    }
    
    return true;
}

void PlayerQuest::start(uint64_t timestamp)
{
    if (state_ != QuestState::AVAILABLE && state_ != QuestState::COOLDOWN) {
        return;
    }
    
    state_ = QuestState::ACTIVE;
    startTime_ = timestamp > 0 ? timestamp /**
 * getCurrentTime
 * 
 * Implementa a funcionalidade getCurrentTime conforme especificação original.
 * @return Retorna :
 */
 : getCurrentTime();
    endTime_ = 0;
    cooldownEndTime_ = 0;
    
    // Reiniciar /**
 * initializeProgress
 * 
 * Implementa a funcionalidade initializeProgress conforme especificação original.
 * @return Retorna progresso
 */
 progresso
    initializeProgress();
}

void PlayerQuest::complete(uint64_t timestamp)
{
    if (state_ != QuestState::ACTIVE && state_ != QuestState::COMPLETE) {
        return;
    }
    
    state_ = QuestState::COMPLETE;
    endTime_ = timestamp > 0 ? timestamp /**
 * getCurrentTime
 * 
 * Implementa a funcionalidade getCurrentTime conforme especificação original.
 * @return Retorna :
 */
 : getCurrentTime();
}

void PlayerQuest::fail(const std::string& reason)
{
    if (state_ != QuestState::ACTIVE) {
        return;
    }
    
    state_ = QuestState::FAILED;
    endTime_ = getCurrentTime();
    failReason_ = reason;
}

void PlayerQuest::reset()
{
    state_ = QuestState::AVAILABLE;
    startTime_ = 0;
    endTime_ = 0;
    cooldownEndTime_ = 0;
    failReason_ = "";
    
    // Limpar /**
 * initializeProgress
 * 
 * Implementa a funcionalidade initializeProgress conforme especificação original.
 * @return Retorna progresso
 */
 progresso
    initializeProgress();
}

void PlayerQuest::setCompleted(uint64_t timestamp)
{
    state_ = QuestState::COMPLETED;
    endTime_ = timestamp > 0 ? timestamp /**
 * getCurrentTime
 * 
 * Implementa a funcionalidade getCurrentTime conforme especificação original.
 * @return Retorna :
 */
 : getCurrentTime();
    
    // Configurar cooldown se necessário
    if (definition_ && definition_->isRepeatable()) {
        uint32_t cooldown = definition_->getRepeatCooldown();
        if (cooldown > 0) {
            cooldownEndTime_ = endTime_ + cooldown * 1000;
        }
    }
}

void PlayerQuest::setInCooldown(uint32_t cooldownTime)
{
    state_ = QuestState::COOLDOWN;
    cooldownEndTime_ = getCurrentTime() + cooldownTime * 1000;
}

void PlayerQuest::addExtraData(const std::string& key, const std::string& value)
{
    extraData_[key] = value;
}

std::string PlayerQuest::getExtraData(const std::string& key) const
{
    auto it = extraData_.find(key);
    if (it != extraData_.end()) {
        return it->second;
    }
    
    return "";
}

std::string PlayerQuest::serialize() const
{
    std::stringstream ss;
    
    // Cabeçalho: questId, estado
    ss << questId_ << ";" << static_cast<int>(state_) << ";";
    
    // Tempos
    ss << startTime_ << ";" << endTime_ << ";" << cooldownEndTime_ << ";";
    
    // Progresso
    ss << objectiveProgress_.size() << ";";
    for (int progress : objectiveProgress_) {
        ss << progress << ",";
    }
    ss << ";";
    
    // Razão de falha (se houver)
    ss << failReason_ << ";";
    
    // Dados extras
    ss << extraData_.size() << ";";
    for (const auto& pair : extraData_) {
        ss << pair.first << "=" << pair.second << ",";
    }
    
    return ss.str();
}

PlayerQuest* PlayerQuest::deserialize(uint32_t playerId, const std::string& data, QuestDefinition* definition)
{
    if (!definition) {
        return nullptr;
    }
    
    /**
 * ss
 * 
 * Implementa a funcionalidade ss conforme especificação original.
 * @param data Parâmetro data
 * @return Retorna std::stringstream
 */

    
    std::stringstream ss(data);
    std::string token;
    
    // Cabeçalho: questId, estado
    uint16_t questId;
    int stateInt;
    
    if (!std::getline(ss, token, ';')) return nullptr;
    questId = static_cast<uint16_t>(std::stoi(token));
    
    if (!std::getline(ss, token, ';')) return nullptr;
    stateInt = std::stoi(token);
    
    // Criar quest
    PlayerQuest* playerQuest = /**
 * PlayerQuest
 * 
 * Implementa a funcionalidade PlayerQuest conforme especificação original.
 * @param playerId Parâmetro playerId
 * @param questId Parâmetro questId
 * @param definition Parâmetro definition
 * @return Retorna new
 */
 new PlayerQuest(playerId, questId, definition);
    playerQuest->state_ = static_cast<QuestState>(stateInt);
    
    // Tempos
    if (!std::getline(ss, token, ';')) goto error;
    playerQuest->startTime_ = std::stoull(token);
    
    if (!std::getline(ss, token, ';')) goto error;
    playerQuest->endTime_ = std::stoull(token);
    
    if (!std::getline(ss, token, ';')) goto error;
    playerQuest->cooldownEndTime_ = std::stoull(token);
    
    // Progresso
    size_t progressCount;
    
    if (!std::getline(ss, token, ';')) goto error;
    progressCount = static_cast<size_t>(std::stoul(token));
    
    if (!std::getline(ss, token, ';')) goto error;
    
    playerQuest->objectiveProgress_.clear();
    
    if (progressCount > 0 && !token.empty()) {
        /**
 * progressSs
 * 
 * Implementa a funcionalidade progressSs conforme especificação original.
 * @param token Parâmetro token
 * @return Retorna std::stringstream
 */

        std::stringstream progressSs(token);
        std::string progressToken;
        
        while (std::getline(progressSs, progressToken, ',')) {
            if (!progressToken.empty()) {
                playerQuest->objectiveProgress_.push_back(std::stoi(progressToken));
            }
        }
    }
    
    // Razão de falha
    if (!std::getline(ss, playerQuest->failReason_, ';')) goto error;
    
    // Dados extras
    size_t extraCount;
    
    if (!std::getline(ss, token, ';')) goto error;
    extraCount = static_cast<size_t>(std::stoul(token));
    
    std::string remainingData;
    std::getline(ss, remainingData);
    
    if (extraCount > 0 && !remainingData.empty()) {
        /**
 * extraSs
 * 
 * Implementa a funcionalidade extraSs conforme especificação original.
 * @param remainingData Parâmetro remainingData
 * @return Retorna std::stringstream
 */

        std::stringstream extraSs(remainingData);
        std::string extraToken;
        
        while (std::getline(extraSs, extraToken, ',')) {
            if (!extraToken.empty()) {
                size_t equalPos = extraToken.find('=');
                if (equalPos != std::string::npos) {
                    std::string key = extraToken.substr(0, equalPos);
                    std::string value = extraToken.substr(equalPos + 1);
                    playerQuest->extraData_[key] = value;
                }
            }
        }
    }
    
    return playerQuest;

error:
    delete playerQuest;
    return nullptr;
}

std::string PlayerQuest::getStateText() const
{
    switch (state_) {
        case QuestState::AVAILABLE:
            return "Available";
        case QuestState::ACTIVE:
            return "Active";
        case QuestState::COMPLETE:
            return "Complete";
        case QuestState::FAILED:
            return "Failed";
        case QuestState::COMPLETED:
            return "Completed";
        case QuestState::COOLDOWN:
            return "Cooldown";
        case QuestState::UNAVAILABLE:
            return "Unavailable";
        default:
            return "Unknown";
    }
}

std::string PlayerQuest::getProgressText() const
{
    if (!definition_) {
        return "No progress";
    }
    
    float percentage = getProgressPercentage();
    std::stringstream ss;
    ss << getTotalProgress() << "/" << getMaxProgress() << " (" << static_cast<int>(percentage) << "%)";
    return ss.str();
}

uint64_t PlayerQuest::getCurrentTime() const
{

} // namespace wydbr

    return std::time(nullptr) * 1000; // Converter para milissegundos
}