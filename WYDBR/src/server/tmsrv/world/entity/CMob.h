/**
 * CMob.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/world/entity/CMob.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef CMOB_H
#define CMOB_H

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <functional>
#include <array>

#include "../../../../core/WYDTypes.h"
#include "../WorldTypes.h"
#include "../../battle/CEntity.h"
#include "../../item/ItemTypes.h"

namespace wydbr {


/**
 * @file CMob.h
 * @brief Definição da classe base para mobs
 * 
 * Este arquivo contém a definição da classe base para mobs, que
 * representa todos os NPCs e monstros controlados pelo sistema.
 * No WYD original, os mobs eram gerenciados por simples estruturas
 * e arrays, causando vários problemas. Esta implementação usa OOP
 * para maior organização e manutenibilidade.
 */

namespace wyd {
namespace server {
namespace world {
namespace entity {

// Forward declarations
class CWorldManager;
class CMobAI;
class CSpawnManager;
class CMobGroup;

/**
 * @brief Classe base para mobs
 * 
 * Esta classe representa um mob no mundo do jogo. Ela herda de CEntity
 * para ter acesso a funcionalidades de combate.
 */
/**
 * Classe WYDMob
 * 
 * Esta classe implementa a funcionalidade WYDMob conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class CMob : public battle::CEntity {
public:
    /**
     * @brief Construtor
     * @param mobID ID do mob
     * @param name Nome do mob
     * @param level Nível do mob
     */
    CMob(WORD mobID, const std::string& name, BYTE level);
    
    /**
     * @brief Destrutor virtual
     */
    virtual ~CMob();
    
    //----- Métodos herdados de CEntity -----//
    
    /**
     * @brief Obtém o tipo da entidade
     * @return Tipo da entidade
     */
    virtual BYTE GetEntityType() const override;
    
    /**
     * @brief Verifica se a entidade está viva
     * @return true se estiver viva
     */
    virtual bool IsAlive() const override;
    
    /**
     * @brief Obtém o nível da entidade
     * @return Nível da entidade
     */
    virtual BYTE GetLevel() const override;
    
    /**
     * @brief Obtém a posição X da entidade
     * @return Posição X
     */
    virtual WORD GetPosX() const override;
    
    /**
     * @brief Obtém a posição Y da entidade
     * @return Posição Y
     */
    virtual WORD GetPosY() const override;
    
    /**
     * @brief Obtém o ID do mapa da entidade
     * @return ID do mapa
     */
    virtual WORD GetMapID() const override;
    
    /**
     * @brief Aplica dano à entidade
     * @param damage Quantidade de dano
     * @param attacker Ponteiro para o atacante
     * @param skillID ID da skill (0 para ataque normal)
     * @return true se a entidade morreu
     */
    virtual bool ApplyDamage(int damage, CEntity* attacker, WORD skillID = 0) override;
    
    /**
     * @brief Aplica cura à entidade
     * @param amount Quantidade de cura
     * @param healer Ponteiro para o curador
     * @param skillID ID da skill (0 para cura natural)
     * @return Quantidade de HP realmente curada
     */
    virtual int ApplyHealing(int amount, CEntity* healer, WORD skillID = 0) override;
    
    /**
     * @brief Aplica um efeito à entidade
     * @param affectType Tipo de efeito
     * @param value Valor do efeito
     * @param duration Duração em ms (0 para permanente)
     * @param applier Ponteiro para quem aplicou o efeito
     * @return true se o efeito foi aplicado com sucesso
     */
    virtual bool ApplyEffect(battle::AffectType affectType, BYTE value, DWORD duration, CEntity* applier) override;
    
    /**
     * @brief Remove um efeito da entidade
     * @param affectType Tipo de efeito
     * @return true se o efeito foi removido com sucesso
     */
    virtual bool RemoveEffect(battle::AffectType affectType) override;
    
    /**
     * @brief Verifica se a entidade tem um efeito
     * @param affectType Tipo de efeito
     * @return true se tiver o efeito
     */
    virtual bool HasEffect(battle::AffectType affectType) const override;
    
