#!/bin/bash

# Script para gerar arquivos do Sistema de Mundo para WYDBR 2.0
# Este script cria os principais arquivos do Sistema de Mundo

# Configuração
OUTPUT_DIR="src/tmsrv/world"
COMMON_DIR="src/common"

# Criar diretórios necessários
mkdir -p "$OUTPUT_DIR"
mkdir -p "$COMMON_DIR/utils"
mkdir -p "$COMMON_DIR/entities"
mkdir -p src/world_system/core
mkdir -p src/world_system/entities
mkdir -p src/world_system/terrain
mkdir -p src/world_system/physics
mkdir -p src/world_system/weather
mkdir -p include/world_system

echo "Gerando arquivos do Sistema de Mundo em $OUTPUT_DIR..."

# Função para gerar o arquivo WorldManager.h
generate_world_manager_h() {
    local file="$OUTPUT_DIR/WorldManager.h"
    echo "Criando $file..."
    
    cat > "$file" << 'EOL'
/**
 * @file WorldManager.h
 * @brief Sistema de gerenciamento do mundo do jogo
 */

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <shared_mutex>
#include "../entities/Character.h"
#include "../entities/Monster.h"
#include "../entities/NPC.h"
#include "../entities/WorldObject.h"
#include "../utils/Vector3.h"
#include "../utils/Logger.h"
#include "MapInstance.h"

namespace wydbr {
namespace tmsrv {
namespace world {

/**
 * @enum WorldState
 * @brief Estados possíveis do mundo do jogo
 */
enum class WorldState {
    INITIALIZING,    // Mundo em processo de inicialização
    ACTIVE,          // Mundo ativo e acessível aos jogadores
    MAINTENANCE,     // Mundo em manutenção (só acessível a GMs)
    SHUTTING_DOWN,   // Mundo em processo de desligamento
    OFFLINE          // Mundo offline
};

/**
 * @enum WorldEventType
 * @brief Tipos de eventos que podem ocorrer no mundo
 */
enum class WorldEventType {
    MAP_LOADED,          // Um mapa foi carregado
    INSTANCE_CREATED,    // Uma instância foi criada
    INSTANCE_DESTROYED,  // Uma instância foi destruída
    PLAYER_ENTERED,      // Um jogador entrou no mundo
    PLAYER_LEFT,         // Um jogador saiu do mundo
    MONSTER_SPAWNED,     // Um monstro foi criado
    MONSTER_KILLED,      // Um monstro foi eliminado
    WEATHER_CHANGED,     // O clima mudou
    TIME_CHANGED,        // A hora do dia mudou
    SEASON_CHANGED,      // A estação do ano mudou
    EVENT_STARTED,       // Um evento global começou
    EVENT_ENDED          // Um evento global terminou
};

/**
 * @struct WorldEvent
 * @brief Estrutura que representa um evento no mundo
 */
struct WorldEvent {
    uint64_t timestamp;
    WorldEventType type;
    std::string mapId;
    uint32_t instanceId;
    uint32_t entityId;
    std::unordered_map<std::string, std::string> parameters;
};

/**
 * @class WorldManager
 * @brief Gerencia o mundo do jogo, seus mapas e instâncias
 * 
 * Esta classe é responsável por gerenciar o estado global do mundo do jogo,
 * incluindo todos os mapas, instâncias, entidades e eventos globais.
 */
class WorldManager {
public:
    /**
     * @brief Obtém a instância singleton do WorldManager
     * @return Referência à instância singleton
     */
    static WorldManager& getInstance();
    
    /**
     * @brief Inicializa o mundo do jogo
     * @param configPath Caminho para o arquivo de configuração
     * @return true se inicializado com sucesso, false caso contrário
     */
    bool initialize(const std::string& configPath);
    
    /**
     * @brief Carrega todos os mapas do mundo
     * @return true se todos os mapas foram carregados com sucesso
     */
    bool loadAllMaps();
    
    /**
     * @brief Carrega um mapa específico
     * @param mapId ID do mapa a ser carregado
     * @return true se o mapa foi carregado com sucesso
     */
    bool loadMap(const std::string& mapId);
    
    /**
     * @brief Obtém uma instância de mapa
     * @param mapId ID do mapa
     * @param instanceId ID da instância (0 para instância principal)
     * @return Ponteiro para a instância do mapa, ou nullptr se não existir
     */
    std::shared_ptr<MapInstance> getMapInstance(const std::string& mapId, uint32_t instanceId = 0);
    
    /**
     * @brief Cria uma nova instância de um mapa
     * @param mapId ID do mapa base
     * @param instanceType Tipo da instância a ser criada
     * @param owner ID do dono da instância (se aplicável)
     * @param parameters Parâmetros adicionais para a criação
     * @return ID da nova instância ou 0 em caso de falha
     */
    uint32_t createMapInstance(
        const std::string& mapId, 
        InstanceType instanceType,
        uint32_t owner = 0,
        const std::unordered_map<std::string, std::string>& parameters = {}
    );
    
    /**
     * @brief Destrói uma instância de mapa
     * @param mapId ID do mapa
     * @param instanceId ID da instância
     * @return true se a instância foi destruída com sucesso
     */
    bool destroyMapInstance(const std::string& mapId, uint32_t instanceId);
    
    /**
     * @brief Adiciona uma entidade ao mundo
     * @param entity Ponteiro para a entidade
     * @param mapId ID do mapa
     * @param instanceId ID da instância
     * @param position Posição inicial
     * @return true se a entidade foi adicionada com sucesso
     */
    bool addEntity(
        const std::shared_ptr<Entity>& entity,
        const std::string& mapId,
        uint32_t instanceId,
        const Vector3& position
    );
    
    /**
     * @brief Remove uma entidade do mundo
     * @param entityId ID da entidade
     * @return true se a entidade foi removida com sucesso
     */
    bool removeEntity(uint32_t entityId);
    
    /**
     * @brief Transfere uma entidade para outro mapa/instância
     * @param entityId ID da entidade
     * @param targetMapId ID do mapa de destino
     * @param targetInstanceId ID da instância de destino
     * @param targetPosition Posição de destino
     * @return true se a entidade foi transferida com sucesso
     */
    bool transferEntity(
        uint32_t entityId,
        const std::string& targetMapId,
        uint32_t targetInstanceId,
        const Vector3& targetPosition
    );
    
    /**
     * @brief Obtém uma entidade pelo seu ID
     * @param entityId ID da entidade
     * @return Ponteiro para a entidade ou nullptr se não encontrada
     */
    std::shared_ptr<Entity> getEntity(uint32_t entityId);
    
    /**
     * @brief Obtém todas as entidades em um raio ao redor de um ponto
     * @param mapId ID do mapa
     * @param instanceId ID da instância
     * @param position Posição central
     * @param radius Raio da busca
     * @param typeFilter Filtro opcional por tipo de entidade
     * @return Lista de entidades encontradas
     */
    std::vector<std::shared_ptr<Entity>> getEntitiesInRadius(
        const std::string& mapId,
        uint32_t instanceId,
        const Vector3& position,
        float radius,
        EntityType typeFilter = EntityType::NONE
    );
    
    /**
     * @brief Atualiza o estado do mundo
     * @param deltaTime Tempo decorrido desde a última atualização (em segundos)
     */
    void update(float deltaTime);
    
    /**
     * @brief Registra um evento no mundo
     * @param event Evento a ser registrado
     */
    void registerWorldEvent(const WorldEvent& event);
    
    /**
     * @brief Obtém o estado atual do mundo
     * @return Estado atual do mundo
     */
    WorldState getWorldState() const;
    
    /**
     * @brief Define o estado do mundo
     * @param state Novo estado
     */
    void setWorldState(WorldState state);
    
    /**
     * @brief Obtém estatísticas gerais do mundo
     * @return Mapa com estatísticas (jogadores online, instâncias, etc.)
     */
    std::unordered_map<std::string, int> getWorldStatistics() const;
    
    /**
     * @brief Finaliza o mundo, salvando dados e liberando recursos
     */
    void shutdown();
    
private:
    // Construtor privado para singleton
    WorldManager();
    ~WorldManager();
    
    // Previne cópia/atribuição
    WorldManager(const WorldManager&) = delete;
    WorldManager& operator=(const WorldManager&) = delete;
    
    // Estado do mundo
    WorldState worldState;
    
    // Mapas e suas instâncias
    std::unordered_map<std::string, std::unordered_map<uint32_t, std::shared_ptr<MapInstance>>> maps;
    
    // Entidades atualmente no mundo
    std::unordered_map<uint32_t, std::shared_ptr<Entity>> entities;
    
    // Mapeamento de entidades para suas localizações
    struct EntityLocation {
        std::string mapId;
        uint32_t instanceId;
        Vector3 position;
    };
    std::unordered_map<uint32_t, EntityLocation> entityLocations;
    
    // Controle de concorrência
    mutable std::shared_mutex worldMutex;
    
    // Configurações
    struct {
        int maxPlayersPerMap;
        int maxInstancesPerMap;
        bool autoCreateInstances;
        float updateInterval;
        int saveInterval;
        std::string worldDataPath;
    } config;
    
    // Próximo ID de instância disponível
    uint32_t nextInstanceId;
    
    // Métodos auxiliares
    bool loadMapData(const std::string& mapId, const std::string& mapFilePath);
    bool saveWorldState();
    void cleanupEmptyInstances();
    bool isValidLocation(const std::string& mapId, uint32_t instanceId, const Vector3& position);
    void broadcastToNearbyEntities(uint32_t sourceEntityId, const std::string& eventName, const std::string& eventData, float radius);
};

} // namespace world
} // namespace tmsrv
} // namespace wydbr
EOL
    
    if [ $? -eq 0 ]; then
        echo "✓ $file criado com sucesso"
    else
        echo "✗ Erro ao criar $file"
        return 1
    fi
    return 0
}

