/**
 * WYDNPC.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/npc/WYDNPC.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef WYD_NPC_H
#define WYD_NPC_H

#include "src/server/tmsrv/npc/WYDEntity.h"
#include <vector>
#include <map>
#include <string>
#include <memory>
#include <mutex>

namespace wydbr {


// Forward declarations
class NPCManager;
class DialogueManager;

/**
 * Tipos de NPC
 */
enum class NPCType : uint8_t {
    MERCHANT,       // Comerciante
    QUEST_GIVER,    // Entregador de quests
    BANKER,         // Banqueiro
    ENCHANTER,      // Encantador
    TRAINER,        // Treinador de skills
    TELEPORTER,     // Teleportador
    AUCTIONEER,     // Leiloeiro
    GUILDMASTER,    // Mestre de Guild
    STORYTELLER,    // Narrador
    SERVICE         // Serviços diversos
};

/**
 * Classe de NPCs amigáveis
 * 
 * Representa entidades não hostis que fornecem serviços, 
 * quests, diálogos e interações diversas para os jogadores.
 */
/**
 * Classe WYDNPC
 * 
 * Esta classe implementa a funcionalidade WYDNPC conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class WYDNPC : public WYDEntity {
public:
    // /**
 * WYDNPC
 * 
 * Implementa a funcionalidade WYDNPC conforme especificação original.
 * @param uniqueId Parâmetro uniqueId
 * @param npcId Parâmetro npcId
 * @param name Parâmetro name
 * @return Retorna Construtor
 */
 Construtor
    WYDNPC(uint32_t uniqueId, uint16_t npcId, const std::string& name);
    
    // Destrutor
    virtual ~WYDNPC();
    
    // Inicialização
    /**
 * initialize
 * 
 * Implementa a funcionalidade initialize conforme especificação original.
 * @param manager Parâmetro manager
 */

    void initialize(NPCManager* manager);
    
    // Atualização
    /**
 * update
 * 
 * Implementa a funcionalidade update conforme especificação original.
 * @param timestamp Parâmetro timestamp
 */

    void update(uint64_t timestamp) override;
    
    // Serialização
    std::string serialize() const override;
    
    // Identificação específica
    /**
 * getNpcId
 * 
 * Implementa a funcionalidade getNpcId conforme especificação original.
 * @return Retorna uint16_t
 */

    uint16_t getNpcId() const{ return npcId_; }
    /**
 * getType
 * 
 * Implementa a funcionalidade getType conforme especificação original.
 * @return Retorna NPCType
 */

    NPCType getType() const{ return type_; }
    
    // Configurações
    /**
 * setType
 * 
 * Implementa a funcionalidade setType conforme especificação original.
 * @param type Parâmetro type
 */

    void setType(NPCType type){ type_ = type; }
    /**
 * setParameters
 * 
 * Implementa a funcionalidade setParameters conforme especificação original.
 * @param std::map<std::string Parâmetro std::map<std::string
 * @param params Parâmetro params
 */

    void setParameters(const std::map<std::string, std::string>& params){ parameters_ = params; }
    /**
 * setWanderArea
 * 
 * Implementa a funcionalidade setWanderArea conforme especificação original.
 * @param centerX Parâmetro centerX
 * @param centerY Parâmetro centerY
 * @param radius Parâmetro radius
 */

    void setWanderArea(uint16_t centerX, uint16_t centerY, uint16_t radius);
    /**
 * setServices
 * 
 * Implementa a funcionalidade setServices conforme especificação original.
 * @param services Parâmetro services
 */

    void setServices(const std::vector<uint16_t>& services){ services_ = services; }
    /**
 * setItems
 * 
 * Implementa a funcionalidade setItems conforme especificação original.
 * @param items Parâmetro items
 */

    void setItems(const std::vector<uint16_t>& items){ items_ = items; }
    
    // Diálogos
    /**
 * setDialogues
 * 
 * Implementa a funcionalidade setDialogues conforme especificação original.
 * @param std::map<uint16_t Parâmetro std::map<uint16_t
 * @param dialogues Parâmetro dialogues
 */

    void setDialogues(const std::map<uint16_t, std::string>& dialogues);
    /**
 * setResponses
 * 
 * Implementa a funcionalidade setResponses conforme especificação original.
 * @param std::map<uint16_t Parâmetro std::map<uint16_t
 * @param responses Parâmetro responses
 */

    void setResponses(const std::map<uint16_t, std::vector<uint16_t>>& responses);
    /**
 * getDialogue
 * 
 * Implementa a funcionalidade getDialogue conforme especificação original.
 * @param dialogueId Parâmetro dialogueId
 * @return Retorna std::string
 */

    std::string getDialogue(uint16_t dialogueId) const;
    std::vector<uint16_t> getResponseOptions(uint16_t dialogueId) const;
    
    // Interação
    /**
 * interact
 * 
 * Implementa a funcionalidade interact conforme especificação original.
 * @param playerId Parâmetro playerId
 * @param 0 Parâmetro 0
 * @return Retorna bool
 */

    bool interact(uint32_t playerId, uint16_t option = 0);
    /**
 * buy
 * 
 * Implementa a funcionalidade buy conforme especificação original.
 * @param playerId Parâmetro playerId
 * @param itemId Parâmetro itemId
 * @param quantity Parâmetro quantity
 * @return Retorna bool
 */

    bool buy(uint32_t playerId, uint16_t itemId, uint16_t quantity);
    /**
 * sell
 * 
 * Implementa a funcionalidade sell conforme especificação original.
 * @param playerId Parâmetro playerId
 * @param itemInstanceId Parâmetro itemInstanceId
 * @return Retorna bool
 */

    bool sell(uint32_t playerId, uint32_t itemInstanceId);
    /**
 * startQuest
 * 
 * Implementa a funcionalidade startQuest conforme especificação original.
 * @param playerId Parâmetro playerId
 * @param questId Parâmetro questId
 * @return Retorna bool
 */

    bool startQuest(uint32_t playerId, uint16_t questId);
    /**
 * completeQuest
 * 
 * Implementa a funcionalidade completeQuest conforme especificação original.
 * @param playerId Parâmetro playerId
 * @param questId Parâmetro questId
 * @return Retorna bool
 */

    bool completeQuest(uint32_t playerId, uint16_t questId);
    /**
 * teleport
 * 
 * Implementa a funcionalidade teleport conforme especificação original.
 * @param playerId Parâmetro playerId
 * @param destMapId Parâmetro destMapId
 * @param destX Parâmetro destX
 * @param destY Parâmetro destY
 * @return Retorna bool
 */

    bool teleport(uint32_t playerId, uint16_t destMapId, uint16_t destX, uint16_t destY);
    /**
 * useService
 * 
 * Implementa a funcionalidade useService conforme especificação original.
 * @param playerId Parâmetro playerId
 * @param serviceId Parâmetro serviceId
 * @return Retorna bool
 */

    bool useService(uint32_t playerId, uint16_t serviceId);
    
    // Acesso a parâmetros e informações
    /**
 * getParameter
 * 
 * Implementa a funcionalidade getParameter conforme especificação original.
 * @param key Parâmetro key
 * @return Retorna std::string
 */

    std::string getParameter(const std::string& key) const;
    /**
 * hasService
 * 
 * Implementa a funcionalidade hasService conforme especificação original.
 * @param serviceId Parâmetro serviceId
 * @return Retorna bool
 */

    bool hasService(uint16_t serviceId) const;
    /**
 * hasItem
 * 
 * Implementa a funcionalidade hasItem conforme especificação original.
 * @param itemId Parâmetro itemId
 * @return Retorna bool
 */

    bool hasItem(uint16_t itemId) const;
    
    // Movimento
    /**
 * moveToPosition
 * 
 * Implementa a funcionalidade moveToPosition conforme especificação original.
 * @param x Parâmetro x
 * @param y Parâmetro y
 */

    void moveToPosition(uint16_t x, uint16_t y);
    /**
 * setMoveSpeed
 * 
 * Implementa a funcionalidade setMoveSpeed conforme especificação original.
 * @param speed Parâmetro speed
 */

    void setMoveSpeed(uint16_t speed){ moveSpeed_ = speed; }
    /**
 * isWandering
 * 
 * Implementa a funcionalidade isWandering conforme especificação original.
 * @return Retorna bool
 */

    bool isWandering() const{ return isWandering_; }
    /**
 * setWandering
 * 
 * Implementa a funcionalidade setWandering conforme especificação original.
 * @param wandering Parâmetro wandering
 */

    void setWandering(bool wandering){ isWandering_ = wandering; }
    
