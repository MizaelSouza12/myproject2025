#pragma once

#include "../core/WYDTypes.h"
#include <unordered_map>
#include <vector>
#include <memory>
#include <string>
#include <functional>
#include <queue>
#include <mutex>
#include <algorithm>

namespace WYD {
    class SistemaRanking {
    private:
        struct RankingEntry {
            DWORD id;
            std::string name;
            DWORD score;
            DWORD level;
            BYTE classType;
            DWORD guildId;
            DWORD lastUpdateTime;
        };

        struct RankingCategory {
            BYTE id;
            std::string name;
            std::string description;
            bool isGlobal;
            bool isGuild;
            bool isClass;
            DWORD updateInterval;
            DWORD lastUpdateTime;
            std::vector<RankingEntry> entries;
        };

        std::unordered_map<BYTE, RankingCategory> categories;
        std::unordered_map<DWORD, std::vector<BYTE>> characterRankings; // characterId -> categories
        std::unordered_map<DWORD, std::vector<BYTE>> guildRankings; // guildId -> categories

        std::mutex rankingMutex;

        // Callbacks
        std::function<void(BYTE, DWORD)> rankingUpdateCallback;
        std::function<void(BYTE, DWORD)> rankingRewardCallback;

    public:
        SistemaRanking() {
            initializeDefaultCategories();
        }

        ~SistemaRanking() = default;

        // Inicialização
        void initializeDefaultCategories() {
            // Ranking Global
            RankingCategory global{};
            global.id = 0;
            global.name = "Global";
            global.description = "Ranking geral de todos os jogadores";
            global.isGlobal = true;
            global.updateInterval = 3600000; // 1 hora
            categories[0] = global;

            // Ranking de Guilda
            RankingCategory guild{};
            guild.id = 1;
            guild.name = "Guildas";
            guild.description = "Ranking de guildas";
            guild.isGuild = true;
            guild.updateInterval = 86400000; // 24 horas
            categories[1] = guild;

            // Ranking por Classe
            RankingCategory warrior{};
            warrior.id = 2;
            warrior.name = "Guerreiros";
            warrior.description = "Ranking de guerreiros";
            warrior.isClass = true;
            warrior.updateInterval = 3600000; // 1 hora
            categories[2] = warrior;

            RankingCategory mage{};
            mage.id = 3;
            mage.name = "Magos";
            mage.description = "Ranking de magos";
            mage.isClass = true;
            mage.updateInterval = 3600000; // 1 hora
            categories[3] = mage;

            RankingCategory archer{};
            archer.id = 4;
            archer.name = "Arqueiros";
            archer.description = "Ranking de arqueiros";
            archer.isClass = true;
            archer.updateInterval = 3600000; // 1 hora
            categories[4] = archer;
        }

        // Gerenciamento de categorias
        bool addCategory(const RankingCategory& category) {
            std::lock_guard<std::mutex> lock(rankingMutex);
            if (categories.find(category.id) != categories.end()) {
                return false;
            }

            categories[category.id] = category;
            return true;
        }

        bool removeCategory(BYTE categoryId) {
            std::lock_guard<std::mutex> lock(rankingMutex);
            return categories.erase(categoryId) > 0;
        }

        // Sistema de pontuação
        bool updateScore(DWORD characterId, const std::string& name, DWORD score, DWORD level, BYTE classType, DWORD guildId) {
            std::lock_guard<std::mutex> lock(rankingMutex);

            // Atualizar rankings globais
            for (auto& category : categories) {
                if (category.second.isGlobal) {
                    updateCategoryScore(category.first, characterId, name, score, level, classType, guildId);
                }
                else if (category.second.isClass && category.second.id == classType) {
                    updateCategoryScore(category.first, characterId, name, score, level, classType, guildId);
                }
            }

            // Atualizar rankings de guilda
            if (guildId > 0) {
                for (auto& category : categories) {
                    if (category.second.isGuild) {
                        updateGuildScore(category.first, guildId, score);
                    }
                }
            }

            return true;
        }

