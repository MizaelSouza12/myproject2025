/**
 * NPC.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/npc/NPC.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef NPC_H
#define NPC_H

#include <string>
#include <vector>
#include <array>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <chrono>
#include <functional>
#include <queue>
#include <random>
#include <algorithm>
#include <bitset>

#include "../../../core/WYDTypes.h"
#include "../world/Position.h"
#include "NPCTypes.h"
#include "WYDAI.h"

namespace wydbr {


/**
 * @file NPC.h
 * @brief Implementação avançada do sistema de NPCs do WYD
 * 
 * Este arquivo implementa o sistema completo de NPCs do WYD, incluindo
 * vendors, monstros, bosses e NPCs de quests. A implementação mantém
 * a estrutura original do jogo, mas adiciona recursos avançados de AI
 * e comportamento para melhorar a jogabilidade.
 * 
 * O sistema implementa:
 * - Inteligência artificial avançada para combate
 * - Sistemas de spawn dinâmico e balanceamento
 * - Gerenciamento de quests e diálogos
 * - Comerciantes com inventário dinâmico
 * - Sistema de boss com padrões de ataque e fases
 */

namespace wyd {
namespace server {

// Forward declarations
class World;
class Zone;
class Player;
class Item;
class Quest;
class NPCSkill;
class NPCManager;

/**
 * @brief Classe que representa um NPC
 * 
 * Esta classe implementa toda a lógica de um NPC, incluindo
 * movimento, combate, interação com jogadores, etc.
 */
/**
 * Classe WYDNPC
 * 
 * Esta classe implementa a funcionalidade WYDNPC conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class NPC {
public:
    //=================================================================
    // Construtores/Destrutores
    //=================================================================
    
    /**
     * @brief Construtor
     * @param npcTemplate ID do template do NPC
     */
    NPC(DWORD npcTemplate);
    
    /**
     * @brief Destrutor
     */
    virtual ~NPC();
    
    /**
     * @brief Desativa o uso do construtor de cópia
     */
    NPC(const NPC&) = delete;
    
    /**
     * @brief Desativa o uso do operador de atribuição
     */
    NPC& operator=(const NPC&) = delete;
    
    //=================================================================
    // Getters/Setters básicos
    //=================================================================
    
    /**
     * @brief Obtém o ID do NPC
     * @return ID do NPC
     */
    DWORD GetID() const { return id_; }
    
    /**
     * @brief Obtém o ID do template do NPC
     * @return ID do template do NPC
     */
    DWORD GetTemplateID() const { return templateId_; }
    
    /**
     * @brief Obtém o nome do NPC
     * @return Nome do NPC
     */
    const char* GetName() const { return npcInfo_.name; }
    
    /**
     * @brief Obtém o tipo do NPC
     * @return Tipo do NPC
     */
    NPCType GetType() const { return npcInfo_.type; }
    
    /**
     * @brief Obtém o nível do NPC
     * @return Nível do NPC
     */
    WORD GetLevel() const { return npcInfo_.level; }
    
    /**
     * @brief Obtém a classe do NPC
     * @return Classe do NPC
     */
    NPCClass GetClass() const { return npcInfo_.npcClass; }
    
    /**
     * @brief Define o ID gerado para o NPC
     * @param id ID do NPC
     */
    void SetID(DWORD id) { id_ = id; }
    
    /**
     * @brief Verifica se o NPC é um monstro
     * @return true se o NPC é um monstro, false caso contrário
     */
    bool IsMonster() const { return npcInfo_.type == NPCType::MONSTER; }
    
    /**
     * @brief Verifica se o NPC é um NPC de quest
     * @return true se o NPC é um NPC de quest, false caso contrário
     */
    bool IsQuestNPC() const { return npcInfo_.type == NPCType::QUEST; }
    
    /**
     * @brief Verifica se o NPC é um vendor (comerciante)
     * @return true se o NPC é um vendor, false caso contrário
     */
    bool IsVendor() const { return npcInfo_.type == NPCType::VENDOR; }
    
    /**
     * @brief Verifica se o NPC é um boss
     * @return true se o NPC é um boss, false caso contrário
     */
    bool IsBoss() const { return npcInfo_.type == NPCType::BOSS; }
    
    /**
     * @brief Obtém a posição do NPC
     * @return Posição do NPC
     */
    Position GetPosition() const;
    
    /**
     * @brief Define a posição do NPC
     * @param x Posição X
     * @param y Posição Y
     * @param mapId ID do mapa (0 para manter o mesmo)
     * @param isTeleport Se é um teleporte (true) ou movimento normal (false)
     * @return true se a mudança de posição foi bem-sucedida, false caso contrário
     */
    bool SetPosition(WORD x, WORD y, WORD mapId = 0, bool isTeleport = false);
    
    //=================================================================
    // Métodos de status e atributos
    //=================================================================
    
    /**
     * @brief Obtém o HP atual do NPC
     * @return HP atual
     */
    DWORD GetHP() const { return currentHP_; }
    
    /**
     * @brief Obtém o HP máximo do NPC
     * @return HP máximo
     */
    DWORD GetMaxHP() const { return npcInfo_.maxHP; }
    
    /**
     * @brief Obtém o MP atual do NPC
     * @return MP atual
     */
    DWORD GetMP() const { return currentMP_; }
    
    /**
     * @brief Obtém o MP máximo do NPC
     * @return MP máximo
     */
    DWORD GetMaxMP() const { return npcInfo_.maxMP; }
    
