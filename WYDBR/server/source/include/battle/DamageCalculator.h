/**
 * DamageCalculator.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/include/battle/DamageCalculator.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef _DAMAGE_CALCULATOR_H_
#define _DAMAGE_CALCULATOR_H_

/**
 * @file DamageCalculator.h
 * @brief Calculador de dano do WYD
 * 
 * Este arquivo contém o sistema de cálculo de dano do WYDBR,
 * responsável por calcular o dano causado e recebido pelos personagens,
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
#include <random>

#include "../GlobalDef.h"
#include "../Basedef.h"
#include "../TM_Mob.h"

namespace wydbr {
namespace battle {

/**
 * @brief Tipo de dano
 */
enum /**
 * Classe WYDDamageType
 * 
 * Esta classe implementa a funcionalidade WYDDamageType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class DamageType {
    NONE = 0,               // Nenhum
    PHYSICAL = 1,           // Físico
    MAGICAL = 2,            // Mágico
    TRUE = 3,               // Verdadeiro
    POISON = 4,             // Veneno
    BURN = 5,               // Queimadura
    BLEED = 6,              // Sangramento
    CUSTOM_1 = 7,           // Personalizado 1
    CUSTOM_2 = 8,           // Personalizado 2
    CUSTOM_3 = 9,           // Personalizado 3
    UNKNOWN = 10            // Desconhecido
};

/**
 * @brief Tipo de resistência
 */
enum /**
 * Classe WYDResistanceType
 * 
 * Esta classe implementa a funcionalidade WYDResistanceType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class ResistanceType {
    NONE = 0,               // Nenhum
    PHYSICAL = 1,           // Físico
    MAGICAL = 2,            // Mágico
    FIRE = 3,               // Fogo
    ICE = 4,                // Gelo
    LIGHTNING = 5,          // Relâmpago
    HOLY = 6,               // Sagrado
    DARK = 7,               // Sombrio
    POISON = 8,             // Veneno
    STUN = 9,               // Atordoamento
    CRITICAL = 10,          // Crítico
    CUSTOM_1 = 11,          // Personalizado 1
    CUSTOM_2 = 12,          // Personalizado 2
    CUSTOM_3 = 13,          // Personalizado 3
    UNKNOWN = 14            // Desconhecido
};

/**
 * @brief Tipo de resultado de dano
 */
enum /**
 * Classe WYDDamageResult
 * 
 * Esta classe implementa a funcionalidade WYDDamageResult conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class DamageResult {
    NORMAL = 0,             // Normal
    CRITICAL = 1,           // Crítico
    MISS = 2,               // Errou
    DODGE = 3,              // Esquivou
    BLOCK = 4,              // Bloqueou
    PARRY = 5,              // Aparou
    RESIST = 6,             // Resistiu
    IMMUNE = 7,             // Imune
    REFLECT = 8,            // Refletiu
    ABSORB = 9,             // Absorveu
    CUSTOM_1 = 10,          // Personalizado 1
    CUSTOM_2 = 11,          // Personalizado 2
    CUSTOM_3 = 12,          // Personalizado 3
    UNKNOWN = 13            // Desconhecido
};

/**
 * @brief Tipo de estado de combate
 */
enum /**
 * Classe WYDCombatState
 * 
 * Esta classe implementa a funcionalidade WYDCombatState conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class CombatState {
    NONE = 0,               // Nenhum
    NORMAL = 1,             // Normal
    COMBAT = 2,             // Combate
    PVP = 3,                // PVP
    MOUNT = 4,              // Montaria
    ARENA = 5,              // Arena
    CUSTOM_1 = 6,           // Personalizado 1
    CUSTOM_2 = 7,           // Personalizado 2
    CUSTOM_3 = 8,           // Personalizado 3
    UNKNOWN = 9             // Desconhecido
};

/**
 * @brief Informações de dano
 */
struct DamageInfo {
    uint32_t AttackerID;            // ID do atacante
    uint32_t DefenderID;            // ID do defensor
    uint32_t SkillID;               // ID da habilidade
    DamageType Type;                // Tipo
    DamageResult Result;            // Resultado
    uint32_t RawDamage;             // Dano bruto
    uint32_t FinalDamage;           // Dano final
    int32_t DamageReduction;        // Redução de dano
    bool IsCritical;                // É crítico
    bool IsBlocked;                 // Foi bloqueado
    bool IsDodged;                  // Foi esquivado
    bool IsParried;                 // Foi aparado
    bool IsReflected;               // Foi refletido
    uint32_t AbsorbedAmount;        // Quantidade absorvida
    uint32_t ReflectedAmount;       // Quantidade refletida
    uint32_t Timestamp;             // Timestamp
    
