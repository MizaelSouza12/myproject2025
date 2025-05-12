/**
 * CharacterDB.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/include/database/CharacterDB.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef _CHARACTER_DB_H_
#define _CHARACTER_DB_H_

/**
 * @file CharacterDB.h
 * @brief Gerenciador de banco de dados de personagens do WYD
 * 
 * Este arquivo contém o sistema de gerenciamento de banco de dados de personagens do WYDBR,
 * responsável por armazenar e gerenciar informações de personagens,
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
#include "DatabaseManager.h"

namespace wydbr {
namespace database {

/**
 * @brief Estado do personagem
 */
enum /**
 * Classe WYDCharacterState
 * 
 * Esta classe implementa a funcionalidade WYDCharacterState conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class CharacterState {
    NONE = 0,               // Nenhum
    NORMAL = 1,             // Normal
    DELETED = 2,            // Excluído
    BLOCKED = 3,            // Bloqueado
    TRANSFERRING = 4,       // Transferindo
    RESTORING = 5,          // Restaurando
    CUSTOM_1 = 6,           // Personalizado 1
    CUSTOM_2 = 7,           // Personalizado 2
    CUSTOM_3 = 8,           // Personalizado 3
    UNKNOWN = 9             // Desconhecido
};

/**
 * @brief Resultado da operação de personagem
 */
enum /**
 * Classe WYDCharacterResult
 * 
 * Esta classe implementa a funcionalidade WYDCharacterResult conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class CharacterResult {
    SUCCESS = 0,                    // Sucesso
    FAILED = 1,                     // Falha
    CHARACTER_EXISTS = 2,           // Personagem existe
    CHARACTER_NOT_FOUND = 3,        // Personagem não encontrado
    CHARACTER_BLOCKED = 4,          // Personagem bloqueado
    CHARACTER_DELETED = 5,          // Personagem excluído
    CHARACTER_LIMIT_REACHED = 6,    // Limite de personagens atingido
    INVALID_NAME = 7,               // Nome inválido
    INVALID_CLASS = 8,              // Classe inválida
    INVALID_LEVEL = 9,              // Nível inválido
    INVALID_STATS = 10,             // Estatísticas inválidas
    INVALID_POSITION = 11,          // Posição inválida
    NAME_TAKEN = 12,                // Nome já utilizado
    ACCOUNT_NOT_FOUND = 13,         // Conta não encontrada
    ALREADY_ONLINE = 14,            // Já está online
    CUSTOM_1 = 15,                  // Personalizado 1
    CUSTOM_2 = 16,                  // Personalizado 2
    CUSTOM_3 = 17,                  // Personalizado 3
    UNKNOWN = 18                    // Desconhecido
};

/**
 * @brief Tipo de log de personagem
 */
enum /**
 * Classe WYDCharacterLogType
 * 
 * Esta classe implementa a funcionalidade WYDCharacterLogType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class CharacterLogType {
    NONE = 0,               // Nenhum
    CREATION = 1,           // Criação
    DELETION = 2,           // Exclusão
    LOGIN = 3,              // Login
    LOGOUT = 4,             // Logout
    LEVEL_UP = 5,           // Subida de nível
    DEATH = 6,              // Morte
    RESURRECTION = 7,       // Ressurreição
    STAT_CHANGE = 8,        // Alteração de estatística
    SKILL_LEARN = 9,        // Aprendizado de habilidade
    ITEM_ACQUIRE = 10,      // Aquisição de item
    ITEM_LOSE = 11,         // Perda de item
    GOLD_CHANGE = 12,       // Alteração de ouro
    QUEST_ACCEPT = 13,      // Aceitação de missão
    QUEST_COMPLETE = 14,    // Conclusão de missão
    GUILD_JOIN = 15,        // Entrada em guilda
    GUILD_LEAVE = 16,       // Saída de guilda
    PVP_KILL = 17,          // Abate PVP
    PVP_DEATH = 18,         // Morte PVP
    TELEPORT = 19,          // Teleporte
    TRADE = 20,             // Comércio
    CUSTOM_1 = 21,          // Personalizado 1
    CUSTOM_2 = 22,          // Personalizado 2
    CUSTOM_3 = 23,          // Personalizado 3
    UNKNOWN = 24            // Desconhecido
};

/**
 * @brief Informações do personagem
 */
