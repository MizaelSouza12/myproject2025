/**
 * WorldEvent.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/world/WorldEvent.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef WORLD_EVENT_H
#define WORLD_EVENT_H

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>
#include <bitset>
#include <chrono>
#include <random>
#include <algorithm>
#include <queue>
#include <mutex>

#include "../../../core/WYDTypes.h"
#include "WorldTypes.h"
#include "Position.h"

namespace wydbr {


/**
 * @file WorldEvent.h
 * @brief Sistema avançado de eventos de mundo
 * 
 * Este arquivo implementa um sistema sofisticado de eventos para o mundo
 * do WYD, permitindo a criação de eventos dinâmicos, temporais e interativos
 * que modificam o mundo do jogo. O sistema inclui suporte a eventos programados,
 * eventos acionados por condições, eventos periódicos e eventos especiais,
 * todos com capacidade de afetar múltiplas zonas e jogadores simultaneamente.
 */

namespace wyd {
namespace server {

// Forward declarations
class World;
class Zone;
class Player;
class NPC;

/**
 * @brief Parâmetros para evento
 */
struct EventParams {
    DWORD id;                   // ID único do evento
    std::string name;           // Nome do evento
    WorldEventType type;        // Tipo do evento
    DWORD startTime;            // Timestamp de início
    DWORD endTime;              // Timestamp de fim (0 = sem fim)
    DWORD interval;             // Intervalo em milissegundos (0 = único)
    WORD mapId;                 // ID do mapa afetado (0 = todos)
    Area area;                  // Área afetada
    BYTE announcementLevel;     // Nível de anúncio (0=nenhum, 1=mapa, 2=global)
    bool active;                // Se o evento está ativo
    INT maxActivations;         // Número máximo de ativações (0 = infinito)
    BYTE rewardScale;           // Escala de recompensa (0-255)
    
    EventParams() 
        : id(0)
        , name("UnknownEvent")
        , type(WorldEventType::NONE)
        , startTime(0)
        , endTime(0)
        , interval(0)
        , mapId(0)
        , announcementLevel(1)
        , active(false)
        , maxActivations(0)
        , rewardScale(100)
    {
    }
};

/**
 * @brief Interface base para eventos de mundo
 */
/**
 * Classe WYDWorldEvent
 * 
 * Esta classe implementa a funcionalidade WYDWorldEvent conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class WorldEvent : public IWorldEvent {
public:
    /**
     * @brief Construtor
     * @param params Parâmetros do evento
     */
    WorldEvent(const EventParams& params);
    
    /**
     * @brief Destrutor virtual
     */
    virtual ~WorldEvent();
    
    /**
     * @brief Obtém o ID do evento
     * @return ID do evento
     */
    virtual DWORD GetID() const override { return params_.id; }
    
    /**
     * @brief Obtém o tipo do evento
     * @return Tipo do evento
     */
    virtual WorldEventType GetType() const override { return params_.type; }
    
    /**
     * @brief Inicializa o evento
     * @return true se o evento foi inicializado, false caso contrário
     */
    virtual bool Initialize() override;
    
    /**
     * @brief Atualiza o evento
     * @param deltaTime Tempo desde a última atualização em milissegundos
     */
    virtual void Update(DWORD deltaTime) override;
    
    /**
     * @brief Finaliza o evento
     */
    virtual void Shutdown() override;
    
    /**
     * @brief Verifica se o evento está ativo
     * @return true se o evento está ativo, false caso contrário
     */
    virtual bool IsActive() const override { return params_.active && isRunning_; }
    
    /**
     * @brief Ativa ou desativa o evento
     * @param active Se o evento deve ser ativado (true) ou desativado (false)
     */
    virtual void SetActive(bool active) override;
    
    /**
     * @brief Obtém o nome do evento
     * @return Nome do evento
     */
    const std::string& GetName() const { return params_.name; }
    
    /**
     * @brief Obtém o mapa afetado pelo evento
     * @return ID do mapa afetado (0 = todos)
     */
    WORD GetMapId() const { return params_.mapId; }
    
    /**
     * @brief Obtém a área afetada pelo evento
     * @return Área afetada
     */
    const Area& GetArea() const { return params_.area; }
    
    /**
     * @brief Obtém o timestamp de início do evento
     * @return Timestamp de início
     */
    DWORD GetStartTime() const { return params_.startTime; }
    
    /**
     * @brief Obtém o timestamp de fim do evento
     * @return Timestamp de fim (0 = sem fim)
     */
    DWORD GetEndTime() const { return params_.endTime; }
    