    /**
     * @brief Obtém o valor de um efeito
     * @param affectType Tipo de efeito
     * @return Valor do efeito, ou 0 se não tiver
     */
    virtual BYTE GetEffectValue(battle::AffectType affectType) const override;
    
    /**
     * @brief Obtém o tempo restante de um efeito
     * @param affectType Tipo de efeito
     * @return Tempo restante em ms, ou 0 se não tiver
     */
    virtual DWORD GetEffectTimeRemaining(battle::AffectType affectType) const override;
    
    /**
     * @brief Atualiza os efeitos da entidade
     * @param elapsed Tempo decorrido desde a última atualização (ms)
     */
    virtual void UpdateEffects(DWORD elapsed) override;
    
    /**
     * @brief Obtém os atributos de batalha da entidade
     * @return Atributos de batalha
     */
    virtual battle::BattleAttributes GetBattleAttributes() const override;
    
    /**
     * @brief Obtém o HP atual da entidade
     * @return HP atual
     */
    virtual int GetHP() const override;
    
    /**
     * @brief Obtém o HP máximo da entidade
     * @return HP máximo
     */
    virtual int GetMaxHP() const override;
    
    /**
     * @brief Obtém o MP atual da entidade
     * @return MP atual
     */
    virtual int GetMP() const override;
    
    /**
     * @brief Obtém o MP máximo da entidade
     * @return MP máximo
     */
    virtual int GetMaxMP() const override;
    
    /**
     * @brief Obtém a força da entidade
     * @return Força
     */
    virtual int GetStr() const override;
    
    /**
     * @brief Obtém a inteligência da entidade
     * @return Inteligência
     */
    virtual int GetInt() const override;
    
    /**
     * @brief Obtém a destreza da entidade
     * @return Destreza
     */
    virtual int GetDex() const override;
    
    /**
     * @brief Obtém a constituição da entidade
     * @return Constituição
     */
    virtual int GetCon() const override;
    
    /**
     * @brief Obtém a velocidade de movimento da entidade
     * @return Velocidade de movimento
     */
    virtual int GetMoveSpeed() const override;
    
    /**
     * @brief Obtém a velocidade de ataque da entidade
     * @return Velocidade de ataque
     */
    virtual int GetAttackSpeed() const override;
    
    /**
     * @brief Obtém o dano físico mínimo da entidade
     * @return Dano físico mínimo
     */
    virtual int GetPhysicalDamageMin() const override;
    
    /**
     * @brief Obtém o dano físico máximo da entidade
     * @return Dano físico máximo
     */
    virtual int GetPhysicalDamageMax() const override;
    
    /**
     * @brief Obtém o dano mágico mínimo da entidade
     * @return Dano mágico mínimo
     */
    virtual int GetMagicDamageMin() const override;
    
    /**
     * @brief Obtém o dano mágico máximo da entidade
     * @return Dano mágico máximo
     */
    virtual int GetMagicDamageMax() const override;
    
    /**
     * @brief Obtém a defesa física da entidade
     * @return Defesa física
     */
    virtual int GetPhysicalDefense() const override;
    
    /**
     * @brief Obtém a defesa mágica da entidade
     * @return Defesa mágica
     */
    virtual int GetMagicDefense() const override;
    
    /**
     * @brief Obtém a taxa de crítico da entidade
     * @return Taxa de crítico
     */
    virtual int GetCriticalRate() const override;
    
    /**
     * @brief Obtém a taxa de evasão da entidade
     * @return Taxa de evasão
     */
    virtual int GetEvasionRate() const override;
    
    /**
     * @brief Obtém a defesa contra elementos
     * @param element Índice do elemento (0-4)
     * @return Valor da defesa
     */
    virtual int GetElementalDefense(int element) const override;
    
    /**
     * @brief Obtém o alcance de ataque da entidade
     * @return Alcance de ataque
     */
    virtual int GetAttackRange() const override;
    
    /**
     * @brief Define o HP atual da entidade
     * @param hp Novo HP
     */
    virtual void SetHP(int hp) override;
    
    /**
     * @brief Define o MP atual da entidade
     * @param mp Novo MP
     */
    virtual void SetMP(int mp) override;
    
