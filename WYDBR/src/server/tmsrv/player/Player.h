/**
 * Player.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/player/Player.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <vector>
#include <array>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <chrono>
#include <bitset>
#include <functional>
#include <queue>
#include <algorithm>

#include "../../../core/WYDTypes.h"
#include "../../../network/WYDPackets.h"
#include "../../../network/NetworkManager.h"
#include "../../../game/ItemSystem.h"
#include "../world/Position.h"
#include "../npc/NPCTypes.h"
#include "PlayerTypes.h"

namespace wydbr {


/**
 * @file Player.h
 * @brief Implementação avançada do sistema de jogadores do WYD
 * 
 * Este arquivo implementa a classe Player, que gerencia um jogador no
 * jogo. A implementação mantém a estrutura original do WYD, mas adiciona
 * melhorias significativas na segurança, performance e funcionalidades.
 * 
 * O sistema implementa verificações avançadas para prevenção de hacks,
 * manipulação de memória e pacotes inválidos, além de corrigir bugs
 * conhecidos do jogo original.
 */

namespace wyd {
namespace server {

// Forward declarations
class World;
class Zone;
class NPC;
class Quest;
class Item;
class Party;
class Guild;
class DatabaseManager;

// Flags de atualização do Player
enum class PlayerUpdateFlag : uint32_t {
    POSITION       = 1 << 0,   // Posição atualizada
    STATUS         = 1 << 1,   // Status (HP/MP) atualizado
    EQUIPMENT      = 1 << 2,   // Equipamento atualizado
    INVENTORY      = 1 << 3,   // Inventário atualizado
    EFFECT         = 1 << 4,   // Efeitos atualizados
    SKILL          = 1 << 5,   // Habilidades atualizadas
    APPEARANCE     = 1 << 6,   // Aparência atualizada
    LEVEL          = 1 << 7,   // Nível atualizado
    ATTRIBUTES     = 1 << 8,   // Atributos atualizados
    GUILD          = 1 << 9,   // Informações de guild atualizadas
    PARTY          = 1 << 10,  // Informações de grupo atualizadas
    COMBAT         = 1 << 11,  // Estado de combate atualizado
    TRADE          = 1 << 12,  // Estado de comércio atualizado
    QUEST          = 1 << 13,  // Informações de quests atualizadas
    ALL            = 0xFFFFFFFF // Todas as flags
};

// Operador de bitwise OR para flags
inline PlayerUpdateFlag operator|(PlayerUpdateFlag a, PlayerUpdateFlag b) {
    return static_cast<PlayerUpdateFlag>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

// Operador de bitwise AND para flags
inline PlayerUpdateFlag operator&(PlayerUpdateFlag a, PlayerUpdateFlag b) {
    return static_cast<PlayerUpdateFlag>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
}

// Função para verificar se uma flag está ativa
inline /**
 * HasFlag
 * 
 * Implementa a funcionalidade HasFlag conforme especificação original.
 * @param flags Parâmetro flags
 * @param flag Parâmetro flag
 * @return Retorna bool
 */
 bool HasFlag(PlayerUpdateFlag flags, PlayerUpdateFlag flag){
    return (static_cast<uint32_t>(flags) & static_cast<uint32_t>(flag)) != 0;
}

/**
 * @brief Classe que representa um jogador no TMServer
 * 
 * Esta classe implementa toda a lógica de um jogador, incluindo
 * movimento, combate, inventário, habilidades, etc.
 */
/**
 * Classe WYDPlayer
 * 
 * Esta classe implementa a funcionalidade WYDPlayer conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class Player {
public:
    //=================================================================
    // Construtores/Destrutores
    //=================================================================
    
    /**
     * @brief Construtor
     * @param connectionId ID da conexão
     * @param accountId ID da conta
     */
    Player(DWORD connectionId, DWORD accountId);
    
    /**
     * @brief Destrutor
     */
    virtual ~Player();
    
    /**
     * @brief Desativa o uso do construtor de cópia
     */
    Player(const Player&) = delete;
    
    /**
     * @brief Desativa o uso do operador de atribuição
     */
    Player& operator=(const Player&) = delete;
    
    //=================================================================
    // Getters/Setters básicos
    //=================================================================
    
    /**
     * @brief Obtém o ID do jogador
     * @return ID do jogador
     */
    DWORD GetID() const { return characterInfo_.basic.charId; }
    
    /**
     * @brief Obtém o ID da conexão
     * @return ID da conexão
     */
    DWORD GetConnectionID() const { return session_.connectionId; }
    
    /**
     * @brief Obtém o ID da conta
     * @return ID da conta
     */
    DWORD GetAccountID() const { return session_.accountId; }
    
    /**
     * @brief Obtém o nome do jogador
     * @return Nome do jogador
     */
    const char* GetName() const { return characterInfo_.basic.name; }
    
    /**
     * @brief Obtém o nível do jogador
     * @return Nível do jogador
     */
    WORD GetLevel() const { return characterInfo_.basic.level; }
    
    /**
     * @brief Obtém o estado do jogador
     * @return Estado do jogador
     */
    PlayerState GetState() const { return state_; }
    
    /**
     * @brief Define o estado do jogador
     * @param state Novo estado
     */
    void SetState(PlayerState state);
    
    /**
     * @brief Obtém a posição do jogador
     * @return Posição do jogador
     */
    Position GetPosition() const;
    
    /**
     * @brief Define a posição do jogador
     * @param x Posição X
     * @param y Posição Y
     * @param mapId ID do mapa (0 para manter o mesmo)
     * @param teleport Se é um teleporte (true) ou movimento normal (false)
     * @return true se a mudança de posição foi bem-sucedida, false caso contrário
     */
    bool SetPosition(WORD x, WORD y, WORD mapId = 0, bool teleport = false);
    
    //=================================================================
    // Métodos de status e atributos
    //=================================================================
    
    /**
     * @brief Obtém o status do jogador
     * @return Status do jogador
     */
    const CharacterStatus& GetStatus() const { return characterInfo_.status; }
    
    /**
     * @brief Obtém o HP atual do jogador
     * @return HP atual
     */
    DWORD GetHP() const { return characterInfo_.status.currentHP; }
    
    /**
     * @brief Obtém o HP máximo do jogador
     * @return HP máximo
     */
    DWORD GetMaxHP() const { return characterInfo_.status.maxHP; }
    
    /**
     * @brief Obtém o MP atual do jogador
     * @return MP atual
     */
    DWORD GetMP() const { return characterInfo_.status.currentMP; }
    
    /**
     * @brief Obtém o MP máximo do jogador
     * @return MP máximo
     */
    DWORD GetMaxMP() const { return characterInfo_.status.maxMP; }
    
    /**
     * @brief Altera o HP do jogador
     * @param amount Quantidade a alterar (positiva para cura, negativa para dano)
     * @param sourceId ID da fonte (jogador, NPC ou habilidade)
     * @param critical Se o dano/cura é crítico
     * @return Quantidade real alterada
     */
    INT ChangeHP(INT amount, DWORD sourceId = 0, bool critical = false);
    
    /**
     * @brief Altera o MP do jogador
     * @param amount Quantidade a alterar (positiva para recuperação, negativa para consumo)
     * @param sourceId ID da fonte (jogador, NPC ou habilidade)
     * @return Quantidade real alterada
     */
    INT ChangeMP(INT amount, DWORD sourceId = 0);
    
    /**
     * @brief Verifica se o jogador está morto
     * @return true se o jogador está morto, false caso contrário
     */
    bool IsDead() const { return characterInfo_.status.currentHP == 0; }
    
    /**
     * @brief Revive o jogador
     * @param fullHP Se deve reviver com HP cheio (true) ou parcial (false)
     * @param restoreEffects Se deve restaurar efeitos ativos antes da morte
     * @return true se o jogador foi revivido, false caso contrário
     */
    bool Revive(bool fullHP = false, bool restoreEffects = false);
    
    /**
     * @brief Obtém os atributos do jogador
     * @return Atributos do jogador
     */
    const CharacterAttributes& GetAttributes() const { return characterInfo_.attributes; }
    
    /**
     * @brief Adiciona pontos em um atributo
     * @param attribute Índice do atributo (0=STR, 1=DEX, 2=CON, 3=INT, 4=WIS, 5=CHA)
     * @param points Quantidade de pontos a adicionar
     * @return true se os pontos foram adicionados, false caso contrário
     */
    bool AddAttributePoints(BYTE attribute, BYTE points);
    
    /**
     * @brief Recalcula todos os atributos derivados
     */
    void RecalculateStats();
    
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
     * @brief Verifica se o jogador está em combate
     * @return true se o jogador está em combate, false caso contrário
     */
    bool IsInCombat() const;
    
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
    // Métodos de inventário e itens
    //=================================================================
    
    /**
     * @brief Obtém o inventário do jogador
     * @return Inventário do jogador
     */
    const CharacterInventory& GetInventory() const { return characterInfo_.inventory; }
    
    /**
     * @brief Obtém o equipamento do jogador
     * @return Equipamento do jogador
     */
    const CharacterEquipment& GetEquipment() const { return characterInfo_.equipment; }
    
    /**
     * @brief Adiciona um item ao inventário
     * @param itemTemplate ID do template do item
     * @param amount Quantidade (para itens empilháveis)
     * @param preferredSlot Slot preferido (-1 para primeiro disponível)
     * @param refineLevel Nível de refinamento (0-15)
     * @param soulbound Se o item é soulbound (vinculado à alma)
     * @return true se o item foi adicionado, false caso contrário
     */
    bool AddItem(WORD itemTemplate, WORD amount = 1, BYTE preferredSlot = 0xFF, BYTE refineLevel = 0, bool soulbound = false);
    
    /**
     * @brief Remove um item do inventário
     * @param slot Slot do item
     * @param amount Quantidade a remover (0 para remover todo o stack)
     * @return true se o item foi removido, false caso contrário
     */
    bool RemoveItem(BYTE slot, WORD amount = 0);
    
    /**
     * @brief Usa um item
     * @param slot Slot do item
     * @param targetId ID do alvo (para itens que exigem alvo)
     * @return true se o item foi usado, false caso contrário
     */
    bool UseItem(BYTE slot, DWORD targetId = 0);
    
    /**
     * @brief Equipa um item
     * @param slot Slot do inventário com o item
     * @return true se o item foi equipado, false caso contrário
     */
    bool EquipItem(BYTE slot);
    
    /**
     * @brief Remove um item equipado
     * @param slot Slot de equipamento
     * @return true se o item foi desequipado, false caso contrário
     */
    bool UnequipItem(BYTE slot);
    
    /**
     * @brief Move um item de um slot para outro
     * @param fromSlot Slot de origem
     * @param toSlot Slot de destino
     * @return true se o item foi movido, false caso contrário
     */
    bool MoveItem(BYTE fromSlot, BYTE toSlot);
    
    /**
     * @brief Divide um stack de itens
     * @param fromSlot Slot de origem
     * @param toSlot Slot de destino
     * @param amount Quantidade a dividir
     * @return true se o stack foi dividido, false caso contrário
     */
    bool SplitItem(BYTE fromSlot, BYTE toSlot, WORD amount);
    
    /**
     * @brief Verifica se o jogador tem um item específico
     * @param itemId ID do item
     * @param amount Quantidade mínima
     * @return true se o jogador tem o item, false caso contrário
     */
    bool HasItem(WORD itemId, WORD amount = 1) const;
    
    /**
     * @brief Encontra um item no inventário
     * @param itemId ID do item
     * @return Slot do item (-1 se não encontrado)
     */
    INT FindItem(WORD itemId) const;
    
    /**
     * @brief Obtém a quantidade de ouro do jogador
     * @return Quantidade de ouro
     */
    DWORD GetGold() const { return characterInfo_.basic.gold; }
    
    /**
     * @brief Adiciona ouro ao jogador
     * @param amount Quantidade a adicionar
     * @return true se o ouro foi adicionado, false caso contrário
     */
    bool AddGold(DWORD amount);
    
    /**
     * @brief Remove ouro do jogador
     * @param amount Quantidade a remover
     * @return true se o ouro foi removido, false caso contrário
     */
    bool RemoveGold(DWORD amount);
    
    //=================================================================
    // Métodos de habilidades
    //=================================================================
    
    /**
     * @brief Obtém as habilidades do jogador
     * @return Habilidades do jogador
     */
    const CharacterSkills& GetSkills() const { return characterInfo_.skills; }
    
    /**
     * @brief Aprende uma habilidade
     * @param skillId ID da habilidade
     * @param level Nível da habilidade
     * @return true se a habilidade foi aprendida, false caso contrário
     */
    bool LearnSkill(WORD skillId, BYTE level = 1);
    
    /**
     * @brief Melhora o nível de uma habilidade
     * @param skillId ID da habilidade
     * @param levelIncrease Aumento de nível (1 por padrão)
     * @return true se a habilidade foi melhorada, false caso contrário
     */
    bool ImproveSkill(WORD skillId, BYTE levelIncrease = 1);
    
    /**
     * @brief Esquece uma habilidade
     * @param skillId ID da habilidade
     * @return true se a habilidade foi esquecida, false caso contrário
     */
    bool ForgetSkill(WORD skillId);
    
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
     * @brief Verifica se o jogador tem uma habilidade
     * @param skillId ID da habilidade
     * @param minLevel Nível mínimo da habilidade
     * @return true se o jogador tem a habilidade, false caso contrário
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
    
    //=================================================================
    // Métodos de efeitos (buffs/debuffs)
    //=================================================================
    
    /**
     * @brief Adiciona um efeito ao jogador
     * @param effectId ID do efeito
     * @param value Valor do efeito
     * @param duration Duração em milissegundos (0 para efeito permanente)
     * @param sourceId ID da fonte do efeito
     * @return true se o efeito foi adicionado, false caso contrário
     */
    bool AddEffect(BYTE effectId, WORD value, DWORD duration, DWORD sourceId = 0);
    
    /**
     * @brief Remove um efeito do jogador
     * @param effectId ID do efeito
     * @param sourceId ID da fonte do efeito (0 para qualquer fonte)
     * @return true se o efeito foi removido, false caso contrário
     */
    bool RemoveEffect(BYTE effectId, DWORD sourceId = 0);
    
    /**
     * @brief Remove todos os efeitos do jogador
     * @param skipPermanent Se deve pular efeitos permanentes
     */
    void RemoveAllEffects(bool skipPermanent = true);
    
    /**
     * @brief Verifica se o jogador tem um efeito
     * @param effectId ID do efeito
     * @param sourceId ID da fonte do efeito (0 para qualquer fonte)
     * @return true se o jogador tem o efeito, false caso contrário
     */
    bool HasEffect(BYTE effectId, DWORD sourceId = 0) const;
    
    /**
     * @brief Atualiza todos os efeitos ativos
     * @param currentTime Timestamp atual
     */
    void UpdateEffects(DWORD currentTime);
    
    //=================================================================
    // Métodos de experiência e nível
    //=================================================================
    
    /**
     * @brief Adiciona experiência ao jogador
     * @param amount Quantidade de experiência
     * @param sourceId ID da fonte (monstro, quest, etc.)
     * @param applyRate Se deve aplicar taxas de experiência
     * @return true se a experiência foi adicionada, false caso contrário
     */
    bool AddExperience(DWORD amount, DWORD sourceId = 0, bool applyRate = true);
    
    /**
     * @brief Aumenta o nível do jogador
     * @param levels Número de níveis a aumentar
     * @param addPoints Se deve adicionar pontos de atributo
     * @return true se o nível foi aumentado, false caso contrário
     */
    bool LevelUp(BYTE levels = 1, bool addPoints = true);
    
    /**
     * @brief Diminui o nível do jogador
     * @param levels Número de níveis a diminuir
     * @param removePoints Se deve remover pontos de atributo
     * @return true se o nível foi diminuído, false caso contrário
     */
    bool LevelDown(BYTE levels = 1, bool removePoints = true);
    
    /**
     * @brief Obtém a experiência necessária para o próximo nível
     * @param level Nível para calcular (0 para o nível atual)
     * @return Experiência necessária
     */
    DWORD GetExperienceForLevel(WORD level = 0) const;
    
    //=================================================================
    // Métodos de grupo e social
    //=================================================================
    
    /**
     * @brief Obtém o ID do grupo
     * @return ID do grupo (0 se não estiver em grupo)
     */
    DWORD GetPartyID() const { return partyId_; }
    
    /**
     * @brief Define o grupo do jogador
     * @param partyId ID do grupo
     */
    void SetPartyID(DWORD partyId) { partyId_ = partyId; }
    
    /**
     * @brief Obtém informações de guild do jogador
     * @return Informações de guild
     */
    const CharacterGuild& GetGuildInfo() const { return characterInfo_.social.guild; }
    
    /**
     * @brief Obtém o ID da guild
     * @return ID da guild (0 se não estiver em guild)
     */
    DWORD GetGuildID() const { return characterInfo_.social.guild.guildId; }
    
    /**
     * @brief Define a guild do jogador
     * @param guildId ID da guild
     * @param guildName Nome da guild
     * @param rank Rank na guild
     * @return true se as informações foram definidas, false caso contrário
     */
    bool SetGuild(DWORD guildId, const char* guildName, BYTE rank);
    
    /**
     * @brief Adiciona um amigo à lista
     * @param friendId ID do jogador a adicionar
     * @return true se o amigo foi adicionado, false caso contrário
     */
    bool AddFriend(DWORD friendId);
    
    /**
     * @brief Remove um amigo da lista
     * @param friendId ID do jogador a remover
     * @return true se o amigo foi removido, false caso contrário
     */
    bool RemoveFriend(DWORD friendId);
    
    /**
     * @brief Verifica se um jogador é amigo
     * @param playerId ID do jogador
     * @return true se o jogador é amigo, false caso contrário
     */
    bool IsFriend(DWORD playerId) const;
    
    /**
     * @brief Adiciona um jogador à lista de bloqueados
     * @param playerId ID do jogador a bloquear
     * @return true se o jogador foi bloqueado, false caso contrário
     */
    bool BlockPlayer(DWORD playerId);
    
    /**
     * @brief Remove um jogador da lista de bloqueados
     * @param playerId ID do jogador a desbloquear
     * @return true se o jogador foi desbloqueado, false caso contrário
     */
    bool UnblockPlayer(DWORD playerId);
    
    /**
     * @brief Verifica se um jogador está bloqueado
     * @param playerId ID do jogador
     * @return true se o jogador está bloqueado, false caso contrário
     */
    bool IsPlayerBlocked(DWORD playerId) const;
    
    //=================================================================
    // Métodos de quests
    //=================================================================
    
    /**
     * @brief Obtém informações de quests do jogador
     * @return Informações de quests
     */
    const CharacterQuests& GetQuestInfo() const { return characterInfo_.quests; }
    
    /**
     * @brief Inicia uma quest
     * @param questId ID da quest
     * @param npcId ID do NPC que deu a quest
     * @return true se a quest foi iniciada, false caso contrário
     */
    bool StartQuest(WORD questId, WORD npcId = 0);
    
    /**
     * @brief Completa uma quest
     * @param questId ID da quest
     * @param rewardChoice Escolha de recompensa (0 para padrão)
     * @return true se a quest foi completada, false caso contrário
     */
    bool CompleteQuest(WORD questId, BYTE rewardChoice = 0);
    
    /**
     * @brief Cancela uma quest
     * @param questId ID da quest
     * @return true se a quest foi cancelada, false caso contrário
     */
    bool CancelQuest(WORD questId);
    
    /**
     * @brief Atualiza o progresso de uma quest
     * @param questId ID da quest
     * @param objectiveIdx Índice do objetivo (0-4)
     * @param progress Progresso a adicionar
     * @return true se o progresso foi atualizado, false caso contrário
     */
    bool UpdateQuestProgress(WORD questId, BYTE objectiveIdx, DWORD progress);
    
    /**
     * @brief Verifica se o jogador tem uma quest ativa
     * @param questId ID da quest
     * @return true se o jogador tem a quest ativa, false caso contrário
     */
    bool HasActiveQuest(WORD questId) const;
    
    /**
     * @brief Verifica se o jogador completou uma quest
     * @param questId ID da quest
     * @return true se o jogador completou a quest, false caso contrário
     */
    bool HasCompletedQuest(WORD questId) const;
    
    //=================================================================
    // Métodos de rede e pacotes
    //=================================================================
    
    /**
     * @brief Envia um pacote para o jogador
     * @param packet Pacote a enviar
     * @return true se o pacote foi enviado, false caso contrário
     */
    template<typename T>
    /**
 * SendPacket
 * 
 * Implementa a funcionalidade SendPacket conforme especificação original.
 * @param packet Parâmetro packet
 * @return Retorna bool
 */

    bool SendPacket(const T& packet){
        auto connection = net::NetworkManager::getInstance().getConnection(session_.connectionId);
        if (connection) {
            return connection->sendPacket(packet);
        }
        return false;
    }
    
    /**
     * @brief Envia informações atualizadas para o jogador e jogadores próximos
     * @param flags Flags indicando o que atualizar
     * @param sendToSelf Se deve enviar a atualização para o próprio jogador
     * @param sendToOthers Se deve enviar a atualização para outros jogadores
     */
    void SendUpdates(PlayerUpdateFlag flags, bool sendToSelf = true, bool sendToOthers = true);
    
    /**
     * @brief Processa um pacote
     * @param packetHeader Cabeçalho do pacote
     * @param packetData Dados do pacote
     * @param packetSize Tamanho do pacote
     * @return true se o pacote foi processado, false caso contrário
     */
    bool ProcessPacket(const net::PacketHeader* packetHeader, const void* packetData, size_t packetSize);
    
    //=================================================================
    // Métodos de atualização e manutenção
    //=================================================================
    
    /**
     * @brief Atualiza o jogador
     * @param deltaTime Tempo desde a última atualização em milissegundos
     */
    void Update(DWORD deltaTime);
    
    /**
     * @brief Salva os dados do jogador no banco de dados
     * @param immediate Se deve salvar imediatamente (true) ou agendar para o próximo ciclo (false)
     * @return true se os dados foram salvos ou agendados, false caso contrário
     */
    bool SaveData(bool immediate = false);
    
    /**
     * @brief Inicializa o jogador com dados carregados
     * @param characterData Dados do personagem
     * @return true se o jogador foi inicializado, false caso contrário
     */
    bool Initialize(const CharacterInfo& characterData);
    
    /**
     * @brief Limpa os dados do jogador (usado ao desconectar)
     */
    void Cleanup();
    
    /**
     * @brief Verifica se o jogador está visível para outro jogador
     * @param player Ponteiro para o outro jogador
     * @return true se o jogador está visível, false caso contrário
     */
    bool IsVisibleTo(const Player* player) const;
    
    /**
     * @brief Verifica se um jogador ou NPC está visível para este jogador
     * @param targetId ID do alvo (jogador ou NPC)
     * @param isNPC Se o alvo é um NPC (true) ou jogador (false)
     * @return true se o alvo está visível, false caso contrário
     */
    bool CanSee(DWORD targetId, bool isNPC = false) const;
    
    /**
     * @brief Verifica se o jogador está dentro do alcance
     * @param x Posição X alvo
     * @param y Posição Y alvo
     * @param range Alcance (0 para alcance padrão)
     * @return true se o alvo está dentro do alcance, false caso contrário
     */
    bool IsInRange(WORD x, WORD y, WORD range = 0) const;
    
    /**
     * @brief Calcula a distância até um ponto
     * @param x Posição X alvo
     * @param y Posição Y alvo
     * @return Distância em tiles
     */
    float GetDistance(WORD x, WORD y) const;
    
    /**
     * @brief Envia uma mensagem para o chat
     * @param message Mensagem
     * @param type Tipo de mensagem (0=normal, 1=privado, 2=grupo, 3=guild, etc.)
     * @param sender Nome do remetente (para mensagens privadas)
     * @param color Cor da mensagem (0xRRGGBB)
     */
    void SendChatMessage(const char* message, BYTE type = 0, const char* sender = nullptr, DWORD color = 0);
    
    /**
     * @brief Envia uma notificação ao jogador
     * @param message Mensagem
     * @param type Tipo de notificação (0=normal, 1=erro, 2=aviso, etc.)
     */
    void SendNotification(const char* message, BYTE type = 0);
    
private:
    //=================================================================
    // Atributos privados
    //=================================================================
    
    PlayerState state_;                      // Estado do jogador
    CharacterInfo characterInfo_;            // Informações completas do personagem
    PlayerSession session_;                  // Sessão do jogador
    
    WORD currentMapId_;                      // ID do mapa atual
    World* currentWorld_;                    // Ponteiro para o mundo atual
    Zone* currentZone_;                      // Ponteiro para a zona atual
    
    DWORD lastUpdateTime_;                   // Timestamp da última atualização
    DWORD lastSaveTime_;                     // Timestamp do último salvamento
    DWORD lastPositionTime_;                 // Timestamp da última atualização de posição
    PlayerUpdateFlag updateFlags_;           // Flags de atualização pendentes
    
    DWORD partyId_;                          // ID do grupo
    DWORD targetId_;                         // ID do alvo atual
    bool inCombat_;                          // Se está em combate
    DWORD combatEndTime_;                    // Timestamp de fim do combate
    
    // Controle de movimento
    WORD lastX_;                             // Última posição X
    WORD lastY_;                             // Última posição Y
    DWORD lastMoveTime_;                     // Timestamp do último movimento
    DWORD lastSpeedHackCheckTime_;           // Timestamp da última verificação de speedhack
    std::queue<Position> moveQueue_;         // Fila de movimentos pendentes
    BYTE moveSpeed_;                         // Velocidade de movimento atual
    
    // Controle de sessão
    std::vector<DWORD> visiblePlayers_;      // Jogadores visíveis para este jogador
    std::vector<DWORD> visibleNPCs_;         // NPCs visíveis para este jogador
    DWORD lastActionTime_;                   // Timestamp da última ação
    DWORD lastPingTime_;                     // Timestamp do último ping
    
    // Flags diversas
    bool isDirty_;                           // Se tem mudanças não salvas
    bool isFirstLogin_;                      // Se é o primeiro login nesta sessão
    bool isWalkable_;                        // Se pode andar
    bool isAttackable_;                      // Se pode atacar
    bool isVisible_;                         // Se está visível para outros jogadores
    
    // Mutex para thread safety
    mutable std::mutex mutex_;
    
    //=================================================================
    // Métodos privados
    //=================================================================
    
    // Métodos para atualização de estatísticas
    /**
 * UpdateHPMP
 * 
 * Implementa a funcionalidade UpdateHPMP conforme especificação original.
 */

    void UpdateHPMP();
    /**
 * CalculateCombatStats
 * 
 * Implementa a funcionalidade CalculateCombatStats conforme especificação original.
 */

    void CalculateCombatStats();
    /**
 * CalculateDefenseStats
 * 
 * Implementa a funcionalidade CalculateDefenseStats conforme especificação original.
 */

    void CalculateDefenseStats();
    /**
 * CalculateAttributeBonuses
 * 
 * Implementa a funcionalidade CalculateAttributeBonuses conforme especificação original.
 */

    void CalculateAttributeBonuses();
    /**
 * ApplyItemBonuses
 * 
 * Implementa a funcionalidade ApplyItemBonuses conforme especificação original.
 */

    void ApplyItemBonuses();
    /**
 * ApplyEffectBonuses
 * 
 * Implementa a funcionalidade ApplyEffectBonuses conforme especificação original.
 */

    void ApplyEffectBonuses();
    /**
 * CheckEquipmentRequirements
 * 
 * Implementa a funcionalidade CheckEquipmentRequirements conforme especificação original.
 */

    void CheckEquipmentRequirements();
    
    // Métodos auxiliares
    /**
 * ValidateMovement
 * 
 * Implementa a funcionalidade ValidateMovement conforme especificação original.
 * @param x Parâmetro x
 * @param y Parâmetro y
 * @return Retorna bool
 */

    bool ValidateMovement(WORD x, WORD y);
    /**
 * ValidateAttack
 * 
 * Implementa a funcionalidade ValidateAttack conforme especificação original.
 * @param targetId Parâmetro targetId
 * @param skillId Parâmetro skillId
 * @return Retorna bool
 */

    bool ValidateAttack(DWORD targetId, WORD skillId);
    /**
 * ValidateUseItem
 * 
 * Implementa a funcionalidade ValidateUseItem conforme especificação original.
 * @param slot Parâmetro slot
 * @param targetId Parâmetro targetId
 * @return Retorna bool
 */

    bool ValidateUseItem(BYTE slot, DWORD targetId);
    /**
 * ValidateUseSkill
 * 
 * Implementa a funcionalidade ValidateUseSkill conforme especificação original.
 * @param skillId Parâmetro skillId
 * @param targetId Parâmetro targetId
 * @param x Parâmetro x
 * @param y Parâmetro y
 * @return Retorna bool
 */

    bool ValidateUseSkill(WORD skillId, DWORD targetId, WORD x, WORD y);
    
    // Handlers para pacotes
    /**
 * HandleMovePacket
 * 
 * Implementa a funcionalidade HandleMovePacket conforme especificação original.
 * @param data Parâmetro data
 * @param size Parâmetro size
 * @return Retorna bool
 */

    bool HandleMovePacket(const void* data, size_t size);
    /**
 * HandleAttackPacket
 * 
 * Implementa a funcionalidade HandleAttackPacket conforme especificação original.
 * @param data Parâmetro data
 * @param size Parâmetro size
 * @return Retorna bool
 */

    bool HandleAttackPacket(const void* data, size_t size);
    /**
 * HandleChatPacket
 * 
 * Implementa a funcionalidade HandleChatPacket conforme especificação original.
 * @param data Parâmetro data
 * @param size Parâmetro size
 * @return Retorna bool
 */

    bool HandleChatPacket(const void* data, size_t size);
    /**
 * HandleItemPacket
 * 
 * Implementa a funcionalidade HandleItemPacket conforme especificação original.
 * @param data Parâmetro data
 * @param size Parâmetro size
 * @return Retorna bool
 */

    bool HandleItemPacket(const void* data, size_t size);
    /**
 * HandleSkillPacket
 * 
 * Implementa a funcionalidade HandleSkillPacket conforme especificação original.
 * @param data Parâmetro data
 * @param size Parâmetro size
 * @return Retorna bool
 */

    bool HandleSkillPacket(const void* data, size_t size);
    /**
 * HandleCharSelectPacket
 * 
 * Implementa a funcionalidade HandleCharSelectPacket conforme especificação original.
 * @param data Parâmetro data
 * @param size Parâmetro size
 * @return Retorna bool
 */

    bool HandleCharSelectPacket(const void* data, size_t size);
    /**
 * HandleQuestPacket
 * 
 * Implementa a funcionalidade HandleQuestPacket conforme especificação original.
 * @param data Parâmetro data
 * @param size Parâmetro size
 * @return Retorna bool
 */

    bool HandleQuestPacket(const void* data, size_t size);
    /**
 * HandlePartyPacket
 * 
 * Implementa a funcionalidade HandlePartyPacket conforme especificação original.
 * @param data Parâmetro data
 * @param size Parâmetro size
 * @return Retorna bool
 */

    bool HandlePartyPacket(const void* data, size_t size);
    /**
 * HandleGuildPacket
 * 
 * Implementa a funcionalidade HandleGuildPacket conforme especificação original.
 * @param data Parâmetro data
 * @param size Parâmetro size
 * @return Retorna bool
 */

    bool HandleGuildPacket(const void* data, size_t size);
    
    // Métodos de sincronização
    /**
 * SyncWithDatabase
 * 
 * Implementa a funcionalidade SyncWithDatabase conforme especificação original.
 */

    void SyncWithDatabase();
    /**
 * SyncWithWorld
 * 
 * Implementa a funcionalidade SyncWithWorld conforme especificação original.
 */

    void SyncWithWorld();
    /**
 * SyncWithZone
 * 
 * Implementa a funcionalidade SyncWithZone conforme especificação original.
 */

    void SyncWithZone();
    /**
 * UpdateVisibility
 * 
 * Implementa a funcionalidade UpdateVisibility conforme especificação original.
 */

    void UpdateVisibility();
    
    // Métodos de notificação
    /**
 * NotifyNearbyPlayers
 * 
 * Implementa a funcionalidade NotifyNearbyPlayers conforme especificação original.
 * @param data Parâmetro data
 * @param size Parâmetro size
 * @param 0 Parâmetro 0
 */

    void NotifyNearbyPlayers(const void* data, size_t size, WORD range = 0);
    /**
 * NotifyGroup
 * 
 * Implementa a funcionalidade NotifyGroup conforme especificação original.
 * @param data Parâmetro data
 * @param size Parâmetro size
 */

    void NotifyGroup(const void* data, size_t size);
    /**
 * NotifyGuild
 * 
 * Implementa a funcionalidade NotifyGuild conforme especificação original.
 * @param data Parâmetro data
 * @param size Parâmetro size
 */

    void NotifyGuild(const void* data, size_t size);
    
    // Anti-hack e validação
    /**
 * ValidatePacket
 * 
 * Implementa a funcionalidade ValidatePacket conforme especificação original.
 * @param header Parâmetro header
 * @param data Parâmetro data
 * @param size Parâmetro size
 * @return Retorna bool
 */

    bool ValidatePacket(const net::PacketHeader* header, const void* data, size_t size);
    /**
 * CheckSpeedHack
 * 
 * Implementa a funcionalidade CheckSpeedHack conforme especificação original.
 * @param x Parâmetro x
 * @param y Parâmetro y
 * @param timestamp Parâmetro timestamp
 * @return Retorna bool
 */

    bool CheckSpeedHack(WORD x, WORD y, DWORD timestamp);
    /**
 * CheckWallHack
 * 
 * Implementa a funcionalidade CheckWallHack conforme especificação original.
 * @param x Parâmetro x
 * @param y Parâmetro y
 * @return Retorna bool
 */

    bool CheckWallHack(WORD x, WORD y);
    /**
 * CheckCooldownHack
 * 
 * Implementa a funcionalidade CheckCooldownHack conforme especificação original.
 * @param skillId Parâmetro skillId
 * @param timestamp Parâmetro timestamp
 * @return Retorna bool
 */

    bool CheckCooldownHack(WORD skillId, DWORD timestamp);
    /**
 * CheckSequenceNumber
 * 
 * Implementa a funcionalidade CheckSequenceNumber conforme especificação original.
 * @param seqNum Parâmetro seqNum
 * @return Retorna bool
 */

    bool CheckSequenceNumber(DWORD seqNum);
    
    // Métodos utilitários
    /**
 * GetTickCount
 * 
 * Implementa a funcionalidade GetTickCount conforme especificação original.
 * @return Retorna DWORD
 */

    DWORD GetTickCount() const;
    /**
 * MarkDirty
 * 
 * Implementa a funcionalidade MarkDirty conforme especificação original.
 */

    void MarkDirty();
};

/**
 * @brief Gerenciador de jogadores
 * 
 * Esta classe gerencia todos os jogadores conectados ao servidor,
 * fornecendo métodos para busca, atualização e manipulação de jogadores.
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
     * @brief Construtor
     */
    PlayerManager();
    