    /**
     * @brief Define o timestamp de fim do evento
     * @param endTime Timestamp de fim (0 = sem fim)
     */
    void SetEndTime(DWORD endTime) { params_.endTime = endTime; }
    
    /**
     * @brief Obtém o intervalo do evento
     * @return Intervalo em milissegundos (0 = único)
     */
    DWORD GetInterval() const { return params_.interval; }
    
    /**
     * @brief Define o intervalo do evento
     * @param interval Intervalo em milissegundos (0 = único)
     */
    void SetInterval(DWORD interval) { params_.interval = interval; }
    
    /**
     * @brief Obtém o nível de anúncio do evento
     * @return Nível de anúncio (0=nenhum, 1=mapa, 2=global)
     */
    BYTE GetAnnouncementLevel() const { return params_.announcementLevel; }
    
    /**
     * @brief Define o nível de anúncio do evento
     * @param level Nível de anúncio (0=nenhum, 1=mapa, 2=global)
     */
    void SetAnnouncementLevel(BYTE level) { params_.announcementLevel = level; }
    
    /**
     * @brief Obtém o número máximo de ativações do evento
     * @return Número máximo de ativações (0 = infinito)
     */
    INT GetMaxActivations() const { return params_.maxActivations; }
    
    /**
     * @brief Obtém o número atual de ativações do evento
     * @return Número atual de ativações
     */
    INT GetActivationCount() const { return activationCount_; }
    
    /**
     * @brief Adiciona um observador ao evento
     * @param playerId ID do jogador
     * @return true se o jogador foi adicionado, false caso contrário
     */
    bool AddObserver(DWORD playerId);
    
    /**
     * @brief Remove um observador do evento
     * @param playerId ID do jogador
     * @return true se o jogador foi removido, false caso contrário
     */
    bool RemoveObserver(DWORD playerId);
    
    /**
     * @brief Verifica se o jogador é um observador do evento
     * @param playerId ID do jogador
     * @return true se o jogador é um observador, false caso contrário
     */
    bool IsObserver(DWORD playerId) const;
    
    /**
     * @brief Notifica todos os observadores do evento
     * @param message Mensagem a enviar
     * @param type Tipo de notificação
     */
    void NotifyObservers(const std::string& message, BYTE type = 0);
    
    /**
     * @brief Obtém a lista de observadores
     * @return Lista de IDs de observadores
     */
    const std::vector<DWORD>& GetObservers() const { return observers_; }
    
protected:
    /**
     * @brief Método virtual protegido para implementação da lógica de inicialização
     * @return true se a inicialização foi bem-sucedida, false caso contrário
     */
    virtual bool OnInitialize() { return true; }
    
    /**
     * @brief Método virtual protegido para implementação da lógica de atualização
     * @param deltaTime Tempo desde a última atualização em milissegundos
     */
    virtual void OnUpdate(DWORD deltaTime) {}
    
    /**
     * @brief Método virtual protegido para implementação da lógica de finalização
     */
    virtual void OnShutdown() {}
    
    /**
     * @brief Método virtual protegido para implementação da lógica de ativação
     * @param active Se o evento está sendo ativado (true) ou desativado (false)
     */
    virtual void OnSetActive(bool active) {}
    
    /**
     * @brief Método virtual protegido para implementação da lógica de início
     */
    virtual void OnStart() {}
    
    /**
     * @brief Método virtual protegido para implementação da lógica de fim
     */
    virtual void OnEnd() {}
    
    /**
     * @brief Método virtual protegido para implementação da lógica de ativação periódica
     */
    virtual void OnActivate() {}
    
    /**
     * @brief Envia uma mensagem para todos os jogadores em uma área
     * @param message Mensagem a enviar
     * @param area Área afetada
     * @param type Tipo de mensagem
     * @return Número de jogadores notificados
     */
    INT BroadcastToArea(const std::string& message, const Area& area, BYTE type = 0);
    
    /**
     * @brief Envia uma mensagem para todos os jogadores em um mapa
     * @param message Mensagem a enviar
     * @param mapId ID do mapa
     * @param type Tipo de mensagem
     * @return Número de jogadores notificados
     */
    INT BroadcastToMap(const std::string& message, WORD mapId, BYTE type = 0);
    
    /**
     * @brief Envia uma mensagem para todos os jogadores do mundo
     * @param message Mensagem a enviar
     * @param type Tipo de mensagem
     * @return Número de jogadores notificados
     */
    INT BroadcastGlobal(const std::string& message, BYTE type = 0);
    
