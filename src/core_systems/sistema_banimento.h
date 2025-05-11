#pragma once

#include "../core/WYDTypes.h"
#include <unordered_map>
#include <vector>
#include <memory>
#include <string>
#include <functional>
#include <queue>
#include <mutex>
#include <ctime>

namespace WYD {
    class SistemaBanimento {
    private:
        struct BanData {
            DWORD id;
            DWORD characterId;
            std::string characterName;
            DWORD adminId;
            std::string adminName;
            std::string reason;
            time_t startTime;
            time_t endTime;
            bool isPermanent;
            bool isActive;
        };

        struct BanHistory {
            DWORD characterId;
            std::vector<BanData> bans;
        };

        std::unordered_map<DWORD, BanHistory> banDatabase;
        std::mutex banMutex;

        // Callbacks
        std::function<void(DWORD, const BanData&)> banCallback;
        std::function<void(DWORD)> unbanCallback;

    public:
        SistemaBanimento() = default;
        ~SistemaBanimento() = default;

        // Gerenciamento de banimentos
        bool banCharacter(DWORD characterId, const std::string& characterName, DWORD adminId, const std::string& adminName, const std::string& reason, time_t duration) {
            std::lock_guard<std::mutex> lock(banMutex);

            // Verificar se já está banido
            if (isCharacterBanned(characterId)) {
                return false;
            }

            // Criar banimento
            BanData ban{};
            ban.id = generateBanId();
            ban.characterId = characterId;
            ban.characterName = characterName;
            ban.adminId = adminId;
            ban.adminName = adminName;
            ban.reason = reason;
            ban.startTime = time(nullptr);
            ban.endTime = (duration > 0) ? ban.startTime + duration : 0;
            ban.isPermanent = (duration == 0);
            ban.isActive = true;

            // Adicionar ao histórico
            banDatabase[characterId].bans.push_back(ban);

            // Chamar callback
            if (banCallback) {
                banCallback(characterId, ban);
            }

            return true;
        }

        bool unbanCharacter(DWORD characterId, DWORD adminId, const std::string& adminName) {
            std::lock_guard<std::mutex> lock(banMutex);

            auto it = banDatabase.find(characterId);
            if (it == banDatabase.end()) {
                return false;
            }

            auto& history = it->second;
            bool found = false;

            // Procurar banimento ativo
            for (auto& ban : history.bans) {
                if (ban.isActive) {
                    ban.isActive = false;
                    ban.endTime = time(nullptr);
                    found = true;
                    break;
                }
            }

            if (!found) {
                return false;
            }

            // Chamar callback
            if (unbanCallback) {
                unbanCallback(characterId);
            }

            return true;
        }

        // Verificações
        bool isCharacterBanned(DWORD characterId) const {
            auto it = banDatabase.find(characterId);
            if (it == banDatabase.end()) {
                return false;
            }

            const auto& history = it->second;
            time_t currentTime = time(nullptr);

            // Verificar banimentos ativos
            for (const auto& ban : history.bans) {
                if (ban.isActive) {
                    if (ban.isPermanent || currentTime < ban.endTime) {
                        return true;
                    }
                }
            }

            return false;
        }

        BanData getActiveBan(DWORD characterId) const {
            auto it = banDatabase.find(characterId);
            if (it == banDatabase.end()) {
                return BanData{};
            }

            const auto& history = it->second;
            time_t currentTime = time(nullptr);

            // Procurar banimento ativo
            for (const auto& ban : history.bans) {
                if (ban.isActive && (ban.isPermanent || currentTime < ban.endTime)) {
                    return ban;
                }
            }

            return BanData{};
        }

        // Histórico
        std::vector<BanData> getBanHistory(DWORD characterId) const {
            auto it = banDatabase.find(characterId);
            return (it != banDatabase.end()) ? it->second.bans : std::vector<BanData>();
        }

        // Callbacks
        void registerBanCallback(std::function<void(DWORD, const BanData&)> callback) {
            banCallback = callback;
        }

        void registerUnbanCallback(std::function<void(DWORD)> callback) {
            unbanCallback = callback;
        }

        // Getters
        size_t getActiveBanCount() const {
            size_t count = 0;
            time_t currentTime = time(nullptr);

            for (const auto& history : banDatabase) {
                for (const auto& ban : history.second.bans) {
                    if (ban.isActive && (ban.isPermanent || currentTime < ban.endTime)) {
                        count++;
                    }
                }
            }

            return count;
        }

        std::vector<BanData> getAllActiveBans() const {
            std::vector<BanData> activeBans;
            time_t currentTime = time(nullptr);

            for (const auto& history : banDatabase) {
                for (const auto& ban : history.second.bans) {
                    if (ban.isActive && (ban.isPermanent || currentTime < ban.endTime)) {
                        activeBans.push_back(ban);
                    }
                }
            }

            return activeBans;
        }

    private:
        DWORD generateBanId() {
            static DWORD nextId = 1;
            return nextId++;
        }
    };
} 