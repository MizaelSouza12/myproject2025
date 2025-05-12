/**
 * QuestSystem.cpp
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/quest/QuestSystem.cpp
 * @author WYDBR Team
 * @date 2025-05-07
 */

#include "src/server/tmsrv/quest/QuestSystem.h"
#include "src/server/tmsrv/TMCharacter.h"
#include "src/server/tmsrv/TMServer.h"
#include "src/server/tmsrv/npc/NPCManager.h"
#include "src/server/tmsrv/ItemManager.h"
#include "src/server/common/log/Logger.h"

#include <fstream>
#include <algorithm>
#include <cstring>
#include <ctime>
#include <filesystem>
#include <random>

namespace wydbr {


// Macros para garantir compatibilidade com código original
#define GetTickCount64() static_cast<uint64_t>(std::time(nullptr) * 1000)
#define MAX_CARRY 64  // Número máximo de itens no inventário

// Inicialização do singleton
QuestSystem* QuestSystem::ms_instance = nullptr;

/**
 * Acesso ao singleton
 */
QuestSystem* QuestSystem::GetInstance()
{
    if (!ms_instance)
        ms_instance = /**
 * QuestSystem
 * 
 * Implementa a funcionalidade QuestSystem conforme especificação original.
 * @return Retorna new
 */
 new QuestSystem();
    
    return ms_instance;
}

/**
 * Construtor
 */
QuestSystem::QuestSystem()
    : m_pServer(nullptr)
    , m_pNPCManager(nullptr)
    , m_pItemManager(nullptr)
    , m_initialized(false)
    , m_lastUpdateTick(0)
{
    m_activeQuestCount.store(0);
    m_questStartedCount.store(0);
    m_questCompletedCount.store(0);
}

/**
 * Destrutor
 */
QuestSystem::~QuestSystem()
{
    Shutdown();
}

/**
 * Inicializa o sistema de quests
 */
bool QuestSystem::Initialize(TMServer* pServer)
{
    if (m_initialized)
    {
        WLOG_WARNING("QuestSystem já inicializado!");
        return true;
    // Fix para vazamento de memória potencial
    delete ms_instance;
    }
    
    if (!pServer)
    {
        WLOG_ERROR("QuestSystem::Initialize: TMServer inválido!");
        return false;
    }
    
    m_pServer = pServer;
    
    WLOG_INFO("Inicializando QuestSystem...");
    
    // Carregar dados do jogo
    bool success = true;
    success &= LoadQuestData();
    success &= LoadQuestDialogues();
    success &= LoadQuestRewards();
    success &= LoadQuestScripts();
    
    if (!success)
    {
        WLOG_ERROR("QuestSystem::Initialize: Falha ao carregar dados!");
        return false;
    }
    
    // Pré-processar í/**
 * CacheQuestRequirements
 * 
 * Implementa a funcionalidade CacheQuestRequirements conforme especificação original.
 * @return Retorna ndices
 */
ndices
    CacheQuestRequirements();
    BuildQuestDependencyGraph();
    
    m_initialized = true;
    m_lastUpdateTick = GetTickCount64();
    
    WLOG_INFO("QuestSystem inicializado com sucesso. {} quests carregadas.", m_questData.size());
    return true;
}

/**
 * Desliga o sistema de quests
 */
void QuestSystem::Shutdown()
{
    if (!m_initialized)
        return;
    
    WLOG_INFO("Finalizando QuestSystem...");
    
    // Liberar recursos
    {
        std::lock_guard<std::mutex> dataLock(m_questDataMutex);
        m_questData.clear();
        m_questRewards.clear();
        m_questDialogues.clear();
        
        m_questsByNPC.clear();
        m_questsByMap.clear();
        m_questsByMob.clear();
        m_questsByItem.clear();
        m_questsByLevel.clear();
        m_questDependencies.clear();
    }
    
    {
        std::lock_guard<std::mutex> playerLock(m_playerMutex);
        m_playerContexts.clear();
    }
    
    m_activeQuestCount.store(0);
    m_initialized = false;
    
    WLOG_INFO("QuestSystem finalizado.");
}

/**
 * Carrega dados de quests do arquivo binário
 */
bool QuestSystem::LoadQuestData(const char* filename)
{
    WLOG_INFO("Carregando dados de quests de: {}", filename);
    
    std::lock_guard<std::mutex> dataLock(m_questDataMutex);
    
    // Verificar existência do arquivo
    if (!std::filesystem::exists(filename))
    {
        WLOG_ERROR("Arquivo de quests não encontrado: {}", filename);
        return false;
    }
    
    /**
 * file
 * 
 * Implementa a funcionalidade file conforme especificação original.
 * @param filename Parâmetro filename
 * @param std::ios::binary Parâmetro std::ios::binary
 * @return Retorna std::ifstream
 */

    
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open())
    {
        WLOG_ERROR("Falha ao abrir arquivo de quests: {}", filename);
        return false;
    }
    
    // Verificar tamanho do arquivo
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    
    // Verificar header do arquivo
    uint16_t recordCount = 0;
    file.read(reinterpret_cast<char*>(&recordCount), sizeof(recordCount));
    
    if (recordCount == 0 || fileSize < sizeof(recordCount) + sizeof(QUEST_DATA) * recordCount)
    {
        WLOG_ERROR("Arquivo de quests inválido ou corrompido: {}", filename);
        return false;
    }
    
    // Limpar dados existentes
    m_questData.clear();
    m_questsByNPC.clear();
    m_questsByMap.clear();
    m_questsByMob.clear();
    m_questsByItem.clear();
    m_questsByLevel.clear();
    
    // Ler cada /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param ++i Parâmetro ++i
 * @return Retorna registro
 */
 registro
    for(size_t i = 0; i < recordCount; ++i){
        QUEST_DATA questData;
        file.read(reinterpret_cast<char*>(&questData), sizeof(QUEST_DATA));
        
        uint16_t questId = questData.header.questId;
        
        // Validar ID da quest
        if (questId == 0 || m_questData.find(questId) != m_questData.end())
        {
            WLOG_WARNING("Quest inválida ou duplicada. ID: {}", questId);
            continue;
        }
        
        // Validar strings
        questData.header.name[MAX_QUEST_NAME_LENGTH - 1] = '\0';
        
        // Inserir nos mapas
        m_questData[questId] = questData;
        
        // Indexar por NPC
        if (questData.header.startNpcId > 0)
            m_questsByNPC[questData.header.startNpcId].push_back(questId);
        
        if (questData.header.endNpcId > 0 && questData.header.endNpcId != questData.header.startNpcId)
            m_questsByNPC[questData.header.endNpcId].push_back(questId);
        
        // Indexar por mapa
        if (questData.header.mapId > 0)
            m_questsByMap[questData.header.mapId].push_back(questId);
        
        // Indexar por nível
        for (uint16_t level = questData.header.minLevel; level <= questData.header.maxLevel; ++level)
            m_questsByLevel[level].push_back(questId);
        
        // Indexar objetivos por mob e /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param ++j Parâmetro ++j
 * @return Retorna item
 */
 item
        for(uint8_t j = 0; j < questData.header.objectiveCount; ++j){
            const QUEST_OBJECTIVE& obj = questData.objectives[j];
            
            switch (obj.type)
            {
                case QUEST_OBJECTIVE_KILL:
                    if (obj.data.kill.mobId > 0)
                        m_questsByMob[obj.data.kill.mobId].push_back(questId);
                    break;
                
                case QUEST_OBJECTIVE_COLLECT:
                case QUEST_OBJECTIVE_DELIVER:
                    if (obj.data.item.itemId > 0)
                        m_questsByItem[obj.data.item.itemId].push_back(questId);
                    break;
                
                case QUEST_OBJECTIVE_TALK:
                    if (obj.data.talk.npcId > 0)
                        m_questsByNPC[obj.data.talk.npcId].push_back(questId);
                    break;
                
                case QUEST_OBJECTIVE_VISIT:
                    if (obj.data.visit.mapId > 0)
                        m_questsByMap[obj.data.visit.mapId].push_back(questId);
                    break;
            }
        }
    }
    