    /**
     * @brief Altera o HP do NPC
     * @param amount Quantidade a alterar (positiva para cura, negativa para dano)
     * @param sourceId ID da fonte (jogador, NPC ou habilidade)
     * @param critical Se o dano/cura é crítico
     * @return Quantidade real alterada
     */
    INT ChangeHP(INT amount, DWORD sourceId = 0, bool critical = false);
    
    /**
     * @brief Altera o MP do NPC
     * @param amount Quantidade a alterar (positiva para recuperação, negativa para consumo)
     * @param sourceId ID da fonte (jogador, NPC ou habilidade)
     * @return Quantidade real alterada
     */
    INT ChangeMP(INT amount, DWORD sourceId = 0);
    
    /**
     * @brief Verifica se o NPC está morto
     * @return true se o NPC está morto, false caso contrário
     */
    bool IsDead() const { return currentHP_ == 0; }
    
    /**
     * @brief Revive o NPC
     * @param fullHP Se deve reviver com HP cheio (true) ou parcial (false)
     * @return true se o NPC foi revivido, false caso contrário
     */
    bool Revive(bool fullHP = true);
    
    /**
     * @brief Mata o NPC
     * @param killerId ID do matador (0 para morte natural)
     * @return true se o NPC foi morto, false caso contrário
     */
    bool Kill(DWORD killerId = 0);
    
    /**
     * @brief Verifica se o NPC é agressivo
     * @return true se o NPC é agressivo, false caso contrário
     */
    bool IsAggressive() const { return npcInfo_.isAggressive; }
    
    /**
     * @brief Define se o NPC é agressivo
     * @param aggressive Se o NPC é agressivo
     */
    void SetAggressive(bool aggressive) { npcInfo_.isAggressive = aggressive; }
    
    /**
     * @brief Verifica se o NPC é invulnerável
     * @return true se o NPC é invulnerável, false caso contrário
     */
    bool IsInvulnerable() const { return isInvulnerable_; }
    
    /**
     * @brief Define se o NPC é invulnerável
     * @param invulnerable Se o NPC é invulnerável
     */
    void SetInvulnerable(bool invulnerable) { isInvulnerable_ = invulnerable; }
    
    //=================================================================
    // Métodos de combate
    //=================================================================
    
    /**
     * @brief Ataca um alvo
     * @param targetId ID do alvo
     * @param skillId ID da habilidade (0 para ataque normal)
     * @return true se o ataque foi iniciado, false caso contrário
     */
    bool Attack(DWORD targetId, WORD skillId = 0);
    
    /**
     * @brief Calcula o dano contra um alvo
     * @param target Ponteiro para o alvo (Jogador ou NPC)
     * @param skillId ID da habilidade (0 para ataque normal)
     * @param criticalOut Retorna se o ataque foi crítico (por referência)
     * @return Dano calculado
     */
    WORD CalculateDamage(void* target, WORD skillId, bool& criticalOut);
    
    /**
     * @brief Recebe dano de uma fonte
     * @param damage Dano a receber
     * @param sourceId ID da fonte do dano
     * @param damageType Tipo de dano (0=físico, 1=mágico, 2=true damage)
     * @param critical Se o dano é crítico
     * @return Dano real recebido após modificadores
     */
    WORD TakeDamage(WORD damage, DWORD sourceId, BYTE damageType = 0, bool critical = false);
    
    /**
     * @brief Adiciona ameaça de um jogador
     * @param playerId ID do jogador
     * @param amount Quantidade de ameaça a adicionar
     */
    void AddThreat(DWORD playerId, float amount);
    
    /**
     * @brief Remove ameaça de um jogador
     * @param playerId ID do jogador
     * @param amount Quantidade de ameaça a remover (0 para remover toda ameaça)
     */
    void RemoveThreat(DWORD playerId, float amount = 0);
    
    /**
     * @brief Obtém a tabela de ameaça
     * @return Tabela de ameaça (playerId -> ameaça)
     */
    const std::unordered_map<DWORD, float>& GetThreatTable() const { return threatTable_; }
    
    /**
     * @brief Obtém o alvo com maior ameaça
     * @return ID do alvo com maior ameaça (0 se nenhum)
     */
    DWORD GetHighestThreatTarget() const;
    
    /**
     * @brief Limpa a tabela de ameaça
     */
    void ClearThreatTable();
    
    /**
     * @brief Verifica se o NPC está em combate
     * @return true se o NPC está em combate, false caso contrário
     */
    bool IsInCombat() const { return inCombat_; }
    
    /**
     * @brief Entra em estado de combate
     * @param combatTime Tempo de combate em milissegundos (0 para usar default)
     */
    void EnterCombat(DWORD combatTime = 0);
    
    /**
     * @brief Sai do estado de combate
     * @param force Se deve forçar a saída de combate
     */
    void LeaveCombat(bool force = false);
    
    //=================================================================
    // Métodos de habilidades
    //=================================================================
    
    /**
     * @brief Obtém as habilidades do NPC
     * @return Lista de habilidades do NPC
     */
    const std::vector<NPCSkill>& GetSkills() const { return skills_; }
    
    /**
     * @brief Adiciona uma habilidade ao NPC
     * @param skillId ID da habilidade
     * @param level Nível da habilidade
     * @return true se a habilidade foi adicionada, false caso contrário
     */
    bool AddSkill(WORD skillId, BYTE level = 1);
    