echo "Iniciando geração dos arquivos do Sistema de Mundo para WYDBR 2.0..."

# Gerando arquivos de cabeçalho
cat > include/world_system/WorldSystem.h << 'EOL'
#pragma once

#include <memory>
#include <string>
#include <vector>
#include "TerrainManager.h"
#include "EntityManager.h"
#include "PhysicsEngine.h"
#include "WeatherSystem.h"

namespace WYDBR {
namespace WorldSystem {

class WorldSystem {
public:
    WorldSystem();
    ~WorldSystem();

    bool Initialize();
    void Update(float deltaTime);
    void Shutdown();

    std::shared_ptr<TerrainManager> GetTerrainManager() const;
    std::shared_ptr<EntityManager> GetEntityManager() const;
    std::shared_ptr<PhysicsEngine> GetPhysicsEngine() const;
    std::shared_ptr<WeatherSystem> GetWeatherSystem() const;

private:
    std::shared_ptr<TerrainManager> m_terrainManager;
    std::shared_ptr<EntityManager> m_entityManager;
    std::shared_ptr<PhysicsEngine> m_physicsEngine;
    std::shared_ptr<WeatherSystem> m_weatherSystem;
    
    bool m_initialized;
};

} // namespace WorldSystem
} // namespace WYDBR
EOL