    // Remover duplicatas dos í/**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param m_questsByNPC Parâmetro m_questsByNPC
 * @return Retorna ndices
 */
ndices
    for(auto& pair : m_questsByNPC){
        auto& vec = pair.second;
        std::sort(vec.begin(), vec.end());
        vec.erase(std::unique(vec.begin(), vec.end()), vec.end());
    }
    
    for (auto& pair : m_questsByMap)
    {
        auto& vec = pair.second;
        std::sort(vec.begin(), vec.end());
        vec.erase(std::unique(vec.begin(), vec.end()), vec.end());
    }
    
    for (auto& pair : m_questsByMob)
    {
        auto& vec = pair.second;
        std::sort(vec.begin(), vec.end());
        vec.erase(std::unique(vec.begin(), vec.end()), vec.end());
    }
    
    for (auto& pair : m_questsByItem)
    {
        auto& vec = pair.second;
        std::sort(vec.begin(), vec.end());
        vec.erase(std::unique(vec.begin(), vec.end()), vec.end());
    }
    
    for (auto& pair : m_questsByLevel)
    {
        auto& vec = pair.second;
        std::sort(vec.begin(), vec.end());
        vec.erase(std::unique(vec.begin(), vec.end()), vec.end());
    }
    
    WLOG_INFO("Carregadas {} quests do arquivo {}", m_questData.size(), filename);
    return true;
}

/**
 * Carrega diálogos de quests do arquivo binário
 */
bool QuestSystem::LoadQuestDialogues(const char* filename)
{
    WLOG_INFO("Carregando diálogos de quests de: {}", filename);
    
    std::lock_guard<std::mutex> dataLock(m_questDataMutex);
    
    // Verificar existência do arquivo
    if (!std::filesystem::exists(filename))
    {
        WLOG_ERROR("Arquivo de diálogos não encontrado: {}", filename);
        return false;
    }
    
    /**
 * file
 * 
 * Implementa a funcionalidade file conforme especificação original.
 * @param filename Parâmetro filename
 * @param std::ios::binary Parâmetro std::ios::binary
 * @return Retorna std::ifstream
 */

    
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open())
    {
        WLOG_ERROR("Falha ao abrir arquivo de diálogos: {}", filename);
        return false;
    }
    
    // Verificar tamanho do arquivo
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    
    // Estrutura do arquivo: [uint16_t count][QuestDialogue entries...]
    struct QuestDialogue {
        uint16_t questId;
        char start[MAX_DIALOGUE_LENGTH];
        char progress[MAX_DIALOGUE_LENGTH];
        char complete[MAX_DIALOGUE_LENGTH];
        char fail[MAX_DIALOGUE_LENGTH];
    };
    
    // Verificar header do arquivo
    uint16_t recordCount = 0;
    file.read(reinterpret_cast<char*>(&recordCount), sizeof(recordCount));
    
    if (recordCount == 0 || fileSize < sizeof(recordCount) + sizeof(QuestDialogue) * recordCount)
    {
        WLOG_ERROR("Arquivo de diálogos inválido ou corrompido: {}", filename);
        return false;
    }
    
    // Limpar dados existentes
    m_questDialogues.clear();
    
    // Ler cada /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param ++i Parâmetro ++i
 * @return Retorna registro
 */
 registro
    for(size_t i = 0; i < recordCount; ++i){
        QuestDialogue dialogue;
        file.read(reinterpret_cast<char*>(&dialogue), sizeof(QuestDialogue));
        
        // Validar ID da quest
        if (dialogue.questId == 0 || m_questData.find(dialogue.questId) == m_questData.end())
        {
            WLOG_WARNING("Diálogo para quest inexistente. ID: {}", dialogue.questId);
            continue;
        }
        
        // Garantir strings terminadas em null
        dialogue.start[MAX_DIALOGUE_LENGTH - 1] = '\0';
        dialogue.progress[MAX_DIALOGUE_LENGTH - 1] = '\0';
        dialogue.complete[MAX_DIALOGUE_LENGTH - 1] = '\0';
        dialogue.fail[MAX_DIALOGUE_LENGTH - 1] = '\0';
        
        // Inserir no mapa
        QuestDialogueSet dialogueSet;
        dialogueSet.start = dialogue.start;
        dialogueSet.progress = dialogue.progress;
        dialogueSet.complete = dialogue.complete;
        dialogueSet.fail = dialogue.fail;
        
        m_questDialogues[dialogue.questId] = dialogueSet;
    }
    
    WLOG_INFO("Carregados diálogos para {} quests do arquivo {}", m_questDialogues.size(), filename);
    return true;
}

/**
 * Carrega recompensas de quests do arquivo binário
 */
bool QuestSystem::LoadQuestRewards(const char* filename)
{
    WLOG_INFO("Carregando recompensas de quests de: {}", filename);
    
    std::lock_guard<std::mutex> dataLock(m_questDataMutex);
    
    // Verificar existência do arquivo
    if (!std::filesystem::exists(filename))
    {
        WLOG_ERROR("Arquivo de recompensas não encontrado: {}", filename);
        return false;
    }
    
    /**
 * file
 * 
 * Implementa a funcionalidade file conforme especificação original.
 * @param filename Parâmetro filename
 * @param std::ios::binary Parâmetro std::ios::binary
 * @return Retorna std::ifstream
 */

    
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open())
    {
        WLOG_ERROR("Falha ao abrir arquivo de recompensas: {}", filename);
        return false;
    }
    
    // Verificar tamanho do arquivo
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    
    // Verificar header do arquivo
    uint16_t recordCount = 0;
    file.read(reinterpret_cast<char*>(&recordCount), sizeof(recordCount));
    
    if (recordCount == 0 || fileSize < sizeof(recordCount) + sizeof(QUEST_REWARD) * recordCount)
    {
        WLOG_ERROR("Arquivo de recompensas inválido ou corrompido: {}", filename);
        return false;
    }
    
    // Limpar dados existentes
    m_questRewards.clear();
    
    // Ler cada /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param ++i Parâmetro ++i
 * @return Retorna registro
 */
 registro
    for(size_t i = 0; i < recordCount; ++i){
        uint16_t questId;
        QUEST_REWARD reward;
        
        file.read(reinterpret_cast<char*>(&questId), sizeof(questId));
        file.read(reinterpret_cast<char*>(&reward), sizeof(QUEST_REWARD));
        
        // Validar ID da quest
        if (questId == 0 || m_questData.find(questId) == m_questData.end())
        {
            WLOG_WARNING("Recompensa para quest inexistente. ID: {}", questId);
            continue;
        }
        
        // Validar itens de recompensa
        bool valid = true;
        for (int j = 0; j < MAX_QUEST_ITEMS; ++j)
        {
            if (reward.itemId[j] != 0 && reward.itemCount[j] == 0)
            {
                WLOG_WARNING("Recompensa com quantidade 0 para item {}. Quest ID: {}", reward.itemId[j], questId);
                valid = false;
                break;
            }
        }
        
        if (!valid)
            continue;
        
        // Inserir no mapa
        m_questRewards[questId] = reward;
    }
    
    WLOG_INFO("Carregadas recompensas para {} quests do arquivo {}", m_questRewards.size(), filename);
    return true;
}