    /**
     * @brief Destrutor
     */
    ~PlayerManager();
    
    /**
     * @brief Inicializa o gerenciador de jogadores
     * @param config Configuração do sistema de jogadores
     * @return true se a inicialização foi bem-sucedida, false caso contrário
     */
    bool Initialize(const PlayerConfig& config);
    
    /**
     * @brief Finaliza o gerenciador de jogadores
     */
    void Shutdown();
    
    /**
     * @brief Atualiza todos os jogadores
     * @param deltaTime Tempo desde a última atualização em milissegundos
     */
    void Update(DWORD deltaTime);
    
    /**
     * @brief Adiciona um jogador ao gerenciador
     * @param connectionId ID da conexão
     * @param accountId ID da conta
     * @return Ponteiro para o jogador criado, nullptr em caso de falha
     */
    Player* AddPlayer(DWORD connectionId, DWORD accountId);
    
    /**
     * @brief Remove um jogador do gerenciador
     * @param playerId ID do jogador
     * @param saveData Se deve salvar os dados do jogador antes de remover
     * @return true se o jogador foi removido, false caso contrário
     */
    bool RemovePlayer(DWORD playerId, bool saveData = true);
    
    /**
     * @brief Remove um jogador pelo ID de conexão
     * @param connectionId ID da conexão
     * @param saveData Se deve salvar os dados do jogador antes de remover
     * @return true se o jogador foi removido, false caso contrário
     */
    bool RemovePlayerByConnection(DWORD connectionId, bool saveData = true);
    
