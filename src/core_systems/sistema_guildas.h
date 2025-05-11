#pragma once

#include "../core/WYDTypes.h"
#include <unordered_map>
#include <vector>
#include <memory>
#include <string>

namespace WYD {
    class SistemaGuildas {
    private:
        struct GuildMember {
            DWORD characterId;
            BYTE rank;
            DWORD joinDate;
            DWORD lastLogin;
            DWORD contribution;
        };

        struct GuildRank {
            BYTE level;
            std::string name;
            std::vector<DWORD> permissions;
        };

        struct GuildData {
            DWORD id;
            std::string name;
            std::string description;
            DWORD leaderId;
            DWORD creationDate;
            DWORD level;
            DWORD experience;
            DWORD gold;
            std::vector<GuildMember> members;
            std::vector<GuildRank> ranks;
            std::vector<DWORD> allies;
            std::vector<DWORD> enemies;
        };

        struct GuildState {
            bool isActive;
            DWORD lastUpdateTime;
            DWORD lastWarTime;
            DWORD lastAllianceTime;
        };

        std::unordered_map<DWORD, GuildData> guildDatabase;
        std::unordered_map<DWORD, GuildState> guildStates;
        std::unordered_map<DWORD, DWORD> characterGuilds; // characterId -> guildId

    public:
        SistemaGuildas() = default;
        ~SistemaGuildas() = default;

        // Inicialização
        void initializeGuildSystem() {
            // Inicializar ranks padrão
            initializeDefaultRanks();
        }

        // Gerenciamento de guildas
        bool createGuild(DWORD leaderId, const std::string& name, const std::string& description) {
            // Verificar se o personagem já está em uma guilda
            if (characterGuilds.find(leaderId) != characterGuilds.end()) {
                return false;
            }

            // Criar nova guilda
            GuildData guild{};
            guild.id = generateGuildId();
            guild.name = name;
            guild.description = description;
            guild.leaderId = leaderId;
            guild.creationDate = GetTickCount();
            guild.level = 1;
            guild.experience = 0;
            guild.gold = 0;

            // Adicionar líder como primeiro membro
            GuildMember leader{};
            leader.characterId = leaderId;
            leader.rank = 0; // Rank mais alto
            leader.joinDate = GetTickCount();
            leader.lastLogin = GetTickCount();
            leader.contribution = 0;
            guild.members.push_back(leader);

            // Inicializar ranks
            guild.ranks = getDefaultRanks();

            // Salvar guilda
            guildDatabase[guild.id] = guild;
            characterGuilds[leaderId] = guild.id;

            // Inicializar estado
            GuildState state{};
            state.isActive = true;
            state.lastUpdateTime = GetTickCount();
            state.lastWarTime = 0;
            state.lastAllianceTime = 0;
            guildStates[guild.id] = state;

            return true;
        }

        bool disbandGuild(DWORD guildId, DWORD leaderId) {
            auto it = guildDatabase.find(guildId);
            if (it == guildDatabase.end() || it->second.leaderId != leaderId) {
                return false;
            }

            // Remover membros
            for (const auto& member : it->second.members) {
                characterGuilds.erase(member.characterId);
            }

            // Remover guilda
            guildDatabase.erase(it);
            guildStates.erase(guildId);

            return true;
        }

        // Gerenciamento de membros
        bool addMember(DWORD guildId, DWORD characterId, BYTE rank) {
            // Verificar se o personagem já está em uma guilda
            if (characterGuilds.find(characterId) != characterGuilds.end()) {
                return false;
            }

            auto& guild = guildDatabase[guildId];
            
            // Verificar limite de membros
            if (guild.members.size() >= getMaxMembers(guild.level)) {
                return false;
            }

            // Adicionar novo membro
            GuildMember member{};
            member.characterId = characterId;
            member.rank = rank;
            member.joinDate = GetTickCount();
            member.lastLogin = GetTickCount();
            member.contribution = 0;
            guild.members.push_back(member);

            characterGuilds[characterId] = guildId;
            return true;
        }

        bool removeMember(DWORD guildId, DWORD characterId) {
            auto& guild = guildDatabase[guildId];
            
            // Encontrar e remover membro
            auto it = std::find_if(guild.members.begin(), guild.members.end(),
                [characterId](const GuildMember& member) {
                    return member.characterId == characterId;
                });

            if (it == guild.members.end()) {
                return false;
            }

            guild.members.erase(it);
            characterGuilds.erase(characterId);
            return true;
        }