    /**
     * @brief Incrementa o contador de ativações do evento
     * @return Novo contador de ativações
     */
    INT IncrementActivationCount() { return ++activationCount_; }
    
    /**
     * @brief Obtém um valor aleatório entre min e max
     * @param min Valor mínimo
     * @param max Valor máximo
     * @return Valor aleatório
     */
    template<typename T>
    /**
 * GetRandomValue
 * 
 * Implementa a funcionalidade GetRandomValue conforme especificação original.
 * @param min Parâmetro min
 * @param max Parâmetro max
 * @return Retorna T
 */

    T GetRandomValue(T min, T max){
        std::uniform_int_distribution<T> dist(min, max);
        /**
 * dist
 * 
 * Implementa a funcionalidade dist conforme especificação original.
 * @param rng_ Parâmetro rng_
 * @return Retorna return
 */

        return dist(rng_);
    }
    
    /**
     * @brief Obtém um valor aleatório float entre min e max
     * @param min Valor mínimo
     * @param max Valor máximo
     * @return Valor aleatório
     */
    float GetRandomFloat(float min, float max) {
        std::uniform_real_distribution<float> dist(min, max);
        /**
 * dist
 * 
 * Implementa a funcionalidade dist conforme especificação original.
 * @param rng_ Parâmetro rng_
 * @return Retorna return
 */

        return dist(rng_);
    }
    
    /**
     * @brief Obtém uma posição aleatória dentro da área do evento
     * @param mustBeWalkable Se a posição deve ser passável
     * @return Posição aleatória
     */
    Position GetRandomPosition(bool mustBeWalkable = true);
    
    /**
     * @brief Verifica se uma posição está dentro da área do evento
     * @param position Posição
     * @return true se a posição está dentro da área, false caso contrário
     */
    bool IsPositionInEventArea(const Position& position) const;
    
    /**
     * @brief Obtém um NPC aleatório da área do evento
     * @param npcType Tipo de NPC (UNKNOWN para qualquer)
     * @return ID do NPC (0 se nenhum)
     */
    DWORD GetRandomNPCInArea(NPCType npcType = NPCType::UNKNOWN);
    
    /**
     * @brief Obtém um jogador aleatório da área do evento
     * @return ID do jogador (0 se nenhum)
     */
    DWORD GetRandomPlayerInArea();
    
    /**
     * @brief Obtém todos os jogadores na área do evento
     * @return Lista de IDs de jogadores
     */
    std::vector<DWORD> GetPlayersInArea();
    
    /**
     * @brief Obtém todos os NPCs na área do evento
     * @param npcType Tipo de NPC (UNKNOWN para qualquer)
     * @return Lista de IDs de NPCs
     */
    std::vector<DWORD> GetNPCsInArea(NPCType npcType = NPCType::UNKNOWN);
    
    /**
     * @brief Teletransporta um jogador para uma posição aleatória dentro da área do evento
     * @param playerId ID do jogador
     * @return true se o jogador foi teleportado, false caso contrário
     */
    bool TeleportPlayerToRandomPosition(DWORD playerId);
    
    /**
     * @brief Altera o clima da área do evento
     * @param weather Novo clima
     * @param duration Duração em milissegundos (0 = permanente)
     * @return true se o clima foi alterado, false caso contrário
     */
    bool SetAreaWeather(Weather weather, DWORD duration = 0);
    
    /**
     * @brief Spawn de um NPC na área do evento
     * @param npcTemplateId ID do template do NPC
     * @param count Número de spawns (1 = único)
     * @param announceSpawn Se deve anunciar o spawn
     * @return Número de NPCs spawnados
     */
    INT SpawnNPC(DWORD npcTemplateId, INT count = 1, bool announceSpawn = false);
    
    /**
     * @brief Spawn de um NPC em uma posição específica
     * @param npcTemplateId ID do template do NPC
     * @param position Posição
     * @param announceSpawn Se deve anunciar o spawn
     * @return ID do NPC spawnado (0 se falhou)
     */
    DWORD SpawnNPCAt(DWORD npcTemplateId, const Position& position, bool announceSpawn = false);
    
    /**
     * @brief Adiciona um item ao inventário de um jogador
     * @param playerId ID do jogador
     * @param itemTemplateId ID do template do item
     * @param amount Quantidade
     * @param refineLevel Nível de refinamento
     * @return true se o item foi adicionado, false caso contrário
     */
    bool AddItemToPlayer(DWORD playerId, WORD itemTemplateId, WORD amount = 1, BYTE refineLevel = 0);
    
