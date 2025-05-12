/**
 * PlayerManager.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/src/tmsrv/player/PlayerManager.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef _PLAYER_MANAGER_H_
#define _PLAYER_MANAGER_H_

/**
 * @file PlayerManager.h
 * @brief Sistema de gerenciamento de jogadores - Simplificado conforme original
 * 
 * Implementa o sistema de gerenciamento de jogadores do servidor WYD,
 * seguindo o design original com simplificações apropriadas para
 * melhor manutenção e correções de bugs conhecidos.
 */

#include <string>
#include <vector>
#include <map>
#include <set>
#include <mutex>
#include <memory>
#include <functional>
#include <chrono>

#include "../../../include/GlobalDef.h"
#include "../../../include/TM_Mob.h"
#include "../../../include/TM_Item.h"
#include "../../../include/TM_Protocol.h"
#include "WYDPlayer.h"

namespace wydbr {

/**
 * @brief Resultado de carregamento de jogador
 */
enum PlayerLoadResult {
    PLAYER_LOAD_SUCCESS = 0,      // Sucesso
    PLAYER_LOAD_NOTFOUND = 1,     // Não encontrado
    PLAYER_LOAD_INVALID = 2,      // Inválido
    PLAYER_LOAD_CORRUPTED = 3,    // Corrompido
    PLAYER_LOAD_FAILED = 4,       // Falha
    PLAYER_LOAD_EXISTS = 5,       // Já existe
    PLAYER_LOAD_MAXLEVEL = 6,     // Nível máximo
    PLAYER_LOAD_BANNED = 7,       // Banido
    PLAYER_LOAD_LOCKED = 8,       // Bloqueado
    PLAYER_LOAD_DELETED = 9,      // Deletado
    PLAYER_LOAD_MEMORY = 10,      // Erro de memória
    PLAYER_LOAD_ACCOUNT = 11      // Erro de conta
};

/**
 * @brief Resultado de salvamento de jogador
 */
enum PlayerSaveResult {
    PLAYER_SAVE_SUCCESS = 0,      // Sucesso
    PLAYER_SAVE_NOTFOUND = 1,     // Não encontrado
    PLAYER_SAVE_INVALID = 2,      // Inválido
    PLAYER_SAVE_FAILED = 3,       // Falha
    PLAYER_SAVE_DIRECTORY = 4,    // Diretório
    PLAYER_SAVE_FILE = 5,         // Arquivo
    PLAYER_SAVE_MEMORY = 6        // Erro de memória
};

/**
 * @brief Resultado de criação de personagem
 */
enum PlayerCreateResult {
    PLAYER_CREATE_SUCCESS = 0,      // Sucesso
    PLAYER_CREATE_MAXCHARS = 1,     // Máximo de personagens
    PLAYER_CREATE_NAMELENGTH = 2,   // Nome muito longo
    PLAYER_CREATE_NAMEINVALID = 3,  // Nome inválido
    PLAYER_CREATE_NAMEEXISTS = 4,   // Nome já existe
    PLAYER_CREATE_BADCLASS = 5,     // Classe inválida
    PLAYER_CREATE_BADINITIAL = 6,   // Iniciais inválidas
    PLAYER_CREATE_FAILED = 7,       // Falha
    PLAYER_CREATE_EXISTS = 8,       // Já existe
    PLAYER_CREATE_MEMORY = 9,       // Erro de memória
    PLAYER_CREATE_FILEWRITE = 10,   // Erro de escrita
    PLAYER_CREATE_ACCOUNT = 11      // Erro de conta
};

/**
 * @brief Resultado de exclusão de personagem
 */
enum PlayerDeleteResult {
    PLAYER_DELETE_SUCCESS = 0,      // Sucesso
    PLAYER_DELETE_NOTFOUND = 1,     // Não encontrado
    PLAYER_DELETE_INVALID = 2,      // Inválido
    PLAYER_DELETE_INGAME = 3,       // Em jogo
    PLAYER_DELETE_PROTECTED = 4,    // Protegido
    PLAYER_DELETE_FAILED = 5,       // Falha
    PLAYER_DELETE_GUILD = 6,        // Em guilda
    PLAYER_DELETE_PASSWORD = 7,     // Senha incorreta
    PLAYER_DELETE_FILEIO = 8,       // Erro de I/O
    PLAYER_DELETE_MEMORY = 9,       // Erro de memória
    PLAYER_DELETE_ACCOUNT = 10      // Erro de conta
};

/**
 * @brief Informações resumidas de personagem
 */
struct PlayerSummary {
    std::string name;               // Nome
    uint8_t level;                  // Nível
    uint8_t classId;                // Classe
    uint8_t slot;                   // Slot
    uint8_t generation;             // Geração
    uint32_t experience;            // Experiência
    uint16_t guild;                 // Guilda
    uint8_t guildLevel;             // Nível na guilda
    STRUCT_ITEM equip[16];          // Equipamentos
    