    /**
     * @brief Define a posição da entidade
     * @param x Posição X
     * @param y Posição Y
     * @param mapID ID do mapa (0 para não alterar)
     */
    virtual void SetPosition(WORD x, WORD y, WORD mapID = 0) override;
    
    /**
     * @brief Obtém o estado de animação da entidade
     * @return Estado de animação
     */
    virtual battle::AnimState GetAnimState() const override;
    
    /**
     * @brief Define o estado de animação da entidade
     * @param state Novo estado
     */
    virtual void SetAnimState(battle::AnimState state) override;
    
    /**
     * @brief Obtém o tipo de movimento da entidade
     * @return Tipo de movimento
     */
    virtual battle::MoveType GetMoveType() const override;
    
    /**
     * @brief Define o tipo de movimento da entidade
     * @param type Novo tipo
     */
    virtual void SetMoveType(battle::MoveType type) override;
    
    /**
     * @brief Processa a morte da entidade
     * @param killer Ponteiro para quem matou (nullptr para morte natural)
     */
    virtual void ProcessDeath(CEntity* killer = nullptr) override;
    
    /**
     * @brief Processa a ressurreição da entidade
     * @param resurrecter Ponteiro para quem ressuscitou (nullptr para ressurreição natural)
     * @param percentHP Porcentagem do HP máximo a restaurar (0-100)
     * @param percentMP Porcentagem do MP máximo a restaurar (0-100)
     * @return true se ressuscitou com sucesso
     */
    virtual bool ProcessResurrection(CEntity* resurrecter = nullptr, int percentHP = 100, int percentMP = 100) override;
    
    /**
     * @brief Atualiza o estado da entidade
     * @param elapsed Tempo decorrido desde a última atualização (ms)
     */
    virtual void Update(DWORD elapsed) override;
    
    //----- Métodos específicos de Mob -----//
    
    /**
     * @brief Obtém o ID do mob (template)
     * @return ID do mob
     */
    WORD GetMobID() const;
    
    /**
     * @brief Obtém o template do mob
     * @return Template do mob
     */
    const MobTemplate* GetTemplate() const;
    
    /**
     * @brief Define o template do mob
     * @param templ Template do mob
     */
    void SetTemplate(const MobTemplate* templ);
    
    /**
     * @brief Verifica se o mob é um NPC
     * @return true se for NPC
     */
    bool IsNPC() const;
    
    /**
     * @brief Verifica se o mob é um chefe
     * @return true se for chefe
     */
    bool IsBoss() const;
    
    /**
     * @brief Verifica se o mob é elite
     * @return true se for elite
     */
    bool IsElite() const;
    
    /**
     * @brief Verifica se o mob é agressivo
     * @return true se for agressivo
     */
    bool IsAggressive() const;
    
    /**
     * @brief Obtém o tipo de movimento do mob
     * @return Tipo de movimento
     */
    MovementType GetMovementType() const;
    
    /**
     * @brief Define o tipo de movimento do mob
     * @param type Tipo de movimento
     */
    void SetMovementType(MovementType type);
    
    /**
     * @brief Obtém o tipo de agressividade do mob
     * @return Tipo de agressividade
     */
    AggroType GetAggroType() const;
    
    /**
     * @brief Define o tipo de agressividade do mob
     * @param type Tipo de agressividade
     */
    void SetAggroType(AggroType type);
    
    /**
     * @brief Obtém o alcance de visão do mob
     * @return Alcance de visão
     */
    WORD GetSightRange() const;
    
    /**
     * @brief Define o alcance de visão do mob
     * @param range Alcance de visão
     */
    void SetSightRange(WORD range);
    
    /**
     * @brief Obtém a raça do mob
     * @return Raça do mob
     */
    MobRace GetRace() const;
    
    /**
     * @brief Obtém a classe do mob
     * @return Classe do mob
     */
    MobClass GetClass() const;
    
    /**
     * @brief Verifica se o mob pode ser montado
     * @return true se puder ser montado
     */
    bool CanBeMounted() const;
    