    DamageInfo() : AttackerID(0), DefenderID(0), SkillID(0),
        Type(DamageType::NONE), Result(DamageResult::NORMAL),
        RawDamage(0), FinalDamage(0), DamageReduction(0),
        IsCritical(false), IsBlocked(false), IsDodged(false),
        IsParried(false), IsReflected(false), AbsorbedAmount(0),
        ReflectedAmount(0), Timestamp(0) {}
};

/**
 * @brief Configuração de combate
 */
struct CombatConfig {
    bool UseDamageFormula;              // Usar fórmula de dano
    std::string DamageFormula;          // Fórmula de dano
    bool UseRandom;                     // Usar aleatório
    uint32_t MinDamageModifier;         // Modificador de dano mínimo
    uint32_t MaxDamageModifier;         // Modificador de dano máximo
    uint32_t CriticalMultiplier;        // Multiplicador crítico
    uint32_t BlockReduction;            // Redução de bloqueio
    uint32_t ParryReduction;            // Redução de aparar
    uint32_t BaseCriticalChance;        // Chance crítica base
    uint32_t BaseDodgeChance;           // Chance de esquiva base
    uint32_t BaseBlockChance;           // Chance de bloqueio base
    uint32_t BaseParryChance;           // Chance de aparar base
    uint32_t BaseResistChance;          // Chance de resistência base
    uint32_t MaxCriticalChance;         // Chance crítica máxima
    uint32_t MaxDodgeChance;            // Chance de esquiva máxima
    uint32_t MaxBlockChance;            // Chance de bloqueio máxima
    uint32_t MaxParryChance;            // Chance de aparar máxima
    uint32_t MaxResistChance;           // Chance de resistência máxima
    float PVPDamageModifier;            // Modificador de dano PVP
    float PVEDamageModifier;            // Modificador de dano PVE
    float GuildWarDamageModifier;       // Modificador de dano guerra de guilda
    uint32_t CombatLogSize;             // Tamanho do log de combate
    bool EnableDamageCap;               // Habilitar limite de dano
    uint32_t DamageCap;                 // Limite de dano
    
    CombatConfig() : UseDamageFormula(false), UseRandom(true),
        MinDamageModifier(80), MaxDamageModifier(120),
        CriticalMultiplier(150), BlockReduction(30),
        ParryReduction(50), BaseCriticalChance(5),
        BaseDodgeChance(5), BaseBlockChance(5),
        BaseParryChance(5), BaseResistChance(5),
        MaxCriticalChance(95), MaxDodgeChance(95),
        MaxBlockChance(95), MaxParryChance(95),
        MaxResistChance(95), PVPDamageModifier(0.7f),
        PVEDamageModifier(1.0f), GuildWarDamageModifier(0.8f),
        CombatLogSize(100), EnableDamageCap(true),
        DamageCap(99999) {
        DamageFormula = "max(1, atk - def / 2) * (std::mt19937(90, 110) / 100)";
    }
};

/**
 * @brief Estatísticas de dano
 */
struct DamageStats {
    uint32_t TotalDamageDealt;         // Dano total causado
    uint32_t TotalDamageTaken;         // Dano total recebido
    uint32_t MaxDamageDealt;           // Dano máximo causado
    uint32_t MaxDamageTaken;           // Dano máximo recebido
    uint32_t TotalCriticals;           // Total de críticos
    uint32_t TotalMisses;              // Total de erros
    uint32_t TotalDodges;              // Total de esquivas
    uint32_t TotalBlocks;              // Total de bloqueios
    uint32_t TotalParries;             // Total de aparações
    uint32_t TotalResists;             // Total de resistências
    uint32_t TotalImmunes;             // Total de imunidades
    uint32_t TotalReflects;            // Total de reflexões
    uint32_t TotalAbsorbs;             // Total de absorções
    float CriticalRate;                // Taxa de crítico
    float MissRate;                    // Taxa de erro
    float DodgeRate;                   // Taxa de esquiva
    float BlockRate;                   // Taxa de bloqueio
    float ParryRate;                   // Taxa de aparação
    float ResistRate;                  // Taxa de resistência
    uint32_t TotalAttacks;             // Total de ataques
    uint32_t TotalHits;                // Total de acertos
    