    PlayerSummary() : level(0), classId(0), slot(0), generation(0), 
                     experience(0), guild(0), guildLevel(0) {
        memset(equip, 0, sizeof(equip));
    }
};

/**
 * @brief Informações de um slot de personagem
 */
struct CharacterSlot {
    bool used;                       // Usado
    std::string name;                // Nome
    uint8_t level;                   // Nível
    uint8_t classId;                 // Classe
    uint8_t slot;                    // Slot
    uint8_t generation;              // Geração
    STRUCT_SCORE score;              // Score
    STRUCT_ITEM equip[16];           // Equipamentos
    
    CharacterSlot() : used(false), level(0), classId(0), slot(0), generation(0) {
        memset(&score, 0, sizeof(score));
        memset(equip, 0, sizeof(equip));
    }
};

/**
 * @brief Informações de conta
 */
struct AccountInfo {
    uint32_t accountId;              // ID da conta
    std::string accountName;         // Nome da conta
    std::string password;            // Senha
    uint8_t permission;              // Permissão
    uint8_t status;                  // Status
    uint8_t numCharacters;           // Número de personagens
    CharacterSlot characters[4];     // Personagens (4 slots)
    uint32_t createTime;             // Tempo de criação
    uint32_t lastLoginTime;          // Último login
    uint32_t lastLogoutTime;         // Último logout
    uint32_t playTime;               // Tempo de jogo
    uint32_t loginCount;             // Contagem de logins
    uint32_t donateAmount;           // Quantidade de doação
    uint32_t banTime;                // Tempo de banimento
    std::string banReason;           // Razão do banimento
    std::string email;               // Email
    std::string realName;            // Nome real
    std::string phoneNumber;         // Número de telefone
    std::string address;             // Endereço
    uint16_t storageGold;            // Ouro do armazém
    STRUCT_ITEM storage[120];        // Armazém (120 slots)
    
    AccountInfo() : accountId(0), permission(ACCOUNT_NORMAL), status(0), 
                   numCharacters(0), createTime(0), lastLoginTime(0), 
                   lastLogoutTime(0), playTime(0), loginCount(0), 
                   donateAmount(0), banTime(0), storageGold(0) {
        memset(storage, 0, sizeof(storage));
    }
};

/**
 * @brief Gerenciador de jogadores
 * 
 * Classe singleton responsável por gerenciar todos os jogadores do servidor,
 * incluindo carregamento, salvamento, autenticação e operações relacionadas.
 */
/**
 * Classe WYDPlayerManager
 * 
 * Esta classe implementa a funcionalidade WYDPlayerManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class PlayerManager {
public:
    /**
     * @brief Obtém a instância única
     * @return Referência para a instância
     */
    static PlayerManager& GetInstance();
    
    /**
     * @brief Inicializa o gerenciador
     * @param accountDir Diretório de contas
     * @param characterDir Diretório de personagens
     * @return true se inicializado com sucesso
     */
    bool Initialize(const std::string& accountDir = "./account/", const std::string& characterDir = "./character/");
    
    /**
     * @brief Finaliza o gerenciador
     */
    void Finalize();
    
    /**
     * @brief Autentica uma conta
     * @param accountName Nome da conta
     * @param password Senha
     * @param connectionId ID da conexão
     * @return true se autenticado com sucesso
     */
    bool AuthenticateAccount(const std::string& accountName, const std::string& password, int connectionId);
    
    /**
     * @brief Desconecta uma conta
     * @param accountId ID da conta
     * @return true se desconectado com sucesso
     */
    bool DisconnectAccount(uint32_t accountId);
    
    /**
     * @brief Obtém a lista de personagens
     * @param accountId ID da conta
     * @param connectionId ID da conexão
     * @param charList Array para receber a lista de personagens
     * @return true se obtida com sucesso
     */
    bool GetCharList(uint32_t accountId, int connectionId, PACKET_CHARLIST& charList);
    
    /**
     * @brief Cria um personagem
     * @param accountId ID da conta
     * @param connectionId ID da conexão
     * @param characterName Nome do personagem
     * @param classId Classe
     * @param slot Slot
     * @return Resultado da criação
     */
    PlayerCreateResult CreateCharacter(uint32_t accountId, int connectionId, const std::string& characterName, uint8_t classId, uint8_t slot);
    
