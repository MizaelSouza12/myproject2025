/**
 * BattleManager.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/battle/BattleManager.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef BATTLEMANAGER_H
#define BATTLEMANAGER_H

#include <vector>
#include <string>
#include <unordered_map>
#include <mutex>
#include <functional>
#include <deque>
#include <random>

#include "BattleTypes.h"
#include "../../../core/WYDTypes.h"
#include "../item/ItemTypes.h"

namespace wydbr {


/**
 * @file BattleManager.h
 * @brief Gerenciador de combate do WYD
 * 
 * Este arquivo contém a definição do gerenciador de combate que implementa
 * as fórmulas de dano, efeitos, e lógica de combate do WYD original.
 * No WYD original, esta lógica estava espalhada em funções como ProcessAttack,
 * GetDamage, ProcessSkill, etc. Esta classe centraliza e melhora essa funcionalidade.
 */

namespace wyd {
namespace server {

// Forward declarations
class CPlayer;
class CMob;
class CEntity;

// Tipo de delegado para eventos de batalha
using BattleEventCallback = std::function<void(const BattleResult&)>;

/**
 * @brief Gerenciador de combate
 * 
 * Esta classe gerencia todas as operações de combate, incluindo
 * cálculo de dano, aplicação de efeitos, e lógica de combate.
 */
/**
 * Classe WYDBattleManager
 * 
 * Esta classe implementa a funcionalidade WYDBattleManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class CBattleManager {
public:
    /**
     * @brief Obtém a instância do singleton
     * @return Referência para a instância
     */
    static CBattleManager& GetInstance();
    
    /**
     * @brief Inicializa o gerenciador de combate
     * @return true se inicializado com sucesso
     */
    bool Initialize();
    
    /**
     * @brief Finaliza o gerenciador de combate
     */
    void Shutdown();
    
    /**
     * @brief Processa um ataque físico
     * @param attacker Entidade atacante
     * @param target Entidade alvo
     * @param skillID ID da skill (0 para ataque normal)
     * @return Resultado do ataque
     */
    BattleResult ProcessAttack(CEntity* attacker, CEntity* target, WORD skillID = 0);
    
    /**
     * @brief Processa uma skill
     * @param caster Entidade que está usando a skill
     * @param targets Lista de alvos
     * @param skillID ID da skill
     * @param skillLevel Nível da skill
     * @return Resultados para cada alvo
     */
    std::vector<BattleResult> ProcessSkill(CEntity* caster, const std::vector<CEntity*>& targets, 
                                          WORD skillID, BYTE skillLevel);
    
    /**
     * @brief Calcula o dano
     * @param attacker Entidade atacante
     * @param target Entidade alvo
     * @param damageType Tipo de dano
     * @param skillID ID da skill (0 para ataque normal)
     * @param skillLevel Nível da skill
     * @return Resultado do cálculo de dano
     */
    BattleResult CalculateDamage(CEntity* attacker, CEntity* target, DamageType damageType, 
                                WORD skillID = 0, BYTE skillLevel = 0);
    
    /**
     * @brief Aplica um efeito a uma entidade
     * @param target Entidade alvo
     * @param affectType Tipo de efeito
     * @param value Valor do efeito
     * @param duration Duração em ms (0 para permanente)
     * @return true se o efeito foi aplicado com sucesso
     */
    bool ApplyEffect(CEntity* target, AffectType affectType, BYTE value, DWORD duration);
    
    /**
     * @brief Remove um efeito de uma entidade
     * @param target Entidade alvo
     * @param affectType Tipo de efeito
     * @return true se o efeito foi removido com sucesso
     */
    bool RemoveEffect(CEntity* target, AffectType affectType);
    
    /**
     * @brief Verifica se o ataque acerta
     * @param attacker Entidade atacante
     * @param target Entidade alvo
     * @param skillID ID da skill (0 para ataque normal)
     * @return true se o ataque acertar
     */
    bool CheckHit(CEntity* attacker, CEntity* target, WORD skillID = 0);
    
    /**
     * @brief Verifica se o ataque é crítico
     * @param attacker Entidade atacante
     * @param target Entidade alvo
     * @param skillID ID da skill (0 para ataque normal)
     * @return true se o ataque for crítico
     */
    bool CheckCritical(CEntity* attacker, CEntity* target, WORD skillID = 0);
    
    /**
     * @brief Obtém os atributos de batalha de uma entidade
     * @param entity Entidade
     * @return Atributos de batalha
     */
    BattleAttributes GetBattleAttributes(CEntity* entity);
    
