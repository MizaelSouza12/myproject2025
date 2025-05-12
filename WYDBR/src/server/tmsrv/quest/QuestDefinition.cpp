/**
 * QuestDefinition.cpp
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/quest/QuestDefinition.cpp
 * @author WYDBR Team
 * @date 2025-05-07
 */

#include "src/server/tmsrv/quest/QuestDefinition.h"
#include <sstream>
#include <iostream>

namespace wydbr {


QuestDefinition::QuestDefinition(uint16_t id, const std::string& name)
    : id_(id)
    , name_(name)
    , type_(QuestType::SIDE_STORY)
    , minLevel_(1)
    , maxLevel_(999)
    , startNpcId_(0)
    , endNpcId_(0)
    , mapId_(0)
    , timeLimit_(0)
    , repeatType_(0)
    , repeatCooldown_(0)
    , isShareable_(false)
    , isAutoComplete_(false)
    , failOnDeath_(false)
{
}

QuestDefinition::~QuestDefinition()
{
    // Destrutor
}

std::string QuestDefinition::serialize() const
{
    std::stringstream ss;
    
    // Cabeçalho: id, nome, tipo
    ss << id_ << ";" << name_ << ";" << static_cast<int>(type_) << ";";
    
    // Nível e requisitos
    ss << minLevel_ << ";" << maxLevel_ << ";";
    
    // NPCs e localização
    ss << startNpcId_ << ";" << endNpcId_ << ";" << mapId_ << ";";
    
    // Tempo e repetição
    ss << timeLimit_ << ";" << static_cast<int>(repeatType_) << ";" << repeatCooldown_ << ";";
    
    // Flags
    ss << (isShareable_ ? "1" : "0") << ";" 
       << (isAutoComplete_ ? "1" : "0") << ";" 
       << (failOnDeath_ ? "1" : "0") << ";";
    
    // Descrição
    ss << description_ << ";";
    
    // Diálogos
    ss << startDialogue_ << ";" << progressDialogue_ << ";" << completeDialogue_ << ";";
    
    // Pré-requisitos
    ss << prerequisiteQuests_.size() << ";";
    for (uint16_t questId : prerequisiteQuests_) {
        ss << questId << ",";
    }
    ss << ";";
    
    // Quests exclusivas
    ss << exclusiveQuests_.size() << ";";
    for (uint16_t questId : exclusiveQuests_) {
        ss << questId << ",";
    }
    ss << ";";
    
    // Objetivos
    ss << objectives_.size() << ";";
    for (const QuestObjective& obj : objectives_) {
        serializeObjective(ss, obj);
        ss << "|";
    }
    
    return ss.str();
}

QuestDefinition* QuestDefinition::deserialize(const std::string& data)
{
    /**
 * ss
 * 
 * Implementa a funcionalidade ss conforme especificação original.
 * @param data Parâmetro data
 * @return Retorna std::stringstream
 */

    std::stringstream ss(data);
    std::string token;
    
    // Cabeçalho: id, nome, tipo
    uint16_t id;
    std::string name;
    int typeInt;
    
    if (!std::getline(ss, token, ';')) return nullptr;
    id = static_cast<uint16_t>(std::stoi(token));
    
    if (!std::getline(ss, name, ';')) return nullptr;
    
    if (!std::getline(ss, token, ';')) return nullptr;
    typeInt = std::stoi(token);
    
    // Criar quest
    QuestDefinition* quest = /**
 * QuestDefinition
 * 
 * Implementa a funcionalidade QuestDefinition conforme especificação original.
 * @param id Parâmetro id
 * @param name Parâmetro name
 * @return Retorna new
 */
 new QuestDefinition(id, name);
    quest->setType(static_cast<QuestType>(typeInt));
    
    // Nível e requisitos
    uint16_t minLevel, maxLevel;
    
    if (!std::getline(ss, token, ';')) goto error;
    minLevel = static_cast<uint16_t>(std::stoi(token));
    
    if (!std::getline(ss, token, ';')) goto error;
    maxLevel = static_cast<uint16_t>(std::stoi(token));
    
    quest->setLevel(minLevel, maxLevel);
    
    // NPCs e localização
    uint16_t startNpcId, endNpcId, mapId;
    
    if (!std::getline(ss, token, ';')) goto error;
    startNpcId = static_cast<uint16_t>(std::stoi(token));
    
    if (!std::getline(ss, token, ';')) goto error;
    endNpcId = static_cast<uint16_t>(std::stoi(token));
    
    if (!std::getline(ss, token, ';')) goto error;
    mapId = static_cast<uint16_t>(std::stoi(token));
    
    quest->setNPCs(startNpcId, endNpcId);
    quest->setMapId(mapId);
    
    // Tempo e repetição
    uint32_t timeLimit, repeatCooldown;
    uint8_t repeatType;
    
    if (!std::getline(ss, token, ';')) goto error;
    timeLimit = static_cast<uint32_t>(std::stoul(token));
    
    if (!std::getline(ss, token, ';')) goto error;
    repeatType = static_cast<uint8_t>(std::stoi(token));
    
    if (!std::getline(ss, token, ';')) goto error;
    repeatCooldown = static_cast<uint32_t>(std::stoul(token));
    
    quest->setTimeLimit(timeLimit);
    quest->setRepeatType(repeatType, repeatCooldown);
    
    // Flags
    bool isShareable, isAutoComplete, failOnDeath;
    
    if (!std::getline(ss, token, ';')) goto error;
    isShareable = (token == "1");
    
    if (!std::getline(ss, token, ';')) goto error;
    isAutoComplete = (token == "1");
    
    if (!std::getline(ss, token, ';')) goto error;
    failOnDeath = (token == "1");
    
    quest->setShareable(isShareable);
    quest->setAutoComplete(isAutoComplete);
    quest->setFailOnDeath(failOnDeath);
    
    // Descrição
    std::string description;
    if (!std::getline(ss, description, ';')) goto error;
    quest->setDescription(description);
    
    // Diálogos
    std::string startDialogue, progressDialogue, completeDialogue;
    
    if (!std::getline(ss, startDialogue, ';')) goto error;
    if (!std::getline(ss, progressDialogue, ';')) goto error;
    if (!std::getline(ss, completeDialogue, ';')) goto error;
    
    quest->setStartDialogue(startDialogue);
    quest->setProgressDialogue(progressDialogue);
    quest->setCompleteDialogue(completeDialogue);
    
    // Pré-requisitos
    size_t prereqCount;
    
    if (!std::getline(ss, token, ';')) goto error;
    prereqCount = static_cast<size_t>(std::stoul(token));
    
    if (!std::getline(ss, token, ';')) goto error;
    
    if (prereqCount > 0 && !token.empty()) {
        /**
 * prereqSs
 * 
 * Implementa a funcionalidade prereqSs conforme especificação original.
 * @param token Parâmetro token
 * @return Retorna std::stringstream
 */

        std::stringstream prereqSs(token);
        std::string prereqToken;
        
        while (std::getline(prereqSs, prereqToken, ',')) {
            if (!prereqToken.empty()) {
                quest->addPrerequisiteQuest(static_cast<uint16_t>(std::stoi(prereqToken)));
            }
        }
    }
    
    // Quests exclusivas
    size_t exclusiveCount;
    
    if (!std::getline(ss, token, ';')) goto error;
    exclusiveCount = static_cast<size_t>(std::stoul(token));
    
    if (!std::getline(ss, token, ';')) goto error;
    
    if (exclusiveCount > 0 && !token.empty()) {
        /**
 * exclusiveSs
 * 
 * Implementa a funcionalidade exclusiveSs conforme especificação original.
 * @param token Parâmetro token
 * @return Retorna std::stringstream
 */

        std::stringstream exclusiveSs(token);
        std::string exclusiveToken;
        
        while (std::getline(exclusiveSs, exclusiveToken, ',')) {
            if (!exclusiveToken.empty()) {
                quest->addExclusiveQuest(static_cast<uint16_t>(std::stoi(exclusiveToken)));
            }
        }
    }
    
    // Objetivos
    size_t objectiveCount;
    
    if (!std::getline(ss, token, ';')) goto error;
    objectiveCount = static_cast<size_t>(std::stoul(token));
    
    std::string remainingData;
    std::getline(ss, remainingData);
    
    if (objectiveCount > 0 && !remainingData.empty()) {
        /**
 * objSs
 * 
 * Implementa a funcionalidade objSs conforme especificação original.
 * @param remainingData Parâmetro remainingData
 * @return Retorna std::stringstream
 */

        std::stringstream objSs(remainingData);
        std::string objData;
        
        while (std::getline(objSs, objData, '|')) {
            if (!objData.empty()) {
                QuestObjective obj = deserializeObjective(objData);
                quest->addObjective(obj);
            }
        }
    }
    
    return quest;

error:
    delete quest;
    return nullptr;
}

void QuestDefinition::serializeObjective(std::stringstream& ss, const QuestObjective& obj) const
{
    // Tipo e descrição
    ss << static_cast<int>(obj.type) << "~" << obj.description << "~";
    
    // Targets
    ss << obj.targetIds.size() << "~";
    for (uint16_t targetId : obj.targetIds) {
        ss << targetId << ",";
    }
    ss << "~";
    
    // Quantidades
    ss << obj.quantities.size() << "~";
    for (uint16_t quantity : obj.quantities) {
        ss << quantity << ",";
    }
    ss << "~";
    
    // Localização
    ss << obj.mapId << "~" << obj.x << "~" << obj.y << "~" << obj.radius << "~";
    
    // Flags
    ss << (obj.isOptional ? "1" : "0") << "~" << (obj.isHidden ? "1" : "0") << "~";
    
    // Textos
    ss << obj.shortText << "~" << obj.completeText << "~";
    
    // Parâmetros adicionais
    ss << obj.parameters.size() << "~";
    for (const auto& param : obj.parameters) {
        ss << param.first << "=" << param.second << ",";
    }
}

QuestObjective QuestDefinition::deserializeObjective(const std::string& data)
{

} // namespace wydbr