    /**
     * @brief Verifica se o mob aceita reflexo de dano
     * @return true se aceitar reflexo
     */
    bool AcceptsReflect() const;
    
    /**
     * @brief Verifica se o mob pode ser domado
     * @return true se puder ser domado
     */
    bool CanBeTamed() const;
    
    /**
     * @brief Verifica se o mob aparece em masmorras
     * @return true se aparecer em masmorras
     */
    bool AppearInDungeons() const;
    
    /**
     * @brief Verifica se o mob sofre knockback
     * @return true se sofrer knockback
     */
    bool CanBeKnockedBack() const;
    
    /**
     * @brief Verifica se o mob foge com HP baixo
     * @return true se fugir com HP baixo
     */
    bool RunsOnLowHealth() const;
    
    /**
     * @brief Verifica se o mob chama por ajuda
     * @return true se chamar por ajuda
     */
    bool CallsForHelp() const;
    
    /**
     * @brief Verifica se o mob invoca ao morrer
     * @return true se invocar ao morrer
     */
    bool SummonsOnDeath() const;
    
    /**
     * @brief Verifica se o mob dá experiência
     * @return true se der experiência
     */
    bool GivesExperience() const;
    
    /**
     * @brief Verifica se o mob dropa itens
     * @return true se dropar itens
     */
    bool DropsItems() const;
    
    /**
     * @brief Obtém o ID da tabela de drops
     * @return ID da tabela de drops
     */
    WORD GetDropTableID() const;
    
    /**
     * @brief Obtém o tamanho do mob
     * @return Tamanho do mob
     */
    BYTE GetSize() const;
    
    /**
     * @brief Define o grupo do mob
     * @param group Grupo do mob
     */
    void SetGroup(CMobGroup* group);
    
    /**
     * @brief Obtém o grupo do mob
     * @return Grupo do mob
     */
    CMobGroup* GetGroup() const;
    
    /**
     * @brief Define um alvo (aquisição de aggro)
     * @param entity Alvo
     * @param initialThreat Ameaça inicial
     * @return true se definido com sucesso
     */
    bool SetTarget(CEntity* entity, int initialThreat = 0);
    
    /**
     * @brief Obtém o alvo atual
     * @return Alvo atual
     */
    CEntity* GetTarget() const;
    
    /**
     * @brief Remove o alvo atual
     */
    void ClearTarget();
    
    /**
     * @brief Adiciona ameaça a uma entidade
     * @param entity Entidade
     * @param amount Quantidade de ameaça
     * @return true se adicionado com sucesso
     */
    bool AddThreat(CEntity* entity, int amount);
    
    /**
     * @brief Remove ameaça de uma entidade
     * @param entity Entidade
     * @param amount Quantidade de ameaça
     * @return true se removido com sucesso
     */
    bool RemoveThreat(CEntity* entity, int amount);
    
    /**
     * @brief Limpa toda a tabela de ameaças
     */
    void ClearThreat();
    
    /**
     * @brief Obtém a entidade com mais ameaça
     * @return Entidade com mais ameaça
     */
    CEntity* GetHighestThreatEntity() const;
    
    /**
     * @brief Obtém o nível de ameaça de uma entidade
     * @param entity Entidade
     * @return Nível de ameaça
     */
    int GetThreatLevel(CEntity* entity) const;
    
    /**
     * @brief Verifica se é hora de respawn
     * @param currentTime Tempo atual
     * @return true se for hora de respawn
     */
    bool IsTimeToRespawn(DWORD currentTime) const;
    
    /**
     * @brief Define o tempo de respawn
     * @param respawnTime Tempo de respawn
     */
    void SetRespawnTime(DWORD respawnTime);
    
    /**
     * @brief Obtém o tempo de respawn
     * @return Tempo de respawn
     */
    DWORD GetRespawnTime() const;
    
    /**
     * @brief Obtém o subtítulo do mob
     * @return Subtítulo
     */
    std::string GetSubtitle() const;
    
    /**
     * @brief Define o subtítulo do mob
     * @param subtitle Subtítulo
     */
    void SetSubtitle(const std::string& subtitle);
    
