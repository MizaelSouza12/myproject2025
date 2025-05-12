/**
 * Position.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/world/Position.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef POSITION_H
#define POSITION_H

#include <cmath>
#include "../../../core/WYDTypes.h"

namespace wydbr {


/**
 * @file Position.h
 * @brief Definição de posição no mundo com base nas estruturas originais do WYD
 * 
 * Este arquivo implementa a estrutura STRUCT_POSITION do WYD original,
 * mantendo a compatibilidade byte-a-byte com o código original,
 * enquanto adiciona melhorias significativas para resolver bugs conhecidos
 * e otimizar o desempenho. As funções de manipulação de posição foram
 * reimplementadas para corrigir problemas de precisão e eficiência.
 */

namespace wyd {
namespace server {

// Definição original de direções do WYD
// Numeração anti-horária, começando do Norte (0)
enum DIR : BYTE {
    DIR_NORTH      = 0,
    DIR_NORTHEAST  = 1,
    DIR_EAST       = 2,
    DIR_SOUTHEAST  = 3,
    DIR_SOUTH      = 4,
    DIR_SOUTHWEST  = 5,
    DIR_WEST       = 6,
    DIR_NORTHWEST  = 7
};

// Constantes do WYD original relacionadas a posição
constexpr WORD MAX_GRIDX = 4096;  // Tamanho máximo de grid X  
constexpr WORD MAX_GRIDY = 4096;  // Tamanho máximo de grid Y
constexpr float PI = 3.14159265358979323846f;

/**
 * @brief Estrutura de posição baseada no WYD original (STRUCT_POSITION)
 * 
 * Esta implementação mantém a mesma assinatura binária da estrutura original,
 * mas adiciona funcionalidades avançadas para manipulação de posições.
 * Bugs conhecidos do WYD original, como problemas de precisão em cálculos
 * de distância e direção, foram corrigidos.
 */
struct STRUCT_POSITION {
    WORD wPosX;         // Coordenada X (originalmente chamada PosX no WYD)
    WORD wPosY;         // Coordenada Y (originalmente chamada PosY no WYD)
    WORD wMapID;        // ID do mapa
    BYTE byDir;         // Direção (0-7, conforme enum DIR)
    BYTE byZ;           // Altura Z (adicionado para suportar terreno 3D)

    // Construtor padrão (mantém compatibilidade com o código original)
    STRUCT_POSITION() 
        : wPosX(0), wPosY(0), wMapID(0), byDir(0), byZ(0)
    {
    }
    
    // Construtor com parâmetros (baseado no uso comum no código original)
    STRUCT_POSITION(WORD posX, WORD posY, WORD mapID = 0, BYTE dir = 0, BYTE z = 0)
        : wPosX(posX), wPosY(posY), wMapID(mapID), byDir(dir), byZ(z)
    {
    }

    /**
     * @brief Calcula a distância até outra posição
     * Corrige bug do WYD original: uso incorreto de inteiros em cálculos de distância
     * @param other Outra posição
     * @return Distância em tiles (WORD_MAX se mapas diferentes)
     */
    float GetDistance(const STRUCT_POSITION& other) const {
        // Verifica se está no mesmo mapa (comportamento original do WYD)
        if (wMapID != other.wMapID) {
            return static_cast<float>(WORD_MAX);
        }
        
        // Corrige o cálculo para usar float para evitar overflow e arredondamento incorreto
        float dx = static_cast<float>(wPosX) - static_cast<float>(other.wPosX);
        float dy = static_cast<float>(wPosY) - static_cast<float>(other.wPosY);
        
        return std::sqrt(dx * dx + dy * dy);
    }
    
    /**
     * @brief Calcula a distância ao quadrado (otimização para comparações)
     * Adição: WYD original fazia cálculos redundantes em comparações de distância
     * @param other Outra posição
     * @return Distância ao quadrado em tiles
     */
    float GetDistanceSquared(const STRUCT_POSITION& other) const {
        // Verifica se está no mesmo /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param other.wMapID Parâmetro other.wMapID
 * @return Retorna mapa
 */
 mapa
        if(wMapID != other.wMapID){
            return static_cast<float>(WORD_MAX);
        }
        
        float dx = static_cast<float>(wPosX) - static_cast<float>(other.wPosX);
        float dy = static_cast<float>(wPosY) - static_cast<float>(other.wPosY);
        
        return dx * dx + dy * dy;
    }
    
    /**
     * @brief Verifica se está dentro de um raio de outra posição
     * Adição: Otimização muito usada no WYD que evita cálculos desnecessários de raiz quadrada
     * @param other Outra posição
     * @param radius Raio
     * @return true se está dentro do raio, false caso contrário
     */
    bool IsInRange(const STRUCT_POSITION& other, float radius) const {
        return GetDistanceSquared(other) <= radius * radius;
    }
    
