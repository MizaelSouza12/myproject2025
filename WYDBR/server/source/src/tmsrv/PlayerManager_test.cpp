/**
 * @file PlayerManager_test.cpp
 * @brief Testes para a classe PlayerManager
 * 
 * Este arquivo contém testes simples para verificar o funcionamento básico
 * do gerenciador de jogadores do servidor WYD.
 */

#include "../../include/tmsrv/PlayerManager.h"
#include <iostream>
#include <cassert>
#include <cstring>
#include <vector>
#include <chrono>
#include <thread>

using namespace wydbr::tmsrv;

/**
 * @brief Cria uma estrutura MOB simples para testes
 * @param name Nome do personagem
 * @return Estrutura MOB preenchida
 */
wydbr::STRUCT_MOB createTestMob(const char* name) {
    wydbr::STRUCT_MOB mob;
    memset(&mob, 0, sizeof(wydbr::STRUCT_MOB));
    
    // Preencher dados bá/**
 * strncpy
 * 
 * Implementa a funcionalidade strncpy conforme especificação original.
 * @param mob.MobName Parâmetro mob.MobName
 * @param name Parâmetro name
 * @param wydbr::NAME_LENGTH Parâmetro wydbr::NAME_LENGTH
 * @return Retorna sicos
 */
sicos
    strncpy(mob.MobName, name, wydbr::NAME_LENGTH);
    mob.CurrentScore.Level = 1;
    mob.CurrentScore.Hp = 100;
    mob.CurrentScore.MaxHp = 100;
    mob.CurrentScore.Mp = 100;
    mob.CurrentScore.MaxMp = 100;
    mob.TargetX = 2000;
    mob.TargetY = 2000;
    
    return mob;
}

/**
 * @brief Callback para eventos do PlayerManager
 * @param event Evento
 */
void onPlayerEvent(const PlayerManagerEvent& event) {
    std::cout << "Evento: ";
    
    switch (event.type) {
        case PlayerManagerEventType::PLAYER_CREATED:
            std::cout << "Jogador criado";
            break;
        case PlayerManagerEventType::PLAYER_DESTROYED:
            std::cout << "Jogador destruído";
            break;
        case PlayerManagerEventType::PLAYER_CONNECTED:
            std::cout << "Jogador conectado";
            break;
        case PlayerManagerEventType::PLAYER_DISCONNECTED:
            std::cout << "Jogador desconectado";
            break;
        case PlayerManagerEventType::PLAYER_LOADED:
            std::cout << "Jogador carregado";
            break;
        case PlayerManagerEventType::PLAYER_SAVED:
            std::cout << "Jogador salvo";
            break;
        default:
            std::cout << "Desconhecido (" << static_cast<int>(event.type) << ")";
            break;
    }
    
    std::cout << " - Player ID: " << event.playerId
              << ", Client ID: " << event.clientId
              << ", Account ID: " << event.accountId
              << std::endl;
}

/**
 * @brief Programa principal de teste
 */
