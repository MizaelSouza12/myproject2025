/**
 * CEntity.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/battle/CEntity.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef CENTITY_H
#define CENTITY_H

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

#include "BattleTypes.h"
#include "../../../core/WYDTypes.h"

namespace wydbr {


/**
 * @file CEntity.h
 * @brief Classe base para entidades de combate
 * 
 * Este arquivo contém a definição da classe base para entidades de combate,
 * que é usada como interface para jogadores, mobs, NPCs, etc.
 * No WYD original, essa funcionalidade era distribuída em várias estruturas
 * específicas. Esta classe centraliza e melhora essa funcionalidade.
 */

namespace wyd {
namespace server {

// Forward declarations
class CBattleManager;

/**
 * @brief Classe base para entidades de combate
 * 
 * Esta classe define a interface comum para todas as entidades que podem
 * participar de combate, como jogadores, mobs, NPCs, pets, etc.
 */
/**
 * Classe WYDEntity
 * 
 * Esta classe implementa a funcionalidade WYDEntity conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class CEntity {
public:
    /**
     * @brief Construtor
     * @param name Nome da entidade
     * @param entityID ID da entidade
     */
    CEntity(const std::string& name, DWORD entityID);
    
    /**
     * @brief Destrutor virtual
     */
    virtual ~CEntity();
    
    /**
     * @brief Obtém o ID da entidade
     * @return ID da entidade
     */
    DWORD GetID() const;
    
    /**
     * @brief Obtém o nome da entidade
     * @return Nome da entidade
     */
    std::string GetName() const;
    
    /**
     * @brief Define o nome da entidade
     * @param name Novo nome
     */
    void SetName(const std::string& name);
    
    /**
     * @brief Obtém o tipo da entidade
     * @return Tipo da entidade
     */
    virtual BYTE GetEntityType() const = 0;
    
    /**
     * @brief Verifica se a entidade está viva
     * @return true se estiver viva
     */
    virtual bool IsAlive() const = 0;
    
    /**
     * @brief Obtém o nível da entidade
     * @return Nível da entidade
     */
    virtual BYTE GetLevel() const = 0;
    
    /**
     * @brief Obtém a posição X da entidade
     * @return Posição X
     */
    virtual WORD GetPosX() const = 0;
    
    /**
     * @brief Obtém a posição Y da entidade
     * @return Posição Y
     */
    virtual WORD GetPosY() const = 0;
    
    /**
     * @brief Obtém o ID do mapa da entidade
     * @return ID do mapa
     */
    virtual WORD GetMapID() const = 0;
    
    /**
     * @brief Aplica dano à entidade
     * @param damage Quantidade de dano
     * @param attacker Ponteiro para o atacante
     * @param skillID ID da skill (0 para ataque normal)
     * @return true se a entidade morreu
     */
    virtual bool ApplyDamage(int damage, CEntity* attacker, WORD skillID = 0) = 0;
    
    /**
     * @brief Aplica cura à entidade
     * @param amount Quantidade de cura
     * @param healer Ponteiro para o curador
     * @param skillID ID da skill (0 para cura natural)
     * @return Quantidade de HP realmente curada
     */
    virtual int ApplyHealing(int amount, CEntity* healer, WORD skillID = 0) = 0;
    
    /**
     * @brief Aplica um efeito à entidade
     * @param affectType Tipo de efeito
     * @param value Valor do efeito
     * @param duration Duração em ms (0 para permanente)
     * @param applier Ponteiro para quem aplicou o efeito
     * @return true se o efeito foi aplicado com sucesso
     */
    virtual bool ApplyEffect(AffectType affectType, BYTE value, DWORD duration, CEntity* applier) = 0;
    
    /**
     * @brief Remove um efeito da entidade
     * @param affectType Tipo de efeito
     * @return true se o efeito foi removido com sucesso
     */
    virtual bool RemoveEffect(AffectType affectType) = 0;
    