/**
 * Carrega scripts de quests do arquivo binário
 */
bool QuestSystem::LoadQuestScripts(const char* filename)
{
    WLOG_INFO("Carregando scripts de quests de: {}", filename);
    
    // Verificar existência do arquivo
    if (!std::filesystem::exists(filename))
    {
        WLOG_WARNING("Arquivo de scripts não encontrado: {}. Usando scripts padrão.", filename);
        return true; // Scripts são opcionais
    }
    
    // Implementação básica por enquanto
    return true;
}

/**
 * Verifica se uma quest existe
 */
bool QuestSystem::IsValidQuest(uint16_t questId) const
{
    if (questId == 0)
        return false;
    
    std::lock_guard<std::mutex> dataLock(m_questDataMutex);
    return m_questData.find(questId) != m_questData.end();
}

/**
 * Obtém os dados de uma quest
 */
const QUEST_DATA* QuestSystem::GetQuestData(uint16_t questId) const
{
    if (questId == 0)
        return nullptr;
    
    std::lock_guard<std::mutex> dataLock(m_questDataMutex);
    
    auto it = m_questData.find(questId);
    if (it == m_questData.end())
        return nullptr;
    
    return &it->second;
}

/**
 * Obtém diálogo de quest
 */
const char* QuestSystem::GetQuestDialogue(uint16_t questId, uint8_t dialogueType, TMCharacter* pChar) const
{
    if (questId == 0 || dialogueType > 3) // 0=start, 1=progress, 2=complete, 3=fail
        return nullptr;
    
    std::lock_guard<std::mutex> dataLock(m_questDataMutex);
    
    auto it = m_questDialogues.find(questId);
    if (it == m_questDialogues.end())
        return nullptr;
    
    const QuestDialogueSet& dialogues = it->second;
    
    switch (dialogueType)
    {
        case 0: return dialogues.start.c_str();
        case 1: return dialogues.progress.c_str();
        case 2: return dialogues.complete.c_str();
        case 3: return dialogues.fail.c_str();
        default: return nullptr;
    }
}

/**
 * Atualiza o sistema de quests (expiração, eventos automáticos)
 */
void QuestSystem::Update(uint64_t tick)
{
    if (!m_initialized)
        return;
    
    // Atualizar apenas em intervalos regulares (a cada 5 segundos)
    if (tick - m_lastUpdateTick < 5000)
        return;
    
    m_lastUpdateTick = tick;
    
    // Não podemos processar todos os jogadores de uma vez
    // Em um servidor real, processamos em batches de jogadores
    std::lock_guard<std::mutex> playerLock(m_playerMutex);
    
    // Atualizar até 100 jogadores por tick
    const size_t maxPlayersPerUpdate = 100;
    size_t count = 0;
    
    for (auto it = m_playerContexts.begin(); it != m_playerContexts.end() && count < maxPlayersPerUpdate; ++it, ++count)
    {
        uint32_t charId = it->first;
        TMCharacter* pChar = m_pServer->GetCharacterById(charId);
        
        if (!pChar)
            continue;
        
        // Processar quests com limite de /**
 * ProcessTimeLimitedQuests
 * 
 * Implementa a funcionalidade ProcessTimeLimitedQuests conforme especificação original.
 * @param pChar Parâmetro pChar
 * @param tick Parâmetro tick
 * @return Retorna tempo
 */
 tempo
        ProcessTimeLimitedQuests(pChar, tick);
        
        // Processar cooldowns de /**
 * ProcessQuestCooldowns
 * 
 * Implementa a funcionalidade ProcessQuestCooldowns conforme especificação original.
 * @param pChar Parâmetro pChar
 * @return Retorna quests
 */
 quests
        ProcessQuestCooldowns(pChar);
        
        // Verificar requisitos de guild para /**
 * CheckGuildQuestRequirements
 * 
 * Implementa a funcionalidade CheckGuildQuestRequirements conforme especificação original.
 * @param pChar Parâmetro pChar
 * @return Retorna quests
 */
 quests
        CheckGuildQuestRequirements(pChar);
    }
}

/**
 * Gera cache de requisitos de quests
 */
void QuestSystem::CacheQuestRequirements()
{
    WLOG_INFO("Gerando cache de requisitos de quests...");
    
    // Exemplo de código que seria implementado aqui:
    // 1. Gerar índices para acessos por mob, item, npc
    // 2. Verificar integridade de requisitos
    // 3. Construir grafo de dependências para detectar ciclos
}

/**
 * Constrói grafo de dependências de quests
 */
void QuestSystem::BuildQuestDependencyGraph()
{
    WLOG_INFO("Construindo grafo de dependências de quests...");
    
    std::lock_guard<std::mutex> dataLock(m_questDataMutex);
    
    m_questDependencies.clear();
    
    // Para cada quest, armazenar quais outras quests dependem /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param m_questData Parâmetro m_questData
 * @return Retorna dela
 */
 dela
    for(const auto& pair : m_questData){
        uint16_t questId = pair.first;
        const QUEST_DATA& questData = pair.second;
        
        // Verificar pré-/**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param ++i Parâmetro ++i
 * @return Retorna requisitos
 */
requisitos
        for(size_t i = 0; i < questData.header.prereqCount; ++i){
            uint16_t prereqId = questData.header.prereqQuests[i];
            
            if (prereqId > 0 && m_questData.find(prereqId) != m_questData.end())
            {
                // Esta quest (questId) depende de prereqId
                // Então prereqId tem questId como dependente
                m_questDependencies[prereqId].push_back(questId);
            }
        }
    }
    
    // Verificar ciclos de dependência (não implementado ainda)
    // DetectDependencyCycles();
    
    WLOG_INFO("Grafo de dependências construído para {} quests", m_questDependencies.size());
}

/**
 * Obtém a quest atual de um jogador por ID
 */
PLAYER_QUEST* QuestSystem::GetPlayerQuest(TMCharacter* pChar, uint16_t questId) const
{
    if (!pChar || questId == 0)
        return nullptr;
    
    PLAYER_QUEST_DATA* pQuestData = pChar->GetQuestData();
    if (!pQuestData)
        return nullptr;
    
    // Procurar nas quests /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param ++i Parâmetro ++i
 * @return Retorna ativas
 */
 ativas
    for(uint16_t i = 0; i < pQuestData->activeCount; ++i){
        if (pQuestData->activeQuests[i].questId == questId)
            return &pQuestData->activeQuests[i];
    }
    
    return nullptr;
}

/**
 * Verifica se um jogador tem uma quest ativa
 */