cat > include/world_system/TerrainManager.h << 'EOL'
#pragma once

#include <string>
#include <vector>
#include <memory>
#include "TerrainChunk.h"

namespace WYDBR {
namespace WorldSystem {

class TerrainManager {
public:
    TerrainManager();
    ~TerrainManager();

    bool Initialize();
    void Update(float deltaTime);
    void Shutdown();

    bool LoadTerrain(const std::string& terrainFile);
    void GenerateTerrain(int width, int height, int seed);
    
    TerrainChunk* GetChunkAt(int x, int y, int z);

private:
    std::vector<std::shared_ptr<TerrainChunk>> m_chunks;
    bool m_initialized;
};

} // namespace WorldSystem
} // namespace WYDBR
EOL

cat > include/world_system/EntityManager.h << 'EOL'
#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "Entity.h"

namespace WYDBR {
namespace WorldSystem {

class EntityManager {
public:
    EntityManager();
    ~EntityManager();

    bool Initialize();
    void Update(float deltaTime);
    void Shutdown();

    std::shared_ptr<Entity> CreateEntity(const std::string& type);
    void DestroyEntity(uint64_t entityId);
    
    std::shared_ptr<Entity> GetEntity(uint64_t entityId);
    std::vector<std::shared_ptr<Entity>> GetEntitiesInRadius(float x, float y, float z, float radius);

private:
    std::unordered_map<uint64_t, std::shared_ptr<Entity>> m_entities;
    uint64_t m_nextEntityId;
    bool m_initialized;
};

} // namespace WorldSystem
} // namespace WYDBR
EOL

cat > include/world_system/PhysicsEngine.h << 'EOL'
#pragma once