    /**
     * @brief Verifica se a entidade tem um efeito
     * @param affectType Tipo de efeito
     * @return true se tiver o efeito
     */
    virtual bool HasEffect(AffectType affectType) const = 0;
    
    /**
     * @brief Obtém o valor de um efeito
     * @param affectType Tipo de efeito
     * @return Valor do efeito, ou 0 se não tiver
     */
    virtual BYTE GetEffectValue(AffectType affectType) const = 0;
    
    /**
     * @brief Obtém o tempo restante de um efeito
     * @param affectType Tipo de efeito
     * @return Tempo restante em ms, ou 0 se não tiver
     */
    virtual DWORD GetEffectTimeRemaining(AffectType affectType) const = 0;
    
    /**
     * @brief Atualiza os efeitos da entidade
     * @param elapsed Tempo decorrido desde a última atualização (ms)
     */
    virtual void UpdateEffects(DWORD elapsed) = 0;
    
    /**
     * @brief Usa uma skill
     * @param skillID ID da skill
     * @param skillLevel Nível da skill
     * @param targets Ponteiros para os alvos
     * @return Resultados para cada alvo
     */
    virtual std::vector<BattleResult> UseSkill(WORD skillID, BYTE skillLevel, const std::vector<CEntity*>& targets);
    
    /**
     * @brief Ataca uma entidade
     * @param target Ponteiro para o alvo
     * @return Resultado do ataque
     */
    virtual BattleResult Attack(CEntity* target);
    
    /**
     * @brief Verifica se a entidade pode atacar outra
     * @param target Ponteiro para o alvo
     * @return true se puder atacar
     */
    virtual bool CanAttack(CEntity* target) const;
    
    /**
     * @brief Verifica se a entidade pode usar uma skill
     * @param skillID ID da skill
     * @param skillLevel Nível da skill
     * @return true se puder usar
     */
    virtual bool CanUseSkill(WORD skillID, BYTE skillLevel) const;
    
    /**
     * @brief Verifica se a entidade pode usar uma skill em um alvo
     * @param skillID ID da skill
     * @param target Ponteiro para o alvo
     * @return true se puder usar
     */
    virtual bool CanTargetWithSkill(WORD skillID, CEntity* target) const;
    
    /**
     * @brief Obtém os atributos de batalha da entidade
     * @return Atributos de batalha
     */
    virtual BattleAttributes GetBattleAttributes() const = 0;
    
    /**
     * @brief Obtém o HP atual da entidade
     * @return HP atual
     */
    virtual int GetHP() const = 0;
    
    /**
     * @brief Obtém o HP máximo da entidade
     * @return HP máximo
     */
    virtual int GetMaxHP() const = 0;
    
    /**
     * @brief Obtém o MP atual da entidade
     * @return MP atual
     */
    virtual int GetMP() const = 0;
    
    /**
     * @brief Obtém o MP máximo da entidade
     * @return MP máximo
     */
    virtual int GetMaxMP() const = 0;
    
    /**
     * @brief Obtém a força da entidade
     * @return Força
     */
    virtual int GetStr() const = 0;
    
    /**
     * @brief Obtém a inteligência da entidade
     * @return Inteligência
     */
    virtual int GetInt() const = 0;
    
    /**
     * @brief Obtém a destreza da entidade
     * @return Destreza
     */
    virtual int GetDex() const = 0;
    
    /**
     * @brief Obtém a constituição da entidade
     * @return Constituição
     */
    virtual int GetCon() const = 0;
    
    /**
     * @brief Obtém a velocidade de movimento da entidade
     * @return Velocidade de movimento
     */
    virtual int GetMoveSpeed() const = 0;
    
    /**
     * @brief Obtém a velocidade de ataque da entidade
     * @return Velocidade de ataque
     */
    virtual int GetAttackSpeed() const = 0;
    
    /**
     * @brief Obtém o dano físico mínimo da entidade
     * @return Dano físico mínimo
     */
    virtual int GetPhysicalDamageMin() const = 0;
    