    DamageStats() : TotalDamageDealt(0), TotalDamageTaken(0),
        MaxDamageDealt(0), MaxDamageTaken(0), TotalCriticals(0),
        TotalMisses(0), TotalDodges(0), TotalBlocks(0),
        TotalParries(0), TotalResists(0), TotalImmunes(0),
        TotalReflects(0), TotalAbsorbs(0), CriticalRate(0.0f),
        MissRate(0.0f), DodgeRate(0.0f), BlockRate(0.0f),
        ParryRate(0.0f), ResistRate(0.0f), TotalAttacks(0),
        TotalHits(0) {}
};

/**
 * @brief Modificadores de dano
 */
struct DamageModifiers {
    float PhysicalDamageMod;          // Modificador de dano físico
    float MagicalDamageMod;           // Modificador de dano mágico
    float CriticalDamageMod;          // Modificador de dano crítico
    float DamageReductionMod;         // Modificador de redução de dano
    float HealingMod;                 // Modificador de cura
    std::map<ResistanceType, int> Resistances; // Resistências
    float BlockChanceMod;             // Modificador de chance de bloqueio
    float DodgeChanceMod;             // Modificador de chance de esquiva
    float ParryChanceMod;             // Modificador de chance de aparar
    float CriticalChanceMod;          // Modificador de chance crítica
    float ResistChanceMod;            // Modificador de chance de resistência
    float ReflectChanceMod;           // Modificador de chance de reflexão
    float AbsorbChanceMod;            // Modificador de chance de absorção
    uint32_t FlatDamageReduction;     // Redução de dano fixa
    
    DamageModifiers() : PhysicalDamageMod(1.0f), MagicalDamageMod(1.0f),
        CriticalDamageMod(1.0f), DamageReductionMod(1.0f),
        HealingMod(1.0f), BlockChanceMod(0.0f),
        DodgeChanceMod(0.0f), ParryChanceMod(0.0f),
        CriticalChanceMod(0.0f), ResistChanceMod(0.0f),
        ReflectChanceMod(0.0f), AbsorbChanceMod(0.0f),
        FlatDamageReduction(0) {}
};

/**
 * @brief Log de combate
 */
struct CombatLog {
    std::vector<DamageInfo> Log;       // Log
    uint32_t MaxSize;                  // Tamanho máximo
    
    CombatLog() /**
 * MaxSize
 * 
 * Implementa a funcionalidade MaxSize conforme especificação original.
 * @param 100 Parâmetro 100
 * @return Retorna :
 */
 : MaxSize(100){}
    
    /**
     * @brief Adiciona uma entrada ao log
     * @param damageInfo Informações de dano
     */
    void addEntry(const DamageInfo& damageInfo) {
        Log.push_back(damageInfo);
        if (Log.size() > MaxSize) {
            Log.erase(Log.begin());
        }
    }
    
    /**
     * @brief Limpa o log
     */
    void clear() {
        Log.clear();
    }
};

/**
 * @brief Callback de dano
 */
using DamageCallback = std::function<void(const DamageInfo&)>;

/**
 * @brief Calculador de dano
 * 
 * Esta classe é responsável por calcular o dano causado e recebido pelos personagens.
 */