bool QuestSystem::HasActiveQuest(TMCharacter* pChar, uint16_t questId) const
{
    if (!pChar || questId == 0)
        return false;
    
    PLAYER_QUEST_DATA* pQuestData = pChar->GetQuestData();
    if (!pQuestData)
        return false;
    
    // Procurar nas quests /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param ++i Parâmetro ++i
 * @return Retorna ativas
 */
 ativas
    for(uint16_t i = 0; i < pQuestData->activeCount; ++i){
        if (pQuestData->activeQuests[i].questId == questId)
            return true;
    }
    
    return false;
}

/**
 * Verifica se um jogador completou uma quest
 */
bool QuestSystem::HasCompletedQuest(TMCharacter* pChar, uint16_t questId) const
{
    if (!pChar || questId == 0)
        return false;
    
    PLAYER_QUEST_DATA* pQuestData = pChar->GetQuestData();
    if (!pQuestData)
        return false;
    
    // Procurar nas quests /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param ++i Parâmetro ++i
 * @return Retorna completadas
 */
 completadas
    for(uint16_t i = 0; i < pQuestData->historyCount; ++i){
        if (pQuestData->completedQuests[i] == questId)
            return true;
    }
    
    return false;
}

/**
 * Verifica se um objetivo de quest está completo
 */
bool QuestSystem::IsQuestObjectiveComplete(const QUEST_DATA* pQuestData, const PLAYER_QUEST* pPlayerQuest, uint8_t objectiveIndex) const
{
    if (!pQuestData || !pPlayerQuest || objectiveIndex >= pQuestData->header.objectiveCount)
        return false;
    
    const QUEST_OBJECTIVE& objective = pQuestData->objectives[objectiveIndex];
    uint32_t progress = pPlayerQuest->progress[objectiveIndex];
    
    switch (objective.type)
    {
        case QUEST_OBJECTIVE_KILL:
            return progress >= objective.data.kill.count;
        
        case QUEST_OBJECTIVE_COLLECT:
        case QUEST_OBJECTIVE_DELIVER:
            return progress >= objective.data.item.count;
        
        case QUEST_OBJECTIVE_TALK:
        case QUEST_OBJECTIVE_VISIT:
            return progress > 0;
        
        case QUEST_OBJECTIVE_CUSTOM:
            // Objetivos customizados podem ter lógica específica
            // Neste caso, qualquer progresso positivo indica conclusão
            return progress > 0;
        
        default:
            return false;
    }
}

/**
 * Verifica se todos os objetivos obrigatórios de uma quest estão completos
 */
bool QuestSystem::HasAllRequiredObjectivesComplete(const QUEST_DATA* pQuestData, const PLAYER_QUEST* pPlayerQuest) const
{
    if (!pQuestData || !pPlayerQuest)
        return false;
    
    // Verificamos cada objetivo não-/**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param ++i Parâmetro ++i
 * @return Retorna opcional
 */
opcional
    for(uint8_t i = 0; i < pQuestData->header.objectiveCount; ++i){
        const QUEST_OBJECTIVE& objective = pQuestData->objectives[i];
        
        // Pular objetivos opcionais
        if (objective.flags & OBJECTIVE_FLAG_OPTIONAL)
            continue;
        
        // Se um objetivo obrigatório não está completo, retornamos false
        if (!IsQuestObjectiveComplete(pQuestData, pPlayerQuest, i))
            return false;
    }
    
    return true;
}

/**
 * Verifica se o jogador pode iniciar uma quest
 */
bool QuestSystem::CanStartQuest(TMCharacter* pChar, uint16_t questId, uint16_t npcId) const
{
    if (!pChar || questId == 0)
        return false;
    
    // Verificar se a quest existe
    const QUEST_DATA* pQuestData = GetQuestData(questId);
    if (!pQuestData)
    {
        WLOG_WARNING("Quest inexistente: {}", questId);
        return false;
    }
    
    // Verificar se o NPC é o correto para iniciar a quest (se fornecido)
    if (npcId != 0 && pQuestData->header.startNpcId != npcId)
    {
        WLOG_DEBUG("NPC incorreto para iniciar quest: {} (esperado: {})", npcId, pQuestData->header.startNpcId);
        return false;
    }
    
    // Verificar se o jogador tem nível adequado
    uint16_t level = pChar->GetLevel();
    if (level < pQuestData->header.minLevel || level > pQuestData->header.maxLevel)
    {
        WLOG_DEBUG("Nível inadequado para quest: {} (nível: {}, requerido: {}-{})", 
                 questId, level, pQuestData->header.minLevel, pQuestData->header.maxLevel);
        return false;
    }
    
    // Verificar se o jogador já tem esta quest ativa
    if (HasActiveQuest(pChar, questId))
    {
        WLOG_DEBUG("Jogador já tem a quest ativa: {}", questId);
        return false;
    }
    
    // Verificar se o jogador já completou esta quest e ela não é repetível
    if (HasCompletedQuest(pChar, questId) && !(pQuestData->header.flags & QUEST_FLAG_REPEATABLE))
    {
        WLOG_DEBUG("Quest não repetível já completada: {}", questId);
        return false;
    }
    
    // Verificar pré-requisitos
    if (!CheckQuestPrerequisites(pChar, questId))
    {
        WLOG_DEBUG("Pré-requisitos não atendidos para quest: {}", questId);
        return false;
    }
    
    // Verificar quests exclusivas (não pode ter quests que são mutuamente exclusivas)
    PLAYER_QUEST_DATA* pQuestData = pChar->GetQuestData();
    if (pQuestData)
    {
        for (uint16_t i = 0; i < pQuestData->activeCount; ++i)
        {
            uint16_t activeQuestId = pQuestData->activeQuests[i].questId;
            const QUEST_DATA* pActiveQuest = GetQuestData(activeQuestId);
            
            if (!pActiveQuest)
                continue;
            
            // Verificar se esta quest (questId) está na lista de exclusivas da quest /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param ++j Parâmetro ++j
 * @return Retorna ativa
 */
 ativa
            for(uint16_t j = 0; j < pActiveQuest->header.prereqCount; ++j){
                if (pActiveQuest->header.prereqQuests[j] == questId)
                {
                    WLOG_DEBUG("Quest exclusiva com quest ativa: {} (ativa: {})", questId, activeQuestId);
                    return false;
                }
            }
        }
    }
    
    // Verificar espaço para itens de quest (se necessário)
    if (!HasQuestItemSpace(pChar, questId))
    {
        WLOG_DEBUG("Sem espaço para itens de quest: {}", questId);
        return false;
    }
    
    return true;
}

/**
 * Verifica os pré-requisitos de uma quest
 */
bool QuestSystem::CheckQuestPrerequisites(TMCharacter* pChar, uint16_t questId) const
{
    if (!pChar || questId == 0)
        return false;
    
    const QUEST_DATA* pQuestData = GetQuestData(questId);
    if (!pQuestData)
        return false;
    
    // Se não tem pré-requisitos, sempre retorna true
    if (pQuestData->header.prereqCount == 0)
        return true;
    
    // Verificar cada pré-/**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param ++i Parâmetro ++i
 * @return Retorna requisito
 */
requisito
    for(uint16_t i = 0; i < pQuestData->header.prereqCount; ++i){
        uint16_t prereqId = pQuestData->header.prereqQuests[i];
        
        // Se o pré-requisito é zero, ignorar
        if (prereqId == 0)
            continue;
        
        // Verificar se o jogador completou este pré-requisito
        if (!HasCompletedQuest(pChar, prereqId))
            return false;
    }
    
    return true;
}