    /**
     * @brief Remove uma habilidade do NPC
     * @param skillId ID da habilidade
     * @return true se a habilidade foi removida, false caso contrário
     */
    bool RemoveSkill(WORD skillId);
    
    /**
     * @brief Usa uma habilidade
     * @param skillId ID da habilidade
     * @param targetId ID do alvo (jogador, NPC, 0 para auto-alvo ou sem alvo)
     * @param targetX Posição X alvo (para habilidades baseadas em área)
     * @param targetY Posição Y alvo (para habilidades baseadas em área)
     * @return true se a habilidade foi usada, false caso contrário
     */
    bool UseSkill(WORD skillId, DWORD targetId = 0, WORD targetX = 0, WORD targetY = 0);
    
    /**
     * @brief Verifica se o NPC tem uma habilidade
     * @param skillId ID da habilidade
     * @param minLevel Nível mínimo da habilidade
     * @return true se o NPC tem a habilidade, false caso contrário
     */
    bool HasSkill(WORD skillId, BYTE minLevel = 1) const;
    
    /**
     * @brief Verifica se uma habilidade está em cooldown
     * @param skillId ID da habilidade
     * @return true se a habilidade está em cooldown, false caso contrário
     */
    bool IsSkillOnCooldown(WORD skillId) const;
    
    /**
     * @brief Obtém o tempo restante de cooldown de uma habilidade
     * @param skillId ID da habilidade
     * @return Tempo restante em milissegundos (0 se não estiver em cooldown)
     */
    DWORD GetSkillCooldown(WORD skillId) const;
    
    /**
     * @brief Redefine o cooldown de uma habilidade
     * @param skillId ID da habilidade (0 para todas as habilidades)
     */
    void ResetSkillCooldown(WORD skillId = 0);
    
    /**
     * @brief Escolhe uma habilidade para usar com base na situação atual
     * @return ID da habilidade escolhida (0 se nenhuma habilidade for apropriada)
     */
    WORD ChooseSkill();
    
    //=================================================================
    // Métodos de spawn e respawn
    //=================================================================
    
    /**
     * @brief Inicia o NPC (spawn)
     * @param mapId ID do mapa
     * @param x Posição X
     * @param y Posição Y
     * @param respawnDelay Atraso para respawn em milissegundos
     * @return true se o NPC foi iniciado, false caso contrário
     */
    bool Spawn(WORD mapId, WORD x, WORD y, DWORD respawnDelay = 0);
    
    /**
     * @brief Faz o respawn do NPC
     * @param forcePosition Se deve forçar a posição de spawn original
     * @return true se o respawn foi bem-sucedido, false caso contrário
     */
    bool Respawn(bool forcePosition = false);
    
    /**
     * @brief Define o atraso para respawn
     * @param delay Atraso em milissegundos
     */
    void SetRespawnDelay(DWORD delay) { respawnDelay_ = delay; }
    
    /**
     * @brief Obtém o atraso para respawn
     * @return Atraso para respawn em milissegundos
     */
    DWORD GetRespawnDelay() const { return respawnDelay_; }
    
    /**
     * @brief Obtém o timestamp de respawn
     * @return Timestamp de respawn (0 se não estiver morto)
     */
    DWORD GetRespawnTime() const { return respawnTime_; }
    
    /**
     * @brief Verifica se o NPC está pronto para respawn
     * @param currentTime Timestamp atual
     * @return true se o NPC está pronto para respawn, false caso contrário
     */
    bool IsReadyToRespawn(DWORD currentTime) const;
    
    //=================================================================
    // Métodos de movimento e AI
    //=================================================================
    
    /**
     * @brief Move o NPC para uma posição
     * @param targetX Posição X alvo
     * @param targetY Posição Y alvo
     * @return true se o movimento foi iniciado, false caso contrário
     */
    bool MoveTo(WORD targetX, WORD targetY);
    
    /**
     * @brief Teleporta o NPC para uma posição
     * @param targetX Posição X alvo
     * @param targetY Posição Y alvo
     * @param mapId ID do mapa (0 para manter o mesmo)
     * @return true se o teleporte foi bem-sucedido, false caso contrário
     */
    bool TeleportTo(WORD targetX, WORD targetY, WORD mapId = 0);
    
    /**
     * @brief Define o caminho de patrulha
     * @param path Lista de posições a percorrer
     * @param repeat Se deve repetir o patrulhamento
     */
    void SetPatrolPath(const std::vector<Position>& path, bool repeat = true);
    
    /**
     * @brief Move o NPC para o próximo ponto de patrulha
     * @return true se o movimento foi iniciado, false caso contrário
     */
    bool MoveToNextPatrolPoint();
    
    /**
     * @brief Define o raio de movimento aleatório
     * @param radius Raio em tiles
     */
    void SetRandomMovementRadius(WORD radius) { randomMovementRadius_ = radius; }
    
    /**
     * @brief Executa um movimento aleatório
     * @return true se o movimento foi iniciado, false caso contrário
     */
    bool MoveRandomly();
    
    /**
     * @brief Define o estado de AI do NPC
     * @param state Estado de AI
     */
    void SetAIState(AIState state);
    
    /**
     * @brief Obtém o estado de AI do NPC
     * @return Estado de AI
     */
    AIState GetAIState() const { return aiState_; }
    
