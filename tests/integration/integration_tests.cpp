/**
 * @file integration_tests.cpp
 * @brief Testes de integração para o sistema WYDBR 2.0
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <chrono>
#include <thread>

#include "Integration/SystemIntegrationManager.h"
#include "Integration/SecurityIntegration.h"
#include "Integration/NetworkIntegration.h"
#include "Integration/GameLogicIntegration.h"

using namespace WYDBR;

class IntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Configurar integração para testes
        Integration::IntegrationConfig config;
        config.autoStart = false;
        config.enforceSecurityChecks = true;
        config.startupTimeoutMs = 5000;
        config.shutdownTimeoutMs = 3000;
        
        // Inicializar gerenciador
        auto& manager = Integration::SystemIntegrationManager::GetInstance();
        manager.Initialize(config);
    }
    
    void TearDown() override {
        // Parar sistema após cada teste
        auto& manager = Integration::SystemIntegrationManager::GetInstance();
        manager.StopSystem(true);
    }
};

/**
 * Testa inicialização do sistema
 */
TEST_F(IntegrationTest, SystemInitialization) {
    auto& manager = Integration::SystemIntegrationManager::GetInstance();
    
    // Verificar estado inicial
    EXPECT_EQ(Integration::SystemState::Uninitialized, manager.GetSystemState());
    
    // Iniciar sistema
    EXPECT_TRUE(manager.StartSystem());
    
    // Verificar se está em execução
    EXPECT_EQ(Integration::SystemState::Running, manager.GetSystemState());
    
    // Parar sistema
    EXPECT_TRUE(manager.StopSystem());
    
    // Verificar se está parado
    EXPECT_EQ(Integration::SystemState::Stopped, manager.GetSystemState());
}

/**
 * Testa integração de segurança
 */
TEST_F(IntegrationTest, SecurityIntegration) {
    // Configurar integração de segurança
    Integration::SecurityIntegrationConfig secConfig;
    secConfig.enforceStrictMode = false;
    secConfig.monitorNetworkTraffic = true;
    secConfig.encryptAllCommunication = true;
    secConfig.securityLevel = 3;
    
    // Inicializar integração de segurança
    auto& secIntegration = Integration::SecurityIntegration::GetInstance();
    EXPECT_TRUE(secIntegration.Initialize(secConfig));
    
    // Verificar nível de segurança
    EXPECT_EQ(3, secIntegration.GetSecurityLevel());
    
    // Testar criptografia
    const uint8_t testData[] = {1, 2, 3, 4, 5};
    std::vector<uint8_t> encrypted;
    EXPECT_TRUE(secIntegration.EncryptSensitiveData(testData, sizeof(testData), encrypted));
    
    // Verificar que dados criptografados são diferentes
    EXPECT_NE(0, memcmp(testData, encrypted.data(), sizeof(testData)));
    
    // Descriptografar
    std::vector<uint8_t> decrypted;
    EXPECT_TRUE(secIntegration.DecryptSensitiveData(encrypted.data(), encrypted.size(), decrypted));
    
    // Verificar dados originais
    ASSERT_EQ(sizeof(testData), decrypted.size());
    EXPECT_EQ(0, memcmp(testData, decrypted.data(), sizeof(testData)));
}

/**
 * Testa integração de rede
 */
TEST_F(IntegrationTest, NetworkIntegration) {
    // Configurar integração de rede
    Integration::NetworkIntegrationConfig netConfig;
    netConfig.enableCompression = true;
    netConfig.enableRateLimiting = true;
    netConfig.enablePacketFragmentation = true;
    netConfig.enableReliableTransport = true;
    netConfig.bufferSizeKB = 64;
    
    // Inicializar integração de rede
    auto& netIntegration = Integration::NetworkIntegration::GetInstance();
    EXPECT_TRUE(netIntegration.Initialize(netConfig));
    
    // Testar envio e recepção de pacote
    bool packetReceived = false;
    
    // Registrar handler para pacote de ping
    EXPECT_TRUE(netIntegration.RegisterPacketHandler(
        Network::PacketType::Ping,
        [&packetReceived](Network::PacketType type, const void* data, size_t size) {
            packetReceived = true;
            EXPECT_EQ(Network::PacketType::Ping, type);
            EXPECT_NE(nullptr, data);
            EXPECT_GT(size, 0);
        }
    ));
    
    // Dados de teste
    const uint8_t testData[] = {1, 2, 3, 4, 5};
    
    // Enviar pacote
    EXPECT_TRUE(netIntegration.SendPacket(Network::PacketType::Ping, testData, sizeof(testData)));
    
    // Aguardar processamento
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Verificar que o pacote foi recebido
    EXPECT_TRUE(packetReceived);
}

/**
 * Testa integração da lógica de jogo
 */
TEST_F(IntegrationTest, GameLogicIntegration) {
    // Configurar integração da lógica de jogo
    Integration::GameLogicIntegrationConfig gameConfig;
    gameConfig.enableAdvancedDamageCalculation = true;
    gameConfig.enablePathfindingCache = true;
    gameConfig.enablePeriodicalDataSync = false;
    gameConfig.simulationTickRateHz = 20;
    
    // Inicializar integração da lógica de jogo
    auto& gameIntegration = Integration::GameLogicIntegration::GetInstance();
    EXPECT_TRUE(gameIntegration.Initialize(gameConfig));
    
    // Testar cálculo de dano
    GameLogic::DamageResult damageResult;
    EXPECT_TRUE(gameIntegration.CalculateDamage(
        1,  // attackerId
        2,  // targetId
        0,  // skillId (ataque normal)
        GameLogic::DamageType::Physical,
        &damageResult
    ));
    
    // Verificar resultado do dano
    EXPECT_GT(damageResult.finalDamage, 0);
    
    // Testar pathfinding
    GameLogic::Position startPos{100, 100, 0};
    GameLogic::Position goalPos{200, 200, 0};
    std::vector<GameLogic::Position> path;
    
    EXPECT_TRUE(gameIntegration.FindPath(
        1,  // entityId
        startPos,
        goalPos,
        &path
    ));
    
    // Verificar caminho encontrado
    EXPECT_GT(path.size(), 0);
    EXPECT_EQ(startPos.x, path.front().x);
    EXPECT_EQ(startPos.y, path.front().y);
    EXPECT_EQ(goalPos.x, path.back().x);
    EXPECT_EQ(goalPos.y, path.back().y);
}

/**
 * Testa integração completa do sistema
 */
TEST_F(IntegrationTest, CompleteSystemIntegration) {
    auto& manager = Integration::SystemIntegrationManager::GetInstance();
    
    // Iniciar sistema
    EXPECT_TRUE(manager.StartSystem());
    
    // Verificar componentes
    auto components = manager.GetAllComponentsInfo();
    EXPECT_GT(components.size(), 0);
    
    // Verificar integridade do sistema
    EXPECT_TRUE(manager.PerformSystemIntegrityCheck());
    
    // Parar sistema
    EXPECT_TRUE(manager.StopSystem());
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