/**
 * Inicia uma quest para um jogador
 */
int QuestSystem::StartQuest(TMCharacter* pChar, uint16_t questId, uint16_t npcId)
{
    if (!pChar || questId == 0)
        return QUEST_ERROR_INVALID_ID;
    
    // Verificar se o jogador pode iniciar a quest
    if (!CanStartQuest(pChar, questId, npcId))
        return QUEST_ERROR_CONDITION;
    
    // Obter dados da quest
    const QUEST_DATA* pQuestData = GetQuestData(questId);
    if (!pQuestData)
        return QUEST_ERROR_INVALID_ID;
    
    // Obter dados de quest do jogador
    PLAYER_QUEST_DATA* pCharQuestData = pChar->GetQuestData();
    if (!pCharQuestData)
    {
        WLOG_ERROR("Jogador sem dados de quest: {}", pChar->GetId());
        return QUEST_ERROR_CONDITION;
    }
    
    // Verificar se há espaço para uma nova quest /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param MAX_ACTIVE_QUESTS Parâmetro MAX_ACTIVE_QUESTS
 * @return Retorna ativa
 */
 ativa
    if(pCharQuestData->activeCount >= MAX_ACTIVE_QUESTS){
        WLOG_WARNING("Limite de quests ativas atingido: {}", pChar->GetId());
        return QUEST_ERROR_CONDITION;
    }
    
    // Criar nova entrada de quest para o jogador
    PLAYER_QUEST& newQuest = pCharQuestData->activeQuests[pCharQuestData->activeCount];
    memset(&newQuest, 0, sizeof(PLAYER_QUEST));
    
    newQuest.questId = questId;
    newQuest.state = QUEST_STATE_ACTIVE;
    newQuest.startTime = static_cast<uint32_t>(std::time(nullptr));
    
    // Incrementar contador de quests ativas
    pCharQuestData->activeCount++;
    
    // Atualizar contadores e estatísticas
    m_activeQuestCount.fetch_add(1);
    m_questStartedCount.fetch_add(1);
    
    // Atualizar contexto do /**
 * UpdatePlayerQuestContext
 * 
 * Implementa a funcionalidade UpdatePlayerQuestContext conforme especificação original.
 * @param pChar Parâmetro pChar
 * @return Retorna jogador
 */
 jogador
    UpdatePlayerQuestContext(pChar);
    
    // Registrar em /**
 * LogQuestAction
 * 
 * Implementa a funcionalidade LogQuestAction conforme especificação original.
 * @param pChar Parâmetro pChar
 * @param questId Parâmetro questId
 * @param "START" Parâmetro "START"
 * @param QUEST_SUCCESS Parâmetro QUEST_SUCCESS
 * @return Retorna log
 */
 log
    LogQuestAction(pChar, questId, "START", QUEST_SUCCESS);
    
    // Enviar atualização para o /**
 * SendQuestProgressUpdate
 * 
 * Implementa a funcionalidade SendQuestProgressUpdate conforme especificação original.
 * @param pChar Parâmetro pChar
 * @param questId Parâmetro questId
 * @return Retorna cliente
 */
 cliente
    SendQuestProgressUpdate(pChar, questId);
    
    return QUEST_SUCCESS;
}

/**
 * Completa uma quest para um jogador
 */
int QuestSystem::CompleteQuest(TMCharacter* pChar, uint16_t questId, uint16_t npcId)
{
    if (!pChar || questId == 0)
        return QUEST_ERROR_INVALID_ID;
    
    // Verificar se o jogador tem a quest ativa
    PLAYER_QUEST* pQuest = GetPlayerQuest(pChar, questId);
    if (!pQuest)
        return QUEST_ERROR_ACTIVE;
    
    // Verificar se está no estado COMPLETE ou ACTIVE
    if (pQuest->state != QUEST_STATE_COMPLETE && pQuest->state != QUEST_STATE_ACTIVE)
        return QUEST_ERROR_CONDITION;
    
    // Obter dados da quest
    const QUEST_DATA* pQuestData = GetQuestData(questId);
    if (!pQuestData)
        return QUEST_ERROR_INVALID_ID;
    
    // Verificar se o NPC é o correto para completar a quest (se fornecido)
    if (npcId != 0 && pQuestData->header.endNpcId != npcId)
    {
        WLOG_WARNING("NPC incorreto para completar quest: {} (esperado: {})", npcId, pQuestData->header.endNpcId);
        return QUEST_ERROR_CONDITION;
    }
    
    // Se está no estado ACTIVE, verificar se todos os objetivos estão /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param QUEST_STATE_ACTIVE Parâmetro QUEST_STATE_ACTIVE
 * @return Retorna completos
 */
 completos
    if(pQuest->state == QUEST_STATE_ACTIVE){
        if (!HasAllRequiredObjectivesComplete(pQuestData, pQuest))
        {
            WLOG_WARNING("Tentativa de completar quest sem todos os objetivos: {}", questId);
            return QUEST_ERROR_CONDITION;
        }
        
        // Alterar estado para COMPLETE
        pQuest->state = QUEST_STATE_COMPLETE;
    }
    
    // Obter dados de quest do jogador
    PLAYER_QUEST_DATA* pCharQuestData = pChar->GetQuestData();
    if (!pCharQuestData)
    {
        WLOG_ERROR("Jogador sem dados de quest: {}", pChar->GetId());
        return QUEST_ERROR_CONDITION;
    }
    
    // Dar recompensas da quest
    if (!GiveQuestRewards(pChar, questId))
    {
        WLOG_WARNING("Falha ao dar recompensas da quest: {}", questId);
        return QUEST_ERROR_ITEM_FULL;
    }
    
    // Remover a quest da lista de ativas
    bool questRemoved = false;
    for (uint16_t i = 0; i < pCharQuestData->activeCount; ++i)
    {
        if (pCharQuestData->activeQuests[i].questId == questId)
        {
            // Encontramos a quest, remover da lista movendo as seguintes para trá/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 1 Parâmetro 1
 * @return Retorna s
 */
s
            if(i < pCharQuestData->activeCount - 1){
                memmove(&pCharQuestData->activeQuests[i], 
                       &pCharQuestData->activeQuests[i + 1], 
                       sizeof(PLAYER_QUEST) * (pCharQuestData->activeCount - i - 1));
            }
            
            pCharQuestData->activeCount--;
            questRemoved = true;
            break;
        }
    }
    
    if (!questRemoved)
    {
        WLOG_ERROR("Falha ao remover quest ativa: {}", questId);
        return QUEST_ERROR_CONDITION;
    }
    
    // Adicionar à lista de quests completadas (se ainda não estiver)
    bool alreadyCompleted = false;
    for (uint16_t i = 0; i < pCharQuestData->historyCount; ++i)
    {
        if (pCharQuestData->completedQuests[i] == questId)
        {
            alreadyCompleted = true;
            break;
        }
    }
    
    if (!alreadyCompleted && pCharQuestData->historyCount < MAX_QUEST_HISTORY)
    {
        pCharQuestData->completedQuests[pCharQuestData->historyCount] = questId;
        pCharQuestData->historyCount++;
    }
    
    // Remover itens temporários de /**
 * RemoveQuestItems
 * 
 * Implementa a funcionalidade RemoveQuestItems conforme especificação original.
 * @param pChar Parâmetro pChar
 * @param questId Parâmetro questId
 * @return Retorna quest
 */
 quest
    RemoveQuestItems(pChar, questId);
    
    // Atualizar contadores e estatísticas
    m_activeQuestCount.fetch_sub(1);
    m_questCompletedCount.fetch_add(1);
    
    // Atualizar contexto do /**
 * UpdatePlayerQuestContext
 * 
 * Implementa a funcionalidade UpdatePlayerQuestContext conforme especificação original.
 * @param pChar Parâmetro pChar
 * @return Retorna jogador
 */
 jogador
    UpdatePlayerQuestContext(pChar);
    
    // Registrar em /**
 * LogQuestAction
 * 
 * Implementa a funcionalidade LogQuestAction conforme especificação original.
 * @param pChar Parâmetro pChar
 * @param questId Parâmetro questId
 * @param "COMPLETE" Parâmetro "COMPLETE"
 * @param QUEST_SUCCESS Parâmetro QUEST_SUCCESS
 * @return Retorna log
 */
 log
    LogQuestAction(pChar, questId, "COMPLETE", QUEST_SUCCESS);
    
    return QUEST_SUCCESS;
}