    /**
     * @brief Define o alvo atual
     * @param targetId ID do alvo
     */
    void SetTarget(DWORD targetId);
    
    /**
     * @brief Obtém o alvo atual
     * @return ID do alvo atual
     */
    DWORD GetTarget() const { return targetId_; }
    
    /**
     * @brief Verifica se um jogador está dentro do raio de agressão
     * @param playerId ID do jogador
     * @return true se o jogador está dentro do raio de agressão, false caso contrário
     */
    bool IsPlayerInAggroRange(DWORD playerId) const;
    
    /**
     * @brief Define o raio de agressão
     * @param radius Raio em tiles
     */
    void SetAggroRadius(WORD radius) { aggroRadius_ = radius; }
    
    /**
     * @brief Obtém o raio de agressão
     * @return Raio de agressão em tiles
     */
    WORD GetAggroRadius() const { return aggroRadius_; }
    
    /**
     * @brief Define o raio de assistência
     * @param radius Raio em tiles
     */
    void SetAssistRadius(WORD radius) { assistRadius_ = radius; }
    
    /**
     * @brief Obtém o raio de assistência
     * @return Raio de assistência em tiles
     */
    WORD GetAssistRadius() const { return assistRadius_; }
    
    /**
     * @brief Define o raio de fuga
     * @param radius Raio em tiles
     */
    void SetLeashRadius(WORD radius) { leashRadius_ = radius; }
    
    /**
     * @brief Obtém o raio de fuga
     * @return Raio de fuga em tiles
     */
    WORD GetLeashRadius() const { return leashRadius_; }
    
    /**
     * @brief Determina se o NPC deve fugir por saúde baixa
     * @param healthPercent Percentual de saúde para fugir
     */
    void SetFleeHealthPercent(BYTE healthPercent) { fleeHealthPercent_ = healthPercent; }
    
    /**
     * @brief Obtém o percentual de saúde para fugir
     * @return Percentual de saúde para fugir
     */
    BYTE GetFleeHealthPercent() const { return fleeHealthPercent_; }
    
    /**
     * @brief Verifica se o NPC deve solicitar ajuda quando atacado
     * @return true se o NPC deve solicitar ajuda, false caso contrário
     */
    bool ShouldCallForHelp() const { return npcInfo_.callForHelp; }
    
    /**
     * @brief Define se o NPC deve solicitar ajuda quando atacado
     * @param callForHelp Se o NPC deve solicitar ajuda
     */
    void SetCallForHelp(bool callForHelp) { npcInfo_.callForHelp = callForHelp; }
    
    //=================================================================
    // Métodos de interação com jogadores
    //=================================================================
    
    /**
     * @brief Inicia um diálogo com um jogador
     * @param playerId ID do jogador
     * @param dialogId ID do diálogo (0 para diálogo inicial)
     * @return true se o diálogo foi iniciado, false caso contrário
     */
    bool StartDialog(DWORD playerId, WORD dialogId = 0);
    
    /**
     * @brief Processa uma resposta de diálogo
     * @param playerId ID do jogador
     * @param responseId ID da resposta
     * @return true se a resposta foi processada, false caso contrário
     */
    bool ProcessDialogResponse(DWORD playerId, WORD responseId);
    
    /**
     * @brief Inicia um comércio com um jogador
     * @param playerId ID do jogador
     * @return true se o comércio foi iniciado, false caso contrário
     */
    bool StartTrade(DWORD playerId);
    
    /**
     * @brief Processa uma compra de um jogador
     * @param playerId ID do jogador
     * @param itemId ID do item a comprar
     * @param quantity Quantidade a comprar
     * @return true se a compra foi processada, false caso contrário
     */
    bool ProcessPurchase(DWORD playerId, WORD itemId, WORD quantity = 1);
    
    /**
     * @brief Processa uma venda de um jogador
     * @param playerId ID do jogador
     * @param inventorySlot Slot do inventário do jogador
     * @param quantity Quantidade a vender
     * @return true se a venda foi processada, false caso contrário
     */
    bool ProcessSale(DWORD playerId, BYTE inventorySlot, WORD quantity = 1);
    
    /**
     * @brief Inicia uma quest para um jogador
     * @param playerId ID do jogador
     * @param questId ID da quest
     * @return true se a quest foi iniciada, false caso contrário
     */
    bool StartQuest(DWORD playerId, WORD questId);
    
    /**
     * @brief Completa uma quest para um jogador
     * @param playerId ID do jogador
     * @param questId ID da quest
     * @param rewardChoice Escolha de recompensa
     * @return true se a quest foi completada, false caso contrário
     */
    bool CompleteQuest(DWORD playerId, WORD questId, BYTE rewardChoice = 0);
    
    /**
     * @brief Verifica se o NPC aceita uma determinada quest
     * @param questId ID da quest
     * @return true se o NPC aceita a quest, false caso contrário
     */
    bool AcceptsQuest(WORD questId) const;
    
    /**
     * @brief Verifica se o NPC completa uma determinada quest
     * @param questId ID da quest
     * @return true se o NPC completa a quest, false caso contrário
     */
    bool CompletesQuest(WORD questId) const;
    
    /**
     * @brief Obtém a lista de quests que o NPC oferece
     * @return Lista de IDs de quests
     */
    const std::vector<WORD>& GetQuestList() const { return questList_; }
    
    //=================================================================
    // Métodos para vendors (comerciantes)
    //=================================================================
    