    /**
     * @brief Adiciona experiência a um jogador
     * @param playerId ID do jogador
     * @param experience Quantidade de experiência
     * @param announce Se deve anunciar o ganho de experiência
     * @return true se a experiência foi adicionada, false caso contrário
     */
    bool AddExperienceToPlayer(DWORD playerId, DWORD experience, bool announce = true);
    
    /**
     * @brief Adiciona ouro a um jogador
     * @param playerId ID do jogador
     * @param gold Quantidade de ouro
     * @param announce Se deve anunciar o ganho de ouro
     * @return true se o ouro foi adicionado, false caso contrário
     */
    bool AddGoldToPlayer(DWORD playerId, DWORD gold, bool announce = true);
    
    /**
     * @brief Aplica um efeito a um jogador
     * @param playerId ID do jogador
     * @param effectId ID do efeito
     * @param effectValue Valor do efeito
     * @param duration Duração em milissegundos
     * @return true se o efeito foi aplicado, false caso contrário
     */
    bool ApplyEffectToPlayer(DWORD playerId, BYTE effectId, WORD effectValue, DWORD duration);
    
    /**
     * @brief Aplica um efeito a todos os jogadores na área
     * @param effectId ID do efeito
     * @param effectValue Valor do efeito
     * @param duration Duração em milissegundos
     * @return Número de jogadores afetados
     */
    INT ApplyEffectToArea(BYTE effectId, WORD effectValue, DWORD duration);
    
    /**
     * @brief Verifica se o evento está dentro do período de atividade
     * @param currentTime Timestamp atual
     * @return true se o evento está no período de atividade, false caso contrário
     */
    bool IsWithinActiveTime(DWORD currentTime) const;
    
    /**
     * @brief Obtém o progresso do evento (0.0 a 1.0)
     * @param currentTime Timestamp atual
     * @return Progresso do evento
     */
    float GetProgress(DWORD currentTime) const;
    
    EventParams params_;                 // Parâmetros do evento
    std::vector<DWORD> observers_;       // Observadores do evento
    std::vector<DWORD> participants_;    // Participantes do evento
    std::vector<DWORD> spawnedNPCs_;     // NPCs spawnados pelo evento
    
    bool isInitialized_;                 // Se o evento foi inicializado
    bool isRunning_;                     // Se o evento está em execução
    DWORD lastUpdateTime_;               // Timestamp da última atualização
    DWORD lastActivationTime_;           // Timestamp da última ativação
    INT activationCount_;                // Contador de ativações
    
    std::mt19937 rng_;                   // Gerador de números aleatórios
    mutable std::mutex mutex_;           // Mutex para thread safety
};

/**
 * @brief Evento de mudança de clima
 */
/**
 * Classe WYDWeatherEvent
 * 
 * Esta classe implementa a funcionalidade WYDWeatherEvent conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class WeatherEvent : public WorldEvent {
public:
    /**
     * @brief Construtor
     * @param params Parâmetros do evento
     */
    WeatherEvent(const EventParams& params);
    
    /**
     * @brief Destrutor
     */
    virtual ~WeatherEvent();
    
protected:
    /**
     * @brief Inicializa o evento
     * @return true se a inicialização foi bem-sucedida, false caso contrário
     */
    virtual bool OnInitialize() override;
    
    /**
     * @brief Atualiza o evento
     * @param deltaTime Tempo desde a última atualização em milissegundos
     */
    virtual void OnUpdate(DWORD deltaTime) override;
    
    /**
     * @brief Finaliza o evento
     */
    virtual void OnShutdown() override;
    
    /**
     * @brief Ativa o evento
     * @param active Se o evento está sendo ativado (true) ou desativado (false)
     */
    virtual void OnSetActive(bool active) override;
    
    /**
     * @brief Lógica de início do evento
     */
    virtual void OnStart() override;
    
    /**
     * @brief Lógica de fim do evento
     */
    virtual void OnEnd() override;
    
    /**
     * @brief Lógica de ativação periódica
     */
    virtual void OnActivate() override;
    
private:
    Weather originalWeather_;             // Clima original
    Weather targetWeather_;               // Clima alvo
    INT intensityScale_;                  // Escala de intensidade
    DWORD weatherDuration_;               // Duração do clima
    bool restoreOriginalWeather_;         // Se deve restaurar o clima original
    std::vector<Weather> weatherSequence_;// Sequência de climas
    INT currentWeatherIndex_;             // Índice atual na sequência
};