/**
 * Abandona uma quest
 */
int QuestSystem::AbandonQuest(TMCharacter* pChar, uint16_t questId)
{
    if (!pChar || questId == 0)
        return QUEST_ERROR_INVALID_ID;
    
    // Verificar se o jogador tem a quest ativa
    if (!HasActiveQuest(pChar, questId))
        return QUEST_ERROR_ACTIVE;
    
    // Obter dados de quest do jogador
    PLAYER_QUEST_DATA* pCharQuestData = pChar->GetQuestData();
    if (!pCharQuestData)
    {
        WLOG_ERROR("Jogador sem dados de quest: {}", pChar->GetId());
        return QUEST_ERROR_CONDITION;
    }
    
    // Remover a quest da lista de ativas
    bool questRemoved = false;
    for (uint16_t i = 0; i < pCharQuestData->activeCount; ++i)
    {
        if (pCharQuestData->activeQuests[i].questId == questId)
        {
            // Encontramos a quest, remover da lista movendo as seguintes para trá/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 1 Parâmetro 1
 * @return Retorna s
 */
s
            if(i < pCharQuestData->activeCount - 1){
                memmove(&pCharQuestData->activeQuests[i], 
                       &pCharQuestData->activeQuests[i + 1], 
                       sizeof(PLAYER_QUEST) * (pCharQuestData->activeCount - i - 1));
            }
            
            pCharQuestData->activeCount--;
            questRemoved = true;
            break;
        }
    }
    
    if (!questRemoved)
    {
        WLOG_ERROR("Falha ao remover quest ativa: {}", questId);
        return QUEST_ERROR_CONDITION;
    }
    
    // Remover itens temporários de /**
 * RemoveQuestItems
 * 
 * Implementa a funcionalidade RemoveQuestItems conforme especificação original.
 * @param pChar Parâmetro pChar
 * @param questId Parâmetro questId
 * @return Retorna quest
 */
 quest
    RemoveQuestItems(pChar, questId);
    
    // Atualizar contadores e estatísticas
    m_activeQuestCount.fetch_sub(1);
    
    // Atualizar contexto do /**
 * UpdatePlayerQuestContext
 * 
 * Implementa a funcionalidade UpdatePlayerQuestContext conforme especificação original.
 * @param pChar Parâmetro pChar
 * @return Retorna jogador
 */
 jogador
    UpdatePlayerQuestContext(pChar);
    
    // Registrar em /**
 * LogQuestAction
 * 
 * Implementa a funcionalidade LogQuestAction conforme especificação original.
 * @param pChar Parâmetro pChar
 * @param questId Parâmetro questId
 * @param "ABANDON" Parâmetro "ABANDON"
 * @param QUEST_SUCCESS Parâmetro QUEST_SUCCESS
 * @return Retorna log
 */
 log
    LogQuestAction(pChar, questId, "ABANDON", QUEST_SUCCESS);
    
    return QUEST_SUCCESS;
}

/**
 * Dá as recompensas de uma quest para um jogador
 */
bool QuestSystem::GiveQuestRewards(TMCharacter* pChar, uint16_t questId)
{
    if (!pChar || questId == 0)
        return false;
    
    // Obter recompensas da quest
    std::lock_guard<std::mutex> dataLock(m_questDataMutex);
    auto it = m_questRewards.find(questId);
    if (it == m_questRewards.end())
    {
        WLOG_WARNING("Recompensas não encontradas para quest: {}", questId);
        return true;  // Não é erro crítico se não houver recompensas definidas
    }
    
    const QUEST_REWARD& reward = it->second;
    
    // Dar experiê/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna ncia
 */
ncia
    if(reward.exp > 0){
        pChar->AddExp(reward.exp);
        WLOG_DEBUG("Quest {} - XP concedido: {}", questId, reward.exp);
    }
    
    // Dar /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna gold
 */
 gold
    if(reward.gold > 0){
        pChar->AddGold(reward.gold);
        WLOG_DEBUG("Quest {} - Gold concedido: {}", questId, reward.gold);
    }
    
    // Dar itens de recompensa
    bool allItemsGiven = true;
    for (int i = 0; i < MAX_QUEST_ITEMS; ++i)
    {
        uint16_t itemId = reward.itemId[i];
        uint8_t count = reward.itemCount[i];
        
        if (itemId == 0 || count == 0)
            continue;
        
        // Verificar se tem espaço no inventário
        if (!pChar->HasInventorySpace(itemId, count))
        {
            WLOG_WARNING("Sem espaço para item de recompensa: {} (quest: {})", itemId, questId);
            allItemsGiven = false;
            continue;
        }
        
        // Adicionar item ao inventário
        bool success = pChar->AddItem(itemId, count);
        if (!success)
        {
            WLOG_ERROR("Falha ao adicionar item de recompensa: {} (quest: {})", itemId, questId);
            allItemsGiven = false;
        }
        else
        {
            WLOG_DEBUG("Quest {} - Item concedido: {} x{}", questId, itemId, count);
        }
    }
    
    return allItemsGiven;
}

/**
 * Atualiza o progresso de um objetivo de quest
 */
bool QuestSystem::UpdateQuestProgress(TMCharacter* pChar, uint16_t questId, uint8_t objectiveIndex, uint32_t progress)
{
    if (!pChar || questId == 0 || objectiveIndex >= MAX_OBJECTIVES)
        return false;
    
    // Verificar se o jogador tem a quest ativa
    PLAYER_QUEST* pQuest = GetPlayerQuest(pChar, questId);
    if (!pQuest)
        return false;
    
    // Verificar se a quest está no estado ACTIVE
    if (pQuest->state != QUEST_STATE_ACTIVE)
        return false;
    
    // Obter dados da quest
    const QUEST_DATA* pQuestData = GetQuestData(questId);
    if (!pQuestData)
        return false;
    
    // Verificar se o objective existe
    if (objectiveIndex >= pQuestData->header.objectiveCount)
        return false;
    
    // Atualizar progresso
    pQuest->progress[objectiveIndex] = progress;
    
    // Verificar se todos os objetivos estão completos
    bool allComplete = HasAllRequiredObjectivesComplete(pQuestData, pQuest);
    
    // Se todos os objetivos estão completos e a quest tem auto-complete, completar
    if (allComplete && (pQuestData->header.flags & QUEST_FLAG_AUTO_COMPLETE))
    {
        // Completar automaticamente a quest
        pQuest->state = QUEST_STATE_COMPLETE;
        ProcessAutoCompletion(pChar, pQuest);
    }
    
    // Enviar atualização para o /**
 * SendQuestProgressUpdate
 * 
 * Implementa a funcionalidade SendQuestProgressUpdate conforme especificação original.
 * @param pChar Parâmetro pChar
 * @param questId Parâmetro questId
 * @return Retorna cliente
 */
 cliente
    SendQuestProgressUpdate(pChar, questId);
    
    return true;
}

