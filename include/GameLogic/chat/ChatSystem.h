/**
 * @file ChatSystem.h
 * @brief Sistema de chat do WYDBR 2.0
 */

#ifndef WYDBR_CHAT_SYSTEM_H
#define WYDBR_CHAT_SYSTEM_H

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <queue>
#include "wyd_core.h"

namespace WYDBR {

/**
 * @class ChatSystem
 * @brief Gerencia o sistema de chat do jogo
 */
class ChatSystem {
public:
    static ChatSystem& GetInstance();

    // Inicialização
    void Initialize();
    void Shutdown();

    // Gerenciamento de mensagens
    bool SendMessage(uint32_t senderId, const std::string& message, uint8_t channel);
    bool SendPrivateMessage(uint32_t senderId, uint32_t receiverId, const std::string& message);
    bool SendGuildMessage(uint32_t senderId, uint32_t guildId, const std::string& message);
    bool SendPartyMessage(uint32_t senderId, uint32_t partyId, const std::string& message);

    // Gerenciamento de canais
    bool JoinChannel(uint32_t characterId, uint8_t channel);
    bool LeaveChannel(uint32_t characterId, uint8_t channel);
    bool CreateChannel(uint8_t channel, const std::string& name);
    bool DeleteChannel(uint8_t channel);

    // Verificações
    bool IsInChannel(uint32_t characterId, uint8_t channel) const;
    bool IsMuted(uint32_t characterId) const;
    bool IsChannelFull(uint8_t channel) const;
    bool HasPermission(uint32_t characterId, uint8_t channel) const;

    // Queries
    std::string GetChannelName(uint8_t channel) const;
    std::vector<uint32_t> GetChannelMembers(uint8_t channel) const;
    std::vector<std::string> GetRecentMessages(uint8_t channel) const;
    uint32_t GetChannelMessageCount(uint8_t channel) const;

    // Utilidades
    void UpdateChat(uint32_t deltaTime);
    void ProcessCommands(uint32_t characterId, const std::string& message);
    void FilterMessage(std::string& message);
    void LogMessage(uint32_t senderId, const std::string& message, uint8_t channel);

private:
    ChatSystem() = default;
    ~ChatSystem() = default;

    // Prevenir cópias
    ChatSystem(const ChatSystem&) = delete;
    ChatSystem& operator=(const ChatSystem&) = delete;

    // Cálculos internos
    bool ValidateMessage(const std::string& message) const;
    bool CheckChannelRequirements(uint32_t characterId, uint8_t channel) const;
    void UpdateChannelStatus(uint8_t channel);
    void HandleChannelOverflow(uint8_t channel);

    // Estado
    struct Message {
        uint32_t senderId;
        std::string content;
        uint32_t timestamp;
        uint8_t channel;
    };

    struct Channel {
        std::string name;
        uint32_t maxMembers;
        uint32_t maxMessages;
        std::vector<uint32_t> members;
        std::queue<Message> messages;
    };

    std::unordered_map<uint8_t, Channel> m_channels;
    std::unordered_map<uint32_t, std::vector<uint8_t>> m_characterChannels;
    std::unordered_map<uint32_t, uint32_t> m_muteTimers;
};

} // namespace WYDBR

#endif // WYDBR_CHAT_SYSTEM_H 