    /**
     * @brief Adiciona um item ao inventário do vendor
     * @param itemId ID do item
     * @param price Preço de venda (0 para preço padrão)
     * @param quantity Quantidade disponível (0 para ilimitado)
     * @param refineLevel Nível de refinamento (0-15)
     * @return true se o item foi adicionado, false caso contrário
     */
    bool AddVendorItem(WORD itemId, DWORD price = 0, WORD quantity = 0, BYTE refineLevel = 0);
    
    /**
     * @brief Remove um item do inventário do vendor
     * @param itemId ID do item
     * @return true se o item foi removido, false caso contrário
     */
    bool RemoveVendorItem(WORD itemId);
    
    /**
     * @brief Obtém o inventário do vendor
     * @return Lista de itens do vendor
     */
    const std::vector<VendorItem>& GetVendorInventory() const { return vendorInventory_; }
    
    /**
     * @brief Define o multiplicador de preços do vendor
     * @param multiplier Multiplicador (1.0 para preço normal)
     */
    void SetPriceMultiplier(float multiplier) { priceMultiplier_ = multiplier; }
    
    /**
     * @brief Obtém o multiplicador de preços do vendor
     * @return Multiplicador de preços
     */
    float GetPriceMultiplier() const { return priceMultiplier_; }
    
    /**
     * @brief Limpa o inventário do vendor
     */
    void ClearVendorInventory() { vendorInventory_.clear(); }
    
    //=================================================================
    // Métodos para drops
    //=================================================================
    
    /**
     * @brief Adiciona um item à tabela de drops
     * @param itemId ID do item
     * @param chance Chance de drop (0.0-1.0)
     * @param minAmount Quantidade mínima
     * @param maxAmount Quantidade máxima
     * @param refineLevel Nível de refinamento (0-15)
     * @return true se o item foi adicionado, false caso contrário
     */
    bool AddDropItem(WORD itemId, float chance, WORD minAmount = 1, WORD maxAmount = 1, BYTE refineLevel = 0);
    
    /**
     * @brief Remove um item da tabela de drops
     * @param itemId ID do item
     * @return true se o item foi removido, false caso contrário
     */
    bool RemoveDropItem(WORD itemId);
    
    /**
     * @brief Obtém a tabela de drops
     * @return Lista de drops
     */
    const std::vector<DropItem>& GetDropTable() const { return dropTable_; }
    
    /**
     * @brief Define a quantidade de ouro que o NPC dropa
     * @param minGold Quantidade mínima de ouro
     * @param maxGold Quantidade máxima de ouro
     */
    void SetGoldDrop(DWORD minGold, DWORD maxGold) {
        minGoldDrop_ = minGold;
        maxGoldDrop_ = maxGold;
    }
    
    /**
     * @brief Obtém a quantidade mínima de ouro que o NPC dropa
     * @return Quantidade mínima de ouro
     */
    DWORD GetMinGoldDrop() const { return minGoldDrop_; }
    
    /**
     * @brief Obtém a quantidade máxima de ouro que o NPC dropa
     * @return Quantidade máxima de ouro
     */
    DWORD GetMaxGoldDrop() const { return maxGoldDrop_; }
    
    /**
     * @brief Gera drops na morte
     * @param killerId ID do matador
     */
    void GenerateDrops(DWORD killerId);
    
    //=================================================================
    // Métodos para bosses
    //=================================================================
    
    /**
     * @brief Verifica se o NPC é um boss
     * @return true se o NPC é um boss, false caso contrário
     */
    bool IsBoss() const { return npcInfo_.type == NPCType::BOSS; }
    
    /**
     * @brief Define o comportamento de boss
     * @param bossSettings Configurações de boss
     */
    void SetBossBehavior(const BossSettings& bossSettings);
    
    /**
     * @brief Obtém as configurações de boss
     * @return Configurações de boss
     */
    const BossSettings& GetBossSettings() const { return bossSettings_; }
    
    /**
     * @brief Define a fase atual do boss
     * @param phase Fase (1-3)
     */
    void SetBossPhase(BYTE phase);
    
    /**
     * @brief Obtém a fase atual do boss
     * @return Fase atual
     */
    BYTE GetBossPhase() const { return currentBossPhase_; }
    
    /**
     * @brief Executa uma habilidade específica de fase de boss
     * @return true se a habilidade foi executada, false caso contrário
     */
    bool CastBossPhaseAbility();
    
    //=================================================================
    // Métodos de notificação de evento
    //=================================================================
    
    /**
     * @brief Processa evento de morte
     * @param killerId ID do matador
     */
    void OnDeath(DWORD killerId);
    
    /**
     * @brief Processa evento de spawn
     */
    void OnSpawn();
    
    /**
     * @brief Processa evento de dano recebido
     * @param damage Dano recebido
     * @param attackerId ID do atacante
     */
    void OnDamaged(WORD damage, DWORD attackerId);
    
    /**
     * @brief Processa evento de ataque realizado
     * @param targetId ID do alvo
     * @param damage Dano causado
     */
    void OnAttacked(DWORD targetId, WORD damage);
    
    /**
     * @brief Processa evento de entrada em combate
     * @param enemyId ID do inimigo
     */
    void OnCombatStart(DWORD enemyId);
    
    /**
     * @brief Processa evento de saída de combate
     */
    void OnCombatEnd();
    