        // Sistema de recompensas
        bool distributeRewards(BYTE categoryId) {
            std::lock_guard<std::mutex> lock(rankingMutex);

            auto it = categories.find(categoryId);
            if (it == categories.end()) {
                return false;
            }

            auto& category = it->second;
            auto& entries = category.entries;

            // Ordenar por pontuação
            std::sort(entries.begin(), entries.end(),
                [](const RankingEntry& a, const RankingEntry& b) {
                    return a.score > b.score;
                });

            // Distribuir recompensas
            for (size_t i = 0; i < entries.size(); ++i) {
                if (i < 10) { // Top 10
                    if (rankingRewardCallback) {
                        rankingRewardCallback(categoryId, entries[i].id);
                    }
                }
            }

            return true;
        }

        // Callbacks
        void registerRankingUpdateCallback(std::function<void(BYTE, DWORD)> callback) {
            rankingUpdateCallback = callback;
        }

        void registerRankingRewardCallback(std::function<void(BYTE, DWORD)> callback) {
            rankingRewardCallback = callback;
        }

        // Getters
        const RankingCategory& getCategory(BYTE categoryId) const {
            return categories.at(categoryId);
        }

        std::vector<RankingEntry> getTopRankings(BYTE categoryId, size_t count) const {
            auto it = categories.find(categoryId);
            if (it == categories.end()) {
                return std::vector<RankingEntry>();
            }

            const auto& entries = it->second.entries;
            count = std::min(count, entries.size());

            return std::vector<RankingEntry>(entries.begin(), entries.begin() + count);
        }

        size_t getCharacterRank(BYTE categoryId, DWORD characterId) const {
            auto it = categories.find(categoryId);
            if (it == categories.end()) {
                return 0;
            }

            const auto& entries = it->second.entries;
            auto entryIt = std::find_if(entries.begin(), entries.end(),
                [characterId](const RankingEntry& e) {
                    return e.id == characterId;
                });

            if (entryIt == entries.end()) {
                return 0;
            }

            return std::distance(entries.begin(), entryIt) + 1;
        }

        std::vector<BYTE> getCharacterCategories(DWORD characterId) const {
            auto it = characterRankings.find(characterId);
            return (it != characterRankings.end()) ? it->second : std::vector<BYTE>();
        }

    private:
        void updateCategoryScore(BYTE categoryId, DWORD characterId, const std::string& name, DWORD score, DWORD level, BYTE classType, DWORD guildId) {
            auto& category = categories[categoryId];
            auto& entries = category.entries;

            // Procurar entrada existente
            auto it = std::find_if(entries.begin(), entries.end(),
                [characterId](const RankingEntry& e) {
                    return e.id == characterId;
                });

            if (it != entries.end()) {
                // Atualizar entrada existente
                it->score = score;
                it->level = level;
                it->lastUpdateTime = GetTickCount();
            }
            else {
                // Adicionar nova entrada
                RankingEntry entry{};
                entry.id = characterId;
                entry.name = name;
                entry.score = score;
                entry.level = level;
                entry.classType = classType;
                entry.guildId = guildId;
                entry.lastUpdateTime = GetTickCount();
                entries.push_back(entry);
            }

            // Ordenar por pontuação
            std::sort(entries.begin(), entries.end(),
                [](const RankingEntry& a, const RankingEntry& b) {
                    return a.score > b.score;
                });

            // Chamar callback
            if (rankingUpdateCallback) {
                rankingUpdateCallback(categoryId, characterId);
            }
        }

        void updateGuildScore(BYTE categoryId, DWORD guildId, DWORD score) {
            auto& category = categories[categoryId];
            auto& entries = category.entries;

            // Procurar entrada existente
            auto it = std::find_if(entries.begin(), entries.end(),
                [guildId](const RankingEntry& e) {
                    return e.id == guildId;
                });

            if (it != entries.end()) {
                // Atualizar entrada existente
                it->score = score;
                it->lastUpdateTime = GetTickCount();
            }
            else {
                // Adicionar nova entrada
                RankingEntry entry{};
                entry.id = guildId;
                entry.score = score;
                entry.lastUpdateTime = GetTickCount();
                entries.push_back(entry);
            }

            // Ordenar por pontuação
            std::sort(entries.begin(), entries.end(),
                [](const RankingEntry& a, const RankingEntry& b) {
                    return a.score > b.score;
                });

            // Chamar callback
            if (rankingUpdateCallback) {
                rankingUpdateCallback(categoryId, guildId);
            }
        }
    };
} 