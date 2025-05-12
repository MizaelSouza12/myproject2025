/**
 * PlayerTypes.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/player/PlayerTypes.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef PLAYER_TYPES_H
#define PLAYER_TYPES_H

#include <string>
#include <vector>
#include <array>
#include <memory>
#include <cstring>
#include "../../../core/WYDTypes.h"
#include "../../../game/ItemSystem.h"

namespace wydbr {


namespace wyd {
namespace server {

// Forward declarations
class Player;
class PlayerManager;

/**
 * @brief Configuração do sistema de jogadores
 */
struct PlayerConfig {
    INT startMap;               // Mapa inicial
    INT startX;                 // Posição X inicial
    INT startY;                 // Posição Y inicial
    INT maxLevel;               // Nível máximo
    FLOAT expRate;              // Taxa de experiência
    FLOAT dropRate;             // Taxa de drop
    FLOAT goldRate;             // Taxa de ouro
    INT maxInventorySize;       // Tamanho máximo do inventário
    INT maxStorageSize;         // Tamanho máximo do armazenamento
    
    PlayerConfig() 
        : startMap(1)
        , startX(2100)
        , startY(2100)
        , maxLevel(400)
        , expRate(1.0f)
        , dropRate(1.0f)
        , goldRate(1.0f)
        , maxInventorySize(64)
        , maxStorageSize(120)
    {
    }
};

/**
 * @brief Estados de um jogador
 */
enum class PlayerState : BYTE {
    NONE = 0,             // Estado não definido
    SELECTING = 1,        // Selecionando personagem
    LOADING = 2,          // Carregando dados
    INGAME = 3,           // Em jogo
    TELEPORTING = 4,      // Teleportando
    DEAD = 5,             // Morto
    DISCONNECTING = 6     // Desconectando
};

/**
 * @brief Classes de personagem
 */
enum class CharacterClass : BYTE {
    TRANSKNIGHT = 0,      // Cavaleiro
    FOEMA = 1,            // Mago
    BEASTMASTER = 2,      // Domador de Bestas
    HUNTRESS = 3,         // Caçadora
    TRANS_FOEMA = 4,      // Mago Transcendental
    TRANS_BEASTMASTER = 5,// Domador Transcendental
    TRANS_HUNTRESS = 6,   // Caçadora Transcendental
    UNKNOWN = 0xFF        // Desconhecido
};

/**
 * @brief Informações de conta
 */
struct AccountInfo {
    DWORD accountId;              // ID da conta
    char username[16];            // Nome de usuário
    BYTE access;                  // Nível de acesso (0=normal, 1=GM, 2=admin)
    bool banned;                  // Se a conta está banida
    DWORD banExpiration;          // Timestamp de expiração do ban (0=permanente)
    char email[64];               // Email
    DWORD creationDate;           // Data de criação
    DWORD lastLoginDate;          // Data do último login
    DWORD premiumExpiration;      // Timestamp de expiração do premium (0=sem premium)
    DWORD donationPoints;         // Pontos de doação
    DWORD eventPoints;            // Pontos de /**
 * AccountInfo
 * 
 * Implementa a funcionalidade AccountInfo conforme especificação original.
 * @return Retorna evento
 */
 evento
    
    AccountInfo(){
        memset(this, 0, sizeof(AccountInfo));
    }
};

/**
 * @brief Informações básicas de personagem
 */
struct CharacterBasicInfo {
    DWORD charId;                 // ID do personagem
    char name[16];                // Nome do personagem
    CharacterClass charClass;     // Classe do personagem
    BYTE gender;                  // Gênero (0=masculino, 1=feminino)
    WORD level;                   // Nível
    DWORD experience;             // Experiência
    DWORD gold;                   // Ouro
    WORD mapId;                   // ID do mapa atual
    WORD posX;                    // Posição X
    WORD posY;                    // Posição Y
    BYTE face;                    // Face (aparência do rosto)
    BYTE hair;                    // Cabelo (aparência do cabelo)
    BYTE hairColor;               // Cor do cabelo
    DWORD lastPlayTime;           // Último timestamp de jogo
    DWORD createTime;             // Timestamp de criação
    bool deleted;                 // Se o personagem foi deletado
    DWORD deleteTime;             // Timestamp de deleçã/**
 * CharacterBasicInfo
 * 
 * Implementa a funcionalidade CharacterBasicInfo conforme especificação original.
 * @return Retorna o
 */
o
    
