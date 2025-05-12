#pragma once

#include <cstdint>
#include <string>
#include <array>

namespace wydbr::tmsrv::world {

/**
 * @struct MapInfo
 * @brief Estrutura que contém informações sobre um mapa.
 * 
 * Esta estrutura armazena todas as informações necessárias para
 * descrever um mapa no mundo do jogo.
 */
struct MapInfo {
    // Nome do mapa (até 32 caracteres)
    char name[32];
    
    // Dimensões do mapa
    uint32_t width;
    uint32_t height;
    
    // Tamanho de cada região
    float regionSize;
    
    // Número de regiões em cada dimensão
    uint32_t regionsX;
    uint32_t regionsY;
    
    // Flags do mapa
    union {
        struct {
            uint32_t isPvP : 1;          // Mapa permite PvP
            uint32_t isInstance : 1;     // Mapa é uma instância
            uint32_t isCity : 1;         // Mapa é uma cidade
            uint32_t isDungeon : 1;      // Mapa é uma dungeon
            uint32_t isArena : 1;        // Mapa é uma arena
            uint32_t isEventMap : 1;     // Mapa é um mapa de evento
            uint32_t allowMount : 1;     // Mapa permite montarias
            uint32_t allowFly : 1;       // Mapa permite voo
            uint32_t allowTeleport : 1;  // Mapa permite teleporte
            uint32_t allowRecall : 1;    // Mapa permite recall
            uint32_t allowStore : 1;     // Mapa permite lojas
            uint32_t allowDrop : 1;      // Mapa permite drop de itens
            uint32_t allowTrade : 1;     // Mapa permite comércio
            uint32_t allowGuild : 1;     // Mapa permite ações de guilda
            uint32_t allowParty : 1;     // Mapa permite ações de grupo
            uint32_t reserved : 17;      // Bits reservados para uso futuro
        };
        uint32_t flags;
    };
    
    // Nível mínimo e máximo recomendado para o mapa
    uint16_t minLevel;
    uint16_t maxLevel;
    
    // Clima padrão do mapa
    uint8_t defaultWeather;
    
    // Taxa de spawn de monstros (multiplicador)
    float monsterSpawnRate;
    
    // Taxa de drop de itens (multiplicador)
    float dropRate;
    
    // Taxa de experiência (multiplicador)
    float expRate;
    
    // Taxa de gold (multiplicador)
    float goldRate;
    
    // Música de fundo
    uint16_t bgmId;
    
    // Posição de entrada padrão
    float defaultEntryX;
    float defaultEntryY;
    
    // Posição de saída padrão (para retorno)
    float defaultExitX;
    float defaultExitY;
    
    // ID do mapa de saída padrão
    uint32_t defaultExitMapId;
    
    // Tempo máximo de permanência em segundos (0 = sem limite)
    uint32_t timeLimit;
    
    // Construtor padrão
    MapInfo()
        : name{}
        , width(0)
        , height(0)
        , regionSize(100.0f)
        , regionsX(0)
        , regionsY(0)
        , flags(0)
        , minLevel(0)
        , maxLevel(0)
        , defaultWeather(0)
        , monsterSpawnRate(1.0f)
        , dropRate(1.0f)
        , expRate(1.0f)
        , goldRate(1.0f)
        , bgmId(0)
        , defaultEntryX(0.0f)
        , defaultEntryY(0.0f)
        , defaultExitX(0.0f)
        , defaultExitY(0.0f)
        , defaultExitMapId(0)
        , timeLimit(0)
    {
    }
    
    // Define o nome do mapa
    void setName(const std::string& mapName) {
        size_t copyLength = std::min(mapName.length(), static_cast<size_t>(31));
        std::copy_n(mapName.begin(), copyLength, name);
        name[copyLength] = '\0';
    }
    
    // Obtém o nome do mapa como string
    std::string getName() const {
        return std::string(name);
    }
};

} // namespace wydbr::tmsrv::world 