struct CharacterInfo {
    STRUCT_MOB MOB;             // MOB
    uint32_t AccountID;         // ID da conta
    CharacterState State;       // Estado
    uint32_t CreationDate;      // Data de criação
    uint32_t LastLoginDate;     // Data do último login
    uint32_t LastLogoutDate;    // Data do último logout
    uint32_t PlayTime;          // Tempo de jogo
    uint32_t DeletedDate;       // Data de exclusão
    std::string DeleteReason;   // Motivo de exclusão
    uint32_t BlockedUntil;      // Bloqueado até
    std::string BlockReason;    // Motivo de bloqueio
    bool IsOnline;              // Está online
    uint8_t Channel;            // Canal
    uint16_t SkillBar[4];       // Barra de habilidades
    
    CharacterInfo() : AccountID(0), State(CharacterState::NORMAL),
        CreationDate(0), LastLoginDate(0), LastLogoutDate(0),
        PlayTime(0), DeletedDate(0), BlockedUntil(0), IsOnline(false),
        Channel(0) {
        memset(&MOB, 0, sizeof(STRUCT_MOB));
        for (int i = 0; i < 4; i++)
            SkillBar[i] = 0;
    }
};

/**
 * @brief Log de personagem
 */
struct CharacterLog {
    uint32_t LogID;             // ID do log
    uint32_t CharacterID;       // ID do personagem
    CharacterLogType Type;      // Tipo
    uint32_t Timestamp;         // Timestamp
    std::string Details;        // Detalhes
    
    CharacterLog() : LogID(0), CharacterID(0), Type(CharacterLogType::NONE),
        Timestamp(0) {}
};

/**
 * @brief Estatísticas de personagem
 */
struct CharacterStats {
    uint32_t TotalCharacters;       // Total de personagens
    uint32_t ActiveCharacters;      // Personagens ativos
    uint32_t DeletedCharacters;     // Personagens excluídos
    uint32_t BlockedCharacters;     // Personagens bloqueados
    uint32_t NewCharactersToday;    // Novos personagens hoje
    uint32_t LoginsToday;           // Logins hoje
    uint32_t AverageLevel;          // Nível médio
    uint32_t MaxLevelCharacters;    // Personagens de nível máximo
    std::unordered_map<uint8_t, uint32_t> CharactersByClass; // Personagens por classe
    
    CharacterStats() : TotalCharacters(0), ActiveCharacters(0),
        DeletedCharacters(0), BlockedCharacters(0), NewCharactersToday(0),
        LoginsToday(0), AverageLevel(0), MaxLevelCharacters(0) {}
};

/**
 * @brief Configuração de personagem
 */
struct CharacterConfig {
    uint8_t MaxCharactersPerAccount;    // Máximo de personagens por conta
    uint8_t MinNameLength;              // Tamanho mínimo do nome
    uint8_t MaxNameLength;              // Tamanho máximo do nome
    uint8_t StartingLevel;              // Nível inicial
    uint16_t StartingStats;             // Estatísticas iniciais
    uint32_t StartingGold;              // Ouro inicial
    uint16_t StartingMapID;             // ID do mapa inicial
    uint16_t StartingPosX;              // Posição X inicial
    uint16_t StartingPosY;              // Posição Y inicial
    uint32_t DeleteDelay;               // Atraso de exclusão
    bool AutoSave;                      // Auto salvar
    uint32_t AutoSaveInterval;          // Intervalo de auto salvamento
    
    CharacterConfig() : MaxCharactersPerAccount(4), MinNameLength(3),
        MaxNameLength(12), StartingLevel(1), StartingStats(5),
        StartingGold(0), StartingMapID(0), StartingPosX(0),
        StartingPosY(0), DeleteDelay(86400), AutoSave(true),
        AutoSaveInterval(300) {}
};

/**
 * @brief Tipo de callback de personagem
 */
using CharacterCallback = std::function<void(const CharacterInfo&, CharacterState)>;

/**
 * @brief Gerenciador de banco de dados de personagens
 * 
 * Esta classe é responsável por gerenciar o banco de dados de personagens.
 */
