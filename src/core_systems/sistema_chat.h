#pragma once

#include "../core/WYDTypes.h"
#include <unordered_map>
#include <vector>
#include <memory>
#include <string>
#include <functional>
#include <queue>
#include <mutex>

namespace WYD {
    class SistemaChat {
    private:
        struct ChatMessage {
            DWORD senderId;
            std::string senderName;
            std::string message;
            DWORD timestamp;
            BYTE channel;
            bool isSystem;
        };

        struct ChatChannel {
            BYTE id;
            std::string name;
            DWORD minLevel;
            bool isGlobal;
            bool isGuild;
            bool isParty;
            bool isPrivate;
            DWORD targetId; // Para canais privados
        };

        struct ChatFilter {
            std::vector<std::string> bannedWords;
            bool enableCapsFilter;
            bool enableSpamFilter;
            DWORD spamThreshold;
            DWORD spamTimeWindow;
        };

        std::unordered_map<BYTE, ChatChannel> channels;
        std::unordered_map<DWORD, std::vector<BYTE>> characterChannels; // characterId -> channels
        std::unordered_map<DWORD, std::queue<ChatMessage>> messageQueues; // characterId -> messages
        std::unordered_map<DWORD, DWORD> lastMessageTime; // characterId -> timestamp
        ChatFilter filter;

        std::mutex messageMutex;
        std::mutex channelMutex;

        // Callbacks
        std::function<void(DWORD, const ChatMessage&)> messageCallback;
        std::function<void(DWORD, BYTE)> channelJoinCallback;
        std::function<void(DWORD, BYTE)> channelLeaveCallback;

    public:
        SistemaChat() {
            initializeDefaultChannels();
            initializeFilter();
        }

        ~SistemaChat() = default;

        // Inicialização
        void initializeDefaultChannels() {
            // Canal Global
            ChatChannel global{};
            global.id = 0;
            global.name = "Global";
            global.minLevel = 1;
            global.isGlobal = true;
            channels[0] = global;

            // Canal de Guilda
            ChatChannel guild{};
            guild.id = 1;
            guild.name = "Guilda";
            guild.minLevel = 1;
            guild.isGuild = true;
            channels[1] = guild;

            // Canal de Grupo
            ChatChannel party{};
            party.id = 2;
            party.name = "Grupo";
            party.minLevel = 1;
            party.isParty = true;
            channels[2] = party;

            // Canal de Comércio
            ChatChannel trade{};
            trade.id = 3;
            trade.name = "Comércio";
            trade.minLevel = 10;
            trade.isGlobal = true;
            channels[3] = trade;
        }

        void initializeFilter() {
            filter.enableCapsFilter = true;
            filter.enableSpamFilter = true;
            filter.spamThreshold = 5;
            filter.spamTimeWindow = 5000; // 5 segundos
        }

        // Gerenciamento de canais
        bool createChannel(const ChatChannel& channel) {
            std::lock_guard<std::mutex> lock(channelMutex);
            if (channels.find(channel.id) != channels.end()) {
                return false;
            }

            channels[channel.id] = channel;
            return true;
        }

        bool removeChannel(BYTE channelId) {
            std::lock_guard<std::mutex> lock(channelMutex);
            return channels.erase(channelId) > 0;
        }

        // Participação em canais
        bool joinChannel(DWORD characterId, BYTE channelId) {
            std::lock_guard<std::mutex> lock(channelMutex);
            auto& characterChannels = this->characterChannels[characterId];

            // Verificar se já está no canal
            if (std::find(characterChannels.begin(), characterChannels.end(), channelId) != characterChannels.end()) {
                return false;
            }

            characterChannels.push_back(channelId);

            // Chamar callback
            if (channelJoinCallback) {
                channelJoinCallback(characterId, channelId);
            }

            return true;
        }

        bool leaveChannel(DWORD characterId, BYTE channelId) {
            std::lock_guard<std::mutex> lock(channelMutex);
            auto& characterChannels = this->characterChannels[characterId];

            auto it = std::find(characterChannels.begin(), characterChannels.end(), channelId);
            if (it == characterChannels.end()) {
                return false;
            }

            characterChannels.erase(it);

            // Chamar callback
            if (channelLeaveCallback) {
                channelLeaveCallback(characterId, channelId);
            }

            return true;
        }