/**
 * @brief Evento de invasão de monstros
 */
/**
 * Classe WYDMonsterInvasionEvent
 * 
 * Esta classe implementa a funcionalidade WYDMonsterInvasionEvent conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class MonsterInvasionEvent : public WorldEvent {
public:
    /**
     * @brief Construtor
     * @param params Parâmetros do evento
     */
    MonsterInvasionEvent(const EventParams& params);
    
    /**
     * @brief Destrutor
     */
    virtual ~MonsterInvasionEvent();
    
protected:
    /**
     * @brief Inicializa o evento
     * @return true se a inicialização foi bem-sucedida, false caso contrário
     */
    virtual bool OnInitialize() override;
    
    /**
     * @brief Atualiza o evento
     * @param deltaTime Tempo desde a última atualização em milissegundos
     */
    virtual void OnUpdate(DWORD deltaTime) override;
    
    /**
     * @brief Finaliza o evento
     */
    virtual void OnShutdown() override;
    
    /**
     * @brief Ativa o evento
     * @param active Se o evento está sendo ativado (true) ou desativado (false)
     */
    virtual void OnSetActive(bool active) override;
    
    /**
     * @brief Lógica de início do evento
     */
    virtual void OnStart() override;
    
    /**
     * @brief Lógica de fim do evento
     */
    virtual void OnEnd() override;
    
    /**
     * @brief Lógica de ativação periódica
     */
    virtual void OnActivate() override;
    
private:
    /**
     * @brief Estrutura de onda de invasão
     */
    struct InvasionWave {
        WORD npcTemplate;                 // Template do NPC
        INT count;                        // Número de NPCs
        DWORD spawnDelay;                 // Intervalo entre spawns
        BYTE difficulty;                  // Dificuldade (0-100)
        WORD radius;                      // Raio de spawn
        Position spawnPoint;              // Ponto de spawn (opcional)
        bool bossWave;                    // Se é uma onda de boss
        
        InvasionWave()
            : npcTemplate(0)
            , count(0)
            , spawnDelay(0)
            , difficulty(0)
            , radius(0)
            , bossWave(false)
        {
        }
    };
    
    std::vector<InvasionWave> waves_;     // Ondas de invasão
    INT currentWave_;                     // Onda atual
    DWORD waveStartTime_;                 // Timestamp de início da onda
    DWORD lastSpawnTime_;                 // Timestamp do último spawn
    INT spawnCount_;                      // Contador de spawns na onda atual
    INT totalKillCount_;                  // Contador total de kills
    INT waveKillCount_;                   // Contador de kills na onda atual
    INT requiredKillCount_;               // Kills necessários para avançar
    std::vector<DWORD> bossIds_;          // IDs dos bosses
    std::vector<Position> spawnPoints_;   // Pontos de spawn
    
    /**
     * @brief Incrementa o contador de kills
     * @param npcId ID do NPC morto
     * @return true se o contador foi incrementado, false caso contrário
     */
    bool IncrementKillCount(DWORD npcId);
    
    /**
     * @brief Inicia a próxima onda
     * @return true se a onda foi iniciada, false caso contrário
     */
    bool StartNextWave();
    
    /**
     * @brief Finaliza a onda atual
     * @param success Se a onda foi bem-sucedida
     */
    void FinishCurrentWave(bool success);
    
    /**
     * @brief Spawn de NPCs da onda atual
     * @return Número de NPCs spawnados
     */
    INT SpawnWaveNPCs();
};

/**
 * @brief Evento de chefe mundial
 */
/**
 * Classe WYDWorldBossEvent
 * 
 * Esta classe implementa a funcionalidade WYDWorldBossEvent conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class WorldBossEvent : public WorldEvent {
public:
    /**
     * @brief Construtor
     * @param params Parâmetros do evento
     */
    WorldBossEvent(const EventParams& params);
    
    /**
     * @brief Destrutor
     */
    virtual ~WorldBossEvent();
    