    /**
     * @brief Obtém o dano físico máximo da entidade
     * @return Dano físico máximo
     */
    virtual int GetPhysicalDamageMax() const = 0;
    
    /**
     * @brief Obtém o dano mágico mínimo da entidade
     * @return Dano mágico mínimo
     */
    virtual int GetMagicDamageMin() const = 0;
    
    /**
     * @brief Obtém o dano mágico máximo da entidade
     * @return Dano mágico máximo
     */
    virtual int GetMagicDamageMax() const = 0;
    
    /**
     * @brief Obtém a defesa física da entidade
     * @return Defesa física
     */
    virtual int GetPhysicalDefense() const = 0;
    
    /**
     * @brief Obtém a defesa mágica da entidade
     * @return Defesa mágica
     */
    virtual int GetMagicDefense() const = 0;
    
    /**
     * @brief Obtém a taxa de crítico da entidade
     * @return Taxa de crítico
     */
    virtual int GetCriticalRate() const = 0;
    
    /**
     * @brief Obtém a taxa de evasão da entidade
     * @return Taxa de evasão
     */
    virtual int GetEvasionRate() const = 0;
    
    /**
     * @brief Obtém a defesa contra elementos
     * @param element Índice do elemento (0-4)
     * @return Valor da defesa
     */
    virtual int GetElementalDefense(int element) const = 0;
    
    /**
     * @brief Obtém o alcance de ataque da entidade
     * @return Alcance de ataque
     */
    virtual int GetAttackRange() const = 0;
    
    /**
     * @brief Define o HP atual da entidade
     * @param hp Novo HP
     */
    virtual void SetHP(int hp) = 0;
    
    /**
     * @brief Define o MP atual da entidade
     * @param mp Novo MP
     */
    virtual void SetMP(int mp) = 0;
    
    /**
     * @brief Define a posição da entidade
     * @param x Posição X
     * @param y Posição Y
     * @param mapID ID do mapa (0 para não alterar)
     */
    virtual void SetPosition(WORD x, WORD y, WORD mapID = 0) = 0;
    
    /**
     * @brief Obtém o estado de animação da entidade
     * @return Estado de animação
     */
    virtual AnimState GetAnimState() const = 0;
    
    /**
     * @brief Define o estado de animação da entidade
     * @param state Novo estado
     */
    virtual void SetAnimState(AnimState state) = 0;
    
    /**
     * @brief Obtém o tipo de movimento da entidade
     * @return Tipo de movimento
     */
    virtual MoveType GetMoveType() const = 0;
    
    /**
     * @brief Define o tipo de movimento da entidade
     * @param type Novo tipo
     */
    virtual void SetMoveType(MoveType type) = 0;
    
    /**
     * @brief Processa a morte da entidade
     * @param killer Ponteiro para quem matou (nullptr para morte natural)
     */
    virtual void ProcessDeath(CEntity* killer = nullptr) = 0;
    
    /**
     * @brief Processa a ressurreição da entidade
     * @param resurrecter Ponteiro para quem ressuscitou (nullptr para ressurreição natural)
     * @param percentHP Porcentagem do HP máximo a restaurar (0-100)
     * @param percentMP Porcentagem do MP máximo a restaurar (0-100)
     * @return true se ressuscitou com sucesso
     */
    virtual bool ProcessResurrection(CEntity* resurrecter = nullptr, int percentHP = 100, int percentMP = 100) = 0;
    
    /**
     * @brief Atualiza o estado da entidade
     * @param elapsed Tempo decorrido desde a última atualização (ms)
     */
    virtual void Update(DWORD elapsed) = 0;
    
protected:
    std::string name_;            // Nome da entidade
    DWORD entityID_;              // ID da entidade
    
    // Referência para o gerenciador de batalha
    CBattleManager& battleManager_;
};

} // namespace server
} // namespace wyd

#endif // CENTITY_H

} // namespace wydbr