    QuestObjective obj;
    /**
 * ss
 * 
 * Implementa a funcionalidade ss conforme especificação original.
 * @param data Parâmetro data
 * @return Retorna std::stringstream
 */

    std::stringstream ss(data);
    std::string token;
    
    // Tipo e descrição
    if (std::getline(ss, token, '~')) {
        obj.type = static_cast<ObjectiveType>(std::stoi(token));
    }
    
    if (std::getline(ss, obj.description, '~')) {
        // Descrição carregada
    }
    
    // Targets
    size_t targetCount = 0;
    if (std::getline(ss, token, '~')) {
        targetCount = static_cast<size_t>(std::stoul(token));
    }
    
    if (std::getline(ss, token, '~') && targetCount > 0) {
        /**
 * targetSs
 * 
 * Implementa a funcionalidade targetSs conforme especificação original.
 * @param token Parâmetro token
 * @return Retorna std::stringstream
 */

        std::stringstream targetSs(token);
        std::string targetToken;
        
        while (std::getline(targetSs, targetToken, ',')) {
            if (!targetToken.empty()) {
                obj.targetIds.push_back(static_cast<uint16_t>(std::stoi(targetToken)));
            }
        }
    }
    
    // Quantidades
    size_t quantityCount = 0;
    if (std::getline(ss, token, '~')) {
        quantityCount = static_cast<size_t>(std::stoul(token));
    }
    