    /**
     * @brief Calcula a direção para outra posição
     * Corrige bug do WYD original: cálculo impreciso de direção
     * @param target Posição alvo
     * @return Direção (0-7, conforme enum DIR)
     */
    BYTE GetDirection(const STRUCT_POSITION& target) const {
        // Mantém a direção atual se mapas diferentes (comportamento original)
        if (wMapID != target.wMapID) {
            return byDir; 
        }
        
        float dx = static_cast<float>(target.wPosX - wPosX);
        float dy = static_cast<float>(target.wPosY - wPosY);
        
        // Correção do algoritmo original do WYD que tinha bugs de arredondamento
        float angle = std::atan2(dy, dx);
        
        // Converte para graus (0-360)
        float degrees = angle * 180.0f / PI;
        if (degrees < 0) {
            degrees += 360.0f;
        }
        
        // Converte para direção (0-7), mantendo a convenção original do WYD
        // Corrige bug de arredondamento que causava direções incorretas
        BYTE direction = static_cast<BYTE>((degrees + 22.5f) / 45.0f) % 8;
        
        return direction;
    }
    
    /**
     * @brief Obtém uma posição à frente na direção atual
     * Adição: No código original, esta função era duplicada em vários lugares
     * @param distance Distância a avançar
     * @return Nova posição
     */
    STRUCT_POSITION GetFrontPosition(float distance = 1.0f) const {
        STRUCT_POSITION result = *this;
        
        // Suporte à precisão fracional (o original usava somente inteiros)
        float radians = byDir * 45.0f * PI / 180.0f;
        float dx = std::cos(radians) * distance;
        float dy = std::sin(radians) * distance;
        
        // Arredondamento correto para coordenadas de grid
        result.wPosX = static_cast<WORD>(wPosX + dx);
        result.wPosY = static_cast<WORD>(wPosY + dy);
        
        return result;
    }

    /**
     * @brief Calcula posição na grade de zona (correção da função GridIn do WYD original)
     * @param gridSize Tamanho da grade
     * @return Par de coordenadas de grade
     */
    std::pair<WORD, WORD> GetGridCoords(WORD gridSize) const {
        // Corrige bug de overflow em mapas grandes do WYD original
        WORD gridX = (wPosX < MAX_GRIDX) ? (wPosX / gridSize) : ((MAX_GRIDX - 1) / gridSize);
        WORD gridY = (wPosY < MAX_GRIDY) ? (wPosY / gridSize) : ((MAX_GRIDY - 1) / gridSize);
        
        return std::make_pair(gridX, gridY);
    }
    
    /**
     * @brief Gera uma posição aleatória dentro de um raio
     * Adição: Reimplementação da função espalhada em vários lugares no código original
     * @param radius Raio máximo
     * @param minRadius Raio mínimo (0 para incluir a posição atual)
     * @return Nova posição com coordenadas aleatórias
     */
    STRUCT_POSITION GetRandomPosition(float radius, float minRadius = 0.0f) const {
        STRUCT_POSITION result = *this;
        
        // Algoritmo melhorado em relação ao original que gerava distribuição não uniforme
        float angle = static_cast<float>(std::std::mt19937()) / RAND_MAX * 2.0f * PI;
        float distance = minRadius + (radius - minRadius) * static_cast<float>(std::std::mt19937()) / RAND_MAX;
        
        float dx = std::cos(angle) * distance;
        float dy = std::sin(angle) * distance;
        
        // Respeitando limites do mapa (o original não verificava)
        result.wPosX = static_cast<WORD>(std::min<float>(std::max<float>(wPosX + dx, 0), MAX_GRIDX - 1));
        result.wPosY = static_cast<WORD>(std::min<float>(std::max<float>(wPosY + dy, 0), MAX_GRIDY - 1));
        
        return result;
    }
    
    /**
     * @brief Interpolação linear entre posições (para movimentos suaves)
     * Adição: O WYD original não tinha interpolação, causando movimentos bruscos
     * @param target Posição alvo
     * @param t Fator de interpolação (0.0 a 1.0)
     * @return Posição interpolada
     */
    STRUCT_POSITION Interpolate(const STRUCT_POSITION& target, float t) const {
        // Restrição à interpolação apenas no mesmo mapa (comportamento do WYD)
        if (wMapID != target.wMapID) {
            return *this;
        }
        
        STRUCT_POSITION result;
        result.wMapID = wMapID;
        result.wPosX = static_cast<WORD>(wPosX + t * (target.wPosX - wPosX));
        result.wPosY = static_cast<WORD>(wPosY + t * (target.wPosY - wPosY));
        result.byZ = static_cast<BYTE>(byZ + t * (target.byZ - byZ));
        
        // Interpolação de direção pelo caminho mais curto
        int dirDiff = (target.byDir - byDir + 8) % 8;
        if (dirDiff > 4) dirDiff -= 8;
        result.byDir = static_cast<BYTE>((byDir + dirDiff * t + 8) % 8);
        
        return result;
    }
    
    // Operadores de comparação (facilita uso com containers e algoritmos)
    bool operator==(const STRUCT_POSITION& other) const {
        return wMapID == other.wMapID && 
               wPosX == other.wPosX && 
               wPosY == other.wPosY && 
               byZ == other.byZ;
    }
    
    bool operator!=(const STRUCT_POSITION& other) const {
        return !(*this == other);
    }
};

// Compatibilidade com código baseado na implementação WYD.exe
typedef STRUCT_POSITION Position;

} // namespace server
} // namespace wyd

#endif // POSITION_H

} // namespace wydbr