    CharacterBasicInfo(){
        memset(this, 0, sizeof(CharacterBasicInfo));
    }
};

/**
 * @brief Atributos de personagem
 */
struct CharacterAttributes {
    WORD strength;                // Força
    WORD dexterity;               // Destreza
    WORD constitution;            // Constituição
    WORD intelligence;            // Inteligência
    WORD wisdom;                  // Sabedoria
    WORD charisma;                // Carisma
    WORD pointsToDistribute;      // Pontos para distribuir
    
    CharacterAttributes() 
        : strength(10)
        , dexterity(10)
        , constitution(10)
        , intelligence(10)
        , wisdom(10)
        , charisma(10)
        , pointsToDistribute(0)
    {
    }
};

/**
 * @brief Status de personagem
 */
struct CharacterStatus {
    DWORD maxHP;                  // HP máximo
    DWORD currentHP;              // HP atual
    DWORD maxMP;                  // MP máximo
    DWORD currentMP;              // MP atual
    WORD attackMin;               // Ataque mínimo
    WORD attackMax;               // Ataque máximo
    WORD magicAttackMin;          // Ataque mágico mínimo
    WORD magicAttackMax;          // Ataque mágico máximo
    WORD defense;                 // Defesa física
    WORD magicDefense;            // Defesa mágica
    WORD accuracy;                // Precisão
    WORD evasion;                 // Evasão
    WORD criticalRate;            // Taxa de crítico
    WORD criticalDamage;          // Dano crítico
    WORD attackSpeed;             // Velocidade de ataque
    WORD moveSpeed;               // Velocidade de movimento
    BYTE resistFire;              // Resistência a fogo
    BYTE resistWater;             // Resistência a água
    BYTE resistEarth;             // Resistência a terra
    BYTE resistWind;              // Resistência a vento
    BYTE resistHoly;              // Resistência a sagrado
    BYTE resistDark;              // Resistência a trevas
    
    CharacterStatus() 
        : maxHP(100)
        , currentHP(100)
        , maxMP(100)
        , currentMP(100)
        , attackMin(1)
        , attackMax(3)
        , magicAttackMin(1)
        , magicAttackMax(3)
        , defense(0)
        , magicDefense(0)
        , accuracy(10)
        , evasion(10)
        , criticalRate(5)
        , criticalDamage(150)
        , attackSpeed(100)
        , moveSpeed(100)
        , resistFire(0)
        , resistWater(0)
        , resistEarth(0)
        , resistWind(0)
        , resistHoly(0)
        , resistDark(0)
    {
    }
};

/**
 * @brief Efeito de buff/debuff
 */
struct CharacterEffect {
    BYTE effectId;                // ID do efeito
    WORD value;                   // Valor do efeito
    DWORD duration;               // Duração em milissegundos
    DWORD startTime;              // Timestamp de início
    DWORD ownerID;                // ID do causador do efeito
    
    CharacterEffect() 
        : effectId(0)
        , value(0)
        , duration(0)
        , startTime(0)
        , ownerID(0)
    {
    }
    
    /**
 * isExpired
 * 
 * Implementa a funcionalidade isExpired conforme especificação original.
 * @param currentTime Parâmetro currentTime
 * @return Retorna bool
 */

    
    bool isExpired(DWORD currentTime) const{
        return duration > 0 && currentTime >= startTime + duration;
    }
};

/**
 * @brief Informação de equipamento
 */
struct CharacterEquipment {
    wyd::game::Item items[MAX_EQUIP_SLOT];  // Itens /**
 * CharacterEquipment
 * 
 * Implementa a funcionalidade CharacterEquipment conforme especificação original.
 * @return Retorna equipados
 */
 equipados
    
    CharacterEquipment(){
        for (int i = 0; i < MAX_EQUIP_SLOT; i++) {
            memset(&items[i], 0, sizeof(wyd::game::Item));
        }
    }
    
    /**
 * isEmpty
 * 
 * Implementa a funcionalidade isEmpty conforme especificação original.
 * @param slot Parâmetro slot
 * @return Retorna bool
 */

    
    bool isEmpty(BYTE slot) const{
        return slot < MAX_EQUIP_SLOT && items[slot].itemId == 0;
    }
};

/**
 * @brief Informação de inventário
 */
struct CharacterInventory {
    wyd::game::Item items[MAX_ITEM_SLOT];   // Itens no inventá/**
 * CharacterInventory
 * 
 * Implementa a funcionalidade CharacterInventory conforme especificação original.
 * @return Retorna rio
 */
rio
    