        // Sistema de mensagens
        bool sendMessage(DWORD senderId, const std::string& senderName, const std::string& message, BYTE channel) {
            std::lock_guard<std::mutex> lock(messageMutex);

            // Verificar spam
            if (isSpamming(senderId)) {
                return false;
            }

            // Filtrar mensagem
            std::string filteredMessage = filterMessage(message);
            if (filteredMessage.empty()) {
                return false;
            }

            // Criar mensagem
            ChatMessage chatMessage{};
            chatMessage.senderId = senderId;
            chatMessage.senderName = senderName;
            chatMessage.message = filteredMessage;
            chatMessage.timestamp = GetTickCount();
            chatMessage.channel = channel;
            chatMessage.isSystem = false;

            // Enviar para destinatários apropriados
            broadcastMessage(chatMessage);

            // Atualizar timestamp
            lastMessageTime[senderId] = chatMessage.timestamp;

            return true;
        }

        bool sendSystemMessage(const std::string& message, BYTE channel) {
            std::lock_guard<std::mutex> lock(messageMutex);

            ChatMessage systemMessage{};
            systemMessage.senderId = 0;
            systemMessage.senderName = "Sistema";
            systemMessage.message = message;
            systemMessage.timestamp = GetTickCount();
            systemMessage.channel = channel;
            systemMessage.isSystem = true;

            broadcastMessage(systemMessage);
            return true;
        }

        // Sistema de filtro
        void addBannedWord(const std::string& word) {
            filter.bannedWords.push_back(word);
        }

        void removeBannedWord(const std::string& word) {
            auto it = std::find(filter.bannedWords.begin(), filter.bannedWords.end(), word);
            if (it != filter.bannedWords.end()) {
                filter.bannedWords.erase(it);
            }
        }

        // Callbacks
        void registerMessageCallback(std::function<void(DWORD, const ChatMessage&)> callback) {
            messageCallback = callback;
        }

        void registerChannelJoinCallback(std::function<void(DWORD, BYTE)> callback) {
            channelJoinCallback = callback;
        }

        void registerChannelLeaveCallback(std::function<void(DWORD, BYTE)> callback) {
            channelLeaveCallback = callback;
        }

        // Getters
        const std::vector<BYTE>& getCharacterChannels(DWORD characterId) const {
            return characterChannels.at(characterId);
        }

        const ChatChannel& getChannel(BYTE channelId) const {
            return channels.at(channelId);
        }

        std::vector<ChatMessage> getMessageHistory(DWORD characterId) {
            std::lock_guard<std::mutex> lock(messageMutex);
            auto& queue = messageQueues[characterId];
            std::vector<ChatMessage> history;

            while (!queue.empty()) {
                history.push_back(queue.front());
                queue.pop();
            }

            return history;
        }

    private:
        bool isSpamming(DWORD characterId) {
            if (!filter.enableSpamFilter) {
                return false;
            }

            auto it = lastMessageTime.find(characterId);
            if (it == lastMessageTime.end()) {
                return false;
            }

            DWORD timeSinceLastMessage = GetTickCount() - it->second;
            return timeSinceLastMessage < filter.spamTimeWindow;
        }

        std::string filterMessage(const std::string& message) {
            std::string filtered = message;

            // Filtrar palavras banidas
            for (const auto& word : filter.bannedWords) {
                size_t pos = 0;
                while ((pos = filtered.find(word, pos)) != std::string::npos) {
                    filtered.replace(pos, word.length(), std::string(word.length(), '*'));
                    pos += word.length();
                }
            }

            // Filtrar caps
            if (filter.enableCapsFilter) {
                int capsCount = 0;
                for (char c : filtered) {
                    if (isupper(c)) capsCount++;
                }

                if (capsCount > filtered.length() * 0.7) { // 70% em caps
                    for (char& c : filtered) {
                        c = tolower(c);
                    }
                }
            }

            return filtered;
        }

        void broadcastMessage(const ChatMessage& message) {
            for (const auto& character : characterChannels) {
                DWORD characterId = character.first;
                const auto& channels = character.second;

                // Verificar se o personagem está no canal
                if (std::find(channels.begin(), channels.end(), message.channel) != channels.end()) {
                    // Adicionar à fila de mensagens do personagem
                    messageQueues[characterId].push(message);

                    // Chamar callback
                    if (messageCallback) {
                        messageCallback(characterId, message);
                    }
                }
            }
        }
    };
} 