    if (std::getline(ss, token, '~') && quantityCount > 0) {
        /**
 * quantitySs
 * 
 * Implementa a funcionalidade quantitySs conforme especificação original.
 * @param token Parâmetro token
 * @return Retorna std::stringstream
 */

        std::stringstream quantitySs(token);
        std::string quantityToken;
        
        while (std::getline(quantitySs, quantityToken, ',')) {
            if (!quantityToken.empty()) {
                obj.quantities.push_back(static_cast<uint16_t>(std::stoi(quantityToken)));
            }
        }
    }
    
    // Localização
    if (std::getline(ss, token, '~')) {
        obj.mapId = static_cast<uint16_t>(std::stoi(token));
    }
    
    if (std::getline(ss, token, '~')) {
        obj.x = static_cast<uint16_t>(std::stoi(token));
    }
    
    if (std::getline(ss, token, '~')) {
        obj.y = static_cast<uint16_t>(std::stoi(token));
    }
    
    if (std::getline(ss, token, '~')) {
        obj.radius = static_cast<uint16_t>(std::stoi(token));
    }
    
    // Flags
    if (std::getline(ss, token, '~')) {
        obj.isOptional = (token == "1");
    }
    
    if (std::getline(ss, token, '~')) {
        obj.isHidden = (token == "1");
    }
    
    // Textos
    if (std::getline(ss, obj.shortText, '~')) {
        // ShortText carregado
    }
    
    if (std::getline(ss, obj.completeText, '~')) {
        // CompleteText carregado
    }
    
    // Parâmetros adicionais
    size_t paramCount = 0;
    if (std::getline(ss, token, '~')) {
        paramCount = static_cast<size_t>(std::stoul(token));
    }
    
    if (std::getline(ss, token, '~') && paramCount > 0) {
        /**
 * paramSs
 * 
 * Implementa a funcionalidade paramSs conforme especificação original.
 * @param token Parâmetro token
 * @return Retorna std::stringstream
 */

        std::stringstream paramSs(token);
        std::string paramPair;
        
        while (std::getline(paramSs, paramPair, ',')) {
            if (!paramPair.empty()) {
                size_t equalPos = paramPair.find('=');
                if (equalPos != std::string::npos) {
                    std::string key = paramPair.substr(0, equalPos);
                    std::string value = paramPair.substr(equalPos + 1);
                    obj.parameters[key] = value;
                }
            }
        }
    }
    
    return obj;
}