/**
 * Classe WYDCharacterDB
 * 
 * Esta classe implementa a funcionalidade WYDCharacterDB conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class CharacterDB {
public:
    /**
     * @brief Obtém a instância única do gerenciador
     * @return Instância
     */
    static CharacterDB& getInstance() {
        static CharacterDB instance;
        return instance;
    }
    
    /**
     * @brief Inicializa o gerenciador
     * @param dbManager Gerenciador de banco de dados
     * @return true se inicializado com sucesso
     */
    bool initialize(DatabaseManager* dbManager);
    
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
     * @brief Carrega a configuração de personagens
     * @return true se carregado com sucesso
     */
    bool loadCharacterConfig();
    
    /**
     * @brief Cria um personagem
     * @param accountID ID da conta
     * @param name Nome
     * @param classType Tipo de classe
     * @return ID do personagem ou 0 em caso de erro
     */
    uint32_t createCharacter(uint32_t accountID, const std::string& name, uint8_t classType);
    
    /**
     * @brief Marca um personagem para exclusão
     * @param characterID ID do personagem
     * @param reason Motivo
     * @return Resultado da operação
     */
    CharacterResult markCharacterForDeletion(uint32_t characterID, const std::string& reason = "");
    
    /**
     * @brief Restaura um personagem excluído
     * @param characterID ID do personagem
     * @return Resultado da operação
     */
    CharacterResult restoreDeletedCharacter(uint32_t characterID);
    
    /**
     * @brief Exclui um personagem permanentemente
     * @param characterID ID do personagem
     * @return Resultado da operação
     */
    CharacterResult permanentlyDeleteCharacter(uint32_t characterID);
    
    /**
     * @brief Bloqueia um personagem
     * @param characterID ID do personagem
     * @param reason Motivo
     * @param duration Duração (0 para permanente)
     * @return Resultado da operação
     */
    CharacterResult blockCharacter(uint32_t characterID, const std::string& reason, uint32_t duration = 0);
    
    /**
     * @brief Desbloqueia um personagem
     * @param characterID ID do personagem
     * @return Resultado da operação
     */
    CharacterResult unblockCharacter(uint32_t characterID);
    
    /**
     * @brief Carrega um personagem
     * @param characterID ID do personagem
     * @return Informações do personagem ou nullptr se não encontrado
     */
    const CharacterInfo* loadCharacter(uint32_t characterID);
    
    /**
     * @brief Carrega os personagens de uma conta
     * @param accountID ID da conta
     * @return Lista de IDs de personagens
     */
    std::vector<uint32_t> loadAccountCharacters(uint32_t accountID);
    
    /**
     * @brief Salva um personagem
     * @param characterInfo Informações do personagem
     * @return Resultado da operação
     */
    CharacterResult saveCharacter(const CharacterInfo& characterInfo);
    
    /**
     * @brief Salva um personagem imediatamente
     * @param characterID ID do personagem
     * @return Resultado da operação
     */
    CharacterResult saveCharacterNow(uint32_t characterID);
    
    /**
     * @brief Registra login de personagem
     * @param characterID ID do personagem
     * @param channel Canal
     * @return Resultado da operação
     */
    CharacterResult registerCharacterLogin(uint32_t characterID, uint8_t channel);
    
    /**
     * @brief Registra logout de personagem
     * @param characterID ID do personagem
     * @return Resultado da operação
     */
    CharacterResult registerCharacterLogout(uint32_t characterID);
    
    /**
     * @brief Adiciona tempo de jogo
     * @param characterID ID do personagem
     * @param seconds Segundos
     * @return Resultado da operação
     */
    CharacterResult addPlayTime(uint32_t characterID, uint32_t seconds);
    
    /**
     * @brief Adiciona experiência
     * @param characterID ID do personagem
     * @param exp Experiência
     * @return Resultado da operação
     */
    CharacterResult addExperience(uint32_t characterID, uint32_t exp);
    
    /**
     * @brief Define o nível
     * @param characterID ID do personagem
     * @param level Nível
     * @return Resultado da operação
     */
    CharacterResult setLevel(uint32_t characterID, uint8_t level);
    
    /**
     * @brief Adiciona pontos de habilidade
     * @param characterID ID do personagem
     * @param skillPoints Pontos de habilidade
     * @return Resultado da operação
     */
    CharacterResult addSkillPoints(uint32_t characterID, uint16_t skillPoints);
    
    /**
     * @brief Adiciona pontos de atributo
     * @param characterID ID do personagem
     * @param statPoints Pontos de atributo
     * @return Resultado da operação
     */
    CharacterResult addStatPoints(uint32_t characterID, uint16_t statPoints);
    
    /**
     * @brief Altera um atributo
     * @param characterID ID do personagem
     * @param statType Tipo de atributo
     * @param value Valor
     * @return Resultado da operação
     */
    CharacterResult changeStat(uint32_t characterID, uint8_t statType, uint16_t value);
    
    /**
     * @brief Altera o ouro
     * @param characterID ID do personagem
     * @param gold Ouro
     * @param relative Relativo
     * @return Resultado da operação
     */
    CharacterResult changeGold(uint32_t characterID, uint32_t gold, bool relative = true);
    
    /**
     * @brief Altera a posição
     * @param characterID ID do personagem
     * @param mapID ID do mapa
     * @param posX Posição X
     * @param posY Posição Y
     * @return Resultado da operação
     */
    CharacterResult changePosition(uint32_t characterID, uint16_t mapID, uint16_t posX, uint16_t posY);
    
    /**
     * @brief Define a barra de habilidades
     * @param characterID ID do personagem
     * @param skillBar Barra de habilidades
     * @return Resultado da operação
     */
    CharacterResult setSkillBar(uint32_t characterID, const uint16_t skillBar[4]);
    
    /**
     * @brief Verifica se um nome de personagem está disponível
     * @param name Nome
     * @return true se estiver disponível
     */
    bool isCharacterNameAvailable(const std::string& name);
    
    /**
     * @brief Verifica se um personagem existe
     * @param characterID ID do personagem
     * @return true se existir
     */
    bool doesCharacterExist(uint32_t characterID);
    
    /**
     * @brief Obtém um personagem por nome
     * @param name Nome
     * @return ID do personagem ou 0 se não encontrado
     */
    uint32_t getCharacterByName(const std::string& name) const;
    
    /**
     * @brief Obtém o proprietário de um personagem
     * @param characterID ID do personagem
     * @return ID da conta ou 0 se não encontrado
     */
    uint32_t getCharacterOwner(uint32_t characterID) const;
    
    /**
     * @brief Obtém o estado de um personagem
     * @param characterID ID do personagem
     * @return Estado
     */
    CharacterState getCharacterState(uint32_t characterID) const;
    
    /**
     * @brief Verifica se um personagem está online
     * @param characterID ID do personagem
     * @return true se estiver online
     */
    bool isCharacterOnline(uint32_t characterID) const;
    
    /**
     * @brief Obtém personagens online
     * @return Lista de IDs de personagens
     */
    std::vector<uint32_t> getOnlineCharacters() const;
    
    /**
     * @brief Obtém personagens por nível
     * @param minLevel Nível mínimo
     * @param maxLevel Nível máximo
     * @return Lista de IDs de personagens
     */
    std::vector<uint32_t> getCharactersByLevel(uint8_t minLevel, uint8_t maxLevel) const;
    
    /**
     * @brief Obtém personagens por classe
     * @param classType Tipo de classe
     * @return Lista de IDs de personagens
     */
    std::vector<uint32_t> getCharactersByClass(uint8_t classType) const;
    
    /**
     * @brief Obtém logs de um personagem
     * @param characterID ID do personagem
     * @param limit Limite (0 para todos)
     * @param offset Deslocamento
     * @return Lista de logs
     */
    std::vector<CharacterLog> getCharacterLogs(uint32_t characterID, uint32_t limit = 0, uint32_t offset = 0) const;
    
    /**
     * @brief Adiciona um log
     * @param characterID ID do personagem
     * @param type Tipo
     * @param details Detalhes
     * @return ID do log
     */
    uint32_t addCharacterLog(uint32_t characterID, CharacterLogType type, const std::string& details = "");
    
    /**
     * @brief Registra um callback de personagem
     * @param callback Função de callback
     * @return ID do callback ou 0 em caso de erro
     */
    uint32_t registerCharacterCallback(CharacterCallback callback);
    
    /**
     * @brief Remove um callback de personagem
     * @param callbackId ID do callback
     * @return true se removido com sucesso
     */
    bool unregisterCharacterCallback(uint32_t callbackId);
    
    /**
     * @brief Obtém estatísticas de personagens
     * @return Estatísticas
     */
    CharacterStats getCharacterStats() const;
    
    /**
     * @brief Obtém a configuração de personagens
     * @return Configuração
     */
    CharacterConfig getCharacterConfig() const;
    
    /**
     * @brief Define a configuração de personagens
     * @param config Configuração
     */
    void setCharacterConfig(const CharacterConfig& config);
    
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
    