    /**
     * @brief Processa evento de interação
     * @param playerId ID do jogador
     * @param interactionType Tipo de interação
     */
    void OnInteraction(DWORD playerId, BYTE interactionType);
    
    //=================================================================
    // Métodos de atualização
    //=================================================================
    
    /**
     * @brief Atualiza o NPC
     * @param deltaTime Tempo desde a última atualização em milissegundos
     */
    void Update(DWORD deltaTime);
    
    /**
     * @brief Inicializa o NPC com dados do template
     * @param npcTemplate Template de NPC
     * @return true se o NPC foi inicializado, false caso contrário
     */
    bool Initialize(const NPCTemplate& npcTemplate);
    
    /**
     * @brief Verifica se o NPC está pronto para atualização
     * @return true se o NPC está pronto para atualização, false caso contrário
     */
    bool IsActive() const { return isActive_; }
    
    /**
     * @brief Define se o NPC está ativo
     * @param active Se o NPC está ativo
     */
    void SetActive(bool active) { isActive_ = active; }
    
    /**
     * @brief Verifica se o NPC está visível para players
     * @return true se o NPC está visível, false caso contrário
     */
    bool IsVisible() const { return isVisible_; }
    
    /**
     * @brief Define se o NPC está visível
     * @param visible Se o NPC está visível
     */
    void SetVisible(bool visible) { isVisible_ = visible; }
    
private:
    //=================================================================
    // Atributos privados
    //=================================================================
    
    DWORD id_;                              // ID único do NPC
    DWORD templateId_;                      // ID do template do NPC
    NPCInfo npcInfo_;                       // Informações básicas do NPC
    
    WORD currentMapId_;                     // ID do mapa atual
    WORD posX_;                             // Posição X atual
    WORD posY_;                             // Posição Y atual
    WORD spawnMapId_;                       // ID do mapa de spawn
    WORD spawnX_;                           // Posição X de spawn
    WORD spawnY_;                           // Posição Y de spawn
    
    DWORD currentHP_;                       // HP atual
    DWORD currentMP_;                       // MP atual
    
    // Status e flags
    bool isActive_;                         // Se o NPC está ativo
    bool isVisible_;                        // Se o NPC está visível
    bool inCombat_;                         // Se está em combate
    bool isMoving_;                         // Se está em movimento
    bool isInvulnerable_;                   // Se é invulnerável
    bool isRooted_;                         // Se está enraizado (não pode mover)
    bool isSilenced_;                       // Se está silenciado (não pode usar skills)
    bool isStunned_;                        // Se está atordoado
    DWORD firstAggro_;                      // ID do primeiro jogador a causar agressão
    
    // Controle de movimento e AI
    AIState aiState_;                       // Estado atual da AI
    std::vector<Position> patrolPath_;      // Caminho de patrulha
    INT currentPatrolPoint_;                // Índice do ponto atual de patrulha
    bool repeatPatrol_;                     // Se deve repetir o patrulhamento
    WORD randomMovementRadius_;             // Raio de movimento aleatório
    DWORD lastMoveTime_;                    // Timestamp do último movimento
    DWORD moveCooldown_;                    // Cooldown de movimento
    DWORD targetId_;                        // ID do alvo atual
    WORD aggroRadius_;                      // Raio de agressão
    WORD assistRadius_;                     // Raio de assistência
    WORD leashRadius_;                      // Raio de fuga
    BYTE fleeHealthPercent_;                // Percentual de HP para fugir
    
    // Controle de combate
    std::unordered_map<DWORD, float> threatTable_;  // Tabela de ameaça
    std::unordered_map<WORD, DWORD> skillCooldowns_; // Cooldowns de habilidades
    std::vector<NPCSkill> skills_;          // Habilidades do NPC
    
    // Controle de respawn
    DWORD deathTime_;                       // Timestamp da morte
    DWORD respawnTime_;                     // Timestamp do respawn
    DWORD respawnDelay_;                    // Atraso para respawn
    
    // Controle de quests e diálogos
    std::vector<WORD> questList_;           // Lista de quests que o NPC oferece
    std::unordered_map<WORD, WORD> dialogMap_; // Mapa de diálogos
    
    // Controle de vendor
    std::vector<VendorItem> vendorInventory_; // Inventário do vendor
    float priceMultiplier_;                 // Multiplicador de preços
    
    // Controle de drops
    std::vector<DropItem> dropTable_;       // Tabela de drops
    DWORD minGoldDrop_;                     // Quantidade mínima de ouro
    DWORD maxGoldDrop_;                     // Quantidade máxima de ouro
    
    // Controle de boss
    BossSettings bossSettings_;             // Configurações de boss
    BYTE currentBossPhase_;                 // Fase atual do boss (1-3)
    
    // Sistema avançado de AI
    std::unique_ptr<AIController> aiController_; // Controlador de AI
    
    // Timestamp da última atualização
    DWORD lastUpdateTime_;                  // Timestamp da última atualização
    
    // Gerador de números aleatórios
    std::mt19937 rng_;                      // Gerador de números aleatórios
    
    // Mutex para thread safety
    mutable std::mutex mutex_;              // Mutex para thread safety
    
    //=================================================================
    // Métodos privados
    //=================================================================
    
    // Métodos de AI
    /**
 * UpdateAI
 * 
 * Implementa a funcionalidade UpdateAI conforme especificação original.
 * @param deltaTime Parâmetro deltaTime
 */

