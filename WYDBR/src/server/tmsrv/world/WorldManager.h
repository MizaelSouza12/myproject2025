/**
 * WorldManager.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/world/WorldManager.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef WORLD_MANAGER_H
#define WORLD_MANAGER_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>

#include "headers/wyd_core.h"
#include "src/server/tmsrv/world/WYDMap.h"

namespace wydbr {


class TMServer;

/**
 * Gerenciador do mundo do jogo
 * 
 * Responsável por gerenciar os mapas, objetos do mundo, eventos globais,
 * e quaisquer outros elementos relacionados ao ambiente de jogo.
 */
/**
 * Classe WYDWorldManager
 * 
 * Esta classe implementa a funcionalidade WYDWorldManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class WorldManager : public WYDSystemManager {
public:
    // Construtor
    /**
 * WorldManager
 * 
 * Implementa a funcionalidade WorldManager conforme especificação original.
 * @param server Parâmetro server
 * @return Retorna explicit
 */

    explicit WorldManager(TMServer* server);
    
    // Destrutor
    virtual ~WorldManager();
    
    // Inicialização
    /**
 * initialize
 * 
 * Implementa a funcionalidade initialize conforme especificação original.
 * @return Retorna bool
 */

    bool initialize() override;
    
    // Atualização
    /**
 * update
 * 
 * Implementa a funcionalidade update conforme especificação original.
 * @param timestamp Parâmetro timestamp
 */

    void update(uint64_t timestamp) override;
    
    // Desligamento
    /**
 * shutdown
 * 
 * Implementa a funcionalidade shutdown conforme especificação original.
 */

    void shutdown() override;
    
    // Carregamento de mapas
    /**
 * loadMap
 * 
 * Implementa a funcionalidade loadMap conforme especificação original.
 * @param mapId Parâmetro mapId
 * @param mapPath Parâmetro mapPath
 * @return Retorna bool
 */

    bool loadMap(const std::string& mapId, const std::string& mapPath);
    
    // Acesso a mapas
    /**
 * getMap
 * 
 * Implementa a funcionalidade getMap conforme especificação original.
 * @param mapId Parâmetro mapId
 * @return Retorna WYDMap
 */

    WYDMap* getMap(const std::string& mapId);
    const std::vector<std::string>& getMapIds() const { return mapIds_; }
    
    // Teletransporte de entidade
    /**
 * teleportEntity
 * 
 * Implementa a funcionalidade teleportEntity conforme especificação original.
 * @param entityId Parâmetro entityId
 * @param mapId Parâmetro mapId
 * @param posX Parâmetro posX
 * @param posY Parâmetro posY
 * @return Retorna bool
 */

    bool teleportEntity(uint32_t entityId, const std::string& mapId, uint16_t posX, uint16_t posY);
    
    // Funções de interação com o mundo
    /**
 * isWalkable
 * 
 * Implementa a funcionalidade isWalkable conforme especificação original.
 * @param mapId Parâmetro mapId
 * @param posX Parâmetro posX
 * @param posY Parâmetro posY
 * @return Retorna bool
 */

    bool isWalkable(const std::string& mapId, uint16_t posX, uint16_t posY) const;
    /**
 * isLineOfSight
 * 
 * Implementa a funcionalidade isLineOfSight conforme especificação original.
 * @param mapId Parâmetro mapId
 * @param startX Parâmetro startX
 * @param startY Parâmetro startY
 * @param endX Parâmetro endX
 * @param endY Parâmetro endY
 * @return Retorna bool
 */

    bool isLineOfSight(const std::string& mapId, uint16_t startX, uint16_t startY, uint16_t endX, uint16_t endY) const;
    
    // Calcula distância entre dois pontos
    /**
 * calculateDistance
 * 
 * Implementa a funcionalidade calculateDistance conforme especificação original.
 * @param x1 Parâmetro x1
 * @param y1 Parâmetro y1
 * @param x2 Parâmetro x2
 * @param y2 Parâmetro y2
 * @return Retorna float
 */

    float calculateDistance(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) const;
    
    // Obtém entidades em uma área
    std::vector<uint32_t> getEntitiesInArea(const std::string& mapId, uint16_t centerX, uint16_t centerY, uint16_t radius);
    
    // Eventos de mundo
    /**
 * startWorldEvent
 * 
 * Implementa a funcionalidade startWorldEvent conforme especificação original.
 * @param eventId Parâmetro eventId
 * @param std::map<std::string Parâmetro std::map<std::string
 * @param {} Parâmetro {}
 * @return Retorna bool
 */

    bool startWorldEvent(const std::string& eventId, const std::map<std::string, std::string>& params = {});
    /**
 * stopWorldEvent
 * 
 * Implementa a funcionalidade stopWorldEvent conforme especificação original.
 * @param eventId Parâmetro eventId
 * @return Retorna bool
 */

    bool stopWorldEvent(const std::string& eventId);
    
    // Tempo do mundo
    /**
 * getWorldTime
 * 
 * Implementa a funcionalidade getWorldTime conforme especificação original.
 * @return Retorna uint32_t
 */

    uint32_t getWorldTime() const{ return worldTime_; }
    /**
 * isDay
 * 
 * Implementa a funcionalidade isDay conforme especificação original.
 * @return Retorna bool
 */

    bool isDay() const{ return (worldTime_ % 24) >= 6 && (worldTime_ % 24) < 18; }
    
private:
    // Mapas do mundo
    std::map<std::string, std::unique_ptr<WYDMap>> maps_;
    std::vector<std::string> mapIds_;
    
    // Controle de tempo do mundo
    uint32_t worldTime_; // Horas desde o início do mundo
    uint32_t lastTimeUpdate_; // Timestamp da última atualização de tempo
    
    // Eventos ativos
    std::map<std::string, std::map<std::string, std::string>> activeEvents_;
    
    // Mutex para controle de acesso
    mutable std::mutex mutex_;
    
    // Método para atualizar tempo do mundo
    /**
 * updateWorldTime
 * 
 * Implementa a funcionalidade updateWorldTime conforme especificação original.
 * @param timestamp Parâmetro timestamp
 */

    void updateWorldTime(uint64_t timestamp);
    
    // Método para processar eventos de mundo
    /**
 * processWorldEvents
 * 
 * Implementa a funcionalidade processWorldEvents conforme especificação original.
 * @param timestamp Parâmetro timestamp
 */

    void processWorldEvents(uint64_t timestamp);
};

#endif // WORLD_MANAGER_H

} // namespace wydbr