    /**
     * @brief Obtém o modelo do mob
     * @return ID do modelo
     */
    WORD GetModelID() const;
    
    /**
     * @brief Define o modelo do mob
     * @param modelID ID do modelo
     */
    void SetModelID(WORD modelID);
    
    /**
     * @brief Obtém o estado de spawn
     * @return Estado de spawn
     */
    SpawnedEntityStatus GetSpawnStatus() const;
    
    /**
     * @brief Define o estado de spawn
     * @param status Estado de spawn
     */
    void SetSpawnStatus(SpawnedEntityStatus status);
    
    /**
     * @brief Obtém a experiência do mob
     * @return Experiência
     */
    DWORD GetExperience() const;
    
    /**
     * @brief Obtém o ouro do mob
     * @return Ouro
     */
    DWORD GetGold() const;
    
    /**
     * @brief Obtém as skills do mob
     * @return Array de IDs de skills
     */
    std::array<WORD, 4> GetSkills() const;
    
    /**
     * @brief Verifica se o mob tem uma skill
     * @param skillID ID da skill
     * @return true se tiver
     */
    bool HasSkill(WORD skillID) const;
    
    /**
     * @brief Verifica se o mob pode usar uma skill
     * @param skillID ID da skill
     * @return true se puder usar
     */
    bool CanUseSkill(WORD skillID) const;
    
    /**
     * @brief Usa uma skill
     * @param skillID ID da skill
     * @param target Alvo
     * @return true se usou com sucesso
     */
    bool UseSkill(WORD skillID, CEntity* target);
    
    /**
     * @brief Define o AI do mob
     * @param ai AI do mob
     */
    void SetAI(std::shared_ptr<CMobAI> ai);
    
    /**
     * @brief Obtém o AI do mob
     * @return AI do mob
     */
    std::shared_ptr<CMobAI> GetAI() const;
    
    /**
     * @brief Processa um evento
     * @param event Evento
     * @param param Parâmetro adicional
     * @return true se processado com sucesso
     */
    bool ProcessEvent(EntityEvent event, void* param = nullptr);
    
    /**
     * @brief Registra um callback para eventos
     * @param callback Callback
     * @return ID do callback
     */
    int RegisterEventCallback(EntityEventCallback callback);
    
    /**
     * @brief Remove um callback
     * @param callbackID ID do callback
     * @return true se removido com sucesso
     */
    bool UnregisterEventCallback(int callbackID);
    
    /**
     * @brief Define a área de spawn
     * @param spawnPoint Ponto de spawn
     * @param radius Raio
     */
    void SetSpawnArea(const SpawnPoint& spawnPoint, WORD radius = 0);
    
    /**
     * @brief Obtém o ponto de spawn
     * @return Ponto de spawn
     */
    const SpawnPoint& GetSpawnPoint() const;
    
    /**
     * @brief Define o evento de mundo associado
     * @param eventID ID do evento
     */
    void SetWorldEvent(WORD eventID);
    
    /**
     * @brief Obtém o evento de mundo associado
     * @return ID do evento
     */
    WORD GetWorldEvent() const;
    
    /**
     * @brief Verifica se o mob está em um evento de mundo
     * @return true se estiver em um evento
     */
    bool IsInWorldEvent() const;
    
protected:
    /**
     * @brief Carrega dados do template
     * @param templ Template do mob
     */
    void LoadFromTemplate(const MobTemplate* templ);
    
    /**
     * @brief Calcula atributos derivados
     */
    void CalculateDerivedStats();
    
    /**
     * @brief Atualiza lógica de movimento
     * @param elapsed Tempo decorrido
     */
    void UpdateMovement(DWORD elapsed);
    
    /**
     * @brief Atualiza lógica de combate
     * @param elapsed Tempo decorrido
     */
    void UpdateCombat(DWORD elapsed);
    
    /**
     * @brief Atualiza lógica de AI
     * @param elapsed Tempo decorrido
     */
    void UpdateAI(DWORD elapsed);
    
    /**
     * @brief Notifica callbacks de evento
     * @param event Evento
     * @param param Parâmetro adicional
     */
    void NotifyEventCallbacks(EntityEvent event, const void* param);
    