    CharacterInventory(){
        for (int i = 0; i < MAX_ITEM_SLOT; i++) {
            memset(&items[i], 0, sizeof(wyd::game::Item));
        }
    }
    
    /**
 * isEmpty
 * 
 * Implementa a funcionalidade isEmpty conforme especificação original.
 * @param slot Parâmetro slot
 * @return Retorna bool
 */

    
    bool isEmpty(BYTE slot) const{
        return slot < MAX_ITEM_SLOT && items[slot].itemId == 0;
    }
};

/**
 * @brief Informação de armazenamento (banco)
 */
struct CharacterStorage {
    wyd::game::Item items[MAX_STORAGE_SLOT];  // Itens no /**
 * CharacterStorage
 * 
 * Implementa a funcionalidade CharacterStorage conforme especificação original.
 * @return Retorna armazenamento
 */
 armazenamento
    
    CharacterStorage(){
        for (int i = 0; i < MAX_STORAGE_SLOT; i++) {
            memset(&items[i], 0, sizeof(wyd::game::Item));
        }
    }
    
    /**
 * isEmpty
 * 
 * Implementa a funcionalidade isEmpty conforme especificação original.
 * @param slot Parâmetro slot
 * @return Retorna bool
 */

    
    bool isEmpty(BYTE slot) const{
        return slot < MAX_STORAGE_SLOT && items[slot].itemId == 0;
    }
};

/**
 * @brief Informação de habilidade
 */
struct CharacterSkill {
    WORD skillId;                  // ID da habilidade
    BYTE skillLevel;               // Nível da habilidade
    DWORD cooldown;                // Cooldown restante (em milissegundos)
    DWORD lastUseTime;             // Timestamp do último uso
    
    CharacterSkill() 
        : skillId(0)
        , skillLevel(0)
        , cooldown(0)
        , lastUseTime(0)
    {
    }
    
    /**
 * isOnCooldown
 * 
 * Implementa a funcionalidade isOnCooldown conforme especificação original.
 * @param currentTime Parâmetro currentTime
 * @return Retorna bool
 */

    
    bool isOnCooldown(DWORD currentTime) const{
        return currentTime < lastUseTime + cooldown;
    }
    
    /**
 * getRemainingCooldown
 * 
 * Implementa a funcionalidade getRemainingCooldown conforme especificação original.
 * @param currentTime Parâmetro currentTime
 * @return Retorna DWORD
 */

    
    DWORD getRemainingCooldown(DWORD currentTime) const{
        if (isOnCooldown(currentTime)) {
            return (lastUseTime + cooldown) - currentTime;
        }
        return 0;
    }
};

/**
 * @brief Informações de habilidades do personagem
 */
struct CharacterSkills {
    std::array<CharacterSkill, 16> skills;       // Habilidades conhecidas
    std::array<WORD, MAX_SKILLBAR> skillBar;     // Barra de /**
 * CharacterSkills
 * 
 * Implementa a funcionalidade CharacterSkills conforme especificação original.
 * @return Retorna habilidades
 */
 habilidades
    
    CharacterSkills(){
        for (auto& skillId : skillBar) {
            skillId = 0;
        }
    }
};

/**
 * @brief Informações de quest
 */
struct CharacterQuest {
    WORD questId;                   // ID da quest
    BYTE state;                     // Estado da quest (0=não iniciada, 1=em progresso, 2=completada)
    DWORD startTime;                // Timestamp de início
    DWORD completionTime;           // Timestamp de conclusão
    std::array<DWORD, 5> progress;  // Progresso em objetivos (ex: matar X monstros)
    
    CharacterQuest() 
        : questId(0)
        , state(0)
        , startTime(0)
        , completionTime(0)
    {
        for (auto& p : progress) {
            p = 0;
        }
    }
};

/**
 * @brief Informações de quests do personagem
 */
struct CharacterQuests {
    std::array<CharacterQuest, 50> quests;   // Quests ativas/completadas
    BYTE activeCount;                        // Número de quests ativas
    
    CharacterQuests() /**
 * activeCount
 * 
 * Implementa a funcionalidade activeCount conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna :
 */
 : activeCount(0){}
    
