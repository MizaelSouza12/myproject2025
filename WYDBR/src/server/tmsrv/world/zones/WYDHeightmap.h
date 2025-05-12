/**
 * WYDHeightmap.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/world/zones/WYDHeightmap.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef WYD_HEIGHTMAP_H
#define WYD_HEIGHTMAP_H

#include <string>
#include <vector>
#include <memory>

namespace wydbr {


class WYDMap;

/**
 * Sistema de altura de terreno (Heightmap)
 * 
 * Gerencia as alturas do terreno para cada célula do mapa.
 * Essencial para cálculos de movimentação, visibilidade e efeitos.
 */
/**
 * Classe WYDHeightmap
 * 
 * Esta classe implementa a funcionalidade WYDHeightmap conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class WYDHeightmap {
public:
    // Construtor
    /**
 * WYDHeightmap
 * 
 * Implementa a funcionalidade WYDHeightmap conforme especificação original.
 * @param map Parâmetro map
 * @return Retorna explicit
 */

    explicit WYDHeightmap(WYDMap* map);
    
    // Destrutor
    ~WYDHeightmap();
    
    // Inicialização a partir de arquivo
    /**
 * loadFromFile
 * 
 * Implementa a funcionalidade loadFromFile conforme especificação original.
 * @param filePath Parâmetro filePath
 * @return Retorna bool
 */

    bool loadFromFile(const std::string& filePath);
    
    // Geração procedural de heightmap
    /**
 * generateProcedural
 * 
 * Implementa a funcionalidade generateProcedural conforme especificação original.
 * @param seed Parâmetro seed
 * @return Retorna bool
 */

    bool generateProcedural(uint16_t seed);
    
    // Obtém altura de uma célula específica
    /**
 * getHeight
 * 
 * Implementa a funcionalidade getHeight conforme especificação original.
 * @param x Parâmetro x
 * @param y Parâmetro y
 * @return Retorna float
 */

    float getHeight(uint16_t x, uint16_t y) const;
    
    // Configura a altura de uma célula específica
    /**
 * setHeight
 * 
 * Implementa a funcionalidade setHeight conforme especificação original.
 * @param x Parâmetro x
 * @param y Parâmetro y
 * @param height Parâmetro height
 */

    void setHeight(uint16_t x, uint16_t y, float height);
    
    // Calcula normal do terreno em um ponto
    /**
 * getNormal
 * 
 * Implementa a funcionalidade getNormal conforme especificação original.
 * @param x Parâmetro x
 * @param y Parâmetro y
 * @param outNX Parâmetro outNX
 * @param outNY Parâmetro outNY
 * @param outNZ Parâmetro outNZ
 */

    void getNormal(uint16_t x, uint16_t y, float& outNX, float& outNY, float& outNZ) const;
    
    // Serializa para arquivo
    /**
 * saveToFile
 * 
 * Implementa a funcionalidade saveToFile conforme especificação original.
 * @param filePath Parâmetro filePath
 * @return Retorna bool
 */

    bool saveToFile(const std::string& filePath) const;
    
    // Aplicar suavização (blur)
    /**
 * applySmoothing
 * 
 * Implementa a funcionalidade applySmoothing conforme especificação original.
 * @param radius Parâmetro radius
 */

    void applySmoothing(uint16_t radius);
    
    // Aplicar erosão (simulação de erosão)
    /**
 * applyErosion
 * 
 * Implementa a funcionalidade applyErosion conforme especificação original.
 * @param iterations Parâmetro iterations
 * @param rainAmount Parâmetro rainAmount
 */

    void applyErosion(uint16_t iterations, float rainAmount);
    
    // Dimensões do heightmap
    /**
 * getWidth
 * 
 * Implementa a funcionalidade getWidth conforme especificação original.
 * @return Retorna uint16_t
 */

    uint16_t getWidth() const{ return width_; }
    /**
 * getHeight
 * 
 * Implementa a funcionalidade getHeight conforme especificação original.
 * @return Retorna uint16_t
 */

    uint16_t getHeight() const{ return height_; }
    
private:
    // Referência ao mapa
    WYDMap* map_;
    
    // Dimensões
    uint16_t width_;
    uint16_t height_;
    
    // Altura mínima e máxima
    float minHeight_;
    float maxHeight_;
    
    // Dados de altura (formato flat para melhor desempenho)
    std::vector<float> heightData_;
    
    // Índice linear para coordenadas
    /**
 * getIndex
 * 
 * Implementa a funcionalidade getIndex conforme especificação original.
 * @param x Parâmetro x
 * @param y Parâmetro y
 * @return Retorna size_t
 */

    size_t getIndex(uint16_t x, uint16_t y) const;
    
    // Verifica se coordenadas são válidas
    /**
 * isValidCoord
 * 
 * Implementa a funcionalidade isValidCoord conforme especificação original.
 * @param x Parâmetro x
 * @param y Parâmetro y
 * @return Retorna bool
 */

    bool isValidCoord(uint16_t x, uint16_t y) const;
    
    // Inicializa dados de altura
    /**
 * initializeHeightData
 * 
 * Implementa a funcionalidade initializeHeightData conforme especificação original.
 * @param width Parâmetro width
 * @param height Parâmetro height
 */

    void initializeHeightData(uint16_t width, uint16_t height);
    
    // Métodos auxiliares para geração procedural
    /**
 * generatePerlinNoise
 * 
 * Implementa a funcionalidade generatePerlinNoise conforme especificação original.
 * @param x Parâmetro x
 * @param y Parâmetro y
 * @param frequency Parâmetro frequency
 * @param seed Parâmetro seed
 * @return Retorna float
 */

    float generatePerlinNoise(float x, float y, float frequency, uint16_t seed) const;
    /**
 * interpolate
 * 
 * Implementa a funcionalidade interpolate conforme especificação original.
 * @param a Parâmetro a
 * @param b Parâmetro b
 * @param t Parâmetro t
 * @return Retorna float
 */

    float interpolate(float a, float b, float t) const;
};

#endif // WYD_HEIGHTMAP_H

} // namespace wydbr