/**
 * Classe WYDDamageCalculator
 * 
 * Esta classe implementa a funcionalidade WYDDamageCalculator conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class DamageCalculator {
public:
    /**
     * @brief Obtém a instância única do calculador
     * @return Instância
     */
    static DamageCalculator& getInstance() {
        static DamageCalculator instance;
        return instance;
    }
    
    /**
     * @brief Inicializa o calculador
     * @return true se inicializado com sucesso
     */
    bool initialize();
    
    /**
     * @brief Finaliza o calculador
     */
    void shutdown();
    
    /**
     * @brief Atualiza o calculador
     * @param deltaTime Tempo decorrido em milissegundos
     */
    void update(uint32_t deltaTime);
    
    /**
     * @brief Carrega a configuração de combate
     * @return true se carregado com sucesso
     */
    bool loadCombatConfig();
    
    /**
     * @brief Calcula o dano
     * @param attacker Atacante
     * @param defender Defensor
     * @param skillID ID da habilidade (0 para ataque básico)
     * @param damageType Tipo de dano
     * @return Informações de dano
     */
    DamageInfo calculateDamage(const STRUCT_MOB& attacker, const STRUCT_MOB& defender, uint32_t skillID = 0, DamageType damageType = DamageType::PHYSICAL);
    
    /**
     * @brief Calcula o dano
     * @param attackerID ID do atacante
     * @param defenderID ID do defensor
     * @param skillID ID da habilidade (0 para ataque básico)
     * @param damageType Tipo de dano
     * @return Informações de dano
     */
    DamageInfo calculateDamage(uint32_t attackerID, uint32_t defenderID, uint32_t skillID = 0, DamageType damageType = DamageType::PHYSICAL);
    
    /**
     * @brief Aplica o dano
     * @param damageInfo Informações de dano
     * @param applyStatus Aplicar status
     * @return true se aplicado com sucesso
     */
    bool applyDamage(const DamageInfo& damageInfo, bool applyStatus = true);
    
    /**
     * @brief Calcula o dano bruto
     * @param attacker Atacante
     * @param defender Defensor
     * @param skillID ID da habilidade (0 para ataque básico)
     * @param damageType Tipo de dano
     * @return Dano bruto
     */
    uint32_t calculateRawDamage(const STRUCT_MOB& attacker, const STRUCT_MOB& defender, uint32_t skillID = 0, DamageType damageType = DamageType::PHYSICAL);
    
    /**
     * @brief Calcula a redução de dano
     * @param attacker Atacante
     * @param defender Defensor
     * @param rawDamage Dano bruto
     * @param damageType Tipo de dano
     * @return Redução de dano
     */
    int32_t calculateDamageReduction(const STRUCT_MOB& attacker, const STRUCT_MOB& defender, uint32_t rawDamage, DamageType damageType);
    
    /**
     * @brief Calcula a chance crítica
     * @param attacker Atacante
     * @param defender Defensor
     * @param skillID ID da habilidade (0 para ataque básico)
     * @return Chance crítica
     */
    float calculateCriticalChance(const STRUCT_MOB& attacker, const STRUCT_MOB& defender, uint32_t skillID = 0);
    
    /**
     * @brief Calcula a chance de esquiva
     * @param attacker Atacante
     * @param defender Defensor
     * @param skillID ID da habilidade (0 para ataque básico)
     * @return Chance de esquiva
     */
    float calculateDodgeChance(const STRUCT_MOB& attacker, const STRUCT_MOB& defender, uint32_t skillID = 0);
    
    /**
     * @brief Calcula a chance de bloqueio
     * @param attacker Atacante
     * @param defender Defensor
     * @param skillID ID da habilidade (0 para ataque básico)
     * @return Chance de bloqueio
     */
    float calculateBlockChance(const STRUCT_MOB& attacker, const STRUCT_MOB& defender, uint32_t skillID = 0);
    
    /**
     * @brief Calcula a chance de aparar
     * @param attacker Atacante
     * @param defender Defensor
     * @param skillID ID da habilidade (0 para ataque básico)
     * @return Chance de aparar
     */
    float calculateParryChance(const STRUCT_MOB& attacker, const STRUCT_MOB& defender, uint32_t skillID = 0);
    
    /**
     * @brief Calcula a chance de resistência
     * @param attacker Atacante
     * @param defender Defensor
     * @param damageType Tipo de dano
     * @param skillID ID da habilidade (0 para ataque básico)
     * @return Chance de resistência
     */
    float calculateResistChance(const STRUCT_MOB& attacker, const STRUCT_MOB& defender, DamageType damageType, uint32_t skillID = 0);
    
    /**
     * @brief Adiciona modificadores de dano
     * @param entityID ID da entidade
     * @param modifiers Modificadores
     * @param duration Duração (0 para permanente)
     * @return true se adicionado com sucesso
     */
    bool addDamageModifiers(uint32_t entityID, const DamageModifiers& modifiers, uint32_t duration = 0);
    
    /**
     * @brief Remove modificadores de dano
     * @param entityID ID da entidade
     * @param sourceID ID da fonte (0 para todos)
     * @return true se removido com sucesso
     */
    bool removeDamageModifiers(uint32_t entityID, uint32_t sourceID = 0);
    
    /**
     * @brief Obtém modificadores de dano
     * @param entityID ID da entidade
     * @return Modificadores
     */
    DamageModifiers getDamageModifiers(uint32_t entityID);
    
    /**
     * @brief Adiciona uma entrada ao log de combate
     * @param damageInfo Informações de dano
     * @return true se adicionado com sucesso
     */
    bool addToCombatLog(const DamageInfo& damageInfo);
    
    /**
     * @brief Obtém o log de combate
     * @param entityID ID da entidade (0 para global)
     * @return Log de combate
     */
    CombatLog getCombatLog(uint32_t entityID = 0);
    
    /**
     * @brief Limpa o log de combate
     * @param entityID ID da entidade (0 para global)
     * @return true se limpo com sucesso
     */
    bool clearCombatLog(uint32_t entityID = 0);
    
    /**
     * @brief Define o estado de combate
     * @param entityID ID da entidade
     * @param state Estado
     * @param duration Duração (0 para padrão)
     * @return true se definido com sucesso
     */
    bool setCombatState(uint32_t entityID, CombatState state, uint32_t duration = 0);
    
    /**
     * @brief Obtém o estado de combate
     * @param entityID ID da entidade
     * @return Estado
     */
    CombatState getCombatState(uint32_t entityID);
    
    /**
     * @brief Verifica se a entidade está em combate
     * @param entityID ID da entidade
     * @return true se estiver em combate
     */
    bool isInCombat(uint32_t entityID);
    
    /**
     * @brief Obtém o tempo restante de combate
     * @param entityID ID da entidade
     * @return Tempo restante
     */
    uint32_t getCombatTimeRemaining(uint32_t entityID);
    
    /**
     * @brief Remove o estado de combate
     * @param entityID ID da entidade
     * @return true se removido com sucesso
     */
    bool removeCombatState(uint32_t entityID);
    
    /**
     * @brief Obtém estatísticas de dano
     * @param entityID ID da entidade (0 para global)
     * @return Estatísticas
     */
    DamageStats getDamageStats(uint32_t entityID = 0);
    
    /**
     * @brief Limpa estatísticas de dano
     * @param entityID ID da entidade (0 para global)
     * @return true se limpo com sucesso
     */
    bool clearDamageStats(uint32_t entityID = 0);
    
    /**
     * @brief Registra um callback de dano
     * @param callback Função de callback
     * @return ID do callback ou 0 em caso de erro
     */
    uint32_t registerDamageCallback(DamageCallback callback);
    
    /**
     * @brief Remove um callback de dano
     * @param callbackId ID do callback
     * @return true se removido com sucesso
     */
    bool unregisterDamageCallback(uint32_t callbackId);
    
    /**
     * @brief Obtém a configuração de combate
     * @return Configuração
     */
    CombatConfig getCombatConfig() const;
    
    /**
     * @brief Define a configuração de combate
     * @param config Configuração
     */
    void setCombatConfig(const CombatConfig& config);
    
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
    DamageCalculator();
    
    /**
     * @brief Destrutor privado
     */
    ~DamageCalculator();
    
    /**
     * @brief Construtor de cópia privado
     */
    DamageCalculator(const DamageCalculator&) = delete;
    
    /**
     * @brief Operador de atribuição privado
     */
    DamageCalculator& operator=(const DamageCalculator&) = delete;
    
    // Configuração
    CombatConfig _combatConfig;
    
    // Logs de combate
    CombatLog _globalCombatLog;
    std::unordered_map<uint32_t, CombatLog> _entityCombatLogs;
    
    // Estados de combate
    struct CombatStateInfo {
        CombatState State;         // Estado
        uint32_t StartTime;        // Tempo de início
        uint32_t Duration;         // Duração
        
        CombatStateInfo() : State(CombatState::NONE), StartTime(0), Duration(0) {}
    };
    std::unordered_map<uint32_t, CombatStateInfo> _combatStates;
    
    // Modificadores de dano
    struct DamageModifierInfo {
        DamageModifiers Modifiers; // Modificadores
        uint32_t StartTime;        // Tempo de início
        uint32_t Duration;         // Duração
        uint32_t SourceID;         // ID da fonte
        
        DamageModifierInfo() : StartTime(0), Duration(0), SourceID(0) {}
    };
    std::unordered_map<uint32_t, std::vector<DamageModifierInfo>> _damageModifiers;
    
    // Estatísticas de dano
    DamageStats _globalDamageStats;
    std::unordered_map<uint32_t, DamageStats> _entityDamageStats;
    
    // Callbacks
    std::vector<DamageCallback> _damageCallbacks;
    std::atomic<uint32_t> _nextCallbackID;
    
    // Thread safety
    mutable std::mutex _configMutex;
    mutable std::mutex _logMutex;
    mutable std::mutex _stateMutex;
    mutable std::mutex _modifierMutex;
    mutable std::mutex _statsMutex;
    mutable std::mutex _callbackMutex;
    
    // Flags
    std::atomic<bool> _initialized;
    std::atomic<bool> _debugMode;
    
    // Geradores aleatórios
    std::mt19937 _rng;
    std::uniform_int_distribution<uint32_t> _damageRandomizer;
    std::uniform_real_distribution<float> _chanceRandomizer;
    
    // Timers
    uint32_t _combatStateCheckTimer;
    uint32_t _damageModifierCheckTimer;
    
    /**
     * @brief Checa estados de combate expirados
     * @param currentTime Tempo atual
     */
    void checkExpiredCombatStates(uint32_t currentTime);
    
    /**
     * @brief Checa modificadores de dano expirados
     * @param currentTime Tempo atual
     */
    void checkExpiredDamageModifiers(uint32_t currentTime);
    
    /**
     * @brief Executa callbacks de dano
     * @param damageInfo Informações de dano
     */
    void executeDamageCallbacks(const DamageInfo& damageInfo);
    
    /**
     * @brief Atualiza estatísticas de dano
     * @param damageInfo Informações de dano
     */
    void updateDamageStats(const DamageInfo& damageInfo);
    
    /**
     * @brief Verifica se um ataque acerta
     * @param attacker Atacante
     * @param defender Defensor
     * @param skillID ID da habilidade (0 para ataque básico)
     * @return true se acertar
     */
    bool rollHitChance(const STRUCT_MOB& attacker, const STRUCT_MOB& defender, uint32_t skillID = 0);
    
    /**
     * @brief Verifica se um ataque é crítico
     * @param attacker Atacante
     * @param defender Defensor
     * @param skillID ID da habilidade (0 para ataque básico)
     * @return true se for crítico
     */
    bool rollCriticalChance(const STRUCT_MOB& attacker, const STRUCT_MOB& defender, uint32_t skillID = 0);
    
    /**
     * @brief Verifica se um ataque é esquivado
     * @param attacker Atacante
     * @param defender Defensor
     * @param skillID ID da habilidade (0 para ataque básico)
     * @return true se for esquivado
     */
    bool rollDodgeChance(const STRUCT_MOB& attacker, const STRUCT_MOB& defender, uint32_t skillID = 0);
    
    /**
     * @brief Verifica se um ataque é bloqueado
     * @param attacker Atacante
     * @param defender Defensor
     * @param skillID ID da habilidade (0 para ataque básico)
     * @return true se for bloqueado
     */
    bool rollBlockChance(const STRUCT_MOB& attacker, const STRUCT_MOB& defender, uint32_t skillID = 0);
    
    /**
     * @brief Verifica se um ataque é aparado
     * @param attacker Atacante
     * @param defender Defensor
     * @param skillID ID da habilidade (0 para ataque básico)
     * @return true se for aparado
     */
    bool rollParryChance(const STRUCT_MOB& attacker, const STRUCT_MOB& defender, uint32_t skillID = 0);
    
    /**
     * @brief Verifica se um ataque é resistido
     * @param attacker Atacante
     * @param defender Defensor
     * @param damageType Tipo de dano
     * @param skillID ID da habilidade (0 para ataque básico)
     * @return true se for resistido
     */
    bool rollResistChance(const STRUCT_MOB& attacker, const STRUCT_MOB& defender, DamageType damageType, uint32_t skillID = 0);
    
    /**
     * @brief Calcula o modificador de dano
     * @param min Mínimo
     * @param max Máximo
     * @return Modificador
     */
    float calculateDamageModifier(uint32_t min, uint32_t max);
    
    /**
     * @brief Calcular dano baseado em fórmula
     * @param formula Fórmula
     * @param attacker Atacante
     * @param defender Defensor
     * @param skillID ID da habilidade (0 para ataque básico)
     * @param damageType Tipo de dano
     * @return Dano calculado
     */
    uint32_t calculateDamageWithFormula(const std::string& formula, const STRUCT_MOB& attacker, const STRUCT_MOB& defender, uint32_t skillID, DamageType damageType);
    
    /**
     * @brief Converte o tipo de dano em string
     * @param type Tipo
     * @return String
     */
    std::string damageTypeToString(DamageType type);
    
    /**
     * @brief Converte o resultado de dano em string
     * @param result Resultado
     * @return String
     */
    std::string damageResultToString(DamageResult result);
};

} // namespace battle
} // namespace wydbr

#endif // _DAMAGE_CALCULATOR_H_