    /**
 * hasActiveQuest
 * 
 * Implementa a funcionalidade hasActiveQuest conforme especificação original.
 * @param questId Parâmetro questId
 * @return Retorna bool
 */

    
    bool hasActiveQuest(WORD questId) const{
        for (const auto& quest : quests) {
            if (quest.questId == questId && quest.state == 1) {
                return true;
            }
        }
        return false;
    }
    
    /**
 * hasCompletedQuest
 * 
 * Implementa a funcionalidade hasCompletedQuest conforme especificação original.
 * @param questId Parâmetro questId
 * @return Retorna bool
 */

    
    bool hasCompletedQuest(WORD questId) const{
        for (const auto& quest : quests) {
            if (quest.questId == questId && quest.state == 2) {
                return true;
            }
        }
        return false;
    }
};

/**
 * @brief Informações de guild
 */
struct CharacterGuild {
    DWORD guildId;                  // ID da guild
    char guildName[16];             // Nome da guild
    BYTE guildRank;                 // Rank na guild (0=membro, 1=oficial, 2=líder)
    DWORD guildJoinTime;            // Timestamp de entrada na guild
    
    CharacterGuild() 
        : guildId(0)
        , guildRank(0)
        , guildJoinTime(0)
    {
        memset(guildName, 0, sizeof(guildName));
    }
};

/**
 * @brief Informações sociais do personagem
 */
struct CharacterSocial {
    std::array<DWORD, 50> friends;       // Lista de amigos (IDs de personagem)
    std::array<DWORD, 50> blocked;       // Lista de bloqueados (IDs de personagem)
    BYTE friendCount;                    // Número de amigos
    BYTE blockedCount;                   // Número de bloqueados
    CharacterGuild guild;                // Informações de guild
    
    CharacterSocial() : friendCount(0), blockedCount(0) {
        for (auto& id : friends) {
            id = 0;
        }
        for (auto& id : blocked) {
            id = 0;
        }
    }
};

/**
 * @brief Informações PvP do personagem
 */
struct CharacterPvP {
    DWORD kills;                     // Total de kills em PvP
    DWORD deaths;                    // Total de mortes em PvP
    DWORD arenaWins;                 // Vitórias na arena
    DWORD arenaLosses;               // Derrotas na arena
    WORD pvpPoints;                  // Pontos de PvP
    BYTE pvpRank;                    // Rank de PvP
    
    CharacterPvP() 
        : kills(0)
        , deaths(0)
        , arenaWins(0)
        , arenaLosses(0)
        , pvpPoints(0)
        , pvpRank(0)
    {
    }
};

/**
 * @brief Informações completas de personagem
 */
struct CharacterInfo {
    CharacterBasicInfo basic;          // Informações básicas
    CharacterAttributes attributes;    // Atributos
    CharacterStatus status;            // Status
    CharacterEquipment equipment;      // Equipamento
    CharacterInventory inventory;      // Inventário
    CharacterStorage storage;          // Armazenamento (banco)
    CharacterSkills skills;            // Habilidades
    CharacterQuests quests;            // Quests
    CharacterSocial social;            // Informações sociais
    CharacterPvP pvp;                  // Informações PvP
    
    // Efeitos ativos (buffs/debuffs)
    std::array<CharacterEffect, MAX_AFFECT> effects;
    BYTE effectCount;
    
    CharacterInfo() /**
 * effectCount
 * 
 * Implementa a funcionalidade effectCount conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna :
 */
 : effectCount(0){}
};

/**
 * @brief Informações de sessão de jogador
 */
struct PlayerSession {
    DWORD sessionId;                   // ID da sessão
    DWORD accountId;                   // ID da conta
    DWORD characterId;                 // ID do personagem ativo
    DWORD connectionId;                // ID da conexão
    DWORD lastActivity;                // Timestamp da última atividade
    DWORD lastMovement;                // Timestamp do último movimento
    DWORD lastCombat;                  // Timestamp do último combate
    BYTE securityKey[16];              // Chave de segurança para criptografia
    DWORD lastPingTime;                // Timestamp do último ping
    WORD clientVersion;                // Versão do cliente
    std::string clientIP;              // IP do cliente
    
    PlayerSession() 
        : sessionId(0)
        , accountId(0)
        , characterId(0)
        , connectionId(0)
        , lastActivity(0)
        , lastMovement(0)
        , lastCombat(0)
        , lastPingTime(0)
        , clientVersion(0)
    {
        memset(securityKey, 0, sizeof(securityKey));
    }
};

} // namespace server
} // namespace wyd

#endif // PLAYER_TYPES_H

} // namespace wydbr