int main() {
    std::cout << "=== Teste simples do PlayerManager ===" << std::endl;
    
    // Inicializar PlayerManager
    PlayerManager& manager = PlayerManager::GetInstance();
    
    std::cout << "Inicializando PlayerManager..." << std::endl;
    bool result = manager.Initialize(100, "player_data", 60, true);
    assert(result && "Falha ao inicializar PlayerManager");
    
    // Registrar callback de eventos
    std::cout << "Registrando callback de eventos..." << std::endl;
    uint32_t callbackId = manager.RegisterEventCallback(onPlayerEvent);
    assert(callbackId > 0 && "Falha ao registrar callback");
    
    // Criar jogador de teste
    std::cout << "Criando jogador de teste..." << std::endl;
    wydbr::STRUCT_MOB testMob = createTestMob("TestPlayer");
    PlayerId playerId = manager.CreatePlayer(&testMob, 12345, "TestAccount", 1);
    assert(playerId > 0 && "Falha ao criar jogador");
    
    // Obter jogador
    std::cout << "Obtendo jogador pelo ID..." << std::endl;
    std::shared_ptr<WYDPlayer> player = manager.GetPlayer(playerId);
    assert(player && "Falha ao obter jogador pelo ID");
    assert(strcmp(player->mob.MobName, "TestPlayer") == 0 && "Nome do jogador incorreto");
    
    // Obter jogador pelo nome
    std::cout << "Obtendo jogador pelo nome..." << std::endl;
    player = manager.GetPlayerByName("TestPlayer");
    assert(player && "Falha ao obter jogador pelo nome");
    assert(player->playerId == playerId && "ID do jogador incorreto");
    
    // Obter jogador pelo cliente
    std::cout << "Obtendo jogador pelo cliente..." << std::endl;
    player = manager.GetPlayerByClient(1);
    assert(player && "Falha ao obter jogador pelo cliente");
    assert(player->playerId == playerId && "ID do jogador incorreto");
    
    // Atualizar posição
    std::cout << "Atualizando posição do jogador..." << std::endl;
    result = manager.UpdatePosition(playerId, 2100, 2100);
    assert(result && "Falha ao atualizar posição");
    
    player = manager.GetPlayer(playerId);
    assert(player->mob.TargetX == 2100 && player->mob.TargetY == 2100 && "Posição incorreta");
    
    // Salvar jogador
    std::cout << "Salvando jogador..." << std::endl;
    result = manager.SavePlayer(playerId);
    assert(result && "Falha ao salvar jogador");
    
    // Desconectar jogador
    std::cout << "Desconectando jogador..." << std::endl;
    result = manager.Disconnect(playerId);
    assert(result && "Falha ao desconectar jogador");
    
    player = manager.GetPlayer(playerId);
    assert(!player->isOnline && "Jogador ainda está online");
    
    // Reconectar jogador
    std::cout << "Reconectando jogador..." << std::endl;
    result = manager.Connect(playerId, 2);
    assert(result && "Falha ao reconectar jogador");
    
    player = manager.GetPlayer(playerId);
    assert(player->isOnline && "Jogador não está online");
    assert(player->clientId == 2 && "ID do cliente incorreto");
    
    // Obter jogadores online
    std::cout << "Obtendo jogadores online..." << std::endl;
    std::vector<PlayerId> onlinePlayers = manager.GetOnlinePlayers();
    assert(onlinePlayers.size() == 1 && "Número incorreto de jogadores online");
    assert(onlinePlayers[0] == playerId && "ID do jogador online incorreto");
    
    // Verificar estatísticas
    std::cout << "Verificando estatísticas..." << std::endl;
    PlayerManagerStats stats = manager.GetStats();
    assert(stats.totalCreated == 1 && "Número incorreto de jogadores criados");
    assert(stats.totalConnected == 2 && "Número incorreto de conexões");
    assert(stats.totalDisconnected == 1 && "Número incorreto de desconexões");
    assert(stats.totalSaved == 1 && "Número incorreto de salvamentos");
    assert(stats.currentConnected == 1 && "Número incorreto de jogadores conectados");
    
    // Salvar todos os jogadores
    std::cout << "Salvando todos os jogadores..." << std::endl;
    uint32_t savedCount = manager.SaveAllPlayers();
    assert(savedCount == 1 && "Número incorreto de jogadores salvos");
    
    // Remover jogador
    std::cout << "Removendo jogador..." << std::endl;
    result = manager.RemovePlayer(playerId);
    assert(result && "Falha ao remover jogador");
    
    player = manager.GetPlayer(playerId);
    assert(!player && "Jogador ainda existe");
    
    // Remover callback
    std::cout << "Removendo callback..." << std::endl;
    result = manager.UnregisterEventCallback(callbackId);
    assert(result && "Falha ao remover callback");
    
    // Finalizar PlayerManager
    std::cout << "Finalizando PlayerManager..." << std::endl;
    manager.Shutdown();
    
    std::cout << "Teste concluído com sucesso!" << std::endl;
    return 0;
}