#include <memory>
#include <vector>

namespace WYDBR {
namespace WorldSystem {

class PhysicsEngine {
public:
    PhysicsEngine();
    ~PhysicsEngine();

    bool Initialize();
    void Update(float deltaTime);
    void Shutdown();

    void SetGravity(float x, float y, float z);
    bool RayCast(float startX, float startY, float startZ, 
                float dirX, float dirY, float dirZ, 
                float& hitX, float& hitY, float& hitZ);

private:
    float m_gravity[3];
    bool m_initialized;
};

} // namespace WorldSystem
} // namespace WYDBR
EOL

cat > include/world_system/WeatherSystem.h << 'EOL'
#pragma once

#include <string>
#include <memory>

namespace WYDBR {
namespace WorldSystem {

enum class WeatherType {
    Clear,
    Cloudy,
    Rainy,
    Stormy,
    Snowy
};

class WeatherSystem {
public:
    WeatherSystem();
    ~WeatherSystem();

    bool Initialize();
    void Update(float deltaTime);
    void Shutdown();

    void SetWeather(WeatherType type);
    WeatherType GetCurrentWeather() const;
    void RandomizeWeather();
    float GetIntensity() const;

private:
    WeatherType m_currentWeather;
    float m_intensity;
    bool m_initialized;
};

} // namespace WorldSystem
} // namespace WYDBR
EOL

# Gerando arquivos de implementação
cat > src/world_system/core/WorldSystem.cpp << 'EOL'
#include "WorldSystem.h"
#include <iostream>

namespace WYDBR {
namespace WorldSystem {

WorldSystem::WorldSystem() : m_initialized(false) {
}

WorldSystem::~WorldSystem() {
    if (m_initialized) {
        Shutdown();
    }
}

bool WorldSystem::Initialize() {
    std::cout << "Inicializando Sistema de Mundo..." << std::endl;
    
    m_terrainManager = std::make_shared<TerrainManager>();
    if (!m_terrainManager->Initialize()) {
        std::cerr << "Falha ao inicializar TerrainManager" << std::endl;
        return false;
    }
    
    m_entityManager = std::make_shared<EntityManager>();
    if (!m_entityManager->Initialize()) {
        std::cerr << "Falha ao inicializar EntityManager" << std::endl;
        return false;
    }
    
    m_physicsEngine = std::make_shared<PhysicsEngine>();
    if (!m_physicsEngine->Initialize()) {
        std::cerr << "Falha ao inicializar PhysicsEngine" << std::endl;
        return false;
    }
    
    m_weatherSystem = std::make_shared<WeatherSystem>();
    if (!m_weatherSystem->Initialize()) {
        std::cerr << "Falha ao inicializar WeatherSystem" << std::endl;
        return false;
    }
    
    m_initialized = true;
    std::cout << "Sistema de Mundo inicializado com sucesso!" << std::endl;
    return true;
}

void WorldSystem::Update(float deltaTime) {
    if (!m_initialized) return;
    
    m_terrainManager->Update(deltaTime);
    m_entityManager->Update(deltaTime);
    m_physicsEngine->Update(deltaTime);
    m_weatherSystem->Update(deltaTime);
}

void WorldSystem::Shutdown() {
    if (!m_initialized) return;
    
    std::cout << "Desligando Sistema de Mundo..." << std::endl;
    
    m_weatherSystem->Shutdown();
    m_physicsEngine->Shutdown();
    m_entityManager->Shutdown();
    m_terrainManager->Shutdown();
    
    m_initialized = false;
    std::cout << "Sistema de Mundo desligado com sucesso!" << std::endl;
}

std::shared_ptr<TerrainManager> WorldSystem::GetTerrainManager() const {
    return m_terrainManager;
}

std::shared_ptr<EntityManager> WorldSystem::GetEntityManager() const {
    return m_entityManager;
}

std::shared_ptr<PhysicsEngine> WorldSystem::GetPhysicsEngine() const {
    return m_physicsEngine;
}

std::shared_ptr<WeatherSystem> WorldSystem::GetWeatherSystem() const {
    return m_weatherSystem;
}

} // namespace WorldSystem
} // namespace WYDBR
EOL

echo "Arquivos do Sistema de Mundo gerados com sucesso na pasta src/world_system e include/world_system!"
echo "Use 'make world_system' para compilar o sistema." 