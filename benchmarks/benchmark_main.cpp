/**
 * @file benchmark_main.cpp
 * @brief Benchmarks para o sistema WYDBR 2.0
 */

#include <benchmark/benchmark.h>
#include <vector>
#include <random>
#include <algorithm>
#include <thread>

#include "Integration/SystemIntegrationManager.h"
#include "Security/crypto/SecureRandom.h"
#include "Network/protocol/UDPProtocolHandler.h"
#include "GameLogic/pathfinding/Pathfinder.h"

using namespace WYDBR;

/**
 * Benchmark para Sistema de Segurança
 */
static void BM_SecurityRandomGeneration(benchmark::State& state) {
    auto& rng = Security::SecureRandom::GetInstance();
    
    for (auto _ : state) {
        std::vector<uint8_t> buffer(state.range(0));
        rng.GetRandomBytes(buffer.data(), buffer.size());
        benchmark::DoNotOptimize(buffer);
    }
    
    state.SetBytesProcessed(int64_t(state.iterations()) * int64_t(state.range(0)));
}

BENCHMARK(BM_SecurityRandomGeneration)->Range(1, 1<<20);

/**
 * Benchmark para Sistema de Rede
 */
static void BM_NetworkPacketProcessing(benchmark::State& state) {
    // Configurar protocolo UDP
    Network::UDPProtocolOptions options;
    options.enableReliability = true;
    options.enableFragmentation = true;
    options.enableOrdering = true;
    
    Network::UDPProtocolHandler handler(0x1234, options);
    
    // Dados de teste
    std::vector<uint8_t> testData(state.range(0));
    Security::SecureRandom::GetInstance().GetRandomBytes(testData.data(), testData.size());
    
    for (auto _ : state) {
        // Enviar pacote
        benchmark::DoNotOptimize(handler.Send(testData.data(), testData.size(), 
                                             Network::DeliveryOption::RELIABLE));
    }
    
    state.SetBytesProcessed(int64_t(state.iterations()) * int64_t(state.range(0)));
}

BENCHMARK(BM_NetworkPacketProcessing)->Range(1, 1<<20);

/**
 * Benchmark para Sistema de Pathfinding
 */
static void BM_PathfindingAlgorithm(benchmark::State& state) {
    // Mock de mapa para navegação
    class MockNavigationMap : public GameLogic::INavigationMap {
    public:
        explicit MockNavigationMap(int size) : m_size(size) {}
        
        bool IsWalkable(const GameLogic::Position& pos, GameLogic::MovementPermission) const override {
            return pos.x >= 0 && pos.x < m_size && pos.y >= 0 && pos.y < m_size;
        }
        
        std::vector<GameLogic::Position> GetNeighbors(const GameLogic::Position& pos,
                                                   const GameLogic::PathfindingOptions& options) const override {
            std::vector<GameLogic::Position> neighbors;
            
            // Vizinhos ortogonais
            const std::pair<int, int> dirs[] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
            
            for (const auto& dir : dirs) {
                GameLogic::Position neighbor{pos.x + dir.first, pos.y + dir.second, pos.z};
                if (IsWalkable(neighbor, options.permission)) {
                    neighbors.push_back(neighbor);
                }
            }
            
            // Vizinhos diagonais
            if (options.allowDiagonal) {
                const std::pair<int, int> diagDirs[] = {{1, 1}, {1, -1}, {-1, -1}, {-1, 1}};
                
                for (const auto& dir : diagDirs) {
                    GameLogic::Position neighbor{pos.x + dir.first, pos.y + dir.second, pos.z};
                    if (IsWalkable(neighbor, options.permission)) {
                        neighbors.push_back(neighbor);
                    }
                }
            }
            
            return neighbors;
        }
        
        float GetMovementCost(const GameLogic::Position& from, const GameLogic::Position& to,
                           const GameLogic::PathfindingOptions& options) const override {
            return IsDiagonalMovement(from, to) ? options.diagonalCost : options.movementCost;
        }
        
        GameLogic::TerrainType GetTerrainType(const GameLogic::Position&) const override {
            return GameLogic::TerrainType::Ground;
        }
        
        float GetElevation(const GameLogic::Position&) const override {
            return 0.0f;
        }
        
        bool IsDiagonalMovement(const GameLogic::Position& from, const GameLogic::Position& to) const override {
            return from.x != to.x && from.y != to.y;
        }
        
        bool CanCutCorner(const GameLogic::Position& from, const GameLogic::Position& to,
                       const GameLogic::PathfindingOptions& options) const override {
            if (!options.cutCorners) {
                return false;
            }
            
            if (!IsDiagonalMovement(from, to)) {
                return true;
            }
            
            GameLogic::Position corner1{from.x, to.y, from.z};
            GameLogic::Position corner2{to.x, from.y, from.z};
            
            return IsWalkable(corner1, options.permission) && IsWalkable(corner2, options.permission);
        }
        
    private:
        int m_size;
    };
    
    // Criar mapa de teste
    int mapSize = state.range(0);
    auto map = std::make_shared<MockNavigationMap>(mapSize);
    
    // Pathfinder
    GameLogic::AStar pathfinder(map);
    
    // Posições de teste (cantos do mapa)
    GameLogic::Position start{0, 0, 0};
    GameLogic::Position goal{mapSize - 1, mapSize - 1, 0};
    
    // Opções de pathfinding
    GameLogic::PathfindingOptions options;
    options.allowDiagonal = true;
    options.cutCorners = true;
    options.heuristicWeight = 1.0f;
    
    for (auto _ : state) {
        auto result = pathfinder.FindPath(start, goal, options);
        benchmark::DoNotOptimize(result);
    }
    
    state.SetComplexityN(state.range(0));
}

BENCHMARK(BM_PathfindingAlgorithm)->RangeMultiplier(2)->Range(32, 1024)->Complexity();

/**
 * Benchmark para Carga do Sistema
 */
static void BM_SystemLoad(benchmark::State& state) {
    auto& manager = Integration::SystemIntegrationManager::GetInstance();
    
    // Configurar integração
    Integration::IntegrationConfig config;
    config.autoStart = false;
    config.enforceSecurityChecks = true;
    config.startupTimeoutMs = 1000;
    config.shutdownTimeoutMs = 1000;
    
    // Inicializar gerenciador
    manager.Initialize(config);
    
    // Número de iterações de carga
    int numIterations = state.range(0);
    
    for (auto _ : state) {
        // Iniciar sistema
        benchmark::DoNotOptimize(manager.StartSystem());
        
        // Executar operações de carga
        for (int i = 0; i < numIterations; ++i) {
            // Simular carga em componentes
            benchmark::DoNotOptimize(manager.GetComponentInfo(Integration::ComponentType::Security));
            benchmark::DoNotOptimize(manager.GetComponentInfo(Integration::ComponentType::Network));
            benchmark::DoNotOptimize(manager.GetComponentInfo(Integration::ComponentType::GameLogic));
        }
        
        // Parar sistema
        benchmark::DoNotOptimize(manager.StopSystem(true));
    }
}

BENCHMARK(BM_SystemLoad)->Range(1, 1000);

BENCHMARK_MAIN();