protected:
    /**
     * @brief Inicializa o evento
     * @return true se a inicialização foi bem-sucedida, false caso contrário
     */
    virtual bool OnInitialize() override;
    
    /**
     * @brief Atualiza o evento
     * @param deltaTime Tempo desde a última atualização em milissegundos
     */
    virtual void OnUpdate(DWORD deltaTime) override;
    
    /**
     * @brief Finaliza o evento
     */
    virtual void OnShutdown() override;
    
    /**
     * @brief Ativa o evento
     * @param active Se o evento está sendo ativado (true) ou desativado (false)
     */
    virtual void OnSetActive(bool active) override;
    
    /**
     * @brief Lógica de início do evento
     */
    virtual void OnStart() override;
    
    /**
     * @brief Lógica de fim do evento
     */
    virtual void OnEnd() override;
    
    /**
     * @brief Lógica de ativação periódica
     */
    virtual void OnActivate() override;
    
private:
    DWORD bossNpcTemplate_;              // Template do NPC do chefe
    DWORD bossId_;                        // ID do chefe
    Position bossSpawnPoint_;             // Ponto de spawn do chefe
    bool bossSpawned_;                    // Se o chefe foi spawnado
    DWORD announceInterval_;              // Intervalo de anúncio
    DWORD lastAnnounceTime_;              // Timestamp do último anúncio
    bool hasPreEventPhase_;               // Se tem fase pré-evento
    DWORD preEventDuration_;              // Duração da fase pré-evento
    std::vector<DWORD> minionNpcTemplates_; // Templates dos NPCs lacaios
    std::vector<DWORD> minionIds_;        // IDs dos lacaios
    std::unordered_map<DWORD, DWORD> damageContributors_; // Contribuidores de dano (jogador -> dano)
    
    /**
     * @brief Spawn do chefe
     * @return ID do chefe (0 se falhou)
     */
    DWORD SpawnBoss();
    
    /**
     * @brief Spawn de lacaios
     * @param count Número de lacaios
     * @return Número de lacaios spawnados
     */
    INT SpawnMinions(INT count);
    
    /**
     * @brief Registra contribuição de dano
     * @param playerId ID do jogador
     * @param damage Dano causado
     */
    void RegisterDamageContribution(DWORD playerId, DWORD damage);
    
    /**
     * @brief Distribui recompensas baseadas na contribuição
     */
    void DistributeRewards();
    
    /**
     * @brief Anuncia o status do evento
     */
    void AnnounceStatus();
};

/**
 * @brief Evento de loteria
 */
/**
 * Classe WYDLotteryEvent
 * 
 * Esta classe implementa a funcionalidade WYDLotteryEvent conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class LotteryEvent : public WorldEvent {
public:
    /**
     * @brief Construtor
     * @param params Parâmetros do evento
     */
    LotteryEvent(const EventParams& params);
    
    /**
     * @brief Destrutor
     */
    virtual ~LotteryEvent();
    
protected:
    /**
     * @brief Inicializa o evento
     * @return true se a inicialização foi bem-sucedida, false caso contrário
     */
    virtual bool OnInitialize() override;
    
    /**
     * @brief Atualiza o evento
     * @param deltaTime Tempo desde a última atualização em milissegundos
     */
    virtual void OnUpdate(DWORD deltaTime) override;
    
    /**
     * @brief Finaliza o evento
     */
    virtual void OnShutdown() override;
    
    /**
     * @brief Ativa o evento
     * @param active Se o evento está sendo ativado (true) ou desativado (false)
     */
    virtual void OnSetActive(bool active) override;
    
    /**
     * @brief Lógica de início do evento
     */
    virtual void OnStart() override;
    
    /**
     * @brief Lógica de fim do evento
     */
    virtual void OnEnd() override;
    
    /**
     * @brief Lógica de ativação periódica
     */
    virtual void OnActivate() override;
    
private:
    struct LotteryTicket {
        DWORD playerId;                  // ID do jogador
        DWORD purchaseTime;              // Timestamp da compra
        DWORD ticketNumber;              // Número do bilhete
        bool winner;                     // Se é um bilhete vencedor
        
        LotteryTicket()
            : playerId(0)
            , purchaseTime(0)
            , ticketNumber(0)
            , winner(false)
        {
        }
    };
    
    DWORD ticketPrice_;                  // Preço do bilhete
    DWORD prizePool_;                    // Prêmio acumulado
    DWORD entryFee_;                     // Taxa de entrada
    INT maxTicketsPerPlayer_;            // Máximo de bilhetes por jogador
    std::vector<LotteryTicket> tickets_; // Bilhetes vendidos
    DWORD ticketSalesEndTime_;           // Timestamp de fim das vendas
    bool drawCompleted_;                 // Se o sorteio foi realizado
    DWORD winningNumber_;                // Número vencedor
    DWORD lastAnnouncementTime_;         // Timestamp do último anúncio
    DWORD announcementInterval_;         // Intervalo de anúncio
    
    /**
     * @brief Vende um bilhete para um jogador
     * @param playerId ID do jogador
     * @return true se o bilhete foi vendido, false caso contrário
     */
    bool SellTicket(DWORD playerId);
    
    /**
     * @brief Realiza o sorteio
     */
    void PerformDraw();
    
    /**
     * @brief Distribui prêmios
     */
    void DistributePrizes();
    
    /**
     * @brief Anuncia o estado do evento
     */
    void AnnounceStatus();
    
    /**
     * @brief Verifica se um jogador pode comprar mais bilhetes
     * @param playerId ID do jogador
     * @return true se o jogador pode comprar mais bilhetes, false caso contrário
     */
    bool CanPlayerBuyMoreTickets(DWORD playerId) const;
    
    /**
     * @brief Obtém o número de bilhetes de um jogador
     * @param playerId ID do jogador
     * @return Número de bilhetes
     */
    INT GetPlayerTicketCount(DWORD playerId) const;
};