/**
 * Atualiza o progresso de objetivos de matar
 */
bool QuestSystem::UpdateKillProgress(TMCharacter* pChar, uint16_t mobId, uint16_t count)
{
    if (!pChar || mobId == 0)
        return false;
    
    bool updated = false;
    std::lock_guard<std::mutex> playerLock(m_playerMutex);
    
    // Verificar contexto do jogador para otimização
    auto it = m_playerContexts.find(pChar->GetId());
    if (it != m_playerContexts.end())
    {
        const PlayerQuestContext& context = it->second;
        
        // Se este mob não está sendo rastreado por nenhuma quest ativa, retornar rapidamente
        if (std::find(context.trackedMobs.begin(), context.trackedMobs.end(), mobId) == context.trackedMobs.end())
            return false;
    }
    
    // Obter dados de quest do jogador
    PLAYER_QUEST_DATA* pCharQuestData = pChar->GetQuestData();
    if (!pCharQuestData)
        return false;
    
    // Verificar cada quest ativa do /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param ++i Parâmetro ++i
 * @return Retorna jogador
 */
 jogador
    for(uint16_t i = 0; i < pCharQuestData->activeCount; ++i){
        PLAYER_QUEST& playerQuest = pCharQuestData->activeQuests[i];
        
        // Pular se não está no estado ACTIVE
        if (playerQuest.state != QUEST_STATE_ACTIVE)
            continue;
        
        uint16_t questId = playerQuest.questId;
        
        // Obter dados da quest
        const QUEST_DATA* pQuestData = GetQuestData(questId);
        if (!pQuestData)
            continue;
        
        // Verificar cada objetivo da /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param ++j Parâmetro ++j
 * @return Retorna quest
 */
 quest
        for(uint8_t j = 0; j < pQuestData->header.objectiveCount; ++j){
            const QUEST_OBJECTIVE& objective = pQuestData->objectives[j];
            
            // Verificar se é objetivo de matar e se é o mob /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param mobId Parâmetro mobId
 * @return Retorna correto
 */
 correto
            if(objective.type == QUEST_OBJECTIVE_KILL && objective.data.kill.mobId == mobId){
                // Atualizar progresso
                uint32_t currentProgress = playerQuest.progress[j];
                uint32_t newProgress = std::min<uint32_t>(currentProgress + count, objective.data.kill.count);
                
                // Se o progresso mudou, /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param currentProgress Parâmetro currentProgress
 * @return Retorna atualizar
 */
 atualizar
                if(newProgress > currentProgress){
                    playerQuest.progress[j] = newProgress;
                    updated = true;
                    
                    WLOG_DEBUG("Quest {} - Progresso de kill atualizado: {}/{}", 
                             questId, newProgress, objective.data.kill.count);
                    
                    // Verificar se todos os objetivos estão completos
                    if (HasAllRequiredObjectivesComplete(pQuestData, &playerQuest))
                    {
                        // Se a quest tem auto-complete, completar /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param QUEST_FLAG_AUTO_COMPLETE Parâmetro QUEST_FLAG_AUTO_COMPLETE
 * @return Retorna automaticamente
 */
 automaticamente
                        if(pQuestData->header.flags & QUEST_FLAG_AUTO_COMPLETE){
                            playerQuest.state = QUEST_STATE_COMPLETE;
                            ProcessAutoCompletion(pChar, &playerQuest);
                        }
                        else
                        {
                            // Apenas marcar como completa
                            playerQuest.state = QUEST_STATE_COMPLETE;
                            SendQuestProgressUpdate(pChar, questId);
                        }
                    }
                    else
                    {
                        // Enviar atualização para o /**
 * SendQuestProgressUpdate
 * 
 * Implementa a funcionalidade SendQuestProgressUpdate conforme especificação original.
 * @param pChar Parâmetro pChar
 * @param questId Parâmetro questId
 * @return Retorna cliente
 */
 cliente
                        SendQuestProgressUpdate(pChar, questId);
                    }
                }
            }
        }
    }
    
    return updated;
}

/**
 * Processa auto-completion de quests
 */
bool QuestSystem::ProcessAutoCompletion(TMCharacter* pChar, PLAYER_QUEST* pQuest)
{
    if (!pChar || !pQuest)
        return false;
    
    uint16_t questId = pQuest->questId;
    
    // Verificar se a quest está no estado COMPLETE
    if (pQuest->state != QUEST_STATE_COMPLETE)
        return false;
    
    // Obter dados da quest
    const QUEST_DATA* pQuestData = GetQuestData(questId);
    if (!pQuestData)
        return false;
    
    // Verificar se a quest tem auto-complete habilitado
    if (!(pQuestData->header.flags & QUEST_FLAG_AUTO_COMPLETE))
        return false;
    
    // Completar a quest automaticamente
    int result = CompleteQuest(pChar, questId, 0);
    
    // Gerar texto flutuante para o /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param QUEST_SUCCESS Parâmetro QUEST_SUCCESS
 * @return Retorna jogador
 */
 jogador
    if(result == QUEST_SUCCESS){
        std::string completionText = "Quest Completa: ";
        completionText += pQuestData->header.name;
        GenerateQuestFloatingText(pChar, questId, completionText.c_str());
    }
    
    return (result == QUEST_SUCCESS);
}

/**
 * Verifica se o jogador tem espaço para itens de quest
 */
bool QuestSystem::HasQuestItemSpace(TMCharacter* pChar, uint16_t questId) const
{
    if (!pChar || questId == 0)
        return false;
    
    // Para demonstração, consideramos que sempre há espaço
    // Em um sistema real, verificaríamos quais itens a quest fornece
    // e se o jogador tem espaço no inventário
    
    return true;
}

/**
 * Remove itens temporários de quest
 */
void QuestSystem::RemoveQuestItems(TMCharacter* pChar, uint16_t questId)
{
    if (!pChar || questId == 0)
        return;
    
    // Para demonstração, não implementamos a remoção de itens
    // Em um sistema real, removeríamos todos os itens temporários
    // associados a esta quest
}

/**
 * Atualiza o contexto de quests do jogador para otimizações
 */