    void UpdateAI(DWORD deltaTime);
    /**
 * UpdateAIIdle
 * 
 * Implementa a funcionalidade UpdateAIIdle conforme especificação original.
 * @param deltaTime Parâmetro deltaTime
 */

    void UpdateAIIdle(DWORD deltaTime);
    /**
 * UpdateAIPatrol
 * 
 * Implementa a funcionalidade UpdateAIPatrol conforme especificação original.
 * @param deltaTime Parâmetro deltaTime
 */

    void UpdateAIPatrol(DWORD deltaTime);
    /**
 * UpdateAICombat
 * 
 * Implementa a funcionalidade UpdateAICombat conforme especificação original.
 * @param deltaTime Parâmetro deltaTime
 */

    void UpdateAICombat(DWORD deltaTime);
    /**
 * UpdateAIEvade
 * 
 * Implementa a funcionalidade UpdateAIEvade conforme especificação original.
 * @param deltaTime Parâmetro deltaTime
 */

    void UpdateAIEvade(DWORD deltaTime);
    /**
 * UpdateAIFlee
 * 
 * Implementa a funcionalidade UpdateAIFlee conforme especificação original.
 * @param deltaTime Parâmetro deltaTime
 */

    void UpdateAIFlee(DWORD deltaTime);
    /**
 * UpdateAIFollow
 * 
 * Implementa a funcionalidade UpdateAIFollow conforme especificação original.
 * @param deltaTime Parâmetro deltaTime
 */

    void UpdateAIFollow(DWORD deltaTime);
    
    // Métodos auxiliares
    /**
 * SelectAppropriateCombatAction
 * 
 * Implementa a funcionalidade SelectAppropriateCombatAction conforme especificação original.
 * @return Retorna bool
 */

    bool SelectAppropriateCombatAction();
    /**
 * CheckForEnemiesInRange
 * 
 * Implementa a funcionalidade CheckForEnemiesInRange conforme especificação original.
 * @return Retorna bool
 */

    bool CheckForEnemiesInRange();
    /**
 * ShouldEvadeFromCombat
 * 
 * Implementa a funcionalidade ShouldEvadeFromCombat conforme especificação original.
 * @return Retorna bool
 */

    bool ShouldEvadeFromCombat();
    /**
 * ShouldFlee
 * 
 * Implementa a funcionalidade ShouldFlee conforme especificação original.
 * @return Retorna bool
 */

    bool ShouldFlee();
    /**
 * CanSeePlayer
 * 
 * Implementa a funcionalidade CanSeePlayer conforme especificação original.
 * @param playerId Parâmetro playerId
 * @return Retorna bool
 */

    bool CanSeePlayer(DWORD playerId) const;
    /**
 * GetPlayerPtr
 * 
 * Implementa a funcionalidade GetPlayerPtr conforme especificação original.
 * @param playerId Parâmetro playerId
 * @return Retorna Player
 */

    Player* GetPlayerPtr(DWORD playerId) const;
    
    // Utilitários
    /**
 * GetTickCount
 * 
 * Implementa a funcionalidade GetTickCount conforme especificação original.
 * @return Retorna DWORD
 */

    DWORD GetTickCount() const;
    /**
 * GetRandomInt
 * 
 * Implementa a funcionalidade GetRandomInt conforme especificação original.
 * @param min Parâmetro min
 * @param max Parâmetro max
 * @return Retorna INT
 */

    INT GetRandomInt(INT min, INT max);
    /**
 * GetRandomFloat
 * 
 * Implementa a funcionalidade GetRandomFloat conforme especificação original.
 * @param min Parâmetro min
 * @param max Parâmetro max
 * @return Retorna float
 */

    float GetRandomFloat(float min, float max);
    /**
 * CalculateDirection
 * 
 * Implementa a funcionalidade CalculateDirection conforme especificação original.
 * @param startX Parâmetro startX
 * @param startY Parâmetro startY
 * @param endX Parâmetro endX
 * @param endY Parâmetro endY
 * @param direction Parâmetro direction
 * @param distance Parâmetro distance
 */

    void CalculateDirection(WORD startX, WORD startY, WORD endX, WORD endY, BYTE& direction, float& distance);
    /**
 * IsValidPosition
 * 
 * Implementa a funcionalidade IsValidPosition conforme especificação original.
 * @param mapId Parâmetro mapId
 * @param x Parâmetro x
 * @param y Parâmetro y
 * @return Retorna bool
 */

    bool IsValidPosition(WORD mapId, WORD x, WORD y) const;
};

/**
 * @brief Gerenciador de NPCs
 * 
 * Esta classe gerencia todos os NPCs no servidor, fornecendo
 * métodos para busca, criação, atualização e remoção de NPCs.
 */
/**
 * Classe WYDNPCManager
 * 
 * Esta classe implementa a funcionalidade WYDNPCManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class NPCManager {
public:
    /**
     * @brief Construtor
     */
    NPCManager();
    
    /**
     * @brief Destrutor
     */
    ~NPCManager();
    
    /**
     * @brief Inicializa o gerenciador de NPCs
     * @param config Configuração do sistema de NPCs
     * @return true se a inicialização foi bem-sucedida, false caso contrário
     */
    bool Initialize(const NPCConfig& config);
    
    /**
     * @brief Finaliza o gerenciador de NPCs
     */
    void Shutdown();
    
    /**
     * @brief Atualiza todos os NPCs
     * @param deltaTime Tempo desde a última atualização em milissegundos
     */
    void Update(DWORD deltaTime);
    