    /**
     * @brief Verifica se uma entidade pode atacar outra
     * @param attacker Entidade atacante
     * @param target Entidade alvo
     * @return true se puder atacar
     */
    bool CanAttack(CEntity* attacker, CEntity* target);
    
    /**
     * @brief Verifica se uma entidade pode usar uma skill
     * @param caster Entidade que quer usar a skill
     * @param skillID ID da skill
     * @param skillLevel Nível da skill
     * @return true se puder usar
     */
    bool CanUseSkill(CEntity* caster, WORD skillID, BYTE skillLevel);
    
    /**
     * @brief Verifica se uma entidade pode usar uma skill em um alvo
     * @param caster Entidade que quer usar a skill
     * @param target Entidade alvo
     * @param skillID ID da skill
     * @return true se puder usar
     */
    bool CanTargetWithSkill(CEntity* caster, CEntity* target, WORD skillID);
    
    /**
     * @brief Obtém a definição de uma skill
     * @param skillID ID da skill
     * @return Ponteiro para a definição da skill
     */
    const STRUCT_SKILL_DATA* GetSkillData(WORD skillID);
    
    /**
     * @brief Registra um callback para eventos de batalha
     * @param callback Função de callback
     * @return ID do callback para posterior remoção
     */
    int RegisterBattleEventCallback(const BattleEventCallback& callback);
    
    /**
     * @brief Remove um callback registrado
     * @param callbackID ID do callback
     * @return true se removido com sucesso
     */
    bool UnregisterBattleEventCallback(int callbackID);
    
    /**
     * @brief Adiciona um registro de dano
     * @param targetID ID da entidade alvo
     * @param attackerID ID da entidade atacante
     * @param damage Quantidade de dano
     */
    void AddDamageRecord(DWORD targetID, DWORD attackerID, int damage);
    
    /**
     * @brief Obtém os registros de dano para uma entidade
     * @param targetID ID da entidade alvo
     * @return Lista de registros de dano
     */
    std::vector<DamageRecord> GetDamageRecords(DWORD targetID);
    
    /**
     * @brief Limpa registros de dano antigos
     * @param currentTime Tempo atual (ms)
     * @param maxAge Idade máxima para manter (ms)
     */
    void CleanupDamageRecords(DWORD currentTime, DWORD maxAge = MAX_DAMAGE_DELAY);
    
    /**
     * @brief Obtém a contribuição de dano de cada entidade
     * @param targetID ID da entidade alvo
     * @return Mapa de ID da entidade para porcentagem de contribuição
     */
    std::unordered_map<DWORD, float> GetDamageContributions(DWORD targetID);
    
    /**
     * @brief Verifica quem deve receber a experiência pela morte
     * @param targetID ID da entidade que morreu
     * @return ID da entidade que deve receber a experiência
     */
    DWORD GetExperienceReceiver(DWORD targetID);
    
    /**
     * @brief Distribui experiência e drops pela morte de uma entidade
     * @param deadEntity Entidade que morreu
     * @param killer Entidade que matou (pode ser nullptr)
     */
    void ProcessDeath(CEntity* deadEntity, CEntity* killer = nullptr);
    
    /**
     * @brief Calcula o delay de ataque baseado em velocidade de ataque
     * @param attackSpeed Velocidade de ataque (100 = normal)
     * @return Delay em milissegundos
     */
    DWORD CalculateAttackDelay(int attackSpeed);
    
    /**
     * @brief Calcula o tempo de recarga de uma skill (cooldown)
     * @param skillID ID da skill
     * @param skillLevel Nível da skill
     * @param attacker Entidade que está usando a skill
     * @return Tempo de recarga em milissegundos
     */
    DWORD CalculateSkillCooldown(WORD skillID, BYTE skillLevel, CEntity* attacker);
    
    /**
     * @brief Calcula a duração de um efeito
     * @param affectType Tipo de efeito
     * @param value Valor do efeito
     * @param caster Entidade que está aplicando o efeito
     * @param target Entidade alvo
     * @return Duração em milissegundos
     */
    DWORD CalculateEffectDuration(AffectType affectType, BYTE value, CEntity* caster, CEntity* target);
    
    /**
     * @brief Calcula o alcance de uma skill
     * @param skillID ID da skill
     * @param skillLevel Nível da skill
     * @param caster Entidade que está usando a skill
     * @return Alcance em tiles
     */
    int CalculateSkillRange(WORD skillID, BYTE skillLevel, CEntity* caster);
    
    /**
     * @brief Calcula a área de efeito de uma skill
     * @param skillID ID da skill
     * @param skillLevel Nível da skill
     * @param caster Entidade que está usando a skill
     * @return Área em tiles
     */
    int CalculateSkillArea(WORD skillID, BYTE skillLevel, CEntity* caster);
    