    /**
     * @brief Busca um jogador pelo ID
     * @param playerId ID do jogador
     * @return Ponteiro para o jogador, nullptr se não encontrado
     */
    Player* GetPlayer(DWORD playerId);
    
    /**
     * @brief Busca um jogador pelo ID de conexão
     * @param connectionId ID da conexão
     * @return Ponteiro para o jogador, nullptr se não encontrado
     */
    Player* GetPlayerByConnection(DWORD connectionId);
    
    /**
     * @brief Busca um jogador pelo nome
     * @param name Nome do jogador
     * @return Ponteiro para o jogador, nullptr se não encontrado
     */
    Player* GetPlayerByName(const char* name);
    
    /**
     * @brief Obtém o número de jogadores
     * @return Número de jogadores
     */
    size_t GetPlayerCount() const { return players_.size(); }
    
    /**
     * @brief Obtém a lista de jogadores
     * @return Lista de jogadores
     */
    const std::unordered_map<DWORD, std::unique_ptr<Player>>& GetPlayers() const { return players_; }
    
    /**
     * @brief Obtém a configuração do sistema de jogadores
     * @return Configuração do sistema de jogadores
     */
    const PlayerConfig& GetConfig() const { return config_; }
    
    /**
     * @brief Executa uma função para cada jogador
     * @param func Função a executar (recebe um ponteiro para Player)
     */
    void ForEachPlayer(std::function<void(Player*)> func);
    
    /**
     * @brief Envia um pacote para todos os jogadores
     * @param packet Pacote a enviar
     * @param exclude ID do jogador a excluir (0 para nenhum)
     */
    template<typename T>
    /**
 * BroadcastPacket
 * 
 * Implementa a funcionalidade BroadcastPacket conforme especificação original.
 * @param packet Parâmetro packet
 * @param 0 Parâmetro 0
 */

    void BroadcastPacket(const T& packet, DWORD exclude = 0){
        std::lock_guard<std::mutex> lock(mutex_);
        
        for (auto& pair : players_) {
            if (pair.first != exclude) {
                pair.second->SendPacket(packet);
            }
        }
    }
    
    /**
     * @brief Envia um pacote para todos os jogadores em um mapa
     * @param packet Pacote a enviar
     * @param mapId ID do mapa
     * @param exclude ID do jogador a excluir (0 para nenhum)
     */
    template<typename T>
    /**
 * BroadcastPacketToMap
 * 
 * Implementa a funcionalidade BroadcastPacketToMap conforme especificação original.
 * @param packet Parâmetro packet
 * @param mapId Parâmetro mapId
 * @param 0 Parâmetro 0
 */

    void BroadcastPacketToMap(const T& packet, WORD mapId, DWORD exclude = 0){
        std::lock_guard<std::mutex> lock(mutex_);
        
        for (auto& pair : players_) {
            if (pair.first != exclude) {
                Player* player = pair.second.get();
                if (player && player->GetPosition().mapId == mapId) {
                    player->SendPacket(packet);
                }
            }
        }
    }
    
    /**
     * @brief Envia um pacote para todos os jogadores em uma área
     * @param packet Pacote a enviar
     * @param mapId ID do mapa
     * @param centerX Posição X central
     * @param centerY Posição Y central
     * @param radius Raio da área
     * @param exclude ID do jogador a excluir (0 para nenhum)
     */
    template<typename T>
    /**
 * BroadcastPacketToArea
 * 
 * Implementa a funcionalidade BroadcastPacketToArea conforme especificação original.
 * @param packet Parâmetro packet
 * @param mapId Parâmetro mapId
 * @param centerX Parâmetro centerX
 * @param centerY Parâmetro centerY
 * @param radius Parâmetro radius
 * @param 0 Parâmetro 0
 */

    void BroadcastPacketToArea(const T& packet, WORD mapId, WORD centerX, WORD centerY, WORD radius, DWORD exclude = 0){
        std::lock_guard<std::mutex> lock(mutex_);
        
        for (auto& pair : players_) {
            if (pair.first != exclude) {
                Player* player = pair.second.get();
                if (player && player->GetPosition().mapId == mapId && player->IsInRange(centerX, centerY, radius)) {
                    player->SendPacket(packet);
                }
            }
        }
    }
    
private:
    std::unordered_map<DWORD, std::unique_ptr<Player>> players_;  // Mapa de jogadores (ID -> Player)
    std::unordered_map<DWORD, DWORD> connectionMap_;              // Mapa de conexões (ConnectionID -> PlayerID)
    std::unordered_map<std::string, DWORD> nameMap_;              // Mapa de nomes (Name -> PlayerID)
    
    PlayerConfig config_;                // Configuração do sistema de jogadores
    DWORD lastUpdateTime_;               // Timestamp da última atualização
    DWORD lastSaveTime_;                 // Timestamp do último salvamento
    
    bool initialized_;                   // Se o gerenciador foi inicializado
    mutable std::mutex mutex_;           // Mutex para thread safety
    
    // Métodos privados
    /**
 * SaveAllPlayers
 * 
 * Implementa a funcionalidade SaveAllPlayers conforme especificação original.
 */

    void SaveAllPlayers();
    /**
 * UpdatePlayerMaps
 * 
 * Implementa a funcionalidade UpdatePlayerMaps conforme especificação original.
 */

    void UpdatePlayerMaps();
    /**
 * PerformMaintenanceTasks
 * 
 * Implementa a funcionalidade PerformMaintenanceTasks conforme especificação original.
 */

    void PerformMaintenanceTasks();
};

} // namespace server
} // namespace wyd

#endif // PLAYER_H

} // namespace wydbr