/**
 * @brief Evento de comércio especial
 */
/**
 * Classe WYDSpecialTradeEvent
 * 
 * Esta classe implementa a funcionalidade WYDSpecialTradeEvent conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class SpecialTradeEvent : public WorldEvent {
public:
    /**
     * @brief Construtor
     * @param params Parâmetros do evento
     */
    SpecialTradeEvent(const EventParams& params);
    
    /**
     * @brief Destrutor
     */
    virtual ~SpecialTradeEvent();
    
protected:
    /**
     * @brief Inicializa o evento
     * @return true se a inicialização foi bem-sucedida, false caso contrário
     */
    virtual bool OnInitialize() override;
    
    /**
     * @brief Atualiza o evento
     * @param deltaTime Tempo desde a última atualização em milissegundos
     */
    virtual void OnUpdate(DWORD deltaTime) override;
    
    /**
     * @brief Finaliza o evento
     */
    virtual void OnShutdown() override;
    
    /**
     * @brief Ativa o evento
     * @param active Se o evento está sendo ativado (true) ou desativado (false)
     */
    virtual void OnSetActive(bool active) override;
    
    /**
     * @brief Lógica de início do evento
     */
    virtual void OnStart() override;
    
    /**
     * @brief Lógica de fim do evento
     */
    virtual void OnEnd() override;
    
    /**
     * @brief Lógica de ativação periódica
     */
    virtual void OnActivate() override;
    
private:
    struct SpecialItem {
        WORD itemTemplate;               // Template do item
        DWORD basePrice;                 // Preço base
        WORD quantity;                   // Quantidade disponível
        BYTE refinement;                 // Nível de refinamento
        float discount;                  // Desconto (0.0 a 1.0)
        bool limitedStock;               // Se o estoque é limitado
        
        SpecialItem()
            : itemTemplate(0)
            , basePrice(0)
            , quantity(0)
            , refinement(0)
            , discount(0.0f)
            , limitedStock(false)
        {
        }
    };
    
    std::vector<SpecialItem> specialItems_; // Itens especiais
    DWORD vendorNpcTemplate_;             // Template do NPC vendedor
    DWORD vendorNpcId_;                   // ID do NPC vendedor
    Position vendorSpawnPoint_;           // Ponto de spawn do vendedor
    std::unordered_map<WORD, WORD> soldItems_; // Itens vendidos (template -> quantidade)
    DWORD lastRestockTime_;               // Timestamp do último reabastecimento
    DWORD restockInterval_;               // Intervalo de reabastecimento
    bool vendorSpawned_;                  // Se o vendedor foi spawnado
    
    /**
     * @brief Spawn do vendedor
     * @return ID do vendedor (0 se falhou)
     */
    DWORD SpawnVendor();
    
    /**
     * @brief Reabastece os itens especiais
     */
    void RestockItems();
    
    /**
     * @brief Processa uma compra
     * @param playerId ID do jogador
     * @param itemTemplate Template do item
     * @param quantity Quantidade
     * @return true se a compra foi processada, false caso contrário
     */
    bool ProcessPurchase(DWORD playerId, WORD itemTemplate, WORD quantity);
    
    /**
     * @brief Obtém o preço real de um item
     * @param itemTemplate Template do item
     * @return Preço real
     */
    DWORD GetItemPrice(WORD itemTemplate) const;
    
    /**
     * @brief Verifica se um item está disponível
     * @param itemTemplate Template do item
     * @param quantity Quantidade
     * @return true se o item está disponível, false caso contrário
     */
    bool IsItemAvailable(WORD itemTemplate, WORD quantity) const;
};