private:
    /**
     * @brief Construtor privado
     */
    CharacterDB();
    
    /**
     * @brief Destrutor privado
     */
    ~CharacterDB();
    
    /**
     * @brief Construtor de cópia privado
     */
    CharacterDB(const CharacterDB&) = delete;
    
    /**
     * @brief Operador de atribuição privado
     */
    CharacterDB& operator=(const CharacterDB&) = delete;
    
    // Gerenciador de banco de dados
    DatabaseManager* _dbManager;
    
    // Cache de personagens
    std::unordered_map<uint32_t, CharacterInfo> _characterCache;
    std::unordered_map<std::string, uint32_t> _nameToID;
    std::unordered_map<uint32_t, std::vector<uint32_t>> _accountCharacters;
    std::atomic<uint32_t> _nextCharacterID;
    
    // Personagens modificados
    std::unordered_set<uint32_t> _modifiedCharacters;
    
    // Personagens online
    std::unordered_set<uint32_t> _onlineCharacters;
    
    // Personagens por classe
    std::unordered_map<uint8_t, std::vector<uint32_t>> _charactersByClass;
    
    // Personagens por nível
    std::map<uint8_t, std::vector<uint32_t>> _charactersByLevel;
    
    // Logs
    std::unordered_map<uint32_t, std::vector<CharacterLog>> _characterLogs;
    std::atomic<uint32_t> _nextLogID;
    
    // Estatísticas
    CharacterStats _characterStats;
    
    // Configuração
    CharacterConfig _characterConfig;
    
    // Callbacks
    std::vector<CharacterCallback> _characterCallbacks;
    std::atomic<uint32_t> _nextCallbackID;
    
    // Thread safety
    mutable std::mutex _characterMutex;
    mutable std::mutex _logMutex;
    mutable std::mutex _statsMutex;
    mutable std::mutex _configMutex;
    mutable std::mutex _callbackMutex;
    
    // Flags
    std::atomic<bool> _initialized;
    std::atomic<bool> _debugMode;
    
    // Timers
    uint32_t _autoSaveTimer;
    uint32_t _deletionCheckTimer;
    
    /**
     * @brief Salva personagens modificados
     */
    void saveModifiedCharacters();
    
    /**
     * @brief Verifica personagens marcados para exclusão
     * @param currentTime Tempo atual
     */
    void checkCharactersForDeletion(uint32_t currentTime);
    
    /**
     * @brief Atualiza as estatísticas
     */
    void updateCharacterStats();
    
    /**
     * @brief Executa callbacks de personagem
     * @param character Personagem
     * @param oldState Estado antigo
     */
    void executeCharacterCallbacks(const CharacterInfo& character, CharacterState oldState);
    
    /**
     * @brief Inicializa um personagem
     * @param characterInfo Informações do personagem
     * @param accountID ID da conta
     * @param name Nome
     * @param classType Tipo de classe
     */
    void initializeCharacter(CharacterInfo& characterInfo, uint32_t accountID, const std::string& name, uint8_t classType);
    
    /**
     * @brief Atualiza os índices de um personagem
     * @param characterInfo Informações do personagem
     */
    void updateCharacterIndices(const CharacterInfo& characterInfo);
    
    /**
     * @brief Carrega um personagem do banco de dados
     * @param characterID ID do personagem
     * @return Informações do personagem ou nullptr se não encontrado
     */
    const CharacterInfo* loadCharacterFromDB(uint32_t characterID);
    
    /**
     * @brief Salva um personagem no banco de dados
     * @param characterInfo Informações do personagem
     * @return true se salvo com sucesso
     */
    bool saveCharacterToDB(const CharacterInfo& characterInfo);
    
    /**
     * @brief Valida um nome de personagem
     * @param name Nome
     * @return true se for válido
     */
    bool isValidCharacterName(const std::string& name);
};

} // namespace database
} // namespace wydbr

#endif // _CHARACTER_DB_H_