    // Atributos do mob
    WORD mobID_;                         // ID do template do mob
    BYTE level_;                         // Nível do mob
    
    // Posição e status
    WORD mapID_;                         // ID do mapa
    WORD posX_;                          // Posição X
    WORD posY_;                          // Posição Y
    bool alive_;                         // Se está vivo
    
    // Atributos
    int currentHP_;                      // HP atual
    int maxHP_;                          // HP máximo
    int currentMP_;                      // MP atual
    int maxMP_;                          // MP máximo
    int str_;                            // Força
    int dex_;                            // Destreza
    int int_;                            // Inteligência
    int con_;                            // Constituição
    int moveSpeed_;                      // Velocidade de movimento
    int attackSpeed_;                    // Velocidade de ataque
    int physicalDamageMin_;              // Dano físico mínimo
    int physicalDamageMax_;              // Dano físico máximo
    int magicDamageMin_;                 // Dano mágico mínimo
    int magicDamageMax_;                 // Dano mágico máximo
    int physicalDefense_;                // Defesa física
    int magicDefense_;                   // Defesa mágica
    int criticalRate_;                   // Taxa de crítico
    int evasionRate_;                    // Taxa de evasão
    int elementalDefense_[5];            // Defesas elementais
    int attackRange_;                    // Alcance de ataque
    WORD sightRange_;                    // Alcance de visão
    BYTE size_;                          // Tamanho
    
    // Atributos do template
    std::string subtitle_;               // Subtítulo
    MobRace race_;                       // Raça
    MobClass class_;                     // Classe
    MovementType movementType_;          // Tipo de movimento
    AggroType aggroType_;                // Tipo de agressividade
    LootType lootType_;                  // Tipo de loot
    WORD modelID_;                       // ID do modelo
    std::array<WORD, 4> skills_;         // Skills
    WORD dropTableID_;                   // ID da tabela de drops
    DWORD experience_;                   // Experiência
    DWORD gold_;                         // Ouro
    BYTE flags_;                         // Flags diversas
    
    // Atributos de spawning
    SpawnPoint spawnPoint_;              // Ponto de spawn
    WORD spawnRadius_;                   // Raio de spawn
    DWORD respawnTime_;                  // Tempo de respawn
    DWORD deathTime_;                    // Timestamp da morte
    SpawnedEntityStatus spawnStatus_;    // Estado de spawn
    WORD worldEventID_;                  // ID do evento de mundo associado
    
    // Atributos de comportamento
    std::shared_ptr<CMobAI> ai_;         // AI do mob
    DWORD lastMoveTime_;                 // Timestamp do último movimento
    DWORD lastAttackTime_;               // Timestamp do último ataque
    DWORD lastSkillTime_;                // Timestamp da última skill
    battle::AnimState animState_;        // Estado de animação
    battle::MoveType moveType_;          // Tipo de movimento (de batalha)
    
    // Referências
    const MobTemplate* template_;        // Template do mob
    CMobGroup* group_;                   // Grupo do mob
    CEntity* target_;                    // Alvo atual
    
    // Tabela de ameaças
    std::unordered_map<DWORD, int> threatTable_; // Tabela de ameaças
    std::mutex threatMutex_;             // Mutex para tabela de ameaças
    
    // Efeitos
    struct Effect {
        battle::AffectType type;         // Tipo de efeito
        BYTE value;                      // Valor do efeito
        DWORD timestamp;                 // Timestamp de aplicação
        DWORD duration;                  // Duração em ms
        DWORD entityID;                  // ID de quem aplicou
    };
    std::vector<Effect> effects_;        // Lista de efeitos
    std::mutex effectsMutex_;            // Mutex para efeitos
    
    // Callbacks de evento
    std::unordered_map<int, EntityEventCallback> eventCallbacks_; // Callbacks
    std::mutex callbacksMutex_;          // Mutex para callbacks
    int nextCallbackID_;                 // Próximo ID de callback
};

} // namespace entity
} // namespace world
} // namespace server
} // namespace wyd

#endif // CMOB_H

} // namespace wydbr