    /**
     * @brief Exclui um personagem
     * @param accountId ID da conta
     * @param connectionId ID da conexão
     * @param characterName Nome do personagem
     * @param password Senha
     * @return Resultado da exclusão
     */
    PlayerDeleteResult DeleteCharacter(uint32_t accountId, int connectionId, const std::string& characterName, const std::string& password);
    
    /**
     * @brief Carrega um personagem
     * @param accountId ID da conta
     * @param connectionId ID da conexão
     * @param characterName Nome do personagem
     * @param slot Slot
     * @return ID do jogador gerado (0 se falhar)
     */
    int LoadCharacter(uint32_t accountId, int connectionId, const std::string& characterName, uint8_t slot);
    
    /**
     * @brief Descarrega um personagem
     * @param playerId ID do jogador
     * @param save Se deve salvar
     * @return true se descarregado com sucesso
     */
    bool UnloadCharacter(int playerId, bool save = true);
    
    /**
     * @brief Salva um personagem
     * @param playerId ID do jogador
     * @return Resultado do salvamento
     */
    PlayerSaveResult SaveCharacter(int playerId);
    
    /**
     * @brief Salva todos os personagens
     * @return Número de personagens salvos
     */
    int SaveAllCharacters();
    
    /**
     * @brief Obtém um jogador pelo ID
     * @param playerId ID do jogador
     * @return Ponteiro para o jogador (nullptr se não existir)
     */
    WYDPlayer* GetPlayer(int playerId);
    
    /**
     * @brief Obtém um jogador pelo nome
     * @param characterName Nome do personagem
     * @return Ponteiro para o jogador (nullptr se não existir)
     */
    WYDPlayer* GetPlayerByName(const std::string& characterName);
    
    /**
     * @brief Obtém um jogador pela conexão
     * @param connectionId ID da conexão
     * @return Ponteiro para o jogador (nullptr se não existir)
     */
    WYDPlayer* GetPlayerByConnection(int connectionId);
    
    /**
     * @brief Obtém uma conta pelo ID
     * @param accountId ID da conta
     * @return Ponteiro para a conta (nullptr se não existir)
     */
    AccountInfo* GetAccount(uint32_t accountId);
    
    /**
     * @brief Obtém uma conta pelo nome
     * @param accountName Nome da conta
     * @return Ponteiro para a conta (nullptr se não existir)
     */
    AccountInfo* GetAccountByName(const std::string& accountName);
    
    /**
     * @brief Obtém um resumo de personagem pelo nome
     * @param characterName Nome do personagem
     * @param summary Referência para receber o resumo
     * @return true se obtido com sucesso
     */
    bool GetCharacterSummary(const std::string& characterName, PlayerSummary& summary);
    
    /**
     * @brief Verifica se um personagem existe
     * @param characterName Nome do personagem
     * @return true se existir
     */
    bool CharacterExists(const std::string& characterName);
    
    /**
     * @brief Verifica se uma conta existe
     * @param accountName Nome da conta
     * @return true se existir
     */
    bool AccountExists(const std::string& accountName);
    
    /**
     * @brief Cria uma conta
     * @param accountName Nome da conta
     * @param password Senha
     * @param email Email (opcional)
     * @return true se criada com sucesso
     */
    bool CreateAccount(const std::string& accountName, const std::string& password, const std::string& email = "");
    
    /**
     * @brief Processa um tick
     * @param currentTime Tempo atual
     */
    void ProcessTick(uint32_t currentTime);
    
    /**
     * @brief Processa um pacote para um jogador
     * @param playerId ID do jogador
     * @param packet Pacote
     * @param size Tamanho
     * @return true se processado com sucesso
     */
    bool ProcessPlayerPacket(int playerId, const void* packet, int size);
    
    /**
     * @brief Envia um pacote para um jogador
     * @param playerId ID do jogador
     * @param data Dados
     * @param size Tamanho
     * @return true se enviado com sucesso
     */
    bool SendPacketToPlayer(int playerId, const void* data, int size);
    
    /**
     * @brief Envia um pacote para uma área
     * @param mapId ID do mapa
     * @param x Posição X
     * @param y Posição Y
     * @param radius Raio
     * @param data Dados
     * @param size Tamanho
     * @param excludePlayerId ID do jogador a excluir (0 para nenhum)
     * @return Número de jogadores que receberam o pacote
     */
    int SendPacketToArea(uint16_t mapId, uint16_t x, uint16_t y, uint16_t radius, const void* data, int size, int excludePlayerId = 0);
    