    /**
     * @brief Calcula o custo de MP de uma skill
     * @param skillID ID da skill
     * @param skillLevel Nível da skill
     * @param caster Entidade que está usando a skill
     * @return Custo de MP
     */
    int CalculateSkillMPCost(WORD skillID, BYTE skillLevel, CEntity* caster);
    
    /**
     * @brief Calcula o custo de HP de uma skill
     * @param skillID ID da skill
     * @param skillLevel Nível da skill
     * @param caster Entidade que está usando a skill
     * @return Custo de HP
     */
    int CalculateSkillHPCost(WORD skillID, BYTE skillLevel, CEntity* caster);
    
private:
    /**
     * @brief Construtor privado (singleton)
     */
    CBattleManager();
    
    /**
     * @brief Destrutor
     */
    ~CBattleManager();
    
    /**
     * @brief Desabilita o construtor de cópia
     */
    CBattleManager(const CBattleManager&) = delete;
    
    /**
     * @brief Desabilita o operador de atribuição
     */
    CBattleManager& operator=(const CBattleManager&) = delete;
    
    /**
     * @brief Notifica eventos de batalha
     * @param result Resultado da batalha
     */
    void NotifyBattleEvent(const BattleResult& result);
    
    /**
     * @brief Obtém uma skill pelo ID
     * @param skillID ID da skill
     * @return Ponteiro para os dados da skill
     */
    STRUCT_SKILL_DATA* FindSkill(WORD skillID);
    
    /**
     * @brief Converte o tipo de dano para um índice
     * @param damageType Tipo de dano
     * @return Índice para resistências
     */
    int DamageTypeToResistIndex(DamageType damageType);
    
    /**
     * @brief Implementação da fórmula de dano físico
     * @param attAttributes Atributos do atacante
     * @param defAttributes Atributos do defensor
     * @param skillData Dados da skill (opcional)
     * @param skillLevel Nível da skill
     * @return Resultado do cálculo
     */
    BattleResult CalculatePhysicalDamage(const BattleAttributes& attAttributes, 
                                        const BattleAttributes& defAttributes,
                                        const STRUCT_SKILL_DATA* skillData = nullptr,
                                        BYTE skillLevel = 0);
    
    /**
     * @brief Implementação da fórmula de dano mágico
     * @param attAttributes Atributos do atacante
     * @param defAttributes Atributos do defensor
     * @param skillData Dados da skill
     * @param skillLevel Nível da skill
     * @return Resultado do cálculo
     */
    BattleResult CalculateMagicDamage(const BattleAttributes& attAttributes, 
                                     const BattleAttributes& defAttributes,
                                     const STRUCT_SKILL_DATA* skillData,
                                     BYTE skillLevel);
    
    /**
     * @brief Implementação da fórmula de cura
     * @param attAttributes Atributos do curador
     * @param defAttributes Atributos do alvo
     * @param skillData Dados da skill
     * @param skillLevel Nível da skill
     * @return Resultado do cálculo
     */
    BattleResult CalculateHealing(const BattleAttributes& attAttributes, 
                                 const BattleAttributes& defAttributes,
                                 const STRUCT_SKILL_DATA* skillData,
                                 BYTE skillLevel);
    
    /**
     * @brief Implementação da fórmula de dano percentual
     * @param attAttributes Atributos do atacante
     * @param defAttributes Atributos do defensor
     * @param skillData Dados da skill
     * @param skillLevel Nível da skill
     * @return Resultado do cálculo
     */
    BattleResult CalculatePercentageDamage(const BattleAttributes& attAttributes, 
                                          const BattleAttributes& defAttributes,
                                          const STRUCT_SKILL_DATA* skillData,
                                          BYTE skillLevel);
    
    // Lista de skills carregadas
    std::unordered_map<WORD, STRUCT_SKILL_DATA> skills_;
    
    // Registros de dano por entidade
    std::unordered_map<DWORD, std::deque<DamageRecord>> damageRecords_;
    std::mutex damageRecordsMutex_;
    
    // Callbacks de eventos de batalha
    std::unordered_map<int, BattleEventCallback> battleEventCallbacks_;
    std::mutex callbacksMutex_;
    int nextCallbackID_;
    
    // Gerador de números aleatórios
    std::mt19937 rng_;
    
    // Estado de inicialização
    bool initialized_;
};

// Definição global para facilitar o acesso
#define g_BattleManager CBattleManager::GetInstance()

} // namespace server
} // namespace wyd

#endif // BATTLEMANAGER_H

} // namespace wydbr