private:
    // Referência ao gerenciador
    NPCManager* manager_;
    
    // Características específicas
    uint16_t npcId_;           // ID do template do NPC
    NPCType type_;             // Tipo do NPC
    std::map<std::string, std::string> parameters_; // Parâmetros adicionais
    
    // Serviços e itens
    std::vector<uint16_t> services_; // IDs de serviços
    std::vector<uint16_t> items_;    // IDs de itens (para venda)
    
    // Movimento
    bool isWandering_;         // Se está perambulando
    uint16_t moveSpeed_;       // Velocidade de movimento
    uint16_t wanderCenterX_;   // Centro X da área de perambulação
    uint16_t wanderCenterY_;   // Centro Y da área de perambulação
    uint16_t wanderRadius_;    // Raio da área de perambulação
    uint64_t lastMoveTime_;    // Último movimento
    
    // Diálogos
    std::map<uint16_t, std::string> dialogues_;         // dialogueId -> texto
    std::map<uint16_t, std::vector<uint16_t>> responses_; // dialogueId -> [responseId, responseId, ...]
    
    // Estado de interação
    struct PlayerInteraction {
        uint32_t playerId;
        uint16_t currentDialogue;
        uint64_t interactionStart;
        uint64_t lastInteraction;
    };
    std::map<uint32_t, PlayerInteraction> activeInteractions_;
    
    // Mutex para proteção
    mutable std::mutex npcMutex_;
    
    // Timers
    uint64_t lastWanderCheck_;
    
    // Métodos auxiliares
    /**
 * updateWandering
 * 
 * Implementa a funcionalidade updateWandering conforme especificação original.
 * @param timestamp Parâmetro timestamp
 */

    void updateWandering(uint64_t timestamp);
    /**
 * updateInteractions
 * 
 * Implementa a funcionalidade updateInteractions conforme especificação original.
 * @param timestamp Parâmetro timestamp
 */

    void updateInteractions(uint64_t timestamp);
    /**
 * cleanupStaleInteractions
 * 
 * Implementa a funcionalidade cleanupStaleInteractions conforme especificação original.
 * @param timestamp Parâmetro timestamp
 */

    void cleanupStaleInteractions(uint64_t timestamp);
    std::pair<uint16_t, uint16_t> getRandomWanderPosition();
    /**
 * isValidPosition
 * 
 * Implementa a funcionalidade isValidPosition conforme especificação original.
 * @param x Parâmetro x
 * @param y Parâmetro y
 * @return Retorna bool
 */

    bool isValidPosition(uint16_t x, uint16_t y) const;
    /**
 * startInteraction
 * 
 * Implementa a funcionalidade startInteraction conforme especificação original.
 * @param playerId Parâmetro playerId
 * @return Retorna bool
 */

    bool startInteraction(uint32_t playerId);
    /**
 * endInteraction
 * 
 * Implementa a funcionalidade endInteraction conforme especificação original.
 * @param playerId Parâmetro playerId
 */

    void endInteraction(uint32_t playerId);
};

#endif // WYD_NPC_H

} // namespace wydbr