    /**
     * @brief Envia um pacote para todos os jogadores
     * @param data Dados
     * @param size Tamanho
     * @param filter Função de filtro (opcional)
     * @return Número de jogadores que receberam o pacote
     */
    int SendPacketToAll(const void* data, int size, std::function<bool(WYDPlayer*)> filter = nullptr);
    
    /**
     * @brief Envia uma mensagem para um jogador
     * @param playerId ID do jogador
     * @param message Mensagem
     * @param type Tipo de mensagem
     * @return true se enviada com sucesso
     */
    bool SendMessageToPlayer(int playerId, const std::string& message, uint16_t type = CHAT_SYSTEM);
    
    /**
     * @brief Envia uma mensagem para uma área
     * @param mapId ID do mapa
     * @param x Posição X
     * @param y Posição Y
     * @param radius Raio
     * @param message Mensagem
     * @param type Tipo de mensagem
     * @param excludePlayerId ID do jogador a excluir (0 para nenhum)
     * @return Número de jogadores que receberam a mensagem
     */
    int SendMessageToArea(uint16_t mapId, uint16_t x, uint16_t y, uint16_t radius, const std::string& message, uint16_t type = CHAT_SYSTEM, int excludePlayerId = 0);
    
    /**
     * @brief Envia uma mensagem para todos os jogadores
     * @param message Mensagem
     * @param type Tipo de mensagem
     * @param filter Função de filtro (opcional)
     * @return Número de jogadores que receberam a mensagem
     */
    int SendMessageToAll(const std::string& message, uint16_t type = CHAT_SYSTEM, std::function<bool(WYDPlayer*)> filter = nullptr);
    
    /**
     * @brief Encontra jogadores em uma área
     * @param mapId ID do mapa
     * @param x Posição X
     * @param y Posição Y
     * @param radius Raio
     * @param excludePlayerId ID do jogador a excluir (0 para nenhum)
     * @return Lista de IDs de jogador
     */
    std::vector<int> FindPlayersInArea(uint16_t mapId, uint16_t x, uint16_t y, uint16_t radius, int excludePlayerId = 0);
    
    /**
     * @brief Encontra o jogador mais próximo
     * @param mapId ID do mapa
     * @param x Posição X
     * @param y Posição Y
     * @param radius Raio
     * @param excludePlayerId ID do jogador a excluir (0 para nenhum)
     * @return ID do jogador (0 se não encontrar)
     */
    int FindNearestPlayer(uint16_t mapId, uint16_t x, uint16_t y, uint16_t radius, int excludePlayerId = 0);
    
    /**
     * @brief Registra um callback para criação de conta
     * @param callback Função de callback
     */
    void RegisterAccountCreateCallback(std::function<bool(const std::string&, const std::string&, const std::string&)> callback);
    
    /**
     * @brief Registra um callback para login de conta
     * @param callback Função de callback
     */
    void RegisterAccountLoginCallback(std::function<bool(uint32_t, int)> callback);
    
    /**
     * @brief Registra um callback para logout de conta
     * @param callback Função de callback
     */
    void RegisterAccountLogoutCallback(std::function<void(uint32_t)> callback);
    
    /**
     * @brief Registra um callback para criação de personagem
     * @param callback Função de callback
     */
    void RegisterCharacterCreateCallback(std::function<bool(uint32_t, const std::string&, uint8_t, uint8_t)> callback);
    
    /**
     * @brief Registra um callback para exclusão de personagem
     * @param callback Função de callback
     */
    void RegisterCharacterDeleteCallback(std::function<bool(uint32_t, const std::string&, const std::string&)> callback);
    
    /**
     * @brief Registra um callback para carregamento de personagem
     * @param callback Função de callback
     */
    void RegisterCharacterLoadCallback(std::function<bool(uint32_t, const std::string&, uint8_t)> callback);
    
    /**
     * @brief Registra um callback para salvamento de personagem
     * @param callback Função de callback
     */
    void RegisterCharacterSaveCallback(std::function<bool(int)> callback);
    
    /**
     * @brief Obtém o número de jogadores online
     * @return Número de jogadores
     */
    int GetOnlinePlayerCount() const;
    
    /**
     * @brief Obtém a lista de IDs de jogadores online
     * @return Lista de IDs
     */
    std::vector<int> GetOnlinePlayerIds() const;
    
    /**
     * @brief Obtém o número de contas logadas
     * @return Número de contas
     */
    int GetLoggedAccountCount() const;
    
    /**
     * @brief Obtém o diretório de contas
     * @return Diretório
     */
    const std::string& GetAccountDirectory() const;
    