/**
 * @brief Gerenciador de eventos de mundo
 */
/**
 * Classe WYDWorldEventManager
 * 
 * Esta classe implementa a funcionalidade WYDWorldEventManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class WorldEventManager {
public:
    /**
     * @brief Obtém a instância do singleton
     * @return Referência para a instância
     */
    static WorldEventManager& getInstance();
    
    /**
     * @brief Inicializa o gerenciador de eventos
     * @return true se o gerenciador foi inicializado, false caso contrário
     */
    bool Initialize();
    
    /**
     * @brief Finaliza o gerenciador de eventos
     */
    void Shutdown();
    
    /**
     * @brief Atualiza todos os eventos
     * @param deltaTime Tempo desde a última atualização em milissegundos
     */
    void Update(DWORD deltaTime);
    
    /**
     * @brief Registra um evento
     * @param event Ponteiro para o evento
     * @return true se o evento foi registrado, false caso contrário
     */
    bool RegisterEvent(std::shared_ptr<WorldEvent> event);
    
    /**
     * @brief Remove um evento
     * @param eventId ID do evento
     * @return true se o evento foi removido, false caso contrário
     */
    bool UnregisterEvent(DWORD eventId);
    
    /**
     * @brief Busca um evento pelo ID
     * @param eventId ID do evento
     * @return Ponteiro para o evento (nullptr se não existir)
     */
    std::shared_ptr<WorldEvent> GetEvent(DWORD eventId);
    
    /**
     * @brief Ativa um evento
     * @param eventId ID do evento
     * @param active Se o evento deve ser ativado (true) ou desativado (false)
     * @return true se o evento foi alterado, false caso contrário
     */
    bool SetEventActive(DWORD eventId, bool active);
    
    /**
     * @brief Cria um evento
     * @param params Parâmetros do evento
     * @return Ponteiro para o evento criado (nullptr se falhou)
     */
    std::shared_ptr<WorldEvent> CreateEvent(const EventParams& params);
    
    /**
     * @brief Agenda um evento para ocorrer após um intervalo
     * @param params Parâmetros do evento
     * @param delay Intervalo em milissegundos
     * @return ID do evento agendado (0 se falhou)
     */
    DWORD ScheduleEvent(const EventParams& params, DWORD delay);
    
    /**
     * @brief Cancela um evento agendado
     * @param scheduledId ID do evento agendado
     * @return true se o evento foi cancelado, false caso contrário
     */
    bool CancelScheduledEvent(DWORD scheduledId);
    
    /**
     * @brief Gera um ID único para evento
     * @return ID único
     */
    DWORD GenerateEventId();
    
private:
    /**
     * @brief Construtor privado (singleton)
     */
    WorldEventManager();
    
    /**
     * @brief Destrutor privado (singleton)
     */
    ~WorldEventManager();
    
    /**
     * @brief Desabilita o construtor de cópia
     */
    WorldEventManager(const WorldEventManager&) = delete;
    
    /**
     * @brief Desabilita o operador de atribuição
     */
    WorldEventManager& operator=(const WorldEventManager&) = delete;
    
    /**
     * @brief Carrega eventos de um arquivo de configuração
     * @param filename Nome do arquivo
     * @return true se os eventos foram carregados, false caso contrário
     */
    bool LoadEvents(const std::string& filename);
    
    /**
     * @brief Salva eventos em um arquivo de configuração
     * @param filename Nome do arquivo
     * @return true se os eventos foram salvos, false caso contrário
     */
    bool SaveEvents(const std::string& filename);
    
    /**
     * @brief Processa eventos agendados
     */
    void ProcessScheduledEvents();
    
    std::unordered_map<DWORD, std::shared_ptr<WorldEvent>> events_; // Eventos registrados
    std::multimap<DWORD, std::pair<DWORD, EventParams>> scheduledEvents_; // Eventos agendados
    
    bool initialized_;                    // Se o gerenciador foi inicializado
    DWORD lastUpdateTime_;                // Timestamp da última atualização
    DWORD nextEventId_;                   // Próximo ID de evento
    
    mutable std::mutex mutex_;            // Mutex para thread safety
    
    // Constantes
    static const DWORD DEFAULT_UPDATE_INTERVAL = 1000; // Intervalo de atualização padrão
};

} // namespace server
} // namespace wyd

#endif // WORLD_EVENT_H

} // namespace wydbr