        bool changeMemberRank(DWORD guildId, DWORD characterId, BYTE newRank) {
            auto& guild = guildDatabase[guildId];
            
            auto it = std::find_if(guild.members.begin(), guild.members.end(),
                [characterId](const GuildMember& member) {
                    return member.characterId == characterId;
                });

            if (it == guild.members.end()) {
                return false;
            }

            it->rank = newRank;
            return true;
        }

        // Sistema de experiência e nível
        bool addGuildExperience(DWORD guildId, DWORD amount) {
            auto& guild = guildDatabase[guildId];
            guild.experience += amount;

            // Verificar level up
            DWORD expNeeded = calculateExpNeeded(guild.level);
            if (guild.experience >= expNeeded) {
                return levelUpGuild(guildId);
            }
            return false;
        }

        bool levelUpGuild(DWORD guildId) {
            auto& guild = guildDatabase[guildId];
            guild.level++;
            guild.experience = 0;
            return true;
        }

        // Sistema de alianças
        bool createAlliance(DWORD guildId1, DWORD guildId2) {
            auto& guild1 = guildDatabase[guildId1];
            auto& guild2 = guildDatabase[guildId2];

            // Verificar se já são aliadas
            if (std::find(guild1.allies.begin(), guild1.allies.end(), guildId2) != guild1.allies.end()) {
                return false;
            }

            // Adicionar aliança
            guild1.allies.push_back(guildId2);
            guild2.allies.push_back(guildId1);

            // Atualizar estado
            guildStates[guildId1].lastAllianceTime = GetTickCount();
            guildStates[guildId2].lastAllianceTime = GetTickCount();

            return true;
        }

        bool breakAlliance(DWORD guildId1, DWORD guildId2) {
            auto& guild1 = guildDatabase[guildId1];
            auto& guild2 = guildDatabase[guildId2];

            // Remover aliança
            auto it1 = std::find(guild1.allies.begin(), guild1.allies.end(), guildId2);
            auto it2 = std::find(guild2.allies.begin(), guild2.allies.end(), guildId1);

            if (it1 != guild1.allies.end()) {
                guild1.allies.erase(it1);
            }
            if (it2 != guild2.allies.end()) {
                guild2.allies.erase(it2);
            }

            return true;
        }

        // Sistema de guerra
        bool declareWar(DWORD guildId1, DWORD guildId2) {
            auto& guild1 = guildDatabase[guildId1];
            auto& guild2 = guildDatabase[guildId2];

            // Verificar se já estão em guerra
            if (std::find(guild1.enemies.begin(), guild1.enemies.end(), guildId2) != guild1.enemies.end()) {
                return false;
            }

            // Adicionar guerra
            guild1.enemies.push_back(guildId2);
            guild2.enemies.push_back(guildId1);

            // Atualizar estado
            guildStates[guildId1].lastWarTime = GetTickCount();
            guildStates[guildId2].lastWarTime = GetTickCount();

            return true;
        }

        bool endWar(DWORD guildId1, DWORD guildId2) {
            auto& guild1 = guildDatabase[guildId1];
            auto& guild2 = guildDatabase[guildId2];

            // Remover guerra
            auto it1 = std::find(guild1.enemies.begin(), guild1.enemies.end(), guildId2);
            auto it2 = std::find(guild2.enemies.begin(), guild2.enemies.end(), guildId1);

            if (it1 != guild1.enemies.end()) {
                guild1.enemies.erase(it1);
            }
            if (it2 != guild2.enemies.end()) {
                guild2.enemies.erase(it2);
            }

            return true;
        }

        // Getters
        const GuildData& getGuildData(DWORD guildId) const {
            return guildDatabase.at(guildId);
        }

        DWORD getCharacterGuild(DWORD characterId) const {
            auto it = characterGuilds.find(characterId);
            return (it != characterGuilds.end()) ? it->second : 0;
        }

        bool isGuildActive(DWORD guildId) const {
            auto it = guildStates.find(guildId);
            return (it != guildStates.end()) ? it->second.isActive : false;
        }

    private:
        void initializeDefaultRanks() {
            // Implementar inicialização dos ranks padrão
        }

        std::vector<GuildRank> getDefaultRanks() {
            std::vector<GuildRank> ranks;
            // Implementar ranks padrão
            return ranks;
        }

        DWORD generateGuildId() {
            static DWORD nextId = 1;
            return nextId++;
        }

        size_t getMaxMembers(BYTE guildLevel) {
            return 20 + (guildLevel * 5); // Fórmula simples para exemplo
        }

        DWORD calculateExpNeeded(BYTE level) {
            return level * 1000; // Fórmula simples para exemplo
        }
    };
} 