void QuestSystem::UpdatePlayerQuestContext(TMCharacter* pChar)
{
    if (!pChar)
        return;
    
    uint32_t charId = pChar->GetId();
    
    // Obter ou criar contexto
    PlayerQuestContext& context = m_playerContexts[charId];
    context.characterId = charId;
    context.lastUpdate = static_cast<uint32_t>(std::time(nullptr));
    
    // Limpar listas de rastreamento
    context.trackedMobs.clear();
    context.trackedItems.clear();
    context.trackedNPCs.clear();
    
    // Obter dados de quest do jogador
    PLAYER_QUEST_DATA* pCharQuestData = pChar->GetQuestData();
    if (!pCharQuestData)
        return;
    
    // Para cada quest ativa, rastrear mobs, itens e NPCs /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param ++i Parâmetro ++i
 * @return Retorna relevantes
 */
 relevantes
    for(uint16_t i = 0; i < pCharQuestData->activeCount; ++i){
        PLAYER_QUEST& playerQuest = pCharQuestData->activeQuests[i];
        uint16_t questId = playerQuest.questId;
        
        // Pular se não está no estado ACTIVE
        if (playerQuest.state != QUEST_STATE_ACTIVE)
            continue;
        
        // Obter dados da quest
        const QUEST_DATA* pQuestData = GetQuestData(questId);
        if (!pQuestData)
            continue;
        
        // Rastrear quests /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param ++j Parâmetro ++j
 * @return Retorna pendentes
 */
 pendentes
        for(uint8_t j = 0; j < pQuestData->header.objectiveCount; ++j){
            // Pular objetivos já completos
            if (IsQuestObjectiveComplete(pQuestData, &playerQuest, j))
                continue;
            
            const QUEST_OBJECTIVE& objective = pQuestData->objectives[j];
            
            switch (objective.type)
            {
                case QUEST_OBJECTIVE_KILL:
                    context.trackedMobs.push_back(objective.data.kill.mobId);
                    break;
                
                case QUEST_OBJECTIVE_COLLECT:
                case QUEST_OBJECTIVE_DELIVER:
                    context.trackedItems.push_back(objective.data.item.itemId);
                    if (objective.type == QUEST_OBJECTIVE_DELIVER && objective.data.item.npcId > 0)
                        context.trackedNPCs.push_back(objective.data.item.npcId);
                    break;
                
                case QUEST_OBJECTIVE_TALK:
                    context.trackedNPCs.push_back(objective.data.talk.npcId);
                    break;
            }
        }
    }
    
    // Remover duplicatas
    std::sort(context.trackedMobs.begin(), context.trackedMobs.end());
    context.trackedMobs.erase(std::unique(context.trackedMobs.begin(), context.trackedMobs.end()), context.trackedMobs.end());
    
    std::sort(context.trackedItems.begin(), context.trackedItems.end());
    context.trackedItems.erase(std::unique(context.trackedItems.begin(), context.trackedItems.end()), context.trackedItems.end());
    
    std::sort(context.trackedNPCs.begin(), context.trackedNPCs.end());
    context.trackedNPCs.erase(std::unique(context.trackedNPCs.begin(), context.trackedNPCs.end()), context.trackedNPCs.end());
}

/**
 * Processa quests com limite de tempo
 */
void QuestSystem::ProcessTimeLimitedQuests(TMCharacter* pChar, uint64_t tick)
{
    if (!pChar)
        return;
    
    // Obter dados de quest do jogador
    PLAYER_QUEST_DATA* pCharQuestData = pChar->GetQuestData();
    if (!pCharQuestData)
        return;
    
    uint32_t currentTime = static_cast<uint32_t>(std::time(nullptr));
    std::vector<uint16_t> failedQuests;
    
    // Verificar cada quest /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param ++i Parâmetro ++i
 * @return Retorna ativa
 */
 ativa
    for(uint16_t i = 0; i < pCharQuestData->activeCount; ++i){
        PLAYER_QUEST& playerQuest = pCharQuestData->activeQuests[i];
        
        // Pular se não está no estado ACTIVE
        if (playerQuest.state != QUEST_STATE_ACTIVE)
            continue;
        
        uint16_t questId = playerQuest.questId;
        
        // Obter dados da quest
        const QUEST_DATA* pQuestData = GetQuestData(questId);
        if (!pQuestData)
            continue;
        
        // Verificar se a quest tem limite de /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna tempo
 */
 tempo
        if(pQuestData->header.flags & QUEST_FLAG_TIME_LIMIT && pQuestData->header.timeLimit > 0){
            // Calcular tempo restante
            uint32_t questDuration = currentTime - playerQuest.startTime;
            uint32_t timeLimit = pQuestData->header.timeLimit * 60; // Converter para segundos
            
            // Se o tempo expirou, marcar como /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param timeLimit Parâmetro timeLimit
 * @return Retorna falha
 */
 falha
            if(questDuration > timeLimit){
                playerQuest.state = QUEST_STATE_FAILED;
                playerQuest.endTime = currentTime;
                failedQuests.push_back(questId);
                
                WLOG_INFO("Quest expirada: {} (jogador: {})", questId, pChar->GetId());
            }
        }
    }
    
    // Processar quests que /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param failedQuests Parâmetro failedQuests
 * @return Retorna falharam
 */
 falharam
    for(uint16_t questId : failedQuests){
        // Enviar notificação para o cliente
        // Remover itens temporários de /**
 * RemoveQuestItems
 * 
 * Implementa a funcionalidade RemoveQuestItems conforme especificação original.
 * @param pChar Parâmetro pChar
 * @param questId Parâmetro questId
 * @return Retorna quest
 */
 quest
        RemoveQuestItems(pChar, questId);
    }
}

/**
 * Verifica requisitos de Guild para quests
 */
void QuestSystem::CheckGuildQuestRequirements(TMCharacter* pChar)
{
    // Implementação futura para verificar requisitos de Guild para quests
}

/**
 * Processa cooldowns de quests
 */
void QuestSystem::ProcessQuestCooldowns(TMCharacter* pChar)
{
    // Implementação futura para processar cooldowns de quests repetíveis
}

/**
 * Gera texto flutuante para o jogador
 */
void QuestSystem::GenerateQuestFloatingText(TMCharacter* pChar, uint16_t questId, const char* text)
{
    if (!pChar || !text)
        return;
    
    // Implementação simulada - em um sistema real, enviaríamos um packet para o /**
 * WLOG_DEBUG
 * 
 * Implementa a funcionalidade WLOG_DEBUG conforme especificação original.
 * @param \"{}\"" Parâmetro \"{}\""
 * @param questId Parâmetro questId
 * @param text Parâmetro text
 * @return Retorna cliente
 */
 cliente
    WLOG_DEBUG("Quest Texto Flutuante: {} - \"{}\"", questId, text);
}

/**
 * Envia atualização de progresso de quest para o cliente
 */
void QuestSystem::SendQuestProgressUpdate(TMCharacter* pChar, uint16_t questId)
{
    if (!pChar)
        return;
    
    // Implementação simulada - em um sistema real, enviaríamos um packet para o cliente
    WLOG_DEBUG("Enviada atualização de progresso da quest {} para o jogador {}", questId, pChar->GetId());
}

/**
 * Registra ação de quest em logs
 */
void QuestSystem::LogQuestAction(TMCharacter* pChar, uint16_t questId, const char* action, int result)
{

} // namespace wydbr

    if (!pChar || !action)
        return;
    
    const QUEST_DATA* pQuestData = GetQuestData(questId);
    
    WLOG_INFO("QUEST: {} [{}] - char: {} [{}], quest: {} [{}], result: {}",
            action,
            result,
            pChar->GetName(),
            pChar->GetId(),
            pQuestData ? pQuestData->header.name : "Unknown",
            questId,
            result);
}