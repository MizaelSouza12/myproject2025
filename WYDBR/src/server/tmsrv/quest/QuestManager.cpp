/**
 * QuestManager.cpp
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/quest/QuestManager.cpp
 * @author WYDBR Team
 * @date 2025-05-07
 */

#include "src/server/tmsrv/quest/QuestManager.h"
#include "src/server/tmsrv/npc/NPCManager.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <ctime>

namespace wydbr {


QuestManager::QuestManager(TMServer* server)
    : WYDSystemManager(server, "QuestManager")
    , npcManager_(nullptr)
{
}

QuestManager::~QuestManager()
{
    shutdown();
}

bool QuestManager::initialize()
{
    Log()->info("Inicializando gerenciador de quests...");
    
    // Carregar dados
    bool success = true;
    success &= loadQuestData("data/quests/QuestList.bin");
    success &= loadQuestDialogues("data/quests/QuestDialogues.bin");
    success &= loadQuestRewards("data/quests/QuestRewards.bin");
    
    if (!success) {
        Log()->error("Falha ao carregar dados de quests");
        return false;
    }
    
    Log()->info("Gerenciador de quests inicializado com sucesso");
    Log()->info("Quests carregadas: {}", questDefinitions_.size());
    
    return true;
}

void QuestManager::update(uint64_t timestamp)
{
    std::lock_guard<std::mutex> guard(playerQuestMutex_);
    
    // Verificar quests ativas de todos os /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param playerQuestStates_ Parâmetro playerQuestStates_
 * @return Retorna jogadores
 */
 jogadores
    for(auto& entry : playerQuestStates_){
        uint32_t playerId = entry.first;
        updatePlayerQuests(playerId, timestamp);
    }
}

void QuestManager::shutdown()
{
    Log()->info("Desligando gerenciador de quests...");
    
    // Limpar definições
    std::lock_guard<std::mutex> guard(questMutex_);
    questDefinitions_.clear();
    questsByLevel_.clear();
    questsByNPC_.clear();
    questsByMap_.clear();
    questRewards_.clear();
    questDialogues_.clear();
    
    Log()->info("Gerenciador de quests desligado");
}

bool QuestManager::loadQuestData(const std::string& filename)
{
    // Em uma implementação real, carregaríamos de um arquivo binário
    // Para demonstração, vamos criar algumas quests manualmente
    
    Log()->info("Carregando dados de quests de: {}", filename);
    
    std::lock_guard<std::mutex> guard(questMutex_);
    
    // Quest 1: Matar Lobos (tutorial)
    auto quest1 = std::make_unique<QuestDefinition>(1001, "Caçador de Lobos");
    quest1->setDescription("Mate 10 Lobos Selvagens para ajudar a proteger a vila.");
    quest1->setType(QuestType::TUTORIAL);
    quest1->setLevel(1, 10);
    quest1->setNPCs(1001, 1001); // Mesmo NPC para iniciar e completar
    quest1->setMapId(1); // Floresta
    
    QuestObjective objective1;
    objective1.type = ObjectiveType::KILL_MONSTERS;
    objective1.description = "Mate Lobos Selvagens";
    objective1.targetIds = {101}; // ID do Lobo Selvagem
    objective1.quantities = {10}; // 10 lobos
    
    quest1->addObjective(objective1);
    
    // Adicionar aos índices
    questDefinitions_[1001] = std::move(quest1);
    questsByLevel_[1].push_back(1001);
    questsByNPC_[1001].push_back(1001);
    questsByMap_[1].push_back(1001);
    
    // Quest 2: Coletar Ervas
    auto quest2 = std::make_unique<QuestDefinition>(1002, "Ervas Medicinais");
    quest2->setDescription("Colete ervas medicinais para o curandeiro da vila.");
    quest2->setType(QuestType::SIDE_STORY);
    quest2->setLevel(5, 15);
    quest2->setNPCs(1002, 1002); // Curandeiro
    quest2->setMapId(1); // Floresta
    
    QuestObjective objective2;
    objective2.type = ObjectiveType::GATHER_ITEMS;
    objective2.description = "Colete Ervas Medicinais";
    objective2.targetIds = {2001, 2002, 2003}; // IDs das ervas
    objective2.quantities = {5, 3, 1}; // Quantidades
    
    quest2->addObjective(objective2);
    
    // Adicionar aos índices
    questDefinitions_[1002] = std::move(quest2);
    questsByLevel_[5].push_back(1002);
    questsByNPC_[1002].push_back(1002);
    questsByMap_[1].push_back(1002);
    
    // Quest 3: Explorar caverna
    auto quest3 = std::make_unique<QuestDefinition>(1003, "Explorando a Caverna");
    quest3->setDescription("Explore a caverna misteriosa e descubra o que há dentro.");
    quest3->setType(QuestType::MAIN_STORY);
    quest3->setLevel(10, 20);
    quest3->setNPCs(1003, 1003); // Aventureiro
    quest3->setMapId(2); // Caverna
    
    QuestObjective objective3a;
    objective3a.type = ObjectiveType::VISIT_LOCATION;
    objective3a.description = "Encontre a entrada da caverna";
    objective3a.mapId = 2;
    objective3a.x = 300;
    objective3a.y = 300;
    objective3a.radius = 50;
    
    QuestObjective objective3b;
    objective3b.type = ObjectiveType::KILL_MONSTERS;
    objective3b.description = "Derrote os Esqueletos Guardiões";
    objective3b.targetIds = {102}; // ID do Esqueleto
    objective3b.quantities = {5}; // 5 esqueletos
    
    quest3->addObjective(objective3a);
    quest3->addObjective(objective3b);
    
    // Adicionar aos índices
    questDefinitions_[1003] = std::move(quest3);
    questsByLevel_[10].push_back(1003);
    questsByNPC_[1003].push_back(1003);
    questsByMap_[2].push_back(1003);
    
    // Quest 4: Confrontar o Dragão
    auto quest4 = std::make_unique<QuestDefinition>(2001, "A Ira do Dragão");
    quest4->setDescription("Confronte o Dragão Antigo que aterroriza a região.");
    quest4->setType(QuestType::MAIN_STORY);
    quest4->setLevel(40, 50);
    quest4->setNPCs(1003, 1003); // Aventureiro
    quest4->setMapId(3); // Montanha
    quest4->setTimeLimit(3600); // 1 hora
    
    // Pré-requisitos
    quest4->addPrerequisiteQuest(1003); // Precisa ter completado a quest da caverna
    
    QuestObjective objective4;
    objective4.type = ObjectiveType::KILL_MONSTERS;
    objective4.description = "Derrote o Dragão Antigo";
    objective4.targetIds = {201}; // ID do Dragão
    objective4.quantities = {1}; // 1 dragão
    
    quest4->addObjective(objective4);
    
    // Adicionar aos índices
    questDefinitions_[2001] = std::move(quest4);
    questsByLevel_[40].push_back(2001);
    questsByNPC_[1003].push_back(2001);
    questsByMap_[3].push_back(2001);
    
    Log()->info("Carregadas {} quests", questDefinitions_.size());
    
    return true;
}

bool QuestManager::loadQuestDialogues(const std::string& filename)
{
    // Em uma implementação real, carregaríamos de um arquivo de texto ou binário
    // Para demonstração, vamos criar alguns diálogos manualmente
    
    Log()->info("Carregando diálogos de quests de: {}", filename);
    
    std::lock_guard<std::mutex> guard(questMutex_);
    
    // Diálogos para Quest 1: Caçador de Lobos
    std::map<std::string, std::string> dialogue1;
    dialogue1["start"] = "Os lobos estão atacando nossas fazendas! Você pode nos ajudar a reduzir o número deles?";
    dialogue1["progress"] = "Como vai a caçada aos lobos? Ainda temos problemas com eles.";
    dialogue1["complete"] = "Muito obrigado! Agora nossas fazendas estarão mais seguras.";
    
    questDialogues_[1001] = dialogue1;
    
    // Diálogos para Quest 2: Ervas Medicinais
    std::map<std::string, std::string> dialogue2;
    dialogue2["start"] = "Preciso de ervas para preparar remédios. Pode coletar algumas para mim?";
    dialogue2["progress"] = "Encontrou as ervas que pedi? Tenho pacientes esperando por tratamento.";
    dialogue2["complete"] = "Excelente! Com estas ervas, poderei ajudar muitas pessoas doentes.";
    
    questDialogues_[1002] = dialogue2;
    
    // Diálogos para Quest 3: Explorando a Caverna
    std::map<std::string, std::string> dialogue3;
    dialogue3["start"] = "Dizem que há um tesouro escondido na caverna ao norte. Está disposto a investigar?";
    dialogue3["progress"] = "Já explorou a caverna? Cuidado com os perigos lá dentro.";
    dialogue3["complete"] = "Incrível! Sua bravura será lembrada em canções e histórias!";
    
    questDialogues_[1003] = dialogue3;
    
    // Diálogos para Quest 4: A Ira do Dragão
    std::map<std::string, std::string> dialogue4;
    dialogue4["start"] = "O lendário Dragão Antigo retornou e ameaça toda a região. Precisamos da sua ajuda!";
    dialogue4["progress"] = "O dragão ainda vive? Nosso tempo está se esgotando...";
    dialogue4["complete"] = "Você conseguiu! Derrotou o Dragão Antigo! Seu nome será lembrado para sempre!";
    
    questDialogues_[2001] = dialogue4;
    
    Log()->info("Carregados diálogos para {} quests", questDialogues_.size());
    
    return true;
}

bool QuestManager::loadQuestRewards(const std::string& filename)
{
    // Em uma implementação real, carregaríamos de um arquivo de texto ou binário
    // Para demonstração, vamos criar algumas recompensas manualmente
    
    Log()->info("Carregando recompensas de quests de: {}", filename);
    
    std::lock_guard<std::mutex> guard(questMutex_);
    
    // Recompensas para Quest 1: Caçador de Lobos
    QuestReward reward1;
    reward1.questId = 1001;
    reward1.experience = 100;
    reward1.gold = 50;
    reward1.items = {{2001, 1}}; // 1x Pele de Lobo
    
    questRewards_[1001] = reward1;
    
    // Recompensas para Quest 2: Ervas Medicinais
    QuestReward reward2;
    reward2.questId = 1002;
    reward2.experience = 200;
    reward2.gold = 75;
    reward2.items = {{1001, 1}}; // 1x Poção de Cura
    
    questRewards_[1002] = reward2;
    
    // Recompensas para Quest 3: Explorando a Caverna
    QuestReward reward3;
    reward3.questId = 1003;
    reward3.experience = 500;
    reward3.gold = 150;
    reward3.items = {{1002, 1}, {2002, 5}}; // 1x Anel de Proteção, 5x Cristais
    
    questRewards_[1003] = reward3;
    
    // Recompensas para Quest 4: A Ira do Dragão
    QuestReward reward4;
    reward4.questId = 2001;
    reward4.experience = 5000;
    reward4.gold = 1000;
    reward4.items = {{3001, 1}}; // 1x Escama de Dragão
    reward4.skills = {{101, 1}}; // Habilidade: Sopro de Fogo
    
    questRewards_[2001] = reward4;
    
    Log()->info("Carregadas recompensas para {} quests", questRewards_.size());
    
    return true;
}

QuestDefinition* QuestManager::getQuestDefinition(uint16_t questId)
{
    std::lock_guard<std::mutex> guard(questMutex_);
    
    auto it = questDefinitions_.find(questId);
    if (it != questDefinitions_.end()) {
        return it->second.get();
    }
    
    return nullptr;
}

std::vector<uint16_t> QuestManager::getQuestsForLevel(uint16_t level)
{
    std::lock_guard<std::mutex> guard(questMutex_);
    
    std::vector<uint16_t> result;
    
    // Encontrar todas as quests que se encaixam neste ní/**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param questDefinitions_ Parâmetro questDefinitions_
 * @return Retorna vel
 */
vel
    for(const auto& entry : questDefinitions_){
        QuestDefinition* quest = entry.second.get();
        if (level >= quest->getMinLevel() && level <= quest->getMaxLevel()) {
            result.push_back(quest->getId());
        }
    }
    
    return result;
}

std::vector<uint16_t> QuestManager::getQuestsForNPC(uint16_t npcId)
{
    std::lock_guard<std::mutex> guard(questMutex_);
    
    auto it = questsByNPC_.find(npcId);
    if (it != questsByNPC_.end()) {
        return it->second;
    }
    
    return {};
}

std::vector<uint16_t> QuestManager::getQuestsInMap(uint16_t mapId)
{
    std::lock_guard<std::mutex> guard(questMutex_);
    
    auto it = questsByMap_.find(mapId);
    if (it != questsByMap_.end()) {
        return it->second;
    }
    
    return {};
}

bool QuestManager::canStartQuest(uint32_t playerId, uint16_t questId)
{
    // Verificar se a quest existe
    QuestDefinition* quest = getQuestDefinition(questId);
    if (!quest) {
        Log()->warn("Tentativa de iniciar quest inexistente: {}", questId);
        return false;
    }
    
    std::lock_guard<std::mutex> guard(playerQuestMutex_);
    
    // Verificar se o jogador já tem esta quest ativa ou concluída
    PlayerQuestState& state = getOrCreatePlayerState(playerId);
    
    if (state.activeQuests.find(questId) != state.activeQuests.end()) {
        return false; // Já tem a quest ativa
    }
    
    // Verificar se a quest está em cooldown
    auto it = state.completedQuests.find(questId);
    if (it != state.completedQuests.end()) {
        PlayerQuest* completedQuest = it->second.get();
        
        // Se está em cooldown e o cooldown não acabou, não pode iniciar
        if (completedQuest->isInCooldown() && completedQuest->getCooldownRemaining() > 0) {
            return false;
        }
    }
    
    // Verificar pré-requisitos
    /**
 * checkQuestPrerequisites
 * 
 * Implementa a funcionalidade checkQuestPrerequisites conforme especificação original.
 * @param playerId Parâmetro playerId
 * @param quest Parâmetro quest
 * @return Retorna return
 */

    return checkQuestPrerequisites(playerId, quest);
}

bool QuestManager::startQuest(uint32_t playerId, uint16_t questId, uint32_t npcId)
{
    // Verificar se o jogador pode iniciar a quest
    if (!canStartQuest(playerId, questId)) {
        return false;
    }
    
    // Verificar se a quest existe
    QuestDefinition* quest = getQuestDefinition(questId);
    if (!quest) {
        return false;
    }
    
    // Verificar se o NPC é o correto (se fornecido)
    if (npcId != 0 && quest->getStartNpcId() != npcId) {
        Log()->warn("Tentativa de iniciar quest {} com NPC incorreto: {} (deveria ser {})",
                  questId, npcId, quest->getStartNpcId());
        return false;
    }
    
    std::lock_guard<std::mutex> guard(playerQuestMutex_);
    
    // Criar a quest para o jogador
    PlayerQuestState& state = getOrCreatePlayerState(playerId);
    
    auto playerQuest = std::make_unique<PlayerQuest>(playerId, questId, quest);
    playerQuest->start();
    
    // Adicionar à lista de quests ativas
    state.activeQuests[questId] = std::move(playerQuest);
    
    Log()->info("Jogador {} iniciou a quest {}: {}", playerId, questId, quest->getName());
    
    return true;
}

bool QuestManager::abandonQuest(uint32_t playerId, uint16_t questId)
{
    std::lock_guard<std::mutex> guard(playerQuestMutex_);
    
    // Verificar se o jogador tem esta quest ativa
    auto it = playerQuestStates_.find(playerId);
    if (it == playerQuestStates_.end()) {
        return false;
    }
    
    PlayerQuestState& state = it->second;
    auto questIt = state.activeQuests.find(questId);
    
    if (questIt == state.activeQuests.end()) {
        return false;
    }
    
    // Remover a quest
    state.activeQuests.erase(questIt);
    
    Log()->info("Jogador {} abandonou a quest {}", playerId, questId);
    
    return true;
}

bool QuestManager::completeQuest(uint32_t playerId, uint16_t questId, uint32_t npcId)
{
    std::lock_guard<std::mutex> guard(playerQuestMutex_);
    
    // Verificar se o jogador tem esta quest ativa ou completa
    auto it = playerQuestStates_.find(playerId);
    if (it == playerQuestStates_.end()) {
        return false;
    }
    
    PlayerQuestState& state = it->second;
    auto questIt = state.activeQuests.find(questId);
    
    if (questIt == state.activeQuests.end()) {
        return false;
    }
    
    PlayerQuest* quest = questIt->second.get();
    
    // Verificar se a quest pode ser completada
    if (!quest->isComplete() && !quest->isActive()) {
        return false;
    }
    
    // Verificar se o NPC é o correto (se fornecido)
    QuestDefinition* definition = quest->getDefinition();
    if (npcId != 0 && definition && definition->getEndNpcId() != npcId) {
        Log()->warn("Tentativa de completar quest {} com NPC incorreto: {} (deveria ser {})",
                  questId, npcId, definition->getEndNpcId());
        return false;
    }
    
    // Completar a quest
    quest->setCompleted();
    
    // Dar recompensas
    bool rewarded = rewardPlayer(playerId, questId);
    
    // Mover para a lista de quests completadas
    state.completedQuests[questId] = std::move(questIt->second);
    state.activeQuests.erase(questIt);
    
    Log()->info("Jogador {} completou a quest {}", playerId, questId);
    
    return rewarded;
}

PlayerQuest* QuestManager::getPlayerQuest(uint32_t playerId, uint16_t questId)
{
    std::lock_guard<std::mutex> guard(playerQuestMutex_);
    
    auto it = playerQuestStates_.find(playerId);
    if (it == playerQuestStates_.end()) {
        return nullptr;
    }
    
    // Verificar quests ativas
    auto& activeQuests = it->second.activeQuests;
    auto activeIt = activeQuests.find(questId);
    
    if (activeIt != activeQuests.end()) {
        return activeIt->second.get();
    }
    
    // Verificar quests completadas
    auto& completedQuests = it->second.completedQuests;
    auto completedIt = completedQuests.find(questId);
    
    if (completedIt != completedQuests.end()) {
        return completedIt->second.get();
    }
    
    return nullptr;
}

std::vector<PlayerQuest*> QuestManager::getPlayerActiveQuests(uint32_t playerId)
{
    std::vector<PlayerQuest*> result;
    
    std::lock_guard<std::mutex> guard(playerQuestMutex_);
    
    auto it = playerQuestStates_.find(playerId);
    if (it == playerQuestStates_.end()) {
        return result;
    }
    
    auto& activeQuests = it->second.activeQuests;
    result.reserve(activeQuests.size());
    
    for (auto& entry : activeQuests) {
        result.push_back(entry.second.get());
    }
    
    return result;
}

std::vector<PlayerQuest*> QuestManager::getPlayerCompletedQuests(uint32_t playerId)
{
    std::vector<PlayerQuest*> result;
    
    std::lock_guard<std::mutex> guard(playerQuestMutex_);
    
    auto it = playerQuestStates_.find(playerId);
    if (it == playerQuestStates_.end()) {
        return result;
    }
    
    auto& completedQuests = it->second.completedQuests;
    result.reserve(completedQuests.size());
    
    for (auto& entry : completedQuests) {
        result.push_back(entry.second.get());
    }
    
    return result;
}

bool QuestManager::hasCompletedQuest(uint32_t playerId, uint16_t questId)
{
    std::lock_guard<std::mutex> guard(playerQuestMutex_);
    
    auto it = playerQuestStates_.find(playerId);
    if (it == playerQuestStates_.end()) {
        return false;
    }
    
    auto& completedQuests = it->second.completedQuests;
    return completedQuests.find(questId) != completedQuests.end();
}

bool QuestManager::updateKillObjective(uint32_t playerId, uint16_t mobId)
{
    std::lock_guard<std::mutex> guard(playerQuestMutex_);
    
    auto it = playerQuestStates_.find(playerId);
    if (it == playerQuestStates_.end()) {
        return false;
    }
    
    bool updated = false;
    
    // Verificar todas as quests ativas do /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param it->second.activeQuests Parâmetro it->second.activeQuests
 * @return Retorna jogador
 */
 jogador
    for(auto& entry : it->second.activeQuests){
        PlayerQuest* quest = entry.second.get();
        
        // Tentar atualizar o objetivo
        if (quest->updateKillProgress(mobId, 1)) {
            updated = true;
        }
    }
    
    return updated;
}

bool QuestManager::updateGatherObjective(uint32_t playerId, uint16_t itemId)
{
    std::lock_guard<std::mutex> guard(playerQuestMutex_);
    
    auto it = playerQuestStates_.find(playerId);
    if (it == playerQuestStates_.end()) {
        return false;
    }
    
    bool updated = false;
    
    // Verificar todas as quests ativas do /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param it->second.activeQuests Parâmetro it->second.activeQuests
 * @return Retorna jogador
 */
 jogador
    for(auto& entry : it->second.activeQuests){
        PlayerQuest* quest = entry.second.get();
        
        // Tentar atualizar o objetivo
        if (quest->updateGatherProgress(itemId, 1)) {
            updated = true;
        }
    }
    
    return updated;
}

bool QuestManager::updateDeliveryObjective(uint32_t playerId, uint16_t npcId)
{
    std::lock_guard<std::mutex> guard(playerQuestMutex_);
    
    auto it = playerQuestStates_.find(playerId);
    if (it == playerQuestStates_.end()) {
        return false;
    }
    
    bool updated = false;
    
    // Verificar todas as quests ativas do /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param it->second.activeQuests Parâmetro it->second.activeQuests
 * @return Retorna jogador
 */
 jogador
    for(auto& entry : it->second.activeQuests){
        PlayerQuest* quest = entry.second.get();
        
        // Tentar atualizar o objetivo
        if (quest->updateDeliveryProgress(npcId)) {
            updated = true;
        }
    }
    
    return updated;
}

bool QuestManager::updateLocationObjective(uint32_t playerId, uint16_t mapId, uint16_t x, uint16_t y)
{
    std::lock_guard<std::mutex> guard(playerQuestMutex_);
    
    auto it = playerQuestStates_.find(playerId);
    if (it == playerQuestStates_.end()) {
        return false;
    }
    
    bool updated = false;
    
    // Verificar todas as quests ativas do /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param it->second.activeQuests Parâmetro it->second.activeQuests
 * @return Retorna jogador
 */
 jogador
    for(auto& entry : it->second.activeQuests){
        PlayerQuest* quest = entry.second.get();
        
        // Tentar atualizar o objetivo
        if (quest->updateLocationProgress(mapId, x, y)) {
            updated = true;
        }
    }
    
    return updated;
}

QuestManager::QuestReward QuestManager::getQuestReward(uint16_t questId)
{
    std::lock_guard<std::mutex> guard(questMutex_);
    
    auto it = questRewards_.find(questId);
    if (it != questRewards_.end()) {
        return it->second;
    }
    
    // Recompensa vazia se não for encontrada
    QuestReward emptyReward;
    emptyReward.questId = questId;
    emptyReward.experience = 0;
    emptyReward.gold = 0;
    
    return emptyReward;
}

bool QuestManager::triggerQuestEvent(uint32_t playerId, const std::string& eventId, 
                                   const std::map<std::string, std::string>& params)
{
    std::lock_guard<std::mutex> guard(playerQuestMutex_);
    
    auto it = playerQuestStates_.find(playerId);
    if (it == playerQuestStates_.end()) {
        return false;
    }
    
    bool triggered = false;
    
    // Verificar todas as quests ativas do /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param it->second.activeQuests Parâmetro it->second.activeQuests
 * @return Retorna jogador
 */
 jogador
    for(auto& entry : it->second.activeQuests){
        PlayerQuest* quest = entry.second.get();
        
        // Verificar se a quest reage a este evento
        std::string eventParam = quest->getExtraData("event_" + eventId);
        if (!eventParam.empty()) {
            // Adicionar o evento como dado extra para a quest
            quest->addExtraData("event_trigger_" + eventId, "1");
            
            // Adicionar parâ/**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param params Parâmetro params
 * @return Retorna metros
 */
metros
            for(const auto& param : params){
                quest->addExtraData("event_param_" + eventId + "_" + param.first, param.second);
            }
            
            triggered = true;
        }
    }
    
    return triggered;
}

std::string QuestManager::getQuestDialogue(uint16_t questId, const std::string& key)
{
    std::lock_guard<std::mutex> guard(questMutex_);
    
    auto it = questDialogues_.find(questId);
    if (it == questDialogues_.end()) {
        return "";
    }
    
    auto& dialogues = it->second;
    auto dialogueIt = dialogues.find(key);
    
    if (dialogueIt != dialogues.end()) {
        return dialogueIt->second;
    }
    
    return "";
}

size_t QuestManager::getActiveQuestsCount() const
{
    std::lock_guard<std::mutex> guard(playerQuestMutex_);
    
    size_t count = 0;
    
    for (const auto& entry : playerQuestStates_) {
        count += entry.second.activeQuests.size();
    }
    
    return count;
}

size_t QuestManager::getQuestDefinitionsCount() const
{
    std::lock_guard<std::mutex> guard(questMutex_);
    return questDefinitions_.size();
}

void QuestManager::updatePlayerQuests(uint32_t playerId, uint64_t timestamp)
{
    auto it = playerQuestStates_.find(playerId);
    if (it == playerQuestStates_.end()) {
        return;
    }
    
    PlayerQuestState& state = it->second;
    
    // Verificar se está na hora de /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 1000 Parâmetro 1000
 * @return Retorna atualizar
 */
 atualizar
    if(timestamp - state.lastUpdate < 1000){ // 1 segundo entre atualizações
        return;
    }
    
    state.lastUpdate = timestamp;
    
    // Verificar quests ativas
    std::vector<uint16_t> failedQuests;
    
    for (auto& entry : state.activeQuests) {
        uint16_t questId = entry.first;
        PlayerQuest* quest = entry.second.get();
        
        // Verificar se expirou
        if (quest->hasExpired()) {
            quest->fail("Tempo expirado");
            failedQuests.push_back(questId);
        }
    }
    
    // Mover quests falhas para completadas (com status de falha)
    for (uint16_t questId : failedQuests) {
        auto questIt = state.activeQuests.find(questId);
        if (questIt != state.activeQuests.end()) {
            state.completedQuests[questId] = std::move(questIt->second);
            state.activeQuests.erase(questIt);
        }
    }
    
    // Verificar quests em cooldown
    std::vector<uint16_t> readyQuests;
    
    for (auto& entry : state.completedQuests) {
        PlayerQuest* quest = entry.second.get();
        
        // Se está em cooldown e o cooldown acabou, resetar para disponível
        if (quest->isInCooldown() && quest->getCooldownRemaining() == 0) {
            quest->reset();
            readyQuests.push_back(entry.first);
        }
    }
}

bool QuestManager::checkQuestPrerequisites(uint32_t playerId, const QuestDefinition* quest)
{
    if (!quest) {
        return false;
    }
    
    const auto& prerequisites = quest->getPrerequisiteQuests();
    
    // Se não tem pré-requisitos, pode iniciar
    if (prerequisites.empty()) {
        return true;
    }
    
    // Verificar cada pré-/**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param prerequisites Parâmetro prerequisites
 * @return Retorna requisito
 */
requisito
    for(uint16_t prereqId : prerequisites){
        if (!hasCompletedQuest(playerId, prereqId)) {
            return false; // Faltando um pré-requisito
        }
    }
    
    return true;
}

bool QuestManager::rewardPlayer(uint32_t playerId, uint16_t questId)
{
    // Em uma implementação real, isso integraria com o sistema de inventário, experiência, etc.
    // Para demonstração, apenas registramos as recompensas
    
    QuestReward reward = getQuestReward(questId);
    
    Log()->info("Jogador {} recebeu recompensas da quest {}: {} XP, {} gold, {} itens",
              playerId, questId, reward.experience, reward.gold, reward.items.size());
    
    return true;
}

PlayerQuestState& QuestManager::getOrCreatePlayerState(uint32_t playerId)
{

} // namespace wydbr

    auto it = playerQuestStates_.find(playerId);
    if (it == playerQuestStates_.end()) {
        // Criar novo estado para o jogador
        PlayerQuestState state;
        state.lastUpdate = std::time(nullptr) * 1000;
        
        auto result = playerQuestStates_.emplace(playerId, std::move(state));
        return result.first->second;
    }
    
    return it->second;
}