#pragma once

// Inclusão das estruturas originais do WYD
// Na implementação real, importaria diretamente do código do jogo
#include <cstdint>
#include <string>
#include <vector>

namespace WYDStudio {

// Estruturas simplificadas do jogo para compatibilidade
// Na implementação real, estas estruturas seriam importadas do código do WYD

/**
 * @struct STRUCT_ITEM
 * @brief Representação de um item no jogo
 */
struct STRUCT_ITEM {
    uint16_t sIndex;      // ID do item
    struct {
        uint8_t cEffect;  // Tipo de efeito
        uint8_t cValue;   // Valor do efeito
        uint16_t sValue;  // Valor composto
    } stEffect[3];        // Até 3 efeitos por item
};

/**
 * @struct STRUCT_SCORE
 * @brief Estatísticas básicas de um personagem
 */
struct STRUCT_SCORE {
    uint16_t Level;       // Nível do personagem
    int32_t Ac;           // Defesa
    int32_t Damage;       // Dano base
    uint8_t Reserved;     // Reservado
    uint8_t AttackRun;    // Velocidade de ataque
    int32_t MaxHp;        // HP máximo
    int32_t MaxMp;        // MP máximo
    int32_t Hp;           // HP atual
    int32_t Mp;           // MP atual
    uint16_t Str;         // Força
    uint16_t Int;         // Inteligência
    uint16_t Dex;         // Destreza
    uint16_t Con;         // Constituição
    uint16_t Special[4];  // Stats especiais
};

/**
 * @struct STRUCT_MOB
 * @brief Representa um personagem ou monstro no jogo
 */
struct STRUCT_MOB {
    char MobName[16];                 // Nome
    uint8_t Clan;                     // Clã
    uint8_t Merchant;                 // Tipo de mercador
    uint16_t Guild;                   // Guild ID
    uint8_t Class;                    // Classe
    uint8_t Rsv;                      // Reservado
    uint16_t Quest;                   // Quest atual
    int32_t Coin;                     // Moedas
    int64_t Exp;                      // Experiência
    uint16_t HomeTownX;               // Posição X da cidade natal
    uint16_t HomeTownY;               // Posição Y da cidade natal
    STRUCT_SCORE BaseScore;           // Stats base
    STRUCT_SCORE CurrentScore;        // Stats atuais
    STRUCT_ITEM Equip[18];            // Equipamento
    STRUCT_ITEM Carry[64];            // Inventário
    uint32_t LearnedSkill[2];         // Skills aprendidas
    uint16_t ScoreBonus;              // Bônus de score
    uint16_t SpecialBonus;            // Bônus especial
    uint16_t SkillBonus;              // Bônus de skill
    uint8_t Critical;                 // Taxa de crítico
    uint8_t SaveMana;                 // Economia de mana
    uint8_t ShortSkill[4];            // Skills rápidas
    uint8_t GuildLevel;               // Nível na guild
    uint8_t Magic;                    // Magia
    uint8_t RegenHP;                  // Regeneração de HP
    uint8_t RegenMP;                  // Regeneração de MP
    uint8_t Resist[4];                // Resistências
    uint8_t dummy[212];               // Dados adicionais
    uint16_t CurrentKill;             // Kills atuais
    uint16_t TotalKill;               // Kills totais
};

/**
 * @struct STRUCT_AFFECT
 * @brief Representa um efeito/buff em um personagem
 */
struct STRUCT_AFFECT {
    uint8_t Type;         // Tipo de efeito
    uint8_t Level;        // Nível do efeito
    uint16_t Value;       // Valor do efeito
    int32_t Time;         // Tempo restante
};

/**
 * @struct GameEvent
 * @brief Representa um evento de jogo
 */
struct GameEvent {
    enum class Type {
        PLAYER_LOGIN,
        PLAYER_LOGOUT,
        ITEM_DROP,
        MONSTER_KILL,
        LEVEL_UP,
        GUILD_WAR,
        SERVER_STATUS
    };
    
    Type type;                // Tipo de evento
    int64_t timestamp;        // Timestamp do evento
    int32_t playerId;         // ID do jogador (se aplicável)
    int32_t targetId;         // ID do alvo (se aplicável)
    std::string location;     // Localização do evento
    std::vector<int32_t> params; // Parâmetros adicionais
};

/**
 * @struct GameEventTemplate
 * @brief Template para criação de eventos de jogo
 */
struct GameEventTemplate {
    std::string name;             // Nome do evento
    std::string description;      // Descrição
    int32_t durationMinutes;      // Duração em minutos
    std::vector<int32_t> rewards; // IDs de recompensas
    std::string script;           // Script do evento
};

/**
 * @struct PlayerAccount
 * @brief Informações da conta de um jogador
 */
struct PlayerAccount {
    int32_t accountId;            // ID da conta
    std::string username;         // Nome de usuário
    int32_t accessLevel;          // Nível de acesso
    int64_t lastLogin;            // Último login
    bool isBanned;                // Está banido
    std::string email;            // Email
};

/**
 * @struct PlayerData
 * @brief Dados completos de um jogador
 */
struct PlayerData {
    int32_t playerId;             // ID do jogador
    int32_t accountId;            // ID da conta
    STRUCT_MOB character;         // Dados do personagem
    std::vector<STRUCT_AFFECT> affects; // Efeitos ativos
    int64_t creationDate;         // Data de criação
    int64_t playTime;             // Tempo de jogo
};

} // namespace WYDStudio