    /**
     * @brief Obtém o diretório de personagens
     * @return Diretório
     */
    const std::string& GetCharacterDirectory() const;
    
/**
 * PlayerManager
 * 
 * Implementa a funcionalidade PlayerManager conforme especificação original.
 * @return Retorna private:
 */

    
private:
    PlayerManager();  // Construtor privado (singleton)
    ~PlayerManager(); // Destrutor privado (singleton)
    
    /**
     * @brief Carrega uma conta
     * @param accountName Nome da conta
     * @return Ponteiro para a conta (nullptr se não existir)
     */
    AccountInfo* LoadAccount(const std::string& accountName);
    
    /**
     * @brief Salva uma conta
     * @param account Conta
     * @return true se salva com sucesso
     */
    bool SaveAccount(const AccountInfo& account);
    
    /**
     * @brief Carrega um personagem do arquivo
     * @param characterName Nome do personagem
     * @param mobData Referência para receber os dados
     * @return true se carregado com sucesso
     */
    bool LoadCharacterData(const std::string& characterName, STRUCT_MOB& mobData);
    
    /**
     * @brief Salva um personagem para o arquivo
     * @param mobData Dados do personagem
     * @return true se salvo com sucesso
     */
    bool SaveCharacterData(const STRUCT_MOB& mobData);
    
    /**
     * @brief Inicializa um personagem novo
     * @param mobData Referência para os dados
     * @param characterName Nome do personagem
     * @param classId Classe
     * @param slot Slot
     */
    void InitializeNewCharacter(STRUCT_MOB& mobData, const std::string& characterName, uint8_t classId, uint8_t slot);
    
    /**
     * @brief Verifica se um nome de personagem é válido
     * @param characterName Nome do personagem
     * @return true se válido
     */
    bool IsValidCharacterName(const std::string& characterName);
    
    /**
     * @brief Verifica se uma classe é válida
     * @param classId Classe
     * @return true se válida
     */
    bool IsValidClass(uint8_t classId);
    
    /**
     * @brief Obtém o próximo ID para um jogador
     * @return Próximo ID
     */
    int GetNextPlayerId();
    
    /**
     * @brief Remove jogadores inativos
     */
    void RemoveInactivePlayers();
    
    static PlayerManager* instance_;                // Instância única (singleton)
    
    // Configurações
    std::string accountDir_;                        // Diretório de contas
    std::string characterDir_;                      // Diretório de personagens
    
    // Estado
    bool initialized_;                              // Inicializado
    int nextPlayerId_;                              // Próximo ID de jogador
    
    // Dados principais
    std::map<int, std::unique_ptr<WYDPlayer>> players_;          // Jogadores por ID
    std::map<std::string, int> playersByName_;                  // Jogadores por nome
    std::map<int, int> playersByConnection_;                    // Jogadores por conexão
    std::map<uint32_t, std::unique_ptr<AccountInfo>> accounts_; // Contas por ID
    std::map<std::string, uint32_t> accountsByName_;            // Contas por nome
    std::map<uint32_t, int> connectionsByAccount_;              // Conexões por conta
    
    // Cache de personagens
    struct CharacterCache {
        std::string name;           // Nome
        uint8_t level;              // Nível
        uint8_t classId;            // Classe
        uint8_t slot;               // Slot
        uint8_t generation;         // Geração
        uint32_t lastAccess;        // Último acesso
    };
    std::map<std::string, CharacterCache> characterCache_;     // Cache de personagens
    
    // Callbacks
    std::function<bool(const std::string&, const std::string&, const std::string&)> accountCreateCallback_;
    std::function<bool(uint32_t, int)> accountLoginCallback_;
    std::function<void(uint32_t)> accountLogoutCallback_;
    std::function<bool(uint32_t, const std::string&, uint8_t, uint8_t)> characterCreateCallback_;
    std::function<bool(uint32_t, const std::string&, const std::string&)> characterDeleteCallback_;
    std::function<bool(uint32_t, const std::string&, uint8_t)> characterLoadCallback_;
    std::function<bool(int)> characterSaveCallback_;
    
    // Mutex para proteção de dados
    mutable std::mutex playersMutex_;                           // Mutex para jogadores
    mutable std::mutex accountsMutex_;                          // Mutex para contas
    mutable std::mutex cacheMutex_;                             // Mutex para cache
    
    // Desabilita cópias (singleton)
    PlayerManager(const PlayerManager&) = delete;
    PlayerManager& operator=(const PlayerManager&) = delete;
};

} // namespace wydbr

#endif // _PLAYER_MANAGER_H_