    /**
     * @brief Cria um NPC
     * @param templateId ID do template do NPC
     * @param mapId ID do mapa
     * @param x Posição X
     * @param y Posição Y
     * @param respawnDelay Atraso para respawn em milissegundos
     * @return Ponteiro para o NPC criado, nullptr em caso de falha
     */
    NPC* CreateNPC(DWORD templateId, WORD mapId, WORD x, WORD y, DWORD respawnDelay = 0);
    
    /**
     * @brief Remove um NPC
     * @param npcId ID do NPC
     * @param permanent Se o NPC deve ser removido permanentemente (sem respawn)
     * @return true se o NPC foi removido, false caso contrário
     */
    bool RemoveNPC(DWORD npcId, bool permanent = false);
    
    /**
     * @brief Busca um NPC pelo ID
     * @param npcId ID do NPC
     * @return Ponteiro para o NPC, nullptr se não encontrado
     */
    NPC* GetNPC(DWORD npcId);
    
    /**
     * @brief Busca NPCs por template
     * @param templateId ID do template
     * @return Lista de ponteiros para NPCs
     */
    std::vector<NPC*> GetNPCsByTemplate(DWORD templateId);
    
    /**
     * @brief Busca NPCs em uma área
     * @param mapId ID do mapa
     * @param centerX Posição X central
     * @param centerY Posição Y central
     * @param radius Raio da busca
     * @return Lista de ponteiros para NPCs
     */
    std::vector<NPC*> GetNPCsInArea(WORD mapId, WORD centerX, WORD centerY, WORD radius);
    
    /**
     * @brief Obtém o número de NPCs
     * @return Número de NPCs
     */
    size_t GetNPCCount() const { return npcs_.size(); }
    
    /**
     * @brief Carrega templates de NPC de um arquivo
     * @param filename Nome do arquivo
     * @return true se os templates foram carregados, false caso contrário
     */
    bool LoadNPCTemplates(const std::string& filename);
    
    /**
     * @brief Carrega spawns de NPC de um arquivo
     * @param filename Nome do arquivo
     * @return true se os spawns foram carregados, false caso contrário
     */
    bool LoadNPCSpawns(const std::string& filename);
    
    /**
     * @brief Obtém um template de NPC pelo ID
     * @param templateId ID do template
     * @return Ponteiro para o template, nullptr se não encontrado
     */
    const NPCTemplate* GetNPCTemplate(DWORD templateId) const;
    
    /**
     * @brief Executa uma função para cada NPC
     * @param func Função a executar (recebe um ponteiro para NPC)
     */
    void ForEachNPC(std::function<void(NPC*)> func);
    
    /**
     * @brief Encontra o NPC mais próximo de uma posição
     * @param mapId ID do mapa
     * @param x Posição X
     * @param y Posição Y
     * @param maxDistance Distância máxima (0 para sem limite)
     * @param typeFilter Tipo de NPC a filtrar (NPCType::UNKNOWN para todos)
     * @return Ponteiro para o NPC mais próximo, nullptr se nenhum for encontrado
     */
    NPC* FindNearestNPC(WORD mapId, WORD x, WORD y, WORD maxDistance = 0, NPCType typeFilter = NPCType::UNKNOWN);
    
    /**
     * @brief Processa o respawn de NPCs mortos
     */
    void ProcessRespawns();
    
    /**
     * @brief Obtém a configuração do sistema de NPCs
     * @return Configuração do sistema de NPCs
     */
    const NPCConfig& GetConfig() const { return config_; }
    
private:
    std::unordered_map<DWORD, std::unique_ptr<NPC>> npcs_;     // Mapa de NPCs (ID -> NPC)
    std::unordered_map<DWORD, NPCTemplate> npcTemplates_;     // Templates de NPC
    std::vector<NPCSpawnInfo> npcSpawns_;                     // Informações de spawn
    
    NPCConfig config_;                    // Configuração do sistema de NPCs
    DWORD lastUpdateTime_;                // Timestamp da última atualização
    DWORD lastRespawnCheck_;              // Timestamp da última verificação de respawn
    bool spawnInitialized_;               // Se os spawns foram inicializados
    
    bool initialized_;                    // Se o gerenciador foi inicializado
    std::atomic<DWORD> nextNpcId_;        // Próximo ID de NPC
    mutable std::mutex mutex_;            // Mutex para thread safety
    
    // Métodos privados
    /**
 * InitializeSpawns
 * 
 * Implementa a funcionalidade InitializeSpawns conforme especificação original.
 */

    void InitializeSpawns();
    /**
 * SpawnNPCFromInfo
 * 
 * Implementa a funcionalidade SpawnNPCFromInfo conforme especificação original.
 * @param spawnInfo Parâmetro spawnInfo
 * @return Retorna bool
 */

    bool SpawnNPCFromInfo(const NPCSpawnInfo& spawnInfo);
    /**
 * UpdateZonePopulation
 * 
 * Implementa a funcionalidade UpdateZonePopulation conforme especificação original.
 */

    void UpdateZonePopulation();
    /**
 * GenerateUniqueNpcId
 * 
 * Implementa a funcionalidade GenerateUniqueNpcId conforme especificação original.
 * @return Retorna DWORD
 */

    DWORD GenerateUniqueNpcId();
};

} // namespace server
} // namespace wyd

#endif // NPC_H

} // namespace wydbr
