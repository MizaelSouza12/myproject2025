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
    class SistemaPartidas {
    private:
        struct MatchPlayer {
            DWORD characterId;
            std::string name;
            BYTE level;
            BYTE classType;
            DWORD joinTime;
            bool isReady;
            bool isLeader;
        };

        struct MatchData {
            DWORD id;
            std::string name;
            BYTE type; // 0: PvP, 1: PvE, 2: Guild War, etc.
            BYTE minLevel;
            BYTE maxLevel;
            BYTE minPlayers;
            BYTE maxPlayers;
            DWORD creationTime;
            DWORD startTime;
            bool isActive;
            bool isPrivate;
            std::string password;
            std::vector<MatchPlayer> players;
        };

        struct MatchSettings {
            bool allowSameClass;
            bool requireTank;
            bool requireHealer;
            bool requireDPS;
            DWORD timeLimit;
            DWORD scoreLimit;
        };

        std::unordered_map<DWORD, MatchData> matchDatabase;
        std::unordered_map<DWORD, DWORD> characterMatches; // characterId -> matchId
        std::unordered_map<DWORD, MatchSettings> matchSettings;

        std::mutex matchMutex;

        // Callbacks
        std::function<void(DWORD, DWORD)> matchStartCallback;
        std::function<void(DWORD, DWORD)> matchEndCallback;
        std::function<void(DWORD, DWORD)> playerJoinCallback;
        std::function<void(DWORD, DWORD)> playerLeaveCallback;

    public:
        SistemaPartidas() = default;
        ~SistemaPartidas() = default;

        // Gerenciamento de partidas
        bool createMatch(const MatchData& match, const MatchSettings& settings) {
            std::lock_guard<std::mutex> lock(matchMutex);

            if (matchDatabase.find(match.id) != matchDatabase.end()) {
                return false;
            }

            matchDatabase[match.id] = match;
            matchSettings[match.id] = settings;
            return true;
        }

        bool removeMatch(DWORD matchId) {
            std::lock_guard<std::mutex> lock(matchMutex);

            auto it = matchDatabase.find(matchId);
            if (it == matchDatabase.end()) {
                return false;
            }

            // Remover jogadores
            for (const auto& player : it->second.players) {
                characterMatches.erase(player.characterId);
            }

            matchDatabase.erase(it);
            matchSettings.erase(matchId);
            return true;
        }

        // Participação em partidas
        bool joinMatch(DWORD matchId, DWORD characterId, const std::string& name, BYTE level, BYTE classType) {
            std::lock_guard<std::mutex> lock(matchMutex);

            auto matchIt = matchDatabase.find(matchId);
            if (matchIt == matchDatabase.end() || !matchIt->second.isActive) {
                return false;
            }

            auto& match = matchIt->second;

            // Verificar se já está em uma partida
            if (characterMatches.find(characterId) != characterMatches.end()) {
                return false;
            }

            // Verificar limite de jogadores
            if (match.players.size() >= match.maxPlayers) {
                return false;
            }

            // Verificar nível
            if (level < match.minLevel || level > match.maxLevel) {
                return false;
            }

            // Verificar classe
            if (!matchSettings[matchId].allowSameClass) {
                for (const auto& player : match.players) {
                    if (player.classType == classType) {
                        return false;
                    }
                }
            }

            // Adicionar jogador
            MatchPlayer player{};
            player.characterId = characterId;
            player.name = name;
            player.level = level;
            player.classType = classType;
            player.joinTime = GetTickCount();
            player.isReady = false;
            player.isLeader = match.players.empty(); // Primeiro jogador é o líder

            match.players.push_back(player);
            characterMatches[characterId] = matchId;

            // Chamar callback
            if (playerJoinCallback) {
                playerJoinCallback(matchId, characterId);
            }

            return true;
        }

        bool leaveMatch(DWORD matchId, DWORD characterId) {
            std::lock_guard<std::mutex> lock(matchMutex);

            auto& match = matchDatabase[matchId];
            
            auto it = std::find_if(match.players.begin(), match.players.end(),
                [characterId](const MatchPlayer& p) {
                    return p.characterId == characterId;
                });

            if (it == match.players.end()) {
                return false;
            }

            bool wasLeader = it->isLeader;
            match.players.erase(it);
            characterMatches.erase(characterId);

            // Se o líder saiu, designar novo líder
            if (wasLeader && !match.players.empty()) {
                match.players[0].isLeader = true;
            }

            // Se não houver mais jogadores, remover partida
            if (match.players.empty()) {
                removeMatch(matchId);
            }

            // Chamar callback
            if (playerLeaveCallback) {
                playerLeaveCallback(matchId, characterId);
            }

            return true;
        }

        // Sistema de prontidão
        bool setPlayerReady(DWORD matchId, DWORD characterId, bool ready) {
            std::lock_guard<std::mutex> lock(matchMutex);

            auto& match = matchDatabase[matchId];
            
            auto it = std::find_if(match.players.begin(), match.players.end(),
                [characterId](const MatchPlayer& p) {
                    return p.characterId == characterId;
                });

            if (it == match.players.end()) {
                return false;
            }

            it->isReady = ready;

            // Verificar se todos estão prontos
            if (ready && allPlayersReady(match)) {
                startMatch(matchId);
            }

            return true;
        }

        // Sistema de início/fim de partida
        bool startMatch(DWORD matchId) {
            std::lock_guard<std::mutex> lock(matchMutex);

            auto& match = matchDatabase[matchId];
            
            if (!match.isActive || match.players.size() < match.minPlayers) {
                return false;
            }

            match.startTime = GetTickCount();

            // Chamar callback
            if (matchStartCallback) {
                matchStartCallback(matchId, 0);
            }

            return true;
        }

        bool endMatch(DWORD matchId) {
            std::lock_guard<std::mutex> lock(matchMutex);

            auto it = matchDatabase.find(matchId);
            if (it == matchDatabase.end()) {
                return false;
            }

            // Remover jogadores
            for (const auto& player : it->second.players) {
                characterMatches.erase(player.characterId);
            }

            // Chamar callback
            if (matchEndCallback) {
                matchEndCallback(matchId, 0);
            }

            matchDatabase.erase(it);
            matchSettings.erase(matchId);
            return true;
        }

        // Callbacks
        void registerMatchStartCallback(std::function<void(DWORD, DWORD)> callback) {
            matchStartCallback = callback;
        }

        void registerMatchEndCallback(std::function<void(DWORD, DWORD)> callback) {
            matchEndCallback = callback;
        }

        void registerPlayerJoinCallback(std::function<void(DWORD, DWORD)> callback) {
            playerJoinCallback = callback;
        }

        void registerPlayerLeaveCallback(std::function<void(DWORD, DWORD)> callback) {
            playerLeaveCallback = callback;
        }

        // Getters
        const MatchData& getMatchData(DWORD matchId) const {
            return matchDatabase.at(matchId);
        }

        const MatchSettings& getMatchSettings(DWORD matchId) const {
            return matchSettings.at(matchId);
        }

        DWORD getCharacterMatch(DWORD characterId) const {
            auto it = characterMatches.find(characterId);
            return (it != characterMatches.end()) ? it->second : 0;
        }

        std::vector<DWORD> getActiveMatches() const {
            std::vector<DWORD> active;
            for (const auto& match : matchDatabase) {
                if (match.second.isActive) {
                    active.push_back(match.first);
                }
            }
            return active;
        }

    private:
        bool allPlayersReady(const MatchData& match) {
            return std::all_of(match.players.begin(), match.players.end(),
                [](const MatchPlayer& p) {
                    return p.isReady;
                });
        }
    };
} 