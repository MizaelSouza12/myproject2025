/**
 * WYDBR 2.0 - Implementação Gerada Automaticamente
 * Arquivo origem: sistema_guildas_completo.txt
 * 
 * Este arquivo contém a definição C++ do sistema, encapsulada em JavaScript
 * para integração no projeto WYDBR 2.0.
 */

// Exporta o código C++ como uma string para ser processado pelo compilador C++
module.exports.cppCode = `
// GuildSystem.h - Sistema de Guildas Completo
#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <mutex>
#include <shared_mutex>
#include <atomic>
#include <chrono>
#include <thread>
#include <functional>
#include <memory>
#include <optional>
#include <nlohmann/json.hpp>
#include "PlayerManager.h"
#include "PermissionSystem.h"

namespace WYDBR {
namespace Guild {

using json = nlohmann::json;
using namespace std::chrono_literals;

enum class GuildRank {
    LEADER,
    CO_LEADER,
    ELDER,
    OFFICER,
    VETERAN,
    MEMBER,
    INITIATE,
    CUSTOM
};

enum class GuildPermission {
    INVITE_MEMBERS,
    KICK_MEMBERS,
    PROMOTE_MEMBERS,
    DEMOTE_MEMBERS,
    EDIT_RANKS,
    EDIT_GUILD_INFO,
    WITHDRAW_RESOURCES,
    DEPOSIT_RESOURCES,
    DECLARE_WAR,
    NEGOTIATE_ALLIANCES,
    MANAGE_GUILD_BANK,
    MANAGE_GUILD_TERRITORY,
    ASSIGN_PERMISSIONS,
    DISBAND_GUILD,
    VIEW_LOGS,
    START_GUILD_EVENTS,
    CUSTOM
};

enum class GuildPropertyType {
    HEADQUARTERS,
    OUTPOST,
    TRADE_POST,
    RESOURCE_NODE,
    TROPHY_HALL,
    GUILD_BANK,
    TRAINING_GROUNDS,
    RESEARCH_LAB,
    FORTIFICATION,
    CUSTOM
};

enum class DiplomacyStatus {
    NEUTRAL,
    ALLIED,
    FRIENDLY,
    HOSTILE,
    AT_WAR,
    NON_AGGRESSION_PACT,
    TRADE_PARTNER,
    VASSAL,
    OVERLORD
};

enum class GuildQuestType {
    HUNTING,
    GATHERING,
    CRAFTING,
    EXPLORATION,
    DEFENSE,
    CONQUEST,
    DIPLOMACY,
    TRAINING,
    CUSTOM
};

struct GuildRankDefinition {
    std::string rankId;
    std::string name;
    GuildRank type;
    uint32_t precedence;
    std::set<GuildPermission> permissions;
    uint32_t maxMembers{0}; // 0 = unlimited
    uint32_t minContribution{0};
    uint32_t minDaysInGuild{0};
    json rankBenefits;
    bool isCustom{false};
};

struct GuildMember {
    std::string playerId;
    std::string rankId;
    std::chrono::system_clock::time_point joinDate;
    uint64_t totalContribution{0};
    uint64_t weeklyContribution{0};
    std::chrono::system_clock::time_point lastActive;
    std::vector<std::string> roles; // Optional specialized roles
    std::unordered_map<std::string, uint64_t> contributionByType;
    std::unordered_map<std::string, bool> permissions; // Custom permissions
    std::optional<std::string> note;
    json memberActivity;
};

struct Guild {
    std::string guildId;
    std::string name;
    std::string tag;
    std::string leaderId;
    std::string description;
    std::optional<std::string> emblemAssetId;
    std::optional<std::string> bannerAssetId;
    std::chrono::system_clock::time_point foundedDate;
    uint32_t level{1};
    uint64_t experience{0};
    uint64_t gold{0};
    bool isRecruiting{true};
    bool isPublic{true};
    uint32_t maxMembers{50};
    uint32_t minLevelToJoin{1};
    std::vector<std::string> achievements;
    std::unordered_map<std::string, uint64_t> resources;
    std::vector<std::string> announcements;
    std::string primaryFocus; // "PVP", "PVE", "CRAFTING", "TRADING", etc.
    std::string language;
    json guildStats;
};

struct GuildProperty {
    std::string propertyId;
    std::string guildId;
    GuildPropertyType type;
    std::string name;
    std::string location;
    uint32_t level{1};
    std::chrono::system_clock::time_point acquiredDate;
    std::unordered_map<std::string, uint64_t> resourceGeneration; // resource -> amount per hour
    std::unordered_map<std::string, uint32_t> bonuses; // bonusType -> percentage
    uint32_t capacity{0};
    uint32_t defense{0};
    bool isActive{true};
    std::optional<std::chrono::system_clock::time_point> upgradeCompleteTime;
    json propertyFeatures;
};

struct GuildBank {
    std::string bankId;
    std::string guildId;
    
    struct BankTab {
        std::string tabId;
        std::string name;
        std::unordered_map<GuildRank, bool> viewPermissions;
        std::unordered_map<GuildRank, bool> depositPermissions;
        std::unordered_map<GuildRank, bool> withdrawPermissions;
        std::vector<std::pair<std::string, uint32_t>> items; // itemId -> quantity
        uint32_t capacity{100};
        std::vector<std::string> logs;
    };
    
    std::vector<BankTab> tabs;
    std::unordered_map<std::string, uint64_t> currencyBalances;
    std::vector<std::string> recentTransactions;
    std::chrono::system_clock::time_point lastUpdated;
};

struct GuildQuest {
    std::string questId;
    std::string guildId;
    std::string name;
    std::string description;
    GuildQuestType type;
    std::unordered_map<std::string, uint64_t> objectives; // objectiveId -> quantity
    std::unordered_map<std::string, uint64_t> progress; // objectiveId -> current progress
    std::unordered_map<std::string, std::set<std::string>> contributorsByObjective; // objectiveId -> set of playerIds
    uint64_t experienceReward{0};
    std::unordered_map<std::string, uint64_t> resourceRewards; // resourceId -> amount
    uint32_t prestigeReward{0};
    json itemRewards;
    std::chrono::system_clock::time_point startTime;
    std::optional<std::chrono::system_clock::time_point> completionTime;
    std::optional<std::chrono::system_clock::time_point> expirationTime;
    json questData;
};

struct GuildAlliance {
    std::string allianceId;
    std::string guildId1;
    std::string guildId2;
    DiplomacyStatus status;
    std::chrono::system_clock::time_point establishedDate;
    std::optional<std::chrono::system_clock::time_point> expirationDate;
    std::string terms;
    bool mutualDefense{false};
    bool resourceSharing{false};
    bool openBorders{false};
    std::vector<std::string> diplomaticEvents;
    json allianceDetails;
};

struct GuildLog {
    std::string logId;
    std::string guildId;
    std::string category; // "MEMBERSHIP", "ADMIN", "RESOURCES", "QUEST", "ALLIANCE", "WAR", etc.
    std::string action;
    std::string actorId;
    std::optional<std::string> targetId;
    std::chrono::system_clock::time_point timestamp;
    json details;
};

class GuildSystem {
public:
    // Singleton
    static GuildSystem& getInstance();
    
    // Inicialização
    bool initialize(const json& config);
    bool start();
    void shutdown();
    
    // Gerenciamento de guildas
    std::string createGuild(const std::string& name, 
                          const std::string& tag, 
                          const std::string& leaderId,
                          const std::string& description = "");
    
    bool disbandGuild(const std::string& guildId, const std::string& leaderId);
    bool updateGuildInfo(const std::string& guildId, const json& updateData);
    Guild getGuild(const std::string& guildId) const;
    std::vector<Guild> getAllGuilds() const;
    std::vector<Guild> searchGuilds(const std::string& query) const;
    
    // Membros
    bool addGuildMember(const std::string& guildId, const std::string& playerId, GuildRank rank = GuildRank::INITIATE);
    bool removeGuildMember(const std::string& guildId, const std::string& playerId, const std::string& removedById, const std::string& reason = "");
    bool updateMemberRank(const std::string& guildId, const std::string& playerId, const std::string& rankId);
    GuildMember getGuildMember(const std::string& guildId, const std::string& playerId) const;
    std::vector<GuildMember> getGuildMembers(const std::string& guildId) const;
    std::vector<GuildMember> getMembersWithRank(const std::string& guildId, const std::string& rankId) const;
    
    // Contribuição e níveis
    bool addGuildExperience(const std::string& guildId, uint64_t experience);
    bool addMemberContribution(const std::string& guildId, const std::string& playerId, uint64_t amount, const std::string& contributionType = "general");
    uint32_t calculateGuildLevel(uint64_t experience) const;
    uint64_t getExperienceForLevel(uint32_t level) const;
    std::unordered_map<std::string, uint64_t> getTopContributors(const std::string& guildId, uint32_t limit = 10, const std::string& period = "all") const;
    
    // Hierarquias e permissões
    std::string createGuildRank(const std::string& guildId, const GuildRankDefinition& rank);
    bool updateGuildRank(const std::string& guildId, const GuildRankDefinition& rank);
    bool deleteGuildRank(const std::string& guildId, const std::string& rankId);
    GuildRankDefinition getGuildRank(const std::string& guildId, const std::string& rankId) const;
    std::vector<GuildRankDefinition> getGuildRanks(const std::string& guildId) const;
    
    bool setCustomMemberPermissions(const std::string& guildId, 
                                  const std::string& playerId,
                                  const std::unordered_map<std::string, bool>& permissions);
    
    bool hasPermission(const std::string& guildId, const std::string& playerId, GuildPermission permission) const;
    
    // Propriedades de guilda
    std::string createGuildProperty(const std::string& guildId, const GuildProperty& property);
    bool updateGuildProperty(const std::string& guildId, const GuildProperty& property);
    bool removeGuildProperty(const std::string& guildId, const std::string& propertyId);
    GuildProperty getGuildProperty(const std::string& propertyId) const;
    std::vector<GuildProperty> getGuildProperties(const std::string& guildId) const;
    std::vector<GuildProperty> getPropertiesByType(const std::string& guildId, GuildPropertyType type) const;
    
    // Recursos de guilda
    bool addGuildResource(const std::string& guildId, const std::string& resourceId, uint64_t amount);
    bool removeGuildResource(const std::string& guildId, const std::string& resourceId, uint64_t amount);
    uint64_t getGuildResourceAmount(const std::string& guildId, const std::string& resourceId) const;
    std::unordered_map<std::string, uint64_t> getAllGuildResources(const std::string& guildId) const;
    
    // Banco de guilda
    GuildBank getGuildBank(const std::string& guildId) const;
    std::string createBankTab(const std::string& guildId, const std::string& name);
    bool depositItemToBank(const std::string& guildId, 
                         const std::string& tabId,
                         const std::string& playerId,
                         const std::string& itemId,
                         uint32_t quantity);
    
    bool withdrawItemFromBank(const std::string& guildId,
                            const std::string& tabId,
                            const std::string& playerId,
                            const std::string& itemId,
                            uint32_t quantity);
    
    bool setBankTabPermissions(const std::string& guildId,
                             const std::string& tabId,
                             GuildRank rank,
                             bool canView,
                             bool canDeposit,
                             bool canWithdraw);
    
    // Benefícios de guilda
    json getGuildBenefits(const std::string& guildId) const;
    json getMemberBenefits(const std::string& guildId, const std::string& playerId) const;
    bool unlockGuildBenefit(const std::string& guildId, const std::string& benefitId);
    bool applyGuildBenefitToMember(const std::string& guildId, const std::string& playerId, const std::string& benefitId);
    
    // Missões de guilda
    std::string createGuildQuest(const std::string& guildId, const GuildQuest& quest);
    bool updateGuildQuestProgress(const std::string& questId, 
                                const std::string& objectiveId,
                                const std::string& playerId,
                                uint64_t progress);
    
    bool completeGuildQuest(const std::string& questId);
    GuildQuest getGuildQuest(const std::string& questId) const;
    std::vector<GuildQuest> getActiveGuildQuests(const std::string& guildId) const;
    std::vector<GuildQuest> getCompletedGuildQuests(const std::string& guildId, uint32_t limit = 10) const;
    
    // Sistema de aliança e diplomacia
    std::string createAlliance(const std::string& guildId1, 
                             const std::string& guildId2,
                             DiplomacyStatus status);
    
    bool updateAllianceStatus(const std::string& allianceId, DiplomacyStatus newStatus);
    bool terminateAlliance(const std::string& allianceId);
    GuildAlliance getAlliance(const std::string& allianceId) const;
    GuildAlliance getAllianceBetweenGuilds(const std::string& guildId1, const std::string& guildId2) const;
    std::vector<GuildAlliance> getGuildAlliances(const std::string& guildId) const;
    DiplomacyStatus getDiplomacyStatus(const std::string& guildId1, const std::string& guildId2) const;
    
    // Logs e registros
    std::string addGuildLog(const std::string& guildId,
                          const std::string& category,
                          const std::string& action,
                          const std::string& actorId,
                          const std::optional<std::string>& targetId = std::nullopt,
                          const json& details = json());
    
    std::vector<GuildLog> getGuildLogs(const std::string& guildId, uint32_t limit = 100) const;
    std::vector<GuildLog> getGuildLogsByCategory(const std::string& guildId, const std::string& category, uint32_t limit = 100) const;
    
    // Callbacks
    void registerGuildCreatedCallback(std::function<void(const Guild&)> callback);
    void registerGuildMemberAddedCallback(std::function<void(const std::string&, const std::string&)> callback);
    void registerGuildMemberRemovedCallback(std::function<void(const std::string&, const std::string&)> callback);
    void registerAllianceStatusChangedCallback(std::function<void(const std::string&, const std::string&, DiplomacyStatus)> callback);
    
private:
    GuildSystem();
    ~GuildSystem();
    
    // Atributos privados
    static std::unique_ptr<GuildSystem> s_instance;
    static std::once_flag s_onceFlag;
    
    std::shared_ptr<Player::PlayerManager> m_playerManager;
    std::shared_ptr<Auth::PermissionSystem> m_permissionSystem;
    
    // Guildas
    std::unordered_map<std::string, Guild> m_guilds;
    mutable std::shared_mutex m_guildsMutex;
    
    // Membros
    std::unordered_map<std::string, std::unordered_map<std::string, GuildMember>> m_guildMembers; // guildId -> (playerId -> member)
    std::unordered_map<std::string, std::string> m_playerGuilds; // playerId -> guildId
    mutable std::shared_mutex m_membersMutex;
    
    // Hierarquias
    std::unordered_map<std::string, std::unordered_map<std::string, GuildRankDefinition>> m_guildRanks; // guildId -> (rankId -> rank)
    mutable std::shared_mutex m_ranksMutex;
    
    // Propriedades
    std::unordered_map<std::string, GuildProperty> m_properties;
    std::unordered_map<std::string, std::vector<std::string>> m_guildProperties; // guildId -> [propertyIds]
    mutable std::shared_mutex m_propertiesMutex;
    
    // Banco
    std::unordered_map<std::string, GuildBank> m_guildBanks; // guildId -> bank
    mutable std::shared_mutex m_banksMutex;
    
    // Missões
    std::unordered_map<std::string, GuildQuest> m_quests;
    std::unordered_map<std::string, std::vector<std::string>> m_guildQuests; // guildId -> [questIds]
    std::set<std::string> m_activeQuestIds;
    std::set<std::string> m_completedQuestIds;
    mutable std::shared_mutex m_questsMutex;
    
    // Alianças
    std::unordered_map<std::string, GuildAlliance> m_alliances;
    std::unordered_map<std::string, std::vector<std::string>> m_guildAlliances; // guildId -> [allianceIds]
    mutable std::shared_mutex m_alliancesMutex;
    
    // Logs
    std::unordered_map<std::string, std::vector<GuildLog>> m_guildLogs; // guildId -> logs
    std::unordered_map<std::string, std::unordered_map<std::string, std::vector<size_t>>> m_logsByCategory; // guildId -> (category -> [indices])
    mutable std::shared_mutex m_logsMutex;
    
    // Callbacks registrados
    std::vector<std::function<void(const Guild&)>> m_guildCreatedCallbacks;
    std::vector<std::function<void(const std::string&, const std::string&)>> m_memberAddedCallbacks;
    std::vector<std::function<void(const std::string&, const std::string&)>> m_memberRemovedCallbacks;
    std::vector<std::function<void(const std::string&, const std::string&, DiplomacyStatus)>> m_allianceStatusChangedCallbacks;
    mutable std::mutex m_callbacksMutex;
    
    // Estatísticas
    struct GuildStats {
        std::atomic<uint32_t> totalGuilds{0};
        std::atomic<uint32_t> totalGuildMembers{0};
        std::atomic<uint32_t> totalActiveQuests{0};
        std::atomic<uint32_t> totalCompletedQuests{0};
        std::atomic<uint32_t> totalAlliances{0};
        std::atomic<uint32_t> totalBankTransactions{0};
        std::atomic<uint64_t> totalContributions{0};
    };
    
    GuildStats m_stats;
    
    // Configuração
    uint32_t m_minNameLength{3};
    uint32_t m_maxNameLength{32};
    uint32_t m_minTagLength{2};
    uint32_t m_maxTagLength{5};
    uint32_t m_initialMaxMembers{50};
    uint32_t m_maxGuildLevel{100};
    uint32_t m_baseExperiencePerLevel{1000};
    float m_experienceScalingFactor{1.5f};
    uint32_t m_maxGuildsPerPlayer{1};
    uint32_t m_defaultBankTabs{1};
    uint32_t m_maxBankTabs{10};
    uint32_t m_maxQuestsPerGuild{5};
    std::chrono::days m_defaultQuestDuration{7};
    std::chrono::days m_defaultAllianceDuration{30}; // 0 = permanent
    uint32_t m_maxLogRetention{1000};
    
    // Threads de trabalho
    std::atomic<bool> m_running{false};
    std::thread m_questProcessingThread;
    std::thread m_propertyProductionThread;
    std::thread m_weeklyResetThread;
    
    // Métodos privados
    std::string generateGuildId();
    std::string generateRankId();
    std::string generatePropertyId();
    std::string generateQuestId();
    std::string generateAllianceId();
    std::string generateBankTabId();
    std::string generateLogId();
    
    void questProcessingThread();
    void propertyProductionThread();
    void weeklyResetThread();
    
    bool validateGuildName(const std::string& name) const;
    bool validateGuildTag(const std::string& tag) const;
    bool isGuildNameTaken(const std::string& name) const;
    bool isGuildTagTaken(const std::string& tag) const;
    
    void setupDefaultRanks(const std::string& guildId);
    bool processGuildLevelUp(Guild& guild);
    void generateResourcesFromProperties();
    void applyPropertyBonuses();
    void updateQuestProgress();
    
    void resetWeeklyContributions();
    
    void notifyGuildCreated(const Guild& guild);
    void notifyMemberAdded(const std::string& guildId, const std::string& playerId);
    void notifyMemberRemoved(const std::string& guildId, const std::string& playerId);
    void notifyAllianceStatusChanged(const std::string& guildId1, const std::string& guildId2, DiplomacyStatus status);
    
    bool validateMemberOperation(const std::string& guildId, const std::string& actorId, const std::string& targetId, GuildPermission permission) const;
    
    void pruneOldLogs();
    void removeExpiredAlliances();
    void expireOldQuests();
};

// Implementação inline básica do singleton
inline GuildSystem& GuildSystem::getInstance() {
    std::call_once(s_onceFlag, [] {
        s_instance = std::unique_ptr<GuildSystem>(new GuildSystem());
    });
    return *s_instance;
}

} // namespace Guild
} // namespace WYDBR

// AllianceSystem.h - Sistema de Alianças e Diplomacia
#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <mutex>
#include <shared_mutex>
#include <chrono>
#include <memory>
#include <functional>
#include <optional>
#include <nlohmann/json.hpp>
#include "GuildSystem.h"

namespace WYDBR {
namespace Guild {

using json = nlohmann::json;

enum class AllianceType {
    DEFENSIVE,
    OFFENSIVE,
    ECONOMIC,
    COMPREHENSIVE,
    NON_AGGRESSION,
    VASSAL,
    CUSTOM
};

enum class AllianceRequestStatus {
    PENDING,
    ACCEPTED,
    REJECTED,
    CANCELED,
    EXPIRED
};

enum class DiplomaticAction {
    ALLIANCE_OFFER,
    PEACE_OFFER,
    DECLARATION_OF_WAR,
    TRADE_AGREEMENT,
    RESOURCE_SHARING,
    TERRITORY_EXCHANGE,
    DEMAND_TRIBUTE,
    OFFER_TRIBUTE,
    CUSTOM
};

struct AlliancePact {
    std::string pactId;
    std::string name;
    AllianceType type;
    std::vector<std::string> memberGuildIds;
    std::chrono::system_clock::time_point formedDate;
    std::optional<std::chrono::system_clock::time_point> expirationDate;
    std::string founderGuildId;
    std::string description;
    std::vector<std::string> terms;
    bool mutualDefense{false};
    bool mutualAggression{false};
    bool resourceSharing{false};
    bool territoryAccess{false};
    std::string status; // "ACTIVE", "DISSOLVED", "SUSPENDED"
    json allianceStats;
};

struct DiplomaticRelation {
    std::string relationId;
    std::string guildId1;
    std::string guildId2;
    DiplomacyStatus status;
    std::chrono::system_clock::time_point establishedDate;
    std::optional<std::chrono::system_clock::time_point> lastUpdated;
    int32_t relationValue{0}; // -100 to 100
    std::vector<std::string> openAgreements; // IDs of active agreements (e.g. trade, non-aggression)
    std::vector<std::string> historicalActions; // IDs of past diplomatic actions
    bool canDeclareWar{true};
    std::optional<std::chrono::system_clock::time_point> peaceTreatyExpiration;
    json relationDetails;
};

struct DiplomaticRequest {
    std::string requestId;
    std::string senderGuildId;
    std::string receiverGuildId;
    DiplomaticAction action;
    AllianceRequestStatus status;
    std::chrono::system_clock::time_point timestamp;
    std::optional<std::chrono::system_clock::time_point> responseTime;
    std::optional<std::chrono::system_clock::time_point> expirationTime;
    std::string message;
    json terms;
    std::optional<std::string> responseMessage;
    std::vector<std::string> witnesses; // Other guilds witnessing this agreement
    bool publicRequest{false};
};

struct DiplomaticEvent {
    std::string eventId;
    std::string primaryGuildId;
    std::optional<std::string> secondaryGuildId;
    std::string eventType; // "WAR_DECLARATION", "ALLIANCE_FORMED", "TREATY_SIGNED", etc.
    std::chrono::system_clock::time_point timestamp;
    int32_t worldImpact{0}; // How much this affects the world: 0-100
    int32_t reputationImpact{0}; // Reputation impact: -100 to 100
    std::string description;
    std::optional<std::string> relatedRequestId;
    bool isPublic{true};
    json eventDetails;
};

class AllianceSystem {
public:
    // Singleton
    static AllianceSystem& getInstance();
    
    // Inicialização
    bool initialize(const json& config);
    bool start();
    void shutdown();
    
    // Alianças
    std::string createAlliance(const std::string& founderGuildId, 
                             const std::string& name,
                             AllianceType type,
                             const std::string& description = "");
    
    bool addGuildToAlliance(const std::string& pactId, const std::string& guildId);
    bool removeGuildFromAlliance(const std::string& pactId, const std::string& guildId);
    bool dissolveAlliance(const std::string& pactId);
    AlliancePact getAlliance(const std::string& pactId) const;
    std::vector<AlliancePact> getAllAlliances() const;
    std::vector<AlliancePact> getGuildAlliances(const std::string& guildId) const;
    
    // Relações diplomáticas
    DiplomaticRelation getRelation(const std::string& guildId1, const std::string& guildId2) const;
    bool setRelationStatus(const std::string& guildId1, const std::string& guildId2, DiplomacyStatus status);
    bool modifyRelationValue(const std::string& guildId1, const std::string& guildId2, int32_t change);
    int32_t getRelationValue(const std::string& guildId1, const std::string& guildId2) const;
    std::vector<DiplomaticRelation> getGuildRelations(const std::string& guildId) const;
    
    // Requisições diplomáticas
    std::string createDiplomaticRequest(const std::string& senderGuildId,
                                      const std::string& receiverGuildId,
                                      DiplomaticAction action,
                                      const std::string& message = "",
                                      const json& terms = json(),
                                      bool publicRequest = false);
    
    bool respondToDiplomaticRequest(const std::string& requestId, 
                                  AllianceRequestStatus response,
                                  const std::string& responseMessage = "");
    
    DiplomaticRequest getDiplomaticRequest(const std::string& requestId) const;
    std::vector<DiplomaticRequest> getPendingRequests(const std::string& guildId) const;
    std::vector<DiplomaticRequest> getSentRequests(const std::string& guildId) const;
    
    // Eventos
    std::string recordDiplomaticEvent(const std::string& primaryGuildId,
                                    const std::string& eventType,
                                    const std::optional<std::string>& secondaryGuildId = std::nullopt,
                                    int32_t worldImpact = 0,
                                    int32_t reputationImpact = 0,
                                    const std::string& description = "",
                                    const json& details = json());
    
    DiplomaticEvent getDiplomaticEvent(const std::string& eventId) const;
    std::vector<DiplomaticEvent> getRecentEvents(uint32_t limit = 10) const;
    std::vector<DiplomaticEvent> getGuildEvents(const std::string& guildId, uint32_t limit = 10) const;
    
    // Análise e estatísticas
    json getWorldDiplomaticState() const;
    std::vector<std::string> getWarringGuilds() const;
    std::vector<std::pair<std::string, int32_t>> getGuildReputation() const;
    std::vector<std::pair<std::string, std::string>> getPotentialConflicts() const;
    
    // Callbacks
    void registerAllianceCreatedCallback(std::function<void(const AlliancePact&)> callback);
    void registerDiplomaticStatusChangedCallback(std::function<void(const std::string&, const std::string&, DiplomacyStatus)> callback);
    void registerDiplomaticRequestCallback(std::function<void(const DiplomaticRequest&)> callback);
    void registerDiplomaticEventCallback(std::function<void(const DiplomaticEvent&)> callback);
    
private:
    AllianceSystem();
    ~AllianceSystem();
    
    // Atributos privados
    static std::unique_ptr<AllianceSystem> s_instance;
    static std::once_flag s_onceFlag;
    
    std::shared_ptr<GuildSystem> m_guildSystem;
    
    // Alianças
    std::unordered_map<std::string, AlliancePact> m_alliances;
    std::unordered_map<std::string, std::vector<std::string>> m_guildAlliances; // guildId -> [allianceIds]
    mutable std::shared_mutex m_alliancesMutex;
    
    // Relações
    std::map<std::pair<std::string, std::string>, DiplomaticRelation> m_relations; // (guildId1, guildId2) -> relation
    std::unordered_map<std::string, std::vector<std::string>> m_guildRelations; // guildId -> [relationIds]
    mutable std::shared_mutex m_relationsMutex;
    
    // Requisições
    std::unordered_map<std::string, DiplomaticRequest> m_diplomaticRequests;
    std::unordered_map<std::string, std::vector<std::string>> m_pendingRequestsByGuild; // guildId -> [requestIds]
    std::unordered_map<std::string, std::vector<std::string>> m_sentRequestsByGuild; // guildId -> [requestIds]
    mutable std::shared_mutex m_requestsMutex;
    
    // Eventos
    std::vector<DiplomaticEvent> m_diplomaticEvents;
    std::unordered_map<std::string, std::vector<size_t>> m_eventsByGuild; // guildId -> [eventIndices]
    mutable std::shared_mutex m_eventsMutex;
    size_t m_maxEventHistory{1000};
    
    // Callbacks registrados
    std::vector<std::function<void(const AlliancePact&)>> m_allianceCreatedCallbacks;
    std::vector<std::function<void(const std::string&, const std::string&, DiplomacyStatus)>> m_statusChangedCallbacks;
    std::vector<std::function<void(const DiplomaticRequest&)>> m_requestCallbacks;
    std::vector<std::function<void(const DiplomaticEvent&)>> m_eventCallbacks;
    mutable std::mutex m_callbacksMutex;
    
    // Estatísticas e configuração
    int32_t m_maxRelationValue{100};
    int32_t m_minRelationValue{-100};
    std::chrono::days m_defaultRequestExpiry{7};
    std::chrono::days m_defaultPeaceTreatyDuration{30};
    int32_t m_warDeclarationReputationImpact{-20};
    int32_t m_peaceOfferReputationBonus{10};
    int32_t m_treatyViolationPenalty{-30};
    
    // Threads de trabalho
    std::atomic<bool> m_running{false};
    std::thread m_requestExpiryThread;
    std::thread m_relationUpdateThread;
    
    // Métodos privados
    std::string generatePactId();
    std::string generateRelationId();
    std::string generateRequestId();
    std::string generateEventId();
    
    void requestExpiryThread();
    void relationUpdateThread();
    
    bool validateNewAlliance(const std::string& founderGuildId, const std::string& name) const;
    bool canChangeRelationStatus(const std::string& guildId1, const std::string& guildId2, DiplomacyStatus newStatus) const;
    
    DiplomaticRelation ensureRelationExists(const std::string& guildId1, const std::string& guildId2);
    std::string normalizeGuildIds(std::string& guildId1, std::string& guildId2) const;
    
    void updateRelationBasedOnEvents(DiplomaticRelation& relation);
    void applyStatusEffects(const std::string& guildId1, const std::string& guildId2, DiplomacyStatus status);
    
    bool processRequestResponse(DiplomaticRequest& request, AllianceRequestStatus response);
    void purgeExpiredRequests();
    
    void notifyAllianceCreated(const AlliancePact& pact);
    void notifyDiplomaticStatusChanged(const std::string& guildId1, const std::string& guildId2, DiplomacyStatus status);
    void notifyDiplomaticRequest(const DiplomaticRequest& request);
    void notifyDiplomaticEvent(const DiplomaticEvent& event);
    
    void pruneOldEvents();
};

// Implementação inline básica do singleton
inline AllianceSystem& AllianceSystem::getInstance() {
    std::call_once(s_onceFlag, [] {
        s_instance = std::unique_ptr<AllianceSystem>(new AllianceSystem());
    });
    return *s_instance;
}

} // namespace Guild
} // namespace WYDBR

// GuildStorageSystem.h - Armazenamento e Propriedades de Guild
#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <mutex>
#include <shared_mutex>
#include <chrono>
#include <memory>
#include <functional>
#include <optional>
#include <nlohmann/json.hpp>
#include "GuildSystem.h"
#include "InventorySystem.h"

namespace WYDBR {
namespace Guild {

using json = nlohmann::json;

enum class StorageUpgradeType {
    CAPACITY,
    SECURITY,
    ORGANIZATION,
    PRESERVATION,
    ACCESS_CONTROL,
    CUSTOM
};

enum class PropertyStatus {
    OWNED,
    RENTED,
    UNDER_CONSTRUCTION,
    UNDER_SIEGE,
    DAMAGED,
    ABANDONED,
    FORECLOSED
};

struct GuildStorageTab {
    std::string tabId;
    std::string guildId;
    std::string name;
    std::string description;
    std::unordered_map<std::string, bool> rankViewPermissions; // rankId -> canView
    std::unordered_map<std::string, bool> rankDepositPermissions;
    std::unordered_map<std::string, bool> rankWithdrawPermissions;
    uint32_t capacity{100};
    uint32_t usedSpace{0};
    std::vector<std::pair<std::string, uint32_t>> items; // itemId -> quantity
    std::vector<std::pair<std::string, uint32_t>> stackLimits; // itemId -> maxStack
    std::vector<std::string> allowedItemTypes;
    std::vector<std::string> forbiddenItemTypes;
    std::chrono::system_clock::time_point lastAccessed;
    bool isLocked{false};
    json tabMetadata;
};

struct GuildStorageLog {
    std::string logId;
    std::string guildId;
    std::string tabId;
    std::string playerId;
    std::string action; // "DEPOSIT", "WITHDRAW", "CREATE_TAB", "DELETE_TAB", "MODIFY_PERMISSIONS", etc.
    std::chrono::system_clock::time_point timestamp;
    std::optional<std::string> itemId;
    std::optional<uint32_t> quantity;
    std::optional<std::string> targetRankId;
    std::optional<std::string> notes;
    json logDetails;
};

struct GuildPropertyDetails {
    std::string propertyId;
    std::string guildId;
    std::string name;
    PropertyStatus status;
    GuildPropertyType type;
    std::string location;
    uint32_t level{1};
    uint32_t defense{0};
    uint32_t maxDurability{100};
    uint32_t currentDurability{100};
    std::chrono::system_clock::time_point acquiredDate;
    std::optional<std::chrono::system_clock::time_point> expirationDate;
    std::unordered_map<std::string, uint64_t> resourceProduction; // resourceId -> amount per hour
    std::unordered_map<std::string, uint64_t> resourceUpkeep; // resourceId -> amount per hour
    std::unordered_map<std::string, uint64_t> upgradeCosts; // resourceId -> amount
    std::vector<std::string> upgrades;
    uint32_t upgradeTimeHours{0}; // If currently upgrading
    std::optional<std::chrono::system_clock::time_point> upgradeCompleteTime;
    bool underAttack{false};
    json propertyStats;
};

struct PropertyUpgrade {
    std::string upgradeId;
    std::string name;
    std::string description;
    GuildPropertyType applicableType;
    std::unordered_map<std::string, uint64_t> resourceCost;
    uint32_t goldCost{0};
    uint32_t upgradeTimeHours{24};
    uint32_t requiredGuildLevel{1};
    std::unordered_map<std::string, float> bonuses; // bonusType -> value
    std::vector<std::string> requiredUpgrades;
    json upgradeEffects;
};

struct PropertyTransaction {
    std::string transactionId;
    std::string propertyId;
    std::string guildId;
    std::string transactionType; // "PURCHASE", "SALE", "RENTAL", "UPGRADE", "REPAIR", etc.
    std::chrono::system_clock::time_point timestamp;
    uint64_t amount;
    std::string currency;
    std::optional<std::string> otherPartyId;
    std::optional<std::chrono::system_clock::time_point> expirationDate;
    std::string terms;
    bool successful{true};
    json transactionDetails;
};

class GuildStorageSystem {
public:
    // Singleton
    static GuildStorageSystem& getInstance();
    
    // Inicialização
    bool initialize(const json& config);
    bool start();
    void shutdown();
    
    // Gestão de tabs de armazenamento
    std::string createStorageTab(const std::string& guildId, 
                              const std::string& name,
                              const std::string& description = "");
    
    bool updateStorageTab(const GuildStorageTab& tab);
    bool deleteStorageTab(const std::string& guildId, const std::string& tabId);
    GuildStorageTab getStorageTab(const std::string& guildId, const std::string& tabId) const;
    std::vector<GuildStorageTab> getGuildStorageTabs(const std::string& guildId) const;
    
    // Permissões de armazenamento
    bool setTabRankPermissions(const std::string& guildId,
                             const std::string& tabId,
                             const std::string& rankId,
                             bool canView,
                             bool canDeposit,
                             bool canWithdraw);
    
    bool canAccessTab(const std::string& guildId, 
                    const std::string& tabId, 
                    const std::string& playerId, 
                    const std::string& accessType) const;
    
    // Operações de item
    bool depositItem(const std::string& guildId,
                   const std::string& tabId,
                   const std::string& playerId,
                   const std::string& itemId,
                   uint32_t quantity);
    
    bool withdrawItem(const std::string& guildId,
                    const std::string& tabId,
                    const std::string& playerId,
                    const std::string& itemId,
                    uint32_t quantity);
    
    bool transferBetweenTabs(const std::string& guildId,
                           const std::string& sourceTabId,
                           const std::string& targetTabId,
                           const std::string& playerId,
                           const std::string& itemId,
                           uint32_t quantity);
    
    // Consultas de itens
    uint32_t getItemQuantity(const std::string& guildId, 
                          const std::string& tabId, 
                          const std::string& itemId) const;
    
    std::vector<std::pair<std::string, uint32_t>> getTabItems(const std::string& guildId, 
                                                           const std::string& tabId) const;
    
    std::vector<std::pair<std::string, uint32_t>> findItemInGuildStorage(const std::string& guildId, 
                                                                      const std::string& itemId) const;
    
    // Logs de armazenamento
    std::vector<GuildStorageLog> getTabLogs(const std::string& guildId, 
                                          const std::string& tabId, 
                                          uint32_t limit = 100) const;
    
    std::vector<GuildStorageLog> getGuildStorageLogs(const std::string& guildId, 
                                                   uint32_t limit = 100) const;
    
    std::vector<GuildStorageLog> getPlayerStorageActions(const std::string& guildId, 
                                                       const std::string& playerId, 
                                                       uint32_t limit = 100) const;
    
    // Upgrades de armazenamento
    bool upgradeStorageTab(const std::string& guildId, 
                         const std::string& tabId, 
                         StorageUpgradeType upgradeType);
    
    std::unordered_map<StorageUpgradeType, uint32_t> getTabUpgradeLevels(const std::string& guildId, 
                                                                       const std::string& tabId) const;
    
    uint32_t getMaxTabCapacity(const std::string& guildId) const;
    
    // Propriedades detalhadas
    std::string createGuildProperty(const std::string& guildId, const GuildPropertyDetails& property);
    bool updateGuildProperty(const GuildPropertyDetails& property);
    bool sellGuildProperty(const std::string& propertyId, 
                         const std::string& buyerGuildId, 
                         uint64_t amount,
                         const std::string& currency = "gold");
    
    GuildPropertyDetails getPropertyDetails(const std::string& propertyId) const;
    std::vector<GuildPropertyDetails> getGuildProperties(const std::string& guildId) const;
    std::vector<GuildPropertyDetails> getPropertiesByType(const std::string& guildId, 
                                                       GuildPropertyType type) const;
    
    // Upgrades de propriedade
    std::string createPropertyUpgrade(const PropertyUpgrade& upgrade);
    bool startPropertyUpgrade(const std::string& propertyId, const std::string& upgradeId);
    bool cancelPropertyUpgrade(const std::string& propertyId);
    PropertyUpgrade getPropertyUpgrade(const std::string& upgradeId) const;
    std::vector<PropertyUpgrade> getAvailableUpgrades(const std::string& propertyId) const;
    
    // Gestão de propriedades
    bool repairProperty(const std::string& propertyId, uint32_t amount);
    bool damageProperty(const std::string& propertyId, uint32_t amount);
    bool abandonProperty(const std::string& propertyId);
    bool collectPropertyResources(const std::string& propertyId);
    uint32_t getPropertyDefense(const std::string& propertyId) const;
    
    // Transações
    PropertyTransaction getPropertyTransaction(const std::string& transactionId) const;
    std::vector<PropertyTransaction> getPropertyTransactions(const std::string& propertyId) const;
    std::vector<PropertyTransaction> getGuildTransactions(const std::string& guildId) const;
    
    // Callbacks
    void registerStorageTabCreatedCallback(std::function<void(const std::string&, const std::string&)> callback);
    void registerItemDepositedCallback(std::function<void(const std::string&, const std::string&, const std::string&, uint32_t)> callback);
    void registerPropertyUpgradeCompletedCallback(std::function<void(const std::string&, const std::string&)> callback);
    
private:
    GuildStorageSystem();
    ~GuildStorageSystem();
    
    // Atributos privados
    static std::unique_ptr<GuildStorageSystem> s_instance;
    static std::once_flag s_onceFlag;
    
    std::shared_ptr<GuildSystem> m_guildSystem;
    std::shared_ptr<Inventory::InventorySystem> m_inventorySystem;
    
    // Tabs de armazenamento
    std::unordered_map<std::string, std::unordered_map<std::string, GuildStorageTab>> m_storageTabs; // guildId -> (tabId -> tab)
    mutable std::shared_mutex m_tabsMutex;
    
    // Logs de armazenamento
    std::unordered_map<std::string, std::vector<GuildStorageLog>> m_guildLogs; // guildId -> logs
    std::unordered_map<std::string, std::vector<size_t>> m_tabLogs; // tabId -> [logIndices]
    std::unordered_map<std::string, std::vector<size_t>> m_playerLogs; // playerId -> [logIndices]
    mutable std::shared_mutex m_logsMutex;
    
    // Propriedades
    std::unordered_map<std::string, GuildPropertyDetails> m_propertyDetails;
    std::unordered_map<std::string, std::vector<std::string>> m_guildPropertyIds; // guildId -> [propertyIds]
    std::unordered_map<GuildPropertyType, std::vector<std::string>> m_propertiesByType; // type -> [propertyIds]
    mutable std::shared_mutex m_propertiesMutex;
    
    // Upgrades
    std::unordered_map<std::string, PropertyUpgrade> m_propertyUpgrades;
    std::unordered_map<GuildPropertyType, std::vector<std::string>> m_upgradesByType; // type -> [upgradeIds]
    mutable std::mutex m_upgradesMutex;
    
    // Transações
    std::unordered_map<std::string, PropertyTransaction> m_transactions;
    std::unordered_map<std::string, std::vector<std::string>> m_propertyTransactions; // propertyId -> [transactionIds]
    std::unordered_map<std::string, std::vector<std::string>> m_guildTransactions; // guildId -> [transactionIds]
    mutable std::mutex m_transactionsMutex;
    
    // Callbacks registrados
    std::vector<std::function<void(const std::string&, const std::string&)>> m_tabCreatedCallbacks;
    std::vector<std::function<void(const std::string&, const std::string&, const std::string&, uint32_t)>> m_itemDepositedCallbacks;
    std::vector<std::function<void(const std::string&, const std::string&)>> m_upgradeCompletedCallbacks;
    mutable std::mutex m_callbacksMutex;
    
    // Configuração
    uint32_t m_baseTabCapacity{100};
    uint32_t m_capacityUpgradeIncrement{50};
    uint32_t m_maxStorageTabUpgrade{10};
    uint32_t m_defaultTabsPerGuild{1};
    uint32_t m_maxTabsPerGuild{10};
    uint32_t m_maxLogRetention{1000};
    uint32_t m_basePropertyUpkeepCost{100};
    std::chrono::hours m_resourceProductionInterval{1};
    
    // Threads de trabalho
    std::atomic<bool> m_running{false};
    std::thread m_upgradeProcessingThread;
    std::thread m_resourceProductionThread;
    std::thread m_propertyMaintenanceThread;
    
    // Métodos privados
    std::string generateTabId();
    std::string generateLogId();
    std::string generatePropertyId();
    std::string generateUpgradeId();
    std::string generateTransactionId();
    
    void upgradeProcessingThread();
    void resourceProductionThread();
    void propertyMaintenanceThread();
    
    void recordStorageLog(const std::string& guildId,
                        const std::string& tabId,
                        const std::string& playerId,
                        const std::string& action,
                        const std::optional<std::string>& itemId = std::nullopt,
                        const std::optional<uint32_t>& quantity = std::nullopt,
                        const std::optional<std::string>& targetRankId = std::nullopt,
                        const std::optional<std::string>& notes = std::nullopt,
                        const json& details = json());
    
    bool validateStorageOperation(const std::string& guildId, 
                                const std::string& tabId, 
                                const std::string& playerId, 
                                const std::string& itemId, 
                                uint32_t quantity, 
                                bool isWithdrawal) const;
    
    bool validatePropertyUpgrade(const std::string& propertyId, const std::string& upgradeId) const;
    PropertyTransaction recordPropertyTransaction(const std::string& propertyId,
                                                const std::string& transactionType,
                                                uint64_t amount,
                                                const std::string& currency,
                                                const std::optional<std::string>& otherPartyId = std::nullopt,
                                                const std::optional<std::chrono::system_clock::time_point>& expirationDate = std::nullopt,
                                                const std::string& terms = "",
                                                bool successful = true,
                                                const json& details = json());
    
    bool processPropertyUpgrade(const std::string& propertyId);
    void generatePropertyResources();
    void collectPropertyUpkeep();
    
    void checkPropertyDurability();
    void checkPropertyExpirations();
    
    void notifyStorageTabCreated(const std::string& guildId, const std::string& tabId);
    void notifyItemDeposited(const std::string& guildId, const std::string& tabId, const std::string& itemId, uint32_t quantity);
    void notifyPropertyUpgradeCompleted(const std::string& propertyId, const std::string& upgradeId);
    
    void pruneOldLogs();
};

// Implementação inline básica do singleton
inline GuildStorageSystem& GuildStorageSystem::getInstance() {
    std::call_once(s_onceFlag, [] {
        s_instance = std::unique_ptr<GuildStorageSystem>(new GuildStorageSystem());
    });
    return *s_instance;
}

} // namespace Guild
} // namespace WYDBR

// GuildQuestSystem.h - Missões e Benefícios de Guild
#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <mutex>
#include <shared_mutex>
#include <atomic>
#include <chrono>
#include <thread>
#include <memory>
#include <functional>
#include <optional>
#include <nlohmann/json.hpp>
#include "GuildSystem.h"
#include "QuestSystem.h"

namespace WYDBR {
namespace Guild {

using json = nlohmann::json;

enum class GuildBenefitType {
    STAT_BOOST,
    RESOURCE_BOOST,
    EXPERIENCE_BOOST,
    CRAFTING_BOOST,
    UNLOCK_FEATURE,
    REDUCED_COSTS,
    EXTENDED_STORAGE,
    TERRITORY_BONUS,
    CUSTOM
};

enum class GuildQuestDifficulty {
    TRIVIAL,
    EASY,
    MEDIUM,
    HARD,
    CHALLENGING,
    HEROIC,
    LEGENDARY
};

enum class GuildQuestReward {
    EXPERIENCE,
    RESOURCES,
    REPUTATION,
    CURRENCY,
    UNLOCK,
    ITEM,
    PROPERTY_BONUS,
    CUSTOM
};

struct GuildQuestObjective {
    std::string objectiveId;
    std::string name;
    std::string objectiveType; // "KILL", "GATHER", "CRAFT", "DEFEND", "EXPLORE", etc.
    std::string targetId; // Monster ID, resource ID, etc.
    uint64_t targetAmount;
    uint64_t currentAmount{0};
    std::set<std::string> contributingPlayers;
    std::unordered_map<std::string, uint64_t> playerContributions; // playerId -> amount
    bool completed{false};
    std::optional<std::chrono::system_clock::time_point> completionTime;
    json objectiveDetails;
};

struct GuildQuestTemplate {
    std::string templateId;
    std::string name;
    std::string description;
    GuildQuestType type;
    GuildQuestDifficulty difficulty;
    std::vector<GuildQuestObjective> objectives;
    std::unordered_map<GuildQuestReward, json> rewards;
    uint32_t minGuildLevel{1};
    uint32_t requiredParticipants{1};
    std::chrono::hours duration{168}; // 7 days default
    std::vector<std::string> requiredPreviousQuests;
    std::vector<std::string> tags;
    bool repeatable{false};
    std::chrono::hours repeatCooldown{168}; // 7 days default
    json templateParameters;
};

struct GuildQuestInstance {
    std::string instanceId;
    std::string templateId;
    std::string guildId;
    std::chrono::system_clock::time_point startTime;
    std::optional<std::chrono::system_clock::time_point> endTime;
    std::optional<std::chrono::system_clock::time_point> completionTime;
    std::string status; // "ACTIVE", "COMPLETED", "FAILED", "EXPIRED", "ABANDONED"
    std::vector<GuildQuestObjective> objectives;
    std::set<std::string> participants;
    std::unordered_map<std::string, uint64_t> playerContributions; // playerId -> total contribution
    float overallProgress{0.0f};
    json questState;
    json questResults;
};

struct GuildBenefit {
    std::string benefitId;
    std::string name;
    std::string description;
    GuildBenefitType type;
    float value;
    std::string targetStat; // The stat or system affected
    bool isPercentage{true};
    uint32_t requiredGuildLevel{1};
    uint64_t unlockCost{0};
    std::string unlockCostType; // "gold", "resource_id", etc.
    std::vector<std::string> requiredPreviousBenefits;
    bool guildwide{true};
    bool stackable{false};
    uint32_t maxStacks{1};
    json benefitParameters;
};

struct GuildBenefitInstance {
    std::string instanceId;
    std::string benefitId;
    std::string guildId;
    std::optional<std::string> targetPlayerId; // If not guildwide
    std::chrono::system_clock::time_point unlockedAt;
    std::optional<std::chrono::system_clock::time_point> expiresAt;
    bool active{true};
    uint32_t currentStacks{1};
    float effectiveValue;
    json applyConditions;
};

class GuildQuestSystem {
public:
    // Singleton
    static GuildQuestSystem& getInstance();
    
    // Inicialização
    bool initialize(const json& config);
    bool start();
    void shutdown();
    
    // Templates de missões
    std::string createQuestTemplate(const GuildQuestTemplate& questTemplate);
    bool updateQuestTemplate(const GuildQuestTemplate& questTemplate);
    bool deleteQuestTemplate(const std::string& templateId);
    GuildQuestTemplate getQuestTemplate(const std::string& templateId) const;
    std::vector<GuildQuestTemplate> getAllQuestTemplates() const;
    std::vector<GuildQuestTemplate> getQuestTemplatesByType(GuildQuestType type) const;
    std::vector<GuildQuestTemplate> getQuestTemplatesByDifficulty(GuildQuestDifficulty difficulty) const;
    
    // Instâncias de missões
    std::string startGuildQuest(const std::string& guildId, 
                              const std::string& templateId,
                              const std::string& initiatorId);
    
    bool abandonGuildQuest(const std::string& instanceId);
    bool updateQuestObjectiveProgress(const std::string& instanceId,
                                    const std::string& objectiveId,
                                    const std::string& playerId,
                                    uint64_t progress);
    
    GuildQuestInstance getQuestInstance(const std::string& instanceId) const;
    std::vector<GuildQuestInstance> getActiveGuildQuests(const std::string& guildId) const;
    std::vector<GuildQuestInstance> getCompletedGuildQuests(const std::string& guildId) const;
    std::vector<GuildQuestInstance> getPlayerParticipatedQuests(const std::string& playerId) const;
    
    // Benefícios
    std::string createGuildBenefit(const GuildBenefit& benefit);
    bool updateGuildBenefit(const GuildBenefit& benefit);
    bool deleteGuildBenefit(const std::string& benefitId);
    GuildBenefit getGuildBenefit(const std::string& benefitId) const;
    std::vector<GuildBenefit> getAllGuildBenefits() const;
    std::vector<GuildBenefit> getBenefitsByType(GuildBenefitType type) const;
    
    // Instâncias de benefícios
    std::string unlockGuildBenefit(const std::string& guildId, 
                                const std::string& benefitId,
                                const std::optional<std::string>& targetPlayerId = std::nullopt);
    
    bool deactivateGuildBenefit(const std::string& instanceId);
    bool addBenefitStack(const std::string& instanceId, uint32_t stacks = 1);
    bool applyTemporaryBenefit(const std::string& guildId,
                             const std::string& benefitId,
                             std::chrono::hours duration,
                             const std::optional<std::string>& targetPlayerId = std::nullopt);
    
    GuildBenefitInstance getBenefitInstance(const std::string& instanceId) const;
    std::vector<GuildBenefitInstance> getActiveGuildBenefits(const std::string& guildId) const;
    std::vector<GuildBenefitInstance> getPlayerBenefits(const std::string& playerId) const;
    
    // Consultas e cálculos
    float calculateTotalBenefitValue(const std::string& guildId, 
                                   const std::string& targetStat,
                                   GuildBenefitType type,
                                   const std::optional<std::string>& playerId = std::nullopt) const;
    
    std::unordered_map<std::string, float> getAllGuildBonuses(const std::string& guildId) const;
    std::unordered_map<std::string, float> getAllPlayerBonuses(const std::string& playerId) const;
    std::vector<GuildBenefit> getUnlockedGuildBenefits(const std::string& guildId) const;
    std::vector<GuildBenefit> getAvailableGuildBenefits(const std::string& guildId) const;
    
    // Recompensas e progressão
    bool distributeQuestRewards(const std::string& instanceId);
    bool trackQuestProgression(const std::string& guildId, const std::string& templateId);
    std::vector<std::string> getAvailableQuestsForGuild(const std::string& guildId) const;
    bool hasCompletedQuest(const std::string& guildId, const std::string& templateId) const;
    
    // Callbacks
    void registerQuestStartedCallback(std::function<void(const std::string&, const std::string&)> callback);
    void registerQuestCompletedCallback(std::function<void(const std::string&, bool)> callback);
    void registerBenefitUnlockedCallback(std::function<void(const std::string&, const std::string&)> callback);
    void registerObjectiveCompletedCallback(std::function<void(const std::string&, const std::string&)> callback);
    
private:
    GuildQuestSystem();
    ~GuildQuestSystem();
    
    // Atributos privados
    static std::unique_ptr<GuildQuestSystem> s_instance;
    static std::once_flag s_onceFlag;
    
    std::shared_ptr<GuildSystem> m_guildSystem;
    std::shared_ptr<Quest::QuestSystem> m_questSystem;
    
    // Templates de missões
    std::unordered_map<std::string, GuildQuestTemplate> m_questTemplates;
    std::unordered_map<GuildQuestType, std::vector<std::string>> m_templatesByType;
    std::unordered_map<GuildQuestDifficulty, std::vector<std::string>> m_templatesByDifficulty;
    mutable std::shared_mutex m_templatesMutex;
    
    // Instâncias de missões
    std::unordered_map<std::string, GuildQuestInstance> m_questInstances;
    std::unordered_map<std::string, std::vector<std::string>> m_guildQuests; // guildId -> [instanceIds]
    std::unordered_map<std::string, std::vector<std::string>> m_playerQuests; // playerId -> [instanceIds]
    std::set<std::string> m_activeInstanceIds;
    std::set<std::string> m_completedInstanceIds;
    mutable std::shared_mutex m_instancesMutex;
    
    // Benefícios
    std::unordered_map<std::string, GuildBenefit> m_guildBenefits;
    std::unordered_map<GuildBenefitType, std::vector<std::string>> m_benefitsByType;
    mutable std::shared_mutex m_benefitsMutex;
    
    // Instâncias de benefícios
    std::unordered_map<std::string, GuildBenefitInstance> m_benefitInstances;
    std::unordered_map<std::string, std::vector<std::string>> m_guildBenefitInstances; // guildId -> [instanceIds]
    std::unordered_map<std::string, std::vector<std::string>> m_playerBenefitInstances; // playerId -> [instanceIds]
    mutable std::shared_mutex m_benefitInstancesMutex;
    
    // Progresso e histórico
    std::unordered_map<std::string, std::set<std::string>> m_completedQuestTemplates; // guildId -> set of templateIds
    std::unordered_map<std::string, std::chrono::system_clock::time_point> m_questCooldowns; // guildId:templateId -> time
    mutable std::mutex m_progressionMutex;
    
    // Callbacks registrados
    std::vector<std::function<void(const std::string&, const std::string&)>> m_questStartedCallbacks;
    std::vector<std::function<void(const std::string&, bool)>> m_questCompletedCallbacks;
    std::vector<std::function<void(const std::string&, const std::string&)>> m_benefitUnlockedCallbacks;
    std::vector<std::function<void(const std::string&, const std::string&)>> m_objectiveCompletedCallbacks;
    mutable std::mutex m_callbacksMutex;
    
    // Configuração
    uint32_t m_maxActiveQuestsPerGuild{5};
    std::chrono::hours m_questCheckInterval{1};
    std::chrono::days m_questHistoryRetention{90};
    float m_contributionWeightForRewards{0.7f}; // 70% based on contribution, 30% even split
    float m_guildLevelBenefitMultiplier{0.02f}; // +2% per guild level
    bool m_autoCompleteQuestsOnObjectives{true};
    
    // Threads de trabalho
    std::atomic<bool> m_running{false};
    std::thread m_questProgressThread;
    std::thread m_benefitExpirationThread;
    std::thread m_maintenanceThread;
    
    // Métodos privados
    std::string generateTemplateId();
    std::string generateInstanceId();
    std::string generateBenefitId();
    std::string generateBenefitInstanceId();
    
    void questProgressThread();
    void benefitExpirationThread();
    void maintenanceThread();
    
    bool validateQuestTemplate(const GuildQuestTemplate& questTemplate) const;
    bool validateBenefit(const GuildBenefit& benefit) const;
    
    bool checkQuestCompletion(GuildQuestInstance& instance);
    bool checkObjectiveCompletion(GuildQuestObjective& objective);
    
    bool canStartQuest(const std::string& guildId, const std::string& templateId) const;
    bool canUnlockBenefit(const std::string& guildId, const std::string& benefitId) const;
    
    void updateQuestProgress(GuildQuestInstance& instance);
    void cleanupExpiredQuests();
    void cleanupExpiredBenefits();
    
    void calculateRewards(GuildQuestInstance& instance);
    float calculatePlayerContributionRatio(const GuildQuestInstance& instance, const std::string& playerId) const;
    
    std::string getCooldownKey(const std::string& guildId, const std::string& templateId) const;
    
    void notifyQuestStarted(const std::string& guildId, const std::string& instanceId);
    void notifyQuestCompleted(const std::string& instanceId, bool success);
    void notifyBenefitUnlocked(const std::string& guildId, const std::string& benefitId);
    void notifyObjectiveCompleted(const std::string& instanceId, const std::string& objectiveId);
};

// Implementação inline básica do singleton
inline GuildQuestSystem& GuildQuestSystem::getInstance() {
    std::call_once(s_onceFlag, [] {
        s_instance = std::unique_ptr<GuildQuestSystem>(new GuildQuestSystem());
    });
    return *s_instance;
}

} // namespace Guild
} // namespace WYDBR
`;

// Fornece uma API JavaScript para integração no projeto
module.exports.setupSystem = function(engineContext) {
  console.log('Inicializando sistema: sistema_guildas_completo');
  
  // Inicializa o sistema no contexto do engine
  engineContext.registerSystem({
    name: 'sistema_guildas_completo',
    source: module.exports.cppCode,
    dependencies: getSystemDependencies()
  });

  return {
    initialize: () => {
      // Código de inicialização específico do sistema
      return true;
    },
    
    shutdown: () => {
      // Código de desligamento do sistema
      return true;
    }
  };
};

// Retorna as dependências específicas deste sistema
function getSystemDependencies() {
  // Analisar o código para determinar dependências automaticamente
  const dependencies = [];
  const code = module.exports.cppCode;
  
  // Análise básica de dependências incluídas no arquivo C++
  const includeRegex = /#include\s+["<]([^">]+)[">]/g;
  let match;
  while ((match = includeRegex.exec(code)) !== null) {
    // Ignora includes padrão do C++
    if (!match[1].startsWith('string') && 
        !match[1].startsWith('vector') && 
        !match[1].startsWith('map') &&
        !match[1].startsWith('unordered_map') &&
        !match[1].startsWith('set') &&
        !match[1].startsWith('mutex') &&
        !match[1].startsWith('shared_mutex') &&
        !match[1].startsWith('atomic') &&
        !match[1].startsWith('chrono') &&
        !match[1].startsWith('thread') &&
        !match[1].startsWith('functional') &&
        !match[1].startsWith('memory') &&
        !match[1].startsWith('optional') &&
        !match[1].startsWith('random') &&
        !match[1].startsWith('variant') &&
        !match[1].startsWith('nlohmann/json')) {
      
      dependencies.push(match[1].replace('.h', ''));
    }
  }
  
  return dependencies;
}
