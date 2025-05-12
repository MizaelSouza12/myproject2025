/**
 * WYDPlayer.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/src/tmsrv/player/WYDPlayer.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef _WYD_PLAYER_H_
#define _WYD_PLAYER_H_

/**
 * @file WYDPlayer.h
 * @brief Classe de jogador - Baseado no código original do WYD
 * 
 * Implementa a classe principal de jogador, encapsulando os dados
 * da estrutura STRUCT_MOB e adicionando funcionalidades orientadas
 * a objeto para manipulação do jogador.
 */

#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <memory>
#include <chrono>

#include "../../../include/GlobalDef.h"
#include "../../../include/TM_Mob.h"
#include "../../../include/TM_Item.h"
#include "../../../include/TM_Protocol.h"

namespace wydbr {

/**
 * @brief Estados de processamento de jogador
 */
enum PlayerState {
    PLAYER_INACTIVE = 0,            // Inativo (não processado)
    PLAYER_CONNECTING = 1,          // Conectando
    PLAYER_CREATED = 2,             // Criado
    PLAYER_LOADING = 3,             // Carregando dados
    PLAYER_ACTIVE = 4,              // Ativo (processado)
    PLAYER_SAVING = 5,              // Salvando dados
    PLAYER_DISCONNECTING = 6,       // Desconectando
    PLAYER_DISCONNECTED = 7         // Desconectado
};

/**
 * @brief Estados de jogo do jogador
 */
enum PlayerGameState {
    GAMESTATE_INITIAL = 0,          // Estado inicial
    GAMESTATE_CHARLIST = 1,         // Lista de personagens
    GAMESTATE_CHARSTATUS = 2,       // Status do personagem
    GAMESTATE_GAME = 3,             // Em jogo
    GAMESTATE_FIXING = 4,           // Em correção
    GAMESTATE_SAVING = 5,           // Salvando
    GAMESTATE_CLOSING = 6,          // Fechando
    GAMESTATE_CLOSED = 7            // Fechado
};

/**
 * @brief Classe de jogador
 * 
 * Representa um jogador no servidor WYD, encapsulando os dados do
 * personagem (STRUCT_MOB) e adicionando funcionalidades.
 */
/**
 * Classe WYDPlayer
 * 
 * Esta classe implementa a funcionalidade WYDPlayer conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class WYDPlayer {
public:
    /**
     * @brief Construtor com ID
     * @param playerId ID do jogador
     * @param accountId ID da conta
     */
    WYDPlayer(int playerId, int accountId);

    /**
     * @brief Destrutor
     */
    virtual ~WYDPlayer();

    /**
     * @brief Inicializa o jogador
     * @param connectionId ID da conexão
     * @return true se inicializado com sucesso
     */
    bool Initialize(int connectionId);

    /**
     * @brief Finaliza o jogador
     * @return true se finalizado com sucesso
     */
    bool Finalize();

    /**
     * @brief Carrega dados do jogador
     * @param mobData Dados do mob
     * @return true se carregado com sucesso
     */
    bool Load(const STRUCT_MOB& mobData);

    /**
     * @brief Salva dados do jogador
     * @param forced Se true, força o salvamento mesmo que não haja alterações
     * @return true se salvo com sucesso
     */
    bool Save(bool forced = false);

    /**
     * @brief Processa um tick de atualização
     * @param currentTick Tick atual
     * @return true se processado com sucesso
     */
    bool ProcessTick(uint32_t currentTick);

    /**
     * @brief Processa um pacote
     * @param packet Pacote a processar
     * @param size Tamanho do pacote
     * @return true se processado com sucesso
     */
    bool ProcessPacket(const void* packet, int size);

    /**
     * @brief Envia um pacote para o jogador
     * @param data Dados a enviar
     * @param size Tamanho dos dados
     * @return true se enviado com sucesso
     */
    bool SendPacket(const void* data, int size);

    /**
     * @brief Teleporta o jogador
     * @param mapId ID do mapa
     * @param x Coordenada X
     * @param y Coordenada Y
     * @return true se teleportado com sucesso
     */
    bool Teleport(int mapId, int x, int y);

    /**
     * @brief Move o jogador
     * @param x Coordenada X
     * @param y Coordenada Y
     * @param effect Efeito de movimento
     * @return true se movido com sucesso
     */
    bool Move(int x, int y, int effect = 0);

    /**
     * @brief Executa um ataque
     * @param targetId ID do alvo
     * @param targetX Coordenada X do alvo
     * @param targetY Coordenada Y do alvo
     * @param skillId ID da skill (0 para ataque básico)
     * @return true se atacou com sucesso
     */
    bool Attack(int targetId, int targetX, int targetY, int skillId = 0);

    /**
     * @brief Causa dano ao jogador
     * @param damage Dano a causar
     * @param attackerId ID do atacante (0 para dano de sistema)
     * @param isMagic Se true, o dano é mágico
     * @return true se o dano foi causado com sucesso
     */
    bool TakeDamage(int damage, int attackerId = 0, bool isMagic = false);

    /**
     * @brief Cura o jogador
     * @param amount Quantidade a curar
     * @param healerId ID do curador (0 para cura de sistema)
     * @return true se curado com sucesso
     */
    bool Heal(int amount, int healerId = 0);

    /**
     * @brief Adiciona experiência ao jogador
     * @param expAmount Quantidade de experiência
     * @return true se adicionada com sucesso
     */
    bool AddExp(int64_t expAmount);

    /**
     * @brief Sobe de nível
     * @return true se subiu de nível
     */
    bool LevelUp();

    /**
     * @brief Adiciona ouro ao jogador
     * @param goldAmount Quantidade de ouro
     * @return true se adicionado com sucesso
     */
    bool AddGold(int goldAmount);

    /**
     * @brief Remove ouro do jogador
     * @param goldAmount Quantidade de ouro
     * @return true se removido com sucesso
     */
    bool RemoveGold(int goldAmount);

    /**
     * @brief Adiciona um item ao inventário
     * @param item Item a adicionar
     * @param count Quantidade
     * @return Posição onde foi adicionado, -1 se falhou
     */
    int AddItem(const STRUCT_ITEM& item, int count = 1);

    /**
     * @brief Remove um item do inventário
     * @param slot Slot do item
     * @param count Quantidade
     * @return true se removido com sucesso
     */
    bool RemoveItem(int slot, int count = 1);

    /**
     * @brief Equipa um item
     * @param inventorySlot Slot no inventário
     * @param equipSlot Slot de equipamento
     * @return true se equipado com sucesso
     */
    bool EquipItem(int inventorySlot, int equipSlot);

    /**
     * @brief Desequipa um item
     * @param equipSlot Slot de equipamento
     * @return true se desequipado com sucesso
     */
    bool UnequipItem(int equipSlot);

    /**
     * @brief Usa um item
     * @param slot Slot do item
     * @return true se usado com sucesso
     */
    bool UseItem(int slot);

    /**
     * @brief Adiciona um efeito ao jogador
     * @param type Tipo de efeito
     * @param value Valor do efeito
     * @param level Nível do efeito
     * @param time Tempo de duração
     * @return true se adicionado com sucesso
     */
    bool AddAffect(uint8_t type, uint8_t value, uint16_t level, uint32_t time);

    /**
     * @brief Remove um efeito do jogador
     * @param type Tipo de efeito
     * @return true se removido com sucesso
     */
    bool RemoveAffect(uint8_t type);

    /**
     * @brief Verifica se o jogador tem um efeito
     * @param type Tipo de efeito
     * @return true se tiver o efeito
     */
    bool HasAffect(uint8_t type);

    /**
     * @brief Processa a manutenção de efeitos
     */
    void ProcessAffects();

    /**
     * @brief Manda uma mensagem de chat para o jogador
     * @param message Mensagem
     * @param type Tipo de chat
     * @return true se enviada com sucesso
     */
    bool SendChatMessage(const std::string& message, ChatType type = CHAT_NORMAL);

    /**
     * @brief Processa uma mensagem de chat
     * @param message Mensagem
     * @param type Tipo de chat
     * @return true se processada com sucesso
     */
    bool ProcessChatMessage(const std::string& message, ChatType type = CHAT_NORMAL);

    /**
     * @brief Processa um comando
     * @param command Comando
     * @return true se processado com sucesso
     */
    bool ProcessCommand(const std::string& command);

    /**
     * @brief Notifica a área ao redor do jogador
     * @param data Dados
     * @param size Tamanho dos dados
     * @param radius Raio da área
     * @param excludeSelf Se true, não notifica o próprio jogador
     * @return Número de jogadores notificados
     */
    int NotifyArea(const void* data, int size, int radius = 0, bool excludeSelf = false);

    /**
     * @brief Processa a remoção da área
     * @return true se processado com sucesso
     */
    bool ProcessRemovedFromArea();

    /**
     * @brief Processa a adição à área
     * @return true se processado com sucesso
     */
    bool ProcessAddedToArea();

    /**
     * @brief Atualiza os atributos base
     * @return true se atualizado com sucesso
     */
    bool UpdateBaseAttributes();

    /**
     * @brief Atualiza os atributos atuais
     * @return true se atualizado com sucesso
     */
    bool UpdateCurrentAttributes();

    /**
     * @brief Adiciona pontos de atributo
     * @param type Tipo de atributo (0=STR, 1=INT, 2=DEX, 3=CON)
     * @param points Pontos a adicionar
     * @return true se adicionado com sucesso
     */
    bool AddAttributePoints(int type, int points = 1);

    /**
     * @brief Adiciona pontos de skill
     * @param skillId ID da skill
     * @param points Pontos a adicionar
     * @return true se adicionado com sucesso
     */
    bool AddSkillPoints(int skillId, int points = 1);

    /**
     * @brief Aprende uma skill
     * @param skillId ID da skill
     * @return true se aprendida com sucesso
     */
    bool LearnSkill(int skillId);

    /**
     * @brief Verifica se o jogador tem uma skill
     * @param skillId ID da skill
     * @return true se tiver a skill
     */
    bool HasSkill(int skillId);

    /**
     * @brief Usa uma skill
     * @param skillId ID da skill
     * @param targetId ID do alvo
     * @param targetX Coordenada X do alvo
     * @param targetY Coordenada Y do alvo
     * @return true se usada com sucesso
     */
    bool UseSkill(int skillId, int targetId, int targetX, int targetY);

    /**
     * @brief Configura a barra de skills
     * @param slot Slot da barra
     * @param skillId ID da skill
     * @return true se configurada com sucesso
     */
    bool SetSkillBar(int slot, int skillId);

    /**
     * @brief Entra em um grupo
     * @param partyId ID do grupo
     * @return true se entrou com sucesso
     */
    bool JoinParty(int partyId);

    /**
     * @brief Sai de um grupo
     * @return true se saiu com sucesso
     */
    bool LeaveParty();

    /**
     * @brief Morre
     * @param killerId ID do assassino (0 para morte de sistema)
     * @return true se morreu com sucesso
     */
    bool Die(int killerId = 0);

    /**
     * @brief Revive
     * @param reviverId ID do revivedor (0 para reviver de sistema)
     * @return true se reviveu com sucesso
     */
    bool Revive(int reviverId = 0);

    /**
     * @brief Cria um item em uma posição específica
     * @param x Coordenada X
     * @param y Coordenada Y
     * @param item Item a criar
     * @param count Quantidade
     * @param dropTime Tempo de drop
     * @return ID do item, 0 se falhou
     */
    int CreateItem(int x, int y, const STRUCT_ITEM& item, int count = 1, int dropTime = 180);

    /**
     * @brief Pega um item do chão
     * @param itemId ID do item
     * @return true se pego com sucesso
     */
    bool PickupItem(int itemId);

    /**
     * @brief Processa a entrada em uma loja
     * @param npcId ID do NPC da loja
     * @return true se processado com sucesso
     */
    bool EnterShop(int npcId);

    /**
     * @brief Processa a compra em uma loja
     * @param itemIndex Índice do item
     * @param quantity Quantidade
     * @return true se processado com sucesso
     */
    bool BuyFromShop(int itemIndex, int quantity = 1);

    /**
     * @brief Processa a venda em uma loja
     * @param inventoryIndex Índice do item no inventário
     * @param quantity Quantidade
     * @return true se processado com sucesso
     */
    bool SellToShop(int inventoryIndex, int quantity = 1);

    /**
     * @brief Abre o armazém
     * @return true se aberto com sucesso
     */
    bool OpenStorage();

    /**
     * @brief Deposita um item no armazém
     * @param inventoryIndex Índice do item no inventário
     * @param quantity Quantidade
     * @return true se depositado com sucesso
     */
    bool DepositItemInStorage(int inventoryIndex, int quantity = 1);

    /**
     * @brief Retira um item do armazém
     * @param storageIndex Índice do item no armazém
     * @param quantity Quantidade
     * @return true se retirado com sucesso
     */
    bool WithdrawItemFromStorage(int storageIndex, int quantity = 1);

    /**
     * @brief Recupera a regeneração de HP
     * @return Taxa de regeneração de HP
     */
    int GetHpRegen();

    /**
     * @brief Recupera a regeneração de MP
     * @return Taxa de regeneração de MP
     */
    int GetMpRegen();

    /**
     * @brief Recupera a velocidade de movimento
     * @return Velocidade de movimento
     */
    int GetMoveSpeed();

    /**
     * @brief Recupera a velocidade de ataque
     * @return Velocidade de ataque
     */
    int GetAttackSpeed();

    /**
     * @brief Recupera o dano de ataque
     * @return Dano de ataque
     */
    int GetAttackDamage();

    /**
     * @brief Recupera a defesa (AC)
     * @return Defesa (AC)
     */
    int GetDefense();

    // Getters
    
    /**
     * @brief Recupera o ID do jogador
     * @return ID do jogador
     */
    int GetId() const { return playerId_; }
    
    /**
     * @brief Recupera o ID da conta
     * @return ID da conta
     */
    int GetAccountId() const { return accountId_; }
    
    /**
     * @brief Recupera o ID da conexão
     * @return ID da conexão
     */
    int GetConnectionId() const { return connectionId_; }
    
    /**
     * @brief Recupera o estado do jogador
     * @return Estado do jogador
     */
    PlayerState GetState() const { return state_; }
    
    /**
     * @brief Recupera o estado de jogo do jogador
     * @return Estado de jogo do jogador
     */
    PlayerGameState GetGameState() const { return gameState_; }
    
    /**
     * @brief Recupera o nome do jogador
     * @return Nome do jogador
     */
    std::string GetName() const { return std::string(mobData_.MobName); }
    
    /**
     * @brief Recupera o ID do mapa
     * @return ID do mapa
     */
    int GetMapId() const { return mapId_; }
    
    /**
     * @brief Recupera a posição X
     * @return Posição X
     */
    int GetPosX() const { return mobData_.LastPosition.X; }
    
    /**
     * @brief Recupera a posição Y
     * @return Posição Y
     */
    int GetPosY() const { return mobData_.LastPosition.Y; }
    
    /**
     * @brief Recupera a classe do jogador
     * @return Classe do jogador
     */
    uint8_t GetClass() const { return mobData_.Class; }
    
    /**
     * @brief Recupera o nível do jogador
     * @return Nível do jogador
     */
    int GetLevel() const { return mobData_.CurrentScore.Level; }
    
    /**
     * @brief Recupera a experiência do jogador
     * @return Experiência do jogador
     */
    int64_t GetExp() const { return mobData_.Exp; }
    
    /**
     * @brief Recupera o HP atual do jogador
     * @return HP atual do jogador
     */
    int GetHp() const { return mobData_.CurrentScore.Hp; }
    
    /**
     * @brief Recupera o HP máximo do jogador
     * @return HP máximo do jogador
     */
    int GetMaxHp() const { return mobData_.CurrentScore.MaxHp; }
    
    /**
     * @brief Recupera o MP atual do jogador
     * @return MP atual do jogador
     */
    int GetMp() const { return mobData_.CurrentScore.Mp; }
    
    /**
     * @brief Recupera o MP máximo do jogador
     * @return MP máximo do jogador
     */
    int GetMaxMp() const { return mobData_.CurrentScore.MaxMp; }
    
    /**
     * @brief Recupera o ouro do jogador
     * @return Ouro do jogador
     */
    uint32_t GetGold() const { return mobData_.Gold; }
    
    /**
     * @brief Recupera os dados do mob
     * @return Referência para os dados do mob
     */
    const STRUCT_MOB& GetMobData() const { return mobData_; }
    
    /**
     * @brief Recupera a guilda do jogador
     * @return ID da guilda do jogador
     */
    uint16_t GetGuild() const { return mobData_.Guild; }
    
    /**
     * @brief Recupera o nível na guilda do jogador
     * @return Nível na guilda do jogador
     */
    uint8_t GetGuildLevel() const { return mobData_.GuildLevel; }
    
    /**
     * @brief Recupera o ID do grupo do jogador
     * @return ID do grupo do jogador
     */
    int GetPartyId() const { return partyId_; }
    
    /**
     * @brief Verifica se o jogador está vivo
     * @return true se o jogador estiver vivo
     */
    bool IsAlive() const { return mobData_.CurrentScore.Hp > 0; }
    
    /**
     * @brief Verifica se o jogador está em uma área segura
     * @return true se o jogador estiver em uma área segura
     */
    bool IsInSafeZone() const { return inSafeZone_; }
    
    /**
     * @brief Verifica se o jogador está em modo PK
     * @return true se o jogador estiver em modo PK
     */
    bool IsPKMode() const { return pkMode_; }

    // Setters
    
    /**
     * @brief Define o estado do jogador
     * @param state Novo estado
     */
    void SetState(PlayerState state) { state_ = state; }
    
    /**
     * @brief Define o estado de jogo do jogador
     * @param gameState Novo estado de jogo
     */
    void SetGameState(PlayerGameState gameState) { gameState_ = gameState; }
    
    /**
     * @brief Define o ID do mapa
     * @param mapId Novo ID do mapa
     */
    void SetMapId(int mapId) { mapId_ = mapId; }
    
    /**
     * @brief Define a posição
     * @param x Nova posição X
     * @param y Nova posição Y
     */
    void SetPosition(int x, int y);
    
    /**
     * @brief Define se o jogador está em uma área segura
     * @param inSafeZone true se estiver em uma área segura
     */
    void SetInSafeZone(bool inSafeZone) { inSafeZone_ = inSafeZone; }
    
    /**
     * @brief Define o modo PK
     * @param pkMode true para habilitar modo PK
     */
    void SetPKMode(bool pkMode) { pkMode_ = pkMode; }
    
    /**
     * @brief Define o ID do grupo
     * @param partyId Novo ID do grupo
     */
    void SetPartyId(int partyId) { partyId_ = partyId; }

private:
    int playerId_;                       // ID do jogador
    int accountId_;                      // ID da conta
    int connectionId_;                   // ID da conexão
    PlayerState state_;                  // Estado do jogador
    PlayerGameState gameState_;          // Estado de jogo do jogador
    STRUCT_MOB mobData_;                 // Dados do mob
    bool dirty_;                         // Flag de modificação
    int mapId_;                          // ID do mapa
    bool inSafeZone_;                    // Em área segura
    bool pkMode_;                        // Modo PK
    int partyId_;                        // ID do grupo
    uint32_t lastSaveTime_;              // Último salvamento
    uint32_t lastMoveTime_;              // Último movimento
    uint32_t lastAttackTime_;            // Último ataque
    uint32_t lastSkillTime_;             // Última skill
    uint32_t lastHealTime_;              // Última cura
    uint32_t lastPingTime_;              // Último ping
    uint32_t lastAffectCheck_;           // Última verificação de efeitos
    int currentShopId_;                  // ID da loja atual
    bool storageOpen_;                   // Armazém aberto
    std::map<int, int> cooldowns_;       // Cooldowns de skills
    std::mutex dataMutex_;               // Mutex para dados
};

} // namespace wydbr

#endif // _WYD_PLAYER_H_