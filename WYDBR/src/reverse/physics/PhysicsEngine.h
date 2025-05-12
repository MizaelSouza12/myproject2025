/**
 * PhysicsEngine.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/reverse/physics/PhysicsEngine.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef PHYSICSENGINE_H
#define PHYSICSENGINE_H

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <array>
#include <mutex>
#include <atomic>
#include <thread>
#include <queue>
#include <cstdint>
#include <optional>

namespace wydbr {


/**
 * @file PhysicsEngine.h
 * @brief Motor de física avançado reconstruído do WYD original
 * 
 * Este arquivo contém a reconstrução do motor de física original do WYD,
 * incluindo detecção de colisão, movimentação de personagens, sistema de
 * gravidade, hitboxes e todas as interações físicas do jogo. Reconstruído
 * a partir de engenharia reversa completa, mantendo compatibilidade total
 * com a versão original, mas com melhorias significativas de desempenho.
 */

namespace wyd {
namespace reverse {
namespace physics {

// Constantes do motor de física
constexpr float DEFAULT_GRAVITY = 9.8f;             // Gravidade padrão
constexpr float DEFAULT_FRICTION = 0.5f;            // Fricção padrão
constexpr float DEFAULT_RESTITUTION = 0.3f;         // Restituição padrão
constexpr float DEFAULT_AIR_RESISTANCE = 0.01f;     // Resistência do ar padrão
constexpr float DEFAULT_WATER_RESISTANCE = 0.1f;    // Resistência da água padrão
constexpr float DEFAULT_TIME_STEP = 1.0f / 60.0f;   // Passo de tempo padrão (60 FPS)
constexpr uint32_t MAX_COLLISION_PAIRS = 4096;      // Máximo de pares de colisão
constexpr uint32_t MAX_RIGID_BODIES = 2048;         // Máximo de corpos rígidos
constexpr uint32_t MAX_STATIC_BODIES = 16384;       // Máximo de corpos estáticos
constexpr uint32_t MAX_COLLISION_SHAPES = 4096;     // Máximo de formas de colisão
constexpr uint32_t MAX_CONSTRAINTS = 1024;          // Máximo de restrições
constexpr uint32_t MAX_TRIGGERS = 512;              // Máximo de gatilhos
constexpr uint32_t BROAD_PHASE_CELL_SIZE = 64;      // Tamanho da célula de fase ampla
constexpr uint32_t MAX_PHYSICS_LAYERS = 32;         // Máximo de camadas de física
constexpr uint32_t MAX_RAY_CASTS = 1024;            // Máximo de ray casts por frame
constexpr uint32_t MAX_SPATIAL_GRID_SIZE = 512;     // Tamanho máximo da grade espacial
constexpr uint32_t MAX_BODY_PARTS = 16;             // Máximo de partes de corpo

/**
 * @brief Tipo de corpo
 */
enum class BodyType : uint32_t {
    STATIC = 0,                        // Estático
    DYNAMIC = 1,                       // Dinâmico
    KINEMATIC = 2,                     // Cinemático
    CHARACTER = 3,                     // Personagem
    RAGDOLL = 4,                       // Ragdoll
    TRIGGER = 5,                       // Gatilho
    SENSOR = 6,                        // Sensor
    GHOST = 7,                         // Fantasma
    CLOTH = 8,                         // Tecido
    SOFT = 9,                          // Macio
    FLUID = 10,                        // Fluido
};

/**
 * @brief Tipo de forma
 */
enum class ShapeType : uint32_t {
    NONE = 0,                          // Nenhum
    BOX = 1,                           // Caixa
    SPHERE = 2,                        // Esfera
    CAPSULE = 3,                       // Cápsula
    CYLINDER = 4,                      // Cilindro
    CONE = 5,                          // Cone
    CONVEX_HULL = 6,                   // Casco convexo
    TRIANGLE_MESH = 7,                 // Malha de triângulos
    HEIGHT_FIELD = 8,                  // Campo de altura
    COMPOUND = 9,                      // Composto
    CUSTOM = 10,                       // Personalizado
};

/**
 * @brief Tipo de restrição
 */
enum class ConstraintType : uint32_t {
    NONE = 0,                          // Nenhum
    POINT = 1,                         // Ponto
    HINGE = 2,                         // Dobradiça
    SLIDER = 3,                        // Deslizante
    CONE_TWIST = 4,                    // Torção de cone
    GENERIC_6DOF = 5,                  // Genérico 6DOF
    SPRING = 6,                        // Mola
    GEAR = 7,                          // Engrenagem
    DISTANCE = 8,                      // Distância
    PULLEY = 9,                        // Polia
    FIXED = 10,                        // Fixo
    CUSTOM = 11,                       // Personalizado
};

/**
 * @brief Tipo de material
 */
enum class MaterialType : uint32_t {
    DEFAULT = 0,                       // Padrão
    METAL = 1,                         // Metal
    WOOD = 2,                          // Madeira
    STONE = 3,                         // Pedra
    GRASS = 4,                         // Grama
    DIRT = 5,                          // Terra
    WATER = 6,                         // Água
    ICE = 7,                           // Gelo
    SAND = 8,                          // Areia
    MUD = 9,                           // Lama
    SNOW = 10,                         // Neve
    CLOTH = 11,                        // Tecido
    RUBBER = 12,                       // Borracha
    GLASS = 13,                        // Vidro
    FLESH = 14,                        // Carne
    CUSTOM = 15,                       // Personalizado
};

/**
 * @brief Tipo de evento de colisão
 */
enum class CollisionEventType : uint32_t {
    NONE = 0,                          // Nenhum
    BEGIN = 1,                         // Início
    STAY = 2,                          // Permanência
    END = 3,                           // Fim
    TRIGGER_ENTER = 4,                 // Entrada de gatilho
    TRIGGER_STAY = 5,                  // Permanência de gatilho
    TRIGGER_EXIT = 6,                  // Saída de gatilho
};

/**
 * @brief Tipo de terreno
 */
enum class TerrainType : uint32_t {
    NONE = 0,                          // Nenhum
    FLAT = 1,                          // Plano
    SLOPED = 2,                        // Inclinado
    UNEVEN = 3,                        // Irregular
    STAIRS = 4,                        // Escadas
    PLATFORM = 5,                      // Plataforma
    WATER = 6,                         // Água
    LAVA = 7,                          // Lava
    CUSTOM = 8,                        // Personalizado
};

/**
 * @brief Vetor 2D
 */
struct Vector2 {
    float x;                           // Componente X
    float y;                           // Componente Y
    
    Vector2()
        : x(0.0f)
        , y(0.0f)
    {}
    
    Vector2(float _x, float _y)
        : x(_x)
        , y(_y)
    {}
    
    // Operadores básicos
    Vector2 operator+(const Vector2& other) const {
        /**
 * Vector2
 * 
 * Implementa a funcionalidade Vector2 conforme especificação original.
 * @param other.x Parâmetro other.x
 * @param other.y Parâmetro other.y
 * @return Retorna return
 */

        return Vector2(x + other.x, y + other.y);
    }
    
    Vector2 operator-(const Vector2& other) const {
        /**
 * Vector2
 * 
 * Implementa a funcionalidade Vector2 conforme especificação original.
 * @param other.x Parâmetro other.x
 * @param other.y Parâmetro other.y
 * @return Retorna return
 */

        return Vector2(x - other.x, y - other.y);
    }
    
    Vector2 operator*(float scalar) const {
        /**
 * Vector2
 * 
 * Implementa a funcionalidade Vector2 conforme especificação original.
 * @param scalar Parâmetro scalar
 * @param scalar Parâmetro scalar
 * @return Retorna return
 */

        return Vector2(x * scalar, y * scalar);
    }
    
    Vector2 operator/(float scalar) const {
        /**
 * Vector2
 * 
 * Implementa a funcionalidade Vector2 conforme especificação original.
 * @param scalar Parâmetro scalar
 * @param scalar Parâmetro scalar
 * @return Retorna return
 */

        return Vector2(x / scalar, y / scalar);
    }
    
    // Produto escalar
    /**
 * Dot
 * 
 * Implementa a funcionalidade Dot conforme especificação original.
 * @param other Parâmetro other
 * @return Retorna float
 */

    float Dot(const Vector2& other) const{
        return x * other.x + y * other.y;
    }
    
    // Comprimento ao quadrado
    /**
 * LengthSquared
 * 
 * Implementa a funcionalidade LengthSquared conforme especificação original.
 * @return Retorna float
 */

    float LengthSquared() const{
        return x * x + y * y;
    }
    
    // Comprimento
    /**
 * Length
 * 
 * Implementa a funcionalidade Length conforme especificação original.
 * @return Retorna float
 */

    float Length() const{
        return std::sqrt(LengthSquared());
    }
    
    // Normalização
    /**
 * Normalized
 * 
 * Implementa a funcionalidade Normalized conforme especificação original.
 * @return Retorna Vector2
 */

    Vector2 Normalized() const{
        float len = Length();
        if (len > 0.0f) {
            /**
 * Vector2
 * 
 * Implementa a funcionalidade Vector2 conforme especificação original.
 * @param len Parâmetro len
 * @param len Parâmetro len
 * @return Retorna return
 */

            return Vector2(x / len, y / len);
        }
        return *this;
    }
    
    // Rotação
    /**
 * Rotated
 * 
 * Implementa a funcionalidade Rotated conforme especificação original.
 * @param angle Parâmetro angle
 * @return Retorna Vector2
 */

    Vector2 Rotated(float angle) const{
        float c = std::cos(angle);
        float s = std::sin(angle);
        /**
 * Vector2
 * 
 * Implementa a funcionalidade Vector2 conforme especificação original.
 * @param s Parâmetro s
 * @param c Parâmetro c
 * @return Retorna return
 */

        return Vector2(x * c - y * s, x * s + y * c);
    }
};

/**
 * @brief Vetor 3D
 */
struct Vector3 {
    float x;                           // Componente X
    float y;                           // Componente Y
    float z;                           // Componente Z
    
    Vector3()
        : x(0.0f)
        , y(0.0f)
        , z(0.0f)
    {}
    
    Vector3(float _x, float _y, float _z)
        : x(_x)
        , y(_y)
        , z(_z)
    {}
    
    // Operadores básicos
    Vector3 operator+(const Vector3& other) const {
        /**
 * Vector3
 * 
 * Implementa a funcionalidade Vector3 conforme especificação original.
 * @param other.x Parâmetro other.x
 * @param other.y Parâmetro other.y
 * @param other.z Parâmetro other.z
 * @return Retorna return
 */

        return Vector3(x + other.x, y + other.y, z + other.z);
    }
    
    Vector3 operator-(const Vector3& other) const {
        /**
 * Vector3
 * 
 * Implementa a funcionalidade Vector3 conforme especificação original.
 * @param other.x Parâmetro other.x
 * @param other.y Parâmetro other.y
 * @param other.z Parâmetro other.z
 * @return Retorna return
 */

        return Vector3(x - other.x, y - other.y, z - other.z);
    }
    
    Vector3 operator*(float scalar) const {
        /**
 * Vector3
 * 
 * Implementa a funcionalidade Vector3 conforme especificação original.
 * @param scalar Parâmetro scalar
 * @param scalar Parâmetro scalar
 * @param scalar Parâmetro scalar
 * @return Retorna return
 */

        return Vector3(x * scalar, y * scalar, z * scalar);
    }
    
    Vector3 operator/(float scalar) const {
        /**
 * Vector3
 * 
 * Implementa a funcionalidade Vector3 conforme especificação original.
 * @param scalar Parâmetro scalar
 * @param scalar Parâmetro scalar
 * @param scalar Parâmetro scalar
 * @return Retorna return
 */

        return Vector3(x / scalar, y / scalar, z / scalar);
    }
    
    // Produto escalar
    /**
 * Dot
 * 
 * Implementa a funcionalidade Dot conforme especificação original.
 * @param other Parâmetro other
 * @return Retorna float
 */

    float Dot(const Vector3& other) const{
        return x * other.x + y * other.y + z * other.z;
    }
    
    // Produto vetorial
    /**
 * Cross
 * 
 * Implementa a funcionalidade Cross conforme especificação original.
 * @param other Parâmetro other
 * @return Retorna Vector3
 */

    Vector3 Cross(const Vector3& other) const{
        /**
 * Vector3
 * 
 * Implementa a funcionalidade Vector3 conforme especificação original.
 * @param other.y Parâmetro other.y
 * @param other.z Parâmetro other.z
 * @param other.x Parâmetro other.x
 * @return Retorna return
 */

        return Vector3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }
    
    // Comprimento ao quadrado
    /**
 * LengthSquared
 * 
 * Implementa a funcionalidade LengthSquared conforme especificação original.
 * @return Retorna float
 */

    float LengthSquared() const{
        return x * x + y * y + z * z;
    }
    
    // Comprimento
    /**
 * Length
 * 
 * Implementa a funcionalidade Length conforme especificação original.
 * @return Retorna float
 */

    float Length() const{
        return std::sqrt(LengthSquared());
    }
    
    // Normalização
    /**
 * Normalized
 * 
 * Implementa a funcionalidade Normalized conforme especificação original.
 * @return Retorna Vector3
 */

    Vector3 Normalized() const{
        float len = Length();
        if (len > 0.0f) {
            /**
 * Vector3
 * 
 * Implementa a funcionalidade Vector3 conforme especificação original.
 * @param len Parâmetro len
 * @param len Parâmetro len
 * @param len Parâmetro len
 * @return Retorna return
 */

            return Vector3(x / len, y / len, z / len);
        }
        return *this;
    }
    
    // Interpolação linear
    /**
 * Lerp
 * 
 * Implementa a funcionalidade Lerp conforme especificação original.
 * @param other Parâmetro other
 * @param t Parâmetro t
 * @return Retorna Vector3
 */

    Vector3 Lerp(const Vector3& other, float t) const{
        return Vector3(
            x + (other.x - x) * t,
            y + (other.y - y) * t,
            z + (other.z - z) * t
        );
    }
};

/**
 * @brief Quaternion
 */
struct Quaternion {
    float x;                           // Componente X
    float y;                           // Componente Y
    float z;                           // Componente Z
    float w;                           // Componente W
    
    Quaternion()
        : x(0.0f)
        , y(0.0f)
        , z(0.0f)
        , w(1.0f)
    {}
    
    Quaternion(float _x, float _y, float _z, float _w)
        : x(_x)
        , y(_y)
        , z(_z)
        , w(_w)
    {}
    
    // Construtor a partir de eixo-â/**
 * Quaternion
 * 
 * Implementa a funcionalidade Quaternion conforme especificação original.
 * @param axis Parâmetro axis
 * @param angle Parâmetro angle
 * @return Retorna ngulo
 */
ngulo
    Quaternion(const Vector3& axis, float angle){
        float halfAngle = angle * 0.5f;
        float s = std::sin(halfAngle);
        float c = std::cos(halfAngle);
        Vector3 normalizedAxis = axis.Normalized();
        x = normalizedAxis.x * s;
        y = normalizedAxis.y * s;
        z = normalizedAxis.z * s;
        w = c;
    }
    
    // Operadores básicos
    Quaternion operator*(const Quaternion& other) const {
        /**
 * Quaternion
 * 
 * Implementa a funcionalidade Quaternion conforme especificação original.
 * @param other.y Parâmetro other.y
 * @param other.x Parâmetro other.x
 * @param other.w Parâmetro other.w
 * @param other.z Parâmetro other.z
 * @return Retorna return
 */

        return Quaternion(
            w * other.x + x * other.w + y * other.z - z * other.y,
            w * other.y - x * other.z + y * other.w + z * other.x,
            w * other.z + x * other.y - y * other.x + z * other.w,
            w * other.w - x * other.x - y * other.y - z * other.z
        );
    }
    
    // Rotação de vetor
    /**
 * Rotate
 * 
 * Implementa a funcionalidade Rotate conforme especificação original.
 * @param v Parâmetro v
 * @return Retorna Vector3
 */

    Vector3 Rotate(const Vector3& v) const{
        Quaternion q = *this;
        Quaternion qConjugate = Conjugate();
        /**
 * qv
 * 
 * Implementa a funcionalidade qv conforme especificação original.
 * @param v.x Parâmetro v.x
 * @param v.y Parâmetro v.y
 * @param v.z Parâmetro v.z
 * @param 0.0f Parâmetro 0.0f
 * @return Retorna Quaternion
 */

        Quaternion qv(v.x, v.y, v.z, 0.0f);
        Quaternion result = q * qv * qConjugate;
        /**
 * Vector3
 * 
 * Implementa a funcionalidade Vector3 conforme especificação original.
 * @param result.x Parâmetro result.x
 * @param result.y Parâmetro result.y
 * @param result.z Parâmetro result.z
 * @return Retorna return
 */

        return Vector3(result.x, result.y, result.z);
    }
    
    // Conjugado
    /**
 * Conjugate
 * 
 * Implementa a funcionalidade Conjugate conforme especificação original.
 * @return Retorna Quaternion
 */

    Quaternion Conjugate() const{
        /**
 * Quaternion
 * 
 * Implementa a funcionalidade Quaternion conforme especificação original.
 * @param -x Parâmetro -x
 * @param -y Parâmetro -y
 * @param -z Parâmetro -z
 * @param w Parâmetro w
 * @return Retorna return
 */

        return Quaternion(-x, -y, -z, w);
    }
    
    // Normalização
    /**
 * Normalized
 * 
 * Implementa a funcionalidade Normalized conforme especificação original.
 * @return Retorna Quaternion
 */

    Quaternion Normalized() const{
        float magnitude = std::sqrt(x * x + y * y + z * z + w * w);
        if (magnitude > 0.0f) {
            /**
 * Quaternion
 * 
 * Implementa a funcionalidade Quaternion conforme especificação original.
 * @param magnitude Parâmetro magnitude
 * @param magnitude Parâmetro magnitude
 * @param magnitude Parâmetro magnitude
 * @param magnitude Parâmetro magnitude
 * @return Retorna return
 */

            return Quaternion(x / magnitude, y / magnitude, z / magnitude, w / magnitude);
        }
        return *this;
    }
    
    // Interpolação esférica
    /**
 * Slerp
 * 
 * Implementa a funcionalidade Slerp conforme especificação original.
 * @param other Parâmetro other
 * @param t Parâmetro t
 * @return Retorna Quaternion
 */

    Quaternion Slerp(const Quaternion& other, float t) const{
        // Implementação da interpolação esférica
        float cosHalfTheta = x * other.x + y * other.y + z * other.z + w * other.w;
        
        if (std::abs(cosHalfTheta) >= 1.0f) {
            return *this;
        }
        
        float halfTheta = std::acos(cosHalfTheta);
        float sinHalfTheta = std::sqrt(1.0f - cosHalfTheta * cosHalfTheta);
        
        if (std::abs(sinHalfTheta) < 0.001f) {
            /**
 * Quaternion
 * 
 * Implementa a funcionalidade Quaternion conforme especificação original.
 * @param 0.5f Parâmetro 0.5f
 * @param 0.5f Parâmetro 0.5f
 * @param 0.5f Parâmetro 0.5f
 * @param 0.5f Parâmetro 0.5f
 * @return Retorna return
 */

            return Quaternion(
                x * 0.5f + other.x * 0.5f,
                y * 0.5f + other.y * 0.5f,
                z * 0.5f + other.z * 0.5f,
                w * 0.5f + other.w * 0.5f
            );
        }
        
        float ratioA = std::sin((1.0f - t) * halfTheta) / sinHalfTheta;
        float ratioB = std::sin(t * halfTheta) / sinHalfTheta;
        
        /**
 * Quaternion
 * 
 * Implementa a funcionalidade Quaternion conforme especificação original.
 * @param ratioB Parâmetro ratioB
 * @param ratioB Parâmetro ratioB
 * @param ratioB Parâmetro ratioB
 * @param ratioB Parâmetro ratioB
 * @return Retorna return
 */

        
        return Quaternion(
            x * ratioA + other.x * ratioB,
            y * ratioA + other.y * ratioB,
            z * ratioA + other.z * ratioB,
            w * ratioA + other.w * ratioB
        );
    }
    
    // Conversão para ângulos de Euler (yaw, pitch, roll)
    /**
 * ToEulerAngles
 * 
 * Implementa a funcionalidade ToEulerAngles conforme especificação original.
 * @return Retorna Vector3
 */

    Vector3 ToEulerAngles() const{
        Vector3 angles;
        
        // Pitch (X)
        float sinp = 2.0f * (w * x - y * z);
        if (std::abs(sinp) >= 1.0f) {
            angles.x = std::copysign(3.14159265f / 2.0f, sinp);
        } else {
            angles.x = std::asin(sinp);
        }
        
        // Yaw (Y)
        float siny_cosp = 2.0f * (w * y + z * x);
        float cosy_cosp = 1.0f - 2.0f * (x * x + y * y);
        angles.y = std::atan2(siny_cosp, cosy_cosp);
        
        // Roll (Z)
        float sinr_cosp = 2.0f * (w * z + x * y);
        float cosr_cosp = 1.0f - 2.0f * (y * y + z * z);
        angles.z = std::atan2(sinr_cosp, cosr_cosp);
        
        return angles;
    }
    
    // Construtor a partir de ângulos de Euler (yaw, pitch, roll)
    /**
 * FromEulerAngles
 * 
 * Implementa a funcionalidade FromEulerAngles conforme especificação original.
 * @param pitch Parâmetro pitch
 * @param yaw Parâmetro yaw
 * @param roll Parâmetro roll
 * @return Retorna Quaternion
 */

    static Quaternion FromEulerAngles(float pitch, float yaw, float roll){
        float cy = std::cos(yaw * 0.5f);
        float sy = std::sin(yaw * 0.5f);
        float cp = std::cos(pitch * 0.5f);
        float sp = std::sin(pitch * 0.5f);
        float cr = std::cos(roll * 0.5f);
        float sr = std::sin(roll * 0.5f);
        
        Quaternion q;
        q.w = cr * cp * cy + sr * sp * sy;
        q.x = sr * cp * cy - cr * sp * sy;
        q.y = cr * sp * cy + sr * cp * sy;
        q.z = cr * cp * sy - sr * sp * cy;
        
        return q;
    }
};

/**
 * @brief Matriz 3x3
 */
struct Matrix3 {
    float m[3][3];                     // Elementos da /**
 * Matrix3
 * 
 * Implementa a funcionalidade Matrix3 conforme especificação original.
 * @return Retorna matriz
 */
 matriz
    
    Matrix3(){
        // Inicializa como matriz /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param i++ Parâmetro i++
 * @return Retorna identidade
 */
 identidade
        for(int i = 0; i < 3; i++){
            for (int j = 0; j < 3; j++) {
                m[i][j] = (i == j) ? 1.0f : 0.0f;
            }
        }
    }
    
    // Construtor com elementos explí/**
 * Matrix3
 * 
 * Implementa a funcionalidade Matrix3 conforme especificação original.
 * @param m00 Parâmetro m00
 * @param m01 Parâmetro m01
 * @param m02 Parâmetro m02
 * @param m10 Parâmetro m10
 * @param m11 Parâmetro m11
 * @param m12 Parâmetro m12
 * @param m20 Parâmetro m20
 * @param m21 Parâmetro m21
 * @param m22 Parâmetro m22
 * @return Retorna citos
 */
citos
    Matrix3(float m00, float m01, float m02,
           float m10, float m11, float m12,
           float m20, float m21, float m22){
        m[0][0] = m00; m[0][1] = m01; m[0][2] = m02;
        m[1][0] = m10; m[1][1] = m11; m[1][2] = m12;
        m[2][0] = m20; m[2][1] = m21; m[2][2] = m22;
    }
    
    // Multiplicação de matriz por vetor
    Vector3 operator*(const Vector3& v) const {
        /**
 * Vector3
 * 
 * Implementa a funcionalidade Vector3 conforme especificação original.
 * @param v.z Parâmetro v.z
 * @param v.z Parâmetro v.z
 * @param v.z Parâmetro v.z
 * @return Retorna return
 */

        return Vector3(
            m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z,
            m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z,
            m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z
        );
    }
    
    // Multiplicação de matriz por matriz
    Matrix3 operator*(const Matrix3& other) const {
        Matrix3 result;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                result.m[i][j] = 0.0f;
                for (int k = 0; k < 3; k++) {
                    result.m[i][j] += m[i][k] * other.m[k][j];
                }
            }
        }
        return result;
    }
    
    // Matriz de rotação a partir de quaternion
    /**
 * FromQuaternion
 * 
 * Implementa a funcionalidade FromQuaternion conforme especificação original.
 * @param q Parâmetro q
 * @return Retorna Matrix3
 */

    static Matrix3 FromQuaternion(const Quaternion& q){
        float xx = q.x * q.x;
        float yy = q.y * q.y;
        float zz = q.z * q.z;
        float xy = q.x * q.y;
        float xz = q.x * q.z;
        float yz = q.y * q.z;
        float wx = q.w * q.x;
        float wy = q.w * q.y;
        float wz = q.w * q.z;
        
        return Matrix3(
            1.0f - 2.0f * (yy + zz), 2.0f * (xy - wz), 2.0f * (xz + wy),
            2.0f * (xy + wz), 1.0f - 2.0f * (xx + zz), 2.0f * (yz - wx),
            2.0f * (xz - wy), 2.0f * (yz + wx), 1.0f - 2.0f * (xx + yy)
        );
    }
    
    // Determinante
    /**
 * Determinant
 * 
 * Implementa a funcionalidade Determinant conforme especificação original.
 * @return Retorna float
 */

    float Determinant() const{
        return m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]) -
               m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0]) +
               m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);
    }
    
    // Inversa
    /**
 * Inverse
 * 
 * Implementa a funcionalidade Inverse conforme especificação original.
 * @return Retorna Matrix3
 */

    Matrix3 Inverse() const{
        float det = Determinant();
        if (std::abs(det) < 0.0001f) {
            /**
 * Matrix3
 * 
 * Implementa a funcionalidade Matrix3 conforme especificação original.
 * @return Retorna return
 */

            return Matrix3(); // Retorna matriz identidade se não for invertível
        }
        
        float invDet = 1.0f / det;
        
        return Matrix3(
            (m[1][1] * m[2][2] - m[1][2] * m[2][1]) * invDet,
            (m[0][2] * m[2][1] - m[0][1] * m[2][2]) * invDet,
            (m[0][1] * m[1][2] - m[0][2] * m[1][1]) * invDet,
            
            (m[1][2] * m[2][0] - m[1][0] * m[2][2]) * invDet,
            (m[0][0] * m[2][2] - m[0][2] * m[2][0]) * invDet,
            (m[0][2] * m[1][0] - m[0][0] * m[1][2]) * invDet,
            
            (m[1][0] * m[2][1] - m[1][1] * m[2][0]) * invDet,
            (m[0][1] * m[2][0] - m[0][0] * m[2][1]) * invDet,
            (m[0][0] * m[1][1] - m[0][1] * m[1][0]) * invDet
        );
    }
    
    // Transposta
    /**
 * Transpose
 * 
 * Implementa a funcionalidade Transpose conforme especificação original.
 * @return Retorna Matrix3
 */

    Matrix3 Transpose() const{
        Matrix3 result;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                result.m[i][j] = m[j][i];
            }
        }
        return result;
    }
};

/**
 * @brief Matriz 4x4
 */
struct Matrix4 {
    float m[4][4];                     // Elementos da /**
 * Matrix4
 * 
 * Implementa a funcionalidade Matrix4 conforme especificação original.
 * @return Retorna matriz
 */
 matriz
    
    Matrix4(){
        // Inicializa como matriz /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param i++ Parâmetro i++
 * @return Retorna identidade
 */
 identidade
        for(int i = 0; i < 4; i++){
            for (int j = 0; j < 4; j++) {
                m[i][j] = (i == j) ? 1.0f : 0.0f;
            }
        }
    }
    
    // Construtor com elementos explícitos (apenas primeiros 3x3)
    Matrix4(const Matrix3& mat3) {
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                m[i][j] = mat3.m[i][j];
            }
            m[i][3] = 0.0f;
        }
        m[3][0] = 0.0f;
        m[3][1] = 0.0f;
        m[3][2] = 0.0f;
        m[3][3] = 1.0f;
    }
    
    // Construtor com rotação e translaçã/**
 * Matrix4
 * 
 * Implementa a funcionalidade Matrix4 conforme especificação original.
 * @param rotation Parâmetro rotation
 * @param translation Parâmetro translation
 * @return Retorna o
 */
o
    Matrix4(const Matrix3& rotation, const Vector3& translation){
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                m[i][j] = rotation.m[i][j];
            }
            m[i][3] = 0.0f;
        }
        m[3][0] = translation.x;
        m[3][1] = translation.y;
        m[3][2] = translation.z;
        m[3][3] = 1.0f;
    }
    
    // Multiplicação de matriz por vetor
    Vector3 operator*(const Vector3& v) const {
        float w = m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3];
        if (std::abs(w) > 0.0001f) {
            w = 1.0f / w;
        } else {
            w = 1.0f;
        }
        
        return Vector3(
            (m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3]) * w,
            (m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3]) * w,
            (m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3]) * w
        );
    }
    
    // Multiplicação de matriz por matriz
    Matrix4 operator*(const Matrix4& other) const {
        Matrix4 result;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                result.m[i][j] = 0.0f;
                for (int k = 0; k < 4; k++) {
                    result.m[i][j] += m[i][k] * other.m[k][j];
                }
            }
        }
        return result;
    }
    
    // Obtém a parte de rotação (3x3)
    /**
 * GetRotation
 * 
 * Implementa a funcionalidade GetRotation conforme especificação original.
 * @return Retorna Matrix3
 */

    Matrix3 GetRotation() const{
        /**
 * Matrix3
 * 
 * Implementa a funcionalidade Matrix3 conforme especificação original.
 * @param m00 Parâmetro m00
 * @param m01 Parâmetro m01
 * @param m02 Parâmetro m02
 * @param m10 Parâmetro m10
 * @param m11 Parâmetro m11
 * @param m12 Parâmetro m12
 * @param m20 Parâmetro m20
 * @param m21 Parâmetro m21
 * @param m22 Parâmetro m22
 * @return Retorna return
 */

        return Matrix3(
            m[0][0], m[0][1], m[0][2],
            m[1][0], m[1][1], m[1][2],
            m[2][0], m[2][1], m[2][2]
        );
    }
    
    // Obtém a parte de translação
    /**
 * GetTranslation
 * 
 * Implementa a funcionalidade GetTranslation conforme especificação original.
 * @return Retorna Vector3
 */

    Vector3 GetTranslation() const{
        /**
 * Vector3
 * 
 * Implementa a funcionalidade Vector3 conforme especificação original.
 * @param m30 Parâmetro m30
 * @param m31 Parâmetro m31
 * @param m32 Parâmetro m32
 * @return Retorna return
 */

        return Vector3(m[3][0], m[3][1], m[3][2]);
    }
    
    // Matriz de transformação a partir de posição e rotação
    /**
 * FromTransform
 * 
 * Implementa a funcionalidade FromTransform conforme especificação original.
 * @param position Parâmetro position
 * @param rotation Parâmetro rotation
 * @return Retorna Matrix4
 */

    static Matrix4 FromTransform(const Vector3& position, const Quaternion& rotation){
        Matrix3 rotMat = Matrix3::FromQuaternion(rotation);
        /**
 * Matrix4
 * 
 * Implementa a funcionalidade Matrix4 conforme especificação original.
 * @param rotMat Parâmetro rotMat
 * @param position Parâmetro position
 * @return Retorna return
 */

        return Matrix4(rotMat, position);
    }
    
    // Matriz de transformação a partir de posição, rotação e escala
    /**
 * FromTransform
 * 
 * Implementa a funcionalidade FromTransform conforme especificação original.
 * @param position Parâmetro position
 * @param rotation Parâmetro rotation
 * @param scale Parâmetro scale
 * @return Retorna Matrix4
 */

    static Matrix4 FromTransform(const Vector3& position, const Quaternion& rotation, const Vector3& scale){
        Matrix3 rotMat = Matrix3::FromQuaternion(rotation);
        Matrix4 result;
        
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                result.m[i][j] = rotMat.m[i][j];
            }
        }
        
        // Aplica escala
        result.m[0][0] *= scale.x;
        result.m[1][1] *= scale.y;
        result.m[2][2] *= scale.z;
        
        // Aplica translação
        result.m[3][0] = position.x;
        result.m[3][1] = position.y;
        result.m[3][2] = position.z;
        
        return result;
    }
    
    // Inversa
    /**
 * Inverse
 * 
 * Implementa a funcionalidade Inverse conforme especificação original.
 * @return Retorna Matrix4
 */

    Matrix4 Inverse() const{
        // Implementação da inversa para matrizes 4x4
        // (Este é um método simplificado que assume que a matriz é uma transformação rígida)
        Matrix3 rotation = GetRotation();
        Vector3 translation = GetTranslation();
        
        Matrix3 invRotation = rotation.Transpose();
        Vector3 invTranslation = invRotation * (translation * -1.0f);
        
        /**
 * result
 * 
 * Implementa a funcionalidade result conforme especificação original.
 * @param invRotation Parâmetro invRotation
 * @param invTranslation Parâmetro invTranslation
 * @return Retorna Matrix4
 */

        
        Matrix4 result(invRotation, invTranslation);
        return result;
    }
    
    // Transposta
    /**
 * Transpose
 * 
 * Implementa a funcionalidade Transpose conforme especificação original.
 * @return Retorna Matrix4
 */

    Matrix4 Transpose() const{
        Matrix4 result;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                result.m[i][j] = m[j][i];
            }
        }
        return result;
    }
};

/**
 * @brief Transformação
 */
struct Transform {
    Vector3 position;                  // Posição
    Quaternion rotation;               // Rotação
    Vector3 scale;                     // Escala
    
    Transform()
        : position()
        , rotation()
        , scale(1.0f, 1.0f, 1.0f)
    {}
    
    Transform(const Vector3& pos, const Quaternion& rot, const Vector3& scl)
        : position(pos)
        , rotation(rot)
        , scale(scl)
    {}
    
    // Converte para matriz 4x4
    /**
 * ToMatrix
 * 
 * Implementa a funcionalidade ToMatrix conforme especificação original.
 * @return Retorna Matrix4
 */

    Matrix4 ToMatrix() const{
        return Matrix4::FromTransform(position, rotation, scale);
    }
    
    // Interpola entre duas transformações
    /**
 * Lerp
 * 
 * Implementa a funcionalidade Lerp conforme especificação original.
 * @param a Parâmetro a
 * @param b Parâmetro b
 * @param t Parâmetro t
 * @return Retorna Transform
 */

    static Transform Lerp(const Transform& a, const Transform& b, float t){
        return Transform(
            a.position.Lerp(b.position, t),
            a.rotation.Slerp(b.rotation, t),
            Vector3(
                a.scale.x + (b.scale.x - a.scale.x) * t,
                a.scale.y + (b.scale.y - a.scale.y) * t,
                a.scale.z + (b.scale.z - a.scale.z) * t
            )
        );
    }
};

/**
 * @brief AABB (Axis-Aligned Bounding Box)
 */
struct AABB {
    Vector3 min;                       // Ponto mínimo
    Vector3 max;                       // Ponto máximo
    
    AABB()
        : min(0.0f, 0.0f, 0.0f)
        , max(0.0f, 0.0f, 0.0f)
    {}
    
    AABB(const Vector3& _min, const Vector3& _max)
        : min(_min)
        , max(_max)
    {}
    
    // Verifica se o AABB contém um ponto
    /**
 * Contains
 * 
 * Implementa a funcionalidade Contains conforme especificação original.
 * @param point Parâmetro point
 * @return Retorna bool
 */

    bool Contains(const Vector3& point) const{
        return point.x >= min.x && point.x <= max.x &&
               point.y >= min.y && point.y <= max.y &&
               point.z >= min.z && point.z <= max.z;
    }
    
    // Verifica se o AABB contém outro AABB
    /**
 * Contains
 * 
 * Implementa a funcionalidade Contains conforme especificação original.
 * @param other Parâmetro other
 * @return Retorna bool
 */

    bool Contains(const AABB& other) const{
        return min.x <= other.min.x && max.x >= other.max.x &&
               min.y <= other.min.y && max.y >= other.max.y &&
               min.z <= other.min.z && max.z >= other.max.z;
    }
    
    // Verifica se o AABB intersecciona com outro AABB
    /**
 * Intersects
 * 
 * Implementa a funcionalidade Intersects conforme especificação original.
 * @param other Parâmetro other
 * @return Retorna bool
 */

    bool Intersects(const AABB& other) const{
        return min.x <= other.max.x && max.x >= other.min.x &&
               min.y <= other.max.y && max.y >= other.min.y &&
               min.z <= other.max.z && max.z >= other.min.z;
    }
    
    // Calcula a união com outro AABB
    /**
 * Union
 * 
 * Implementa a funcionalidade Union conforme especificação original.
 * @param other Parâmetro other
 * @return Retorna AABB
 */

    AABB Union(const AABB& other) const{
        return AABB(
            Vector3(
                std::min(min.x, other.min.x),
                std::min(min.y, other.min.y),
                std::min(min.z, other.min.z)
            ),
            Vector3(
                std::max(max.x, other.max.x),
                std::max(max.y, other.max.y),
                std::max(max.z, other.max.z)
            )
        );
    }
    
    // Calcula o centro do AABB
    /**
 * GetCenter
 * 
 * Implementa a funcionalidade GetCenter conforme especificação original.
 * @return Retorna Vector3
 */

    Vector3 GetCenter() const{
        return Vector3(
            (min.x + max.x) * 0.5f,
            (min.y + max.y) * 0.5f,
            (min.z + max.z) * 0.5f
        );
    }
    
    // Calcula as dimensões do AABB
    /**
 * GetExtents
 * 
 * Implementa a funcionalidade GetExtents conforme especificação original.
 * @return Retorna Vector3
 */

    Vector3 GetExtents() const{
        return Vector3(
            (max.x - min.x) * 0.5f,
            (max.y - min.y) * 0.5f,
            (max.z - min.z) * 0.5f
        );
    }
    
    // Dimensões totais
    /**
 * GetSize
 * 
 * Implementa a funcionalidade GetSize conforme especificação original.
 * @return Retorna Vector3
 */

    Vector3 GetSize() const{
        /**
 * Vector3
 * 
 * Implementa a funcionalidade Vector3 conforme especificação original.
 * @param min.x Parâmetro min.x
 * @param min.y Parâmetro min.y
 * @param min.z Parâmetro min.z
 * @return Retorna return
 */

        return Vector3(
            max.x - min.x,
            max.y - min.y,
            max.z - min.z
        );
    }
    
    // Expande o AABB por um valor
    /**
 * Expand
 * 
 * Implementa a funcionalidade Expand conforme especificação original.
 * @param amount Parâmetro amount
 */

    void Expand(float amount){
        min.x -= amount;
        min.y -= amount;
        min.z -= amount;
        max.x += amount;
        max.y += amount;
        max.z += amount;
    }
    
    // Transforma o AABB por uma matriz
    /**
 * Transformed
 * 
 * Implementa a funcionalidade Transformed conforme especificação original.
 * @param matrix Parâmetro matrix
 * @return Retorna AABB
 */

    AABB Transformed(const Matrix4& matrix) const{
        // Transformação de um AABB por uma matriz
        // Essa implementação mantém o alinhamento aos eixos após a transformação
        Vector3 corners[8] = {
            Vector3(min.x, min.y, min.z), // 000
            Vector3(max.x, min.y, min.z), // 100
            Vector3(min.x, max.y, min.z), // 010
            Vector3(max.x, max.y, min.z), // 110
            Vector3(min.x, min.y, max.z), // 001
            Vector3(max.x, min.y, max.z), // 101
            Vector3(min.x, max.y, max.z), // 011
            Vector3(max.x, max.y, max.z)  // 111
        };
        
        Vector3 newMin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
        Vector3 newMax(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest());
        
        for (int i = 0; i < 8; i++) {
            Vector3 transformed = matrix * corners[i];
            
            newMin.x = std::min(newMin.x, transformed.x);
            newMin.y = std::min(newMin.y, transformed.y);
            newMin.z = std::min(newMin.z, transformed.z);
            
            newMax.x = std::max(newMax.x, transformed.x);
            newMax.y = std::max(newMax.y, transformed.y);
            newMax.z = std::max(newMax.z, transformed.z);
        }
        
        /**
 * AABB
 * 
 * Implementa a funcionalidade AABB conforme especificação original.
 * @param newMin Parâmetro newMin
 * @param newMax Parâmetro newMax
 * @return Retorna return
 */

        
        return AABB(newMin, newMax);
    }
};

/**
 * @brief Raio
 */
struct Ray {
    Vector3 origin;                    // Origem
    Vector3 direction;                 // Direção
    float maxDistance;                 // Distância máxima
    
    Ray()
        : origin()
        , direction(0.0f, 0.0f, 1.0f)
        , maxDistance(std::numeric_limits<float>::max())
    {}
    
    Ray(const Vector3& org, const Vector3& dir, float maxDist = std::numeric_limits<float>::max())
        : origin(org)
        , direction(dir.Normalized())
        , maxDistance(maxDist)
    {}
    
    // Obtém um ponto ao longo do raio
    /**
 * GetPoint
 * 
 * Implementa a funcionalidade GetPoint conforme especificação original.
 * @param distance Parâmetro distance
 * @return Retorna Vector3
 */

    Vector3 GetPoint(float distance) const{
        return origin + direction * distance;
    }
    
    // Intersecção com AABB
    /**
 * Intersects
 * 
 * Implementa a funcionalidade Intersects conforme especificação original.
 * @param aabb Parâmetro aabb
 * @param distance Parâmetro distance
 * @return Retorna bool
 */

    bool Intersects(const AABB& aabb, float& distance) const{
        Vector3 invDir(
            direction.x != 0.0f ? 1.0f / direction.x : std::numeric_limits<float>::max(),
            direction.y != 0.0f ? 1.0f / direction.y : std::numeric_limits<float>::max(),
            direction.z != 0.0f ? 1.0f / direction.z : std::numeric_limits<float>::max()
        );
        
        Vector3 t0 = (aabb.min - origin) * invDir;
        Vector3 t1 = (aabb.max - origin) * invDir;
        
        Vector3 tmin(
            std::min(t0.x, t1.x),
            std::min(t0.y, t1.y),
            std::min(t0.z, t1.z)
        );
        
        Vector3 tmax(
            std::max(t0.x, t1.x),
            std::max(t0.y, t1.y),
            std::max(t0.z, t1.z)
        );
        
        float dmin = std::max(std::max(tmin.x, tmin.y), tmin.z);
        float dmax = std::min(std::min(tmax.x, tmax.y), tmax.z);
        
        if (dmax < 0.0f || dmin > dmax || dmin > maxDistance) {
            return false;
        }
        
        distance = dmin;
        return true;
    }
    
    // Intersecção com esfera
    /**
 * Intersects
 * 
 * Implementa a funcionalidade Intersects conforme especificação original.
 * @param center Parâmetro center
 * @param radius Parâmetro radius
 * @param distance Parâmetro distance
 * @return Retorna bool
 */

    bool Intersects(const Vector3& center, float radius, float& distance) const{
        Vector3 oc = origin - center;
        float a = direction.Dot(direction);
        float b = 2.0f * oc.Dot(direction);
        float c = oc.Dot(oc) - radius * radius;
        float discriminant = b * b - 4.0f * a * c;
        
        if (discriminant < 0.0f) {
            return false;
        }
        
        float sqrtd = std::sqrt(discriminant);
        float t1 = (-b - sqrtd) / (2.0f * a);
        float t2 = (-b + sqrtd) / (2.0f * a);
        
        if (t1 < 0.0f && t2 < 0.0f) {
            return false;
        }
        
        if (t1 < 0.0f) {
            distance = t2;
        } /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 0.0f Parâmetro 0.0f
 * @return Retorna else
 */
 else if(t2 < 0.0f){
            distance = t1;
        } else {
            distance = std::min(t1, t2);
        }
        
        if (distance > maxDistance) {
            return false;
        }
        
        return true;
    }
};

/**
 * @brief Plano
 */
struct Plane {
    Vector3 normal;                    // Normal
    float distance;                    // Distância
    
    Plane()
        : normal(0.0f, 1.0f, 0.0f)
        , distance(0.0f)
    {}
    
    Plane(const Vector3& n, float d)
        : normal(n.Normalized())
        , distance(d)
    {}
    
    Plane(const Vector3& n, const Vector3& point)
        : normal(n.Normalized())
        , distance(-normal.Dot(point))
    {}
    
    // Classificação de ponto
    /**
 * ClassifyPoint
 * 
 * Implementa a funcionalidade ClassifyPoint conforme especificação original.
 * @param point Parâmetro point
 * @return Retorna float
 */

    float ClassifyPoint(const Vector3& point) const{
        return normal.Dot(point) + distance;
    }
    
    // Verifica de que lado do plano está um ponto
    /**
 * GetSide
 * 
 * Implementa a funcionalidade GetSide conforme especificação original.
 * @param point Parâmetro point
 * @return Retorna int
 */

    int GetSide(const Vector3& point) const{
        float d = ClassifyPoint(point);
        if (d > 0.0001f) return 1;     // Frente
        if (d < -0.0001f) return -1;   // Atrás
        return 0;                       // No plano
    }
    
    // Intersecção com raio
    /**
 * Intersects
 * 
 * Implementa a funcionalidade Intersects conforme especificação original.
 * @param ray Parâmetro ray
 * @param distance Parâmetro distance
 * @return Retorna bool
 */

    bool Intersects(const Ray& ray, float& distance) const{
        float denom = normal.Dot(ray.direction);
        if (std::abs(denom) < 0.0001f) {
            return false; // Raio paralelo ao plano
        }
        
        float t = -(normal.Dot(ray.origin) + this->distance) / denom;
        if (t < 0.0f || t > ray.maxDistance) {
            return false; // Intersecção fora dos limites do raio
        }
        
        distance = t;
        return true;
    }
    
    // Projeta um ponto no plano
    /**
 * ProjectPoint
 * 
 * Implementa a funcionalidade ProjectPoint conforme especificação original.
 * @param point Parâmetro point
 * @return Retorna Vector3
 */

    Vector3 ProjectPoint(const Vector3& point) const{
        float d = ClassifyPoint(point);
        return point - normal * d;
    }
};

/**
 * @brief Resultado de detecção de colisão
 */
struct CollisionResult {
    bool hasCollision;                 // Tem colisão
    Vector3 point;                     // Ponto de colisão
    Vector3 normal;                    // Normal de colisão
    float penetration;                 // Penetração
    float friction;                    // Fricção
    float restitution;                 // Restituição
    uint32_t shapeIdA;                 // ID da forma A
    uint32_t shapeIdB;                 // ID da forma B
    uint32_t bodyIdA;                  // ID do corpo A
    uint32_t bodyIdB;                  // ID do corpo B
    
    CollisionResult()
        : hasCollision(false)
        , point()
        , normal()
        , penetration(0.0f)
        , friction(DEFAULT_FRICTION)
        , restitution(DEFAULT_RESTITUTION)
        , shapeIdA(0)
        , shapeIdB(0)
        , bodyIdA(0)
        , bodyIdB(0)
    {}
};

/**
 * @brief Resultado de ray cast
 */
struct RaycastResult {
    bool hasHit;                       // Tem hit
    float distance;                    // Distância
    Vector3 point;                     // Ponto de hit
    Vector3 normal;                    // Normal
    uint32_t shapeId;                  // ID da forma
    uint32_t bodyId;                   // ID do corpo
    uint32_t faceIndex;                // Índice da face
    
    RaycastResult()
        : hasHit(false)
        , distance(0.0f)
        , point()
        , normal()
        , shapeId(0)
        , bodyId(0)
        , faceIndex(0)
    {}
};

/**
 * @brief Material físico
 */
struct PhysicsMaterial {
    float friction;                    // Fricção
    float restitution;                 // Restituição
    float density;                     // Densidade
    MaterialType type;                 // Tipo
    
    PhysicsMaterial()
        : friction(DEFAULT_FRICTION)
        , restitution(DEFAULT_RESTITUTION)
        , density(1.0f)
        , type(MaterialType::DEFAULT)
    {}
    
    PhysicsMaterial(float f, float r, float d, MaterialType t)
        : friction(f)
        , restitution(r)
        , density(d)
        , type(t)
    {}
    
    // Combinação de materiais
    /**
 * Combine
 * 
 * Implementa a funcionalidade Combine conforme especificação original.
 * @param a Parâmetro a
 * @param b Parâmetro b
 * @return Retorna PhysicsMaterial
 */

    static PhysicsMaterial Combine(const PhysicsMaterial& a, const PhysicsMaterial& b){
        return PhysicsMaterial(
            std::sqrt(a.friction * b.friction),
            std::max(a.restitution, b.restitution),
            (a.density + b.density) * 0.5f,
            a.type // Mantém o tipo do primeiro material
        );
    }
};

/**
 * @brief Forma de colisão
 */
struct CollisionShape {
    uint32_t id;                       // ID
    ShapeType type;                    // Tipo
    PhysicsMaterial material;          // Material
    Vector3 localPosition;             // Posição local
    Quaternion localRotation;          // Rotação local
    Vector3 localScale;                // Escala local
    AABB boundingBox;                  // Bounding box
    float volume;                      // Volume
    bool isSensor;                     // É sensor
    bool isEnabled;                    // Está habilitado
    void* userData;                    // Dados do usuário
    uint32_t bodyId;                   // ID do corpo
    uint32_t collisionGroup;           // Grupo de colisão
    uint32_t collisionMask;            // Máscara de colisão
    std::string name;                  // Nome
    
    union {
        struct {
            Vector3 halfExtents;       // Box: Metade das extensões
        } box;
        
        struct {
            float radius;              // Sphere: Raio
        } sphere;
        
        struct {
            float radius;              // Capsule: Raio
            float height;              // Capsule: Altura
        } capsule;
        
        struct {
            float radius;              // Cylinder: Raio
            float height;              // Cylinder: Altura
        } cylinder;
        
        struct {
            float radius;              // Cone: Raio
            float height;              // Cone: Altura
        } cone;
        
        struct {
            uint32_t numVertices;      // ConvexHull: Número de vértices
            Vector3* vertices;         // ConvexHull: Vértices
        } convexHull;
        
        struct {
            uint32_t numVertices;      // TriangleMesh: Número de vértices
            uint32_t numIndices;       // TriangleMesh: Número de índices
            Vector3* vertices;         // TriangleMesh: Vértices
            uint32_t* indices;         // TriangleMesh: Índices
        } triangleMesh;
        
        struct {
            uint32_t numRows;          // HeightField: Número de linhas
            uint32_t numCols;          // HeightField: Número de colunas
            float* heights;            // HeightField: Alturas
            float minHeight;           // HeightField: Altura mínima
            float maxHeight;           // HeightField: Altura máxima
            float cellSize;            // HeightField: Tamanho da célula
        } heightField;
        
        struct {
            uint32_t numShapes;        // Compound: Número de formas
            uint32_t* shapeIds;        // Compound: IDs das formas
        } compound;
    };
    
    CollisionShape()
        : id(0)
        , type(ShapeType::NONE)
        , material()
        , localPosition()
        , localRotation()
        , localScale(1.0f, 1.0f, 1.0f)
        , boundingBox()
        , volume(0.0f)
        , isSensor(false)
        , isEnabled(true)
        , userData(nullptr)
        , bodyId(0)
        , collisionGroup(1)
        , collisionMask(0xFFFFFFFF)
        , name("")
    {
        // Inicializa a união com zeros
        memset(&box, 0, sizeof(box));
    }
    
    // Obtém a transformação local
    /**
 * GetLocalTransform
 * 
 * Implementa a funcionalidade GetLocalTransform conforme especificação original.
 * @return Retorna Transform
 */

    Transform GetLocalTransform() const{
        /**
 * Transform
 * 
 * Implementa a funcionalidade Transform conforme especificação original.
 * @param localPosition Parâmetro localPosition
 * @param localRotation Parâmetro localRotation
 * @param localScale Parâmetro localScale
 * @return Retorna return
 */

        return Transform(localPosition, localRotation, localScale);
    }
    
    // Atualiza o bounding box baseado no tipo e parâmetros
    /**
 * UpdateBoundingBox
 * 
 * Implementa a funcionalidade UpdateBoundingBox conforme especificação original.
 */

    void UpdateBoundingBox(){
        switch (type) {
            case ShapeType::BOX: {
                Vector3 extents = box.halfExtents * localScale;
                boundingBox = AABB(localPosition - extents, localPosition + extents);
                break;
            }
            case ShapeType::SPHERE: {
                float scaledRadius = sphere.radius * std::max(std::max(localScale.x, localScale.y), localScale.z);
                /**
 * extents
 * 
 * Implementa a funcionalidade extents conforme especificação original.
 * @param scaledRadius Parâmetro scaledRadius
 * @param scaledRadius Parâmetro scaledRadius
 * @param scaledRadius Parâmetro scaledRadius
 * @return Retorna Vector3
 */

                Vector3 extents(scaledRadius, scaledRadius, scaledRadius);
                boundingBox = AABB(localPosition - extents, localPosition + extents);
                break;
            }
            case ShapeType::CAPSULE: {
                float scaledRadius = capsule.radius * std::max(localScale.x, localScale.z);
                float scaledHeight = capsule.height * localScale.y;
                /**
 * extents
 * 
 * Implementa a funcionalidade extents conforme especificação original.
 * @param scaledRadius Parâmetro scaledRadius
 * @param scaledRadius Parâmetro scaledRadius
 * @param scaledRadius Parâmetro scaledRadius
 * @return Retorna Vector3
 */

                Vector3 extents(scaledRadius, scaledHeight * 0.5f + scaledRadius, scaledRadius);
                boundingBox = AABB(localPosition - extents, localPosition + extents);
                break;
            }
            // Implementações para outros tipos de forma...
            default:
                // Forma desconhecida ou não implementada, usa um bounding box padrão
                boundingBox = AABB(localPosition - Vector3(1.0f, 1.0f, 1.0f), localPosition + Vector3(1.0f, 1.0f, 1.0f));
                break;
        }
    }
    
    // Calcula o volume baseado no tipo e parâmetros
    /**
 * CalculateVolume
 * 
 * Implementa a funcionalidade CalculateVolume conforme especificação original.
 */

    void CalculateVolume(){
        switch (type) {
            case ShapeType::BOX: {
                Vector3 scaledExtents = box.halfExtents * localScale;
                volume = 8.0f * scaledExtents.x * scaledExtents.y * scaledExtents.z;
                break;
            }
            case ShapeType::SPHERE: {
                float scaledRadius = sphere.radius * std::max(std::max(localScale.x, localScale.y), localScale.z);
                volume = (4.0f / 3.0f) * 3.14159265f * scaledRadius * scaledRadius * scaledRadius;
                break;
            }
            case ShapeType::CAPSULE: {
                float scaledRadius = capsule.radius * std::max(localScale.x, localScale.z);
                float scaledHeight = capsule.height * localScale.y;
                float sphereVolume = (4.0f / 3.0f) * 3.14159265f * scaledRadius * scaledRadius * scaledRadius;
                float cylinderVolume = 3.14159265f * scaledRadius * scaledRadius * scaledHeight;
                volume = sphereVolume + cylinderVolume;
                break;
            }
            // Implementações para outros tipos de forma...
            default:
                volume = 0.0f;
                break;
        }
    }
    
    // Verifica se há colisão com outro shape
    /**
 * Collides
 * 
 * Implementa a funcionalidade Collides conforme especificação original.
 * @param other Parâmetro other
 * @param transformA Parâmetro transformA
 * @param transformB Parâmetro transformB
 * @param result Parâmetro result
 * @return Retorna bool
 */

    bool Collides(const CollisionShape& other, const Transform& transformA, const Transform& transformB, CollisionResult& result) const{
        // Implementação de verificação de colisão entre este shape e outro
        // Esta é uma função complexa que depende dos tipos dos shapes
        
        // Primeiro, verifica bounding boxes
        AABB aabbA = boundingBox;
        AABB aabbB = other.boundingBox;
        
        // Transforma os AABBs para o espaço global
        Matrix4 matA = transformA.ToMatrix();
        Matrix4 matB = transformB.ToMatrix();
        
        aabbA = aabbA.Transformed(matA);
        aabbB = aabbB.Transformed(matB);
        
        if (!aabbA.Intersects(aabbB)) {
            return false;
        }
        
        // Verifica colisão específica por tipo
        // Aqui, implementaríamos os algoritmos específicos para cada combinação de tipos de formas
        // Como exemplo, vamos implementar esfera vs /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param ShapeType::SPHERE Parâmetro ShapeType::SPHERE
 * @return Retorna esfera
 */
 esfera
        
        if(type == ShapeType::SPHERE && other.type == ShapeType::SPHERE){
            Vector3 posA = transformA.position + transformA.rotation.Rotate(localPosition);
            Vector3 posB = transformB.position + transformB.rotation.Rotate(other.localPosition);
            
            float scaledRadiusA = sphere.radius * std::max(std::max(transformA.scale.x * localScale.x, 
                                                                  transformA.scale.y * localScale.y), 
                                                         transformA.scale.z * localScale.z);
            
            float scaledRadiusB = other.sphere.radius * std::max(std::max(transformB.scale.x * other.localScale.x, 
                                                                        transformB.scale.y * other.localScale.y), 
                                                               transformB.scale.z * other.localScale.z);
            
            Vector3 ab = posB - posA;
            float distance = ab.Length();
            float radiiSum = scaledRadiusA + scaledRadiusB;
            
            if (distance < radiiSum) {
                result.hasCollision = true;
                result.penetration = radiiSum - distance;
                
                if (distance > 0.0001f) {
                    result.normal = ab * (1.0f / distance);
                } else {
                    result.normal = Vector3(0.0f, 1.0f, 0.0f); // Usa um vetor para cima se os centros estão muito próximos
                }
                
                result.point = posA + result.normal * (scaledRadiusA - result.penetration * 0.5f);
                result.friction = material.friction * other.material.friction;
                result.restitution = std::max(material.restitution, other.material.restitution);
                result.shapeIdA = id;
                result.shapeIdB = other.id;
                result.bodyIdA = bodyId;
                result.bodyIdB = other.bodyId;
                
                return true;
            }
            
            return false;
        }
        
        // Implementações para outras combinações de tipos...
        
        return false;
    }
    
    // Verifica se um ray intersecta este shape
    /**
 * Raycast
 * 
 * Implementa a funcionalidade Raycast conforme especificação original.
 * @param ray Parâmetro ray
 * @param transform Parâmetro transform
 * @param result Parâmetro result
 * @return Retorna bool
 */

    bool Raycast(const Ray& ray, const Transform& transform, RaycastResult& result) const{
        // Implementação de ray cast para este shape
        // Esta é uma função complexa que depende do tipo do shape
        
        // Transforma o raio para o espaço local do shape
        Matrix4 worldToLocal = Matrix4::FromTransform(transform.position, transform.rotation, transform.scale).Inverse();
        
        Vector3 localOrigin = worldToLocal * ray.origin;
        Vector3 localDirection = (worldToLocal * (ray.origin + ray.direction)) - localOrigin;
        float localDirLength = localDirection.Length();
        localDirection = localDirection * (1.0f / localDirLength);
        
        /**
 * localRay
 * 
 * Implementa a funcionalidade localRay conforme especificação original.
 * @param localOrigin Parâmetro localOrigin
 * @param localDirection Parâmetro localDirection
 * @param localDirLength Parâmetro localDirLength
 * @return Retorna Ray
 */

        
        Ray localRay(localOrigin, localDirection, ray.maxDistance * localDirLength);
        
        bool hit = false;
        float distance = std::numeric_limits<float>::max();
        Vector3 hitPoint;
        Vector3 hitNormal;
        
        switch (type) {
            case ShapeType::SPHERE: {
                // Ray vs Sphere no espaço local
                hit = localRay.Intersects(Vector3(0, 0, 0), sphere.radius, distance);
                
                if (hit) {
                    hitPoint = localRay.GetPoint(distance);
                    hitNormal = hitPoint.Normalized();
                }
                break;
            }
            case ShapeType::BOX: {
                // Ray vs Box no espaço local
                AABB localBox(Vector3(-box.halfExtents.x, -box.halfExtents.y, -box.halfExtents.z),
                             Vector3(box.halfExtents.x, box.halfExtents.y, box.halfExtents.z));
                
                hit = localRay.Intersects(localBox, distance);
                
                if (hit) {
                    hitPoint = localRay.GetPoint(distance);
                    
                    // Determina a normal baseada em qual face foi atingida
                    float epsilon = 0.0001f;
                    if (std::abs(hitPoint.x - box.halfExtents.x) < epsilon) hitNormal = Vector3(1, 0, 0);
                    else if (std::abs(hitPoint.x + box.halfExtents.x) < epsilon) hitNormal = Vector3(-1, 0, 0);
                    else if (std::abs(hitPoint.y - box.halfExtents.y) < epsilon) hitNormal = Vector3(0, 1, 0);
                    else if (std::abs(hitPoint.y + box.halfExtents.y) < epsilon) hitNormal = Vector3(0, -1, 0);
                    else if (std::abs(hitPoint.z - box.halfExtents.z) < epsilon) hitNormal = Vector3(0, 0, 1);
                    else hitNormal = Vector3(0, 0, -1);
                }
                break;
            }
            // Implementações para outros tipos de forma...
            default:
                hit = false;
                break;
        }
        
        if (hit) {
            // Transforma o resultado de volta para o espaço global
            Matrix4 localToWorld = Matrix4::FromTransform(transform.position, transform.rotation, transform.scale);
            
            result.hasHit = true;
            result.distance = distance / localDirLength;
            result.point = localToWorld * hitPoint;
            result.normal = (localToWorld.GetRotation() * hitNormal).Normalized();
            result.shapeId = id;
            result.bodyId = bodyId;
            
            return true;
        }
        
        return false;
    }
};

/**
 * @brief Restrição
 */
struct Constraint {
    uint32_t id;                       // ID
    ConstraintType type;               // Tipo
    uint32_t bodyIdA;                  // ID do corpo A
    uint32_t bodyIdB;                  // ID do corpo B
    Vector3 pivotA;                    // Pivô em A
    Vector3 pivotB;                    // Pivô em B
    Vector3 axisA;                     // Eixo em A
    Vector3 axisB;                     // Eixo em B
    bool enabled;                      // Habilitado
    float breakingThreshold;           // Limite de quebra
    float impulseClamp;                // Clamp de impulso
    float damping;                     // Amortecimento
    float stiffness;                   // Rigidez
    float limitLow;                    // Limite inferior
    float limitHigh;                   // Limite superior
    
    Constraint()
        : id(0)
        , type(ConstraintType::NONE)
        , bodyIdA(0)
        , bodyIdB(0)
        , pivotA()
        , pivotB()
        , axisA(0.0f, 1.0f, 0.0f)
        , axisB(0.0f, 1.0f, 0.0f)
        , enabled(true)
        , breakingThreshold(std::numeric_limits<float>::max())
        , impulseClamp(0.0f)
        , damping(0.3f)
        , stiffness(0.7f)
        , limitLow(0.0f)
        , limitHigh(0.0f)
    {}
};

/**
 * @brief Corpo rígido
 */
struct RigidBody {
    uint32_t id;                       // ID
    BodyType type;                     // Tipo
    Transform transform;               // Transformação
    Vector3 linearVelocity;            // Velocidade linear
    Vector3 angularVelocity;           // Velocidade angular
    Vector3 forces;                    // Forças
    Vector3 torque;                    // Torque
    float mass;                        // Massa
    float inverseMass;                 // Massa inversa
    Matrix3 inertiaTensor;             // Tensor de inércia
    Matrix3 inverseInertiaTensor;      // Tensor de inércia inverso
    float linearDamping;               // Amortecimento linear
    float angularDamping;              // Amortecimento angular
    bool isStatic;                     // É estático
    bool isKinematic;                  // É cinemático
    bool isSleeping;                   // Está dormindo
    bool isEnabled;                    // Está habilitado
    bool useGravity;                   // Usa gravidade
    bool freezeRotation;               // Congela rotação
    uint32_t collisionGroup;           // Grupo de colisão
    uint32_t collisionMask;            // Máscara de colisão
    std::vector<uint32_t> shapeIds;    // IDs das formas
    AABB boundingBox;                  // Bounding box
    void* userData;                    // Dados do usuário
    std::string name;                  // Nome
    
    RigidBody()
        : id(0)
        , type(BodyType::DYNAMIC)
        , transform()
        , linearVelocity()
        , angularVelocity()
        , forces()
        , torque()
        , mass(1.0f)
        , inverseMass(1.0f)
        , inertiaTensor()
        , inverseInertiaTensor()
        , linearDamping(0.01f)
        , angularDamping(0.01f)
        , isStatic(false)
        , isKinematic(false)
        , isSleeping(false)
        , isEnabled(true)
        , useGravity(true)
        , freezeRotation(false)
        , collisionGroup(1)
        , collisionMask(0xFFFFFFFF)
        , shapeIds()
        , boundingBox()
        , userData(nullptr)
        , name("")
    {}
    
    // Aplica uma força
    /**
 * ApplyForce
 * 
 * Implementa a funcionalidade ApplyForce conforme especificação original.
 * @param force Parâmetro force
 */

    void ApplyForce(const Vector3& force){
        forces = forces + force;
    }
    
    // Aplica uma força em um ponto
    /**
 * ApplyForceAtPoint
 * 
 * Implementa a funcionalidade ApplyForceAtPoint conforme especificação original.
 * @param force Parâmetro force
 * @param point Parâmetro point
 */

    void ApplyForceAtPoint(const Vector3& force, const Vector3& point){
        forces = forces + force;
        
        // Calcula o torque
        Vector3 relativePos = point - transform.position;
        Vector3 resultingTorque = relativePos.Cross(force);
        torque = torque + resultingTorque;
    }
    
    // Aplica um impulso
    /**
 * ApplyImpulse
 * 
 * Implementa a funcionalidade ApplyImpulse conforme especificação original.
 * @param impulse Parâmetro impulse
 */

    void ApplyImpulse(const Vector3& impulse){
        linearVelocity = linearVelocity + impulse * inverseMass;
    }
    
    // Aplica um impulso em um ponto
    /**
 * ApplyImpulseAtPoint
 * 
 * Implementa a funcionalidade ApplyImpulseAtPoint conforme especificação original.
 * @param impulse Parâmetro impulse
 * @param point Parâmetro point
 */

    void ApplyImpulseAtPoint(const Vector3& impulse, const Vector3& point){
        linearVelocity = linearVelocity + impulse * inverseMass;
        
        // Calcula o impulso angular
        Vector3 relativePos = point - transform.position;
        Vector3 angularImpulse = relativePos.Cross(impulse);
        angularVelocity = angularVelocity + inverseInertiaTensor * angularImpulse;
    }
    
    // Calcula a massa e o tensor de inércia
    /**
 * CalculateMassProperties
 * 
 * Implementa a funcionalidade CalculateMassProperties conforme especificação original.
 * @param shapes Parâmetro shapes
 */

    void CalculateMassProperties(const std::vector<CollisionShape*>& shapes){
        if (isStatic || isKinematic) {
            mass = 0.0f;
            inverseMass = 0.0f;
            inertiaTensor = Matrix3();
            inverseInertiaTensor = Matrix3();
            return;
        }
        
        if (shapes.empty()) {
            return;
        }
        
        // Calcula a massa e o centro de massa
        mass = 0.0f;
        /**
 * centerOfMass
 * 
 * Implementa a funcionalidade centerOfMass conforme especificação original.
 * @param 0.0f Parâmetro 0.0f
 * @param 0.0f Parâmetro 0.0f
 * @param 0.0f Parâmetro 0.0f
 * @return Retorna Vector3
 */

        Vector3 centerOfMass(0.0f, 0.0f, 0.0f);
        float totalVolume = 0.0f;
        
        for (const CollisionShape* shape : shapes) {
            float shapeMass = shape->volume * shape->material.density;
            mass += shapeMass;
            centerOfMass = centerOfMass + shape->localPosition * shapeMass;
            totalVolume += shape->volume;
        }
        
        if (mass > 0.0f) {
            inverseMass = 1.0f / mass;
            centerOfMass = centerOfMass * inverseMass;
        } else {
            mass = 1.0f;
            inverseMass = 1.0f;
        }
        
        // Calcula o tensor de inércia
        Matrix3 tensor;
        
        for (const CollisionShape* shape : shapes) {
            // Calcula o tensor baseado no tipo do shape
            Matrix3 shapeTensor;
            
            switch (shape->type) {
                case ShapeType::BOX: {
                    Vector3 extents = shape->box.halfExtents * 2.0f;
                    float shapeMass = shape->volume * shape->material.density;
                    float xx = (1.0f / 12.0f) * shapeMass * (extents.y * extents.y + extents.z * extents.z);
                    float yy = (1.0f / 12.0f) * shapeMass * (extents.x * extents.x + extents.z * extents.z);
                    float zz = (1.0f / 12.0f) * shapeMass * (extents.x * extents.x + extents.y * extents.y);
                    
                    shapeTensor = Matrix3(
                        xx, 0.0f, 0.0f,
                        0.0f, yy, 0.0f,
                        0.0f, 0.0f, zz
                    );
                    break;
                }
                case ShapeType::SPHERE: {
                    float shapeMass = shape->volume * shape->material.density;
                    float inertia = (2.0f / 5.0f) * shapeMass * shape->sphere.radius * shape->sphere.radius;
                    
                    shapeTensor = Matrix3(
                        inertia, 0.0f, 0.0f,
                        0.0f, inertia, 0.0f,
                        0.0f, 0.0f, inertia
                    );
                    break;
                }
                // Implementações para outros tipos de forma...
                default:
                    // Usa um tensor padrão para formas não implementadas
                    float shapeMass = shape->volume * shape->material.density;
                    float inertia = shapeMass;
                    
                    shapeTensor = Matrix3(
                        inertia, 0.0f, 0.0f,
                        0.0f, inertia, 0.0f,
                        0.0f, 0.0f, inertia
                    );
                    break;
            }
            
            // Usa o teorema dos eixos paralelos para ajustar o tensor ao centro de massa
            Vector3 r = shape->localPosition - centerOfMass;
            float r2 = r.LengthSquared();
            float shapeMass = shape->volume * shape->material.density;
            
            /**
 * parallelAxisTerm
 * 
 * Implementa a funcionalidade parallelAxisTerm conforme especificação original.
 * @param r.x Parâmetro r.x
 * @param r.y Parâmetro r.y
 * @param r.z Parâmetro r.z
 * @param r.x Parâmetro r.x
 * @param r.y Parâmetro r.y
 * @param r.z Parâmetro r.z
 * @param r.x Parâmetro r.x
 * @param r.y Parâmetro r.y
 * @param r.z Parâmetro r.z
 * @return Retorna Matrix3
 */

            
            Matrix3 parallelAxisTerm(
                r2 - r.x * r.x, -r.x * r.y, -r.x * r.z,
                -r.y * r.x, r2 - r.y * r.y, -r.y * r.z,
                -r.z * r.x, -r.z * r.y, r2 - r.z * r.z
            );
            
            shapeTensor = shapeTensor + parallelAxisTerm * shapeMass;
            
            // Rotaciona o tensor para o espaço global
            Matrix3 rotation = Matrix3::FromQuaternion(shape->localRotation);
            shapeTensor = rotation * shapeTensor * rotation.Transpose();
            
            tensor = tensor + shapeTensor;
        }
        
        inertiaTensor = tensor;
        
        // Calcula a inversa do tensor
        inverseInertiaTensor = inertiaTensor.Inverse();
    }
    
    // Atualiza o bounding box
    /**
 * UpdateBoundingBox
 * 
 * Implementa a funcionalidade UpdateBoundingBox conforme especificação original.
 * @param shapes Parâmetro shapes
 */

    void UpdateBoundingBox(const std::vector<CollisionShape*>& shapes){
        if (shapes.empty()) {
            boundingBox = AABB(transform.position - Vector3(0.5f, 0.5f, 0.5f), transform.position + Vector3(0.5f, 0.5f, 0.5f));
            return;
        }
        
        Matrix4 worldTransform = transform.ToMatrix();
        AABB result = shapes[0]->boundingBox.Transformed(worldTransform);
        
        for (size_t i = 1; i < shapes.size(); ++i) {
            AABB shapeBox = shapes[i]->boundingBox.Transformed(worldTransform);
            result = result.Union(shapeBox);
        }
        
        boundingBox = result;
    }
    
    // Atualiza a transformação
    /**
 * UpdateTransform
 * 
 * Implementa a funcionalidade UpdateTransform conforme especificação original.
 * @param deltaTime Parâmetro deltaTime
 */

    void UpdateTransform(float deltaTime){
        if (isStatic || isKinematic || !isEnabled) {
            return;
        }
        
        // Atualiza a posição com a velocidade linear
        transform.position = transform.position + linearVelocity * deltaTime;
        
        // Atualiza a rotação com a velocidade /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param !freezeRotation Parâmetro !freezeRotation
 * @return Retorna angular
 */
 angular
        if(!freezeRotation){
            // Converte a velocidade angular para um quaternion
            float angularSpeed = angularVelocity.Length();
            if (angularSpeed > 0.0001f) {
                Vector3 axis = angularVelocity * (1.0f / angularSpeed);
                float angle = angularSpeed * deltaTime;
                /**
 * deltaRotation
 * 
 * Implementa a funcionalidade deltaRotation conforme especificação original.
 * @param axis Parâmetro axis
 * @param angle Parâmetro angle
 * @return Retorna Quaternion
 */

                Quaternion deltaRotation(axis, angle);
                transform.rotation = (deltaRotation * transform.rotation).Normalized();
            }
        }
        
        // Reseta forças e torques
        forces = Vector3(0.0f, 0.0f, 0.0f);
        torque = Vector3(0.0f, 0.0f, 0.0f);
    }
    
    // Integra a velocidade
    /**
 * IntegrateVelocity
 * 
 * Implementa a funcionalidade IntegrateVelocity conforme especificação original.
 * @param deltaTime Parâmetro deltaTime
 * @param gravity Parâmetro gravity
 */

    void IntegrateVelocity(float deltaTime, const Vector3& gravity){
        if (isStatic || isKinematic || !isEnabled) {
            return;
        }
        
        // Atualiza a velocidade linear com as forças
        Vector3 acceleration = forces * inverseMass;
        if (useGravity) {
            acceleration = acceleration + gravity;
        }
        
        linearVelocity = linearVelocity + acceleration * deltaTime;
        
        // Aplica amortecimento linear
        linearVelocity = linearVelocity * (1.0f - linearDamping * deltaTime);
        
        // Atualiza a velocidade angular com o /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param !freezeRotation Parâmetro !freezeRotation
 * @return Retorna torque
 */
 torque
        if(!freezeRotation){
            Vector3 angularAcceleration = inverseInertiaTensor * torque;
            angularVelocity = angularVelocity + angularAcceleration * deltaTime;
            
            // Aplica amortecimento angular
            angularVelocity = angularVelocity * (1.0f - angularDamping * deltaTime);
        }
    }
};

/**
 * @brief Personagem de física
 */
struct PhysicsCharacter {
    uint32_t id;                       // ID
    RigidBody* body;                   // Corpo
    float height;                      // Altura
    float radius;                      // Raio
    float stepHeight;                  // Altura do passo
    float slopeLimit;                  // Limite de inclinação
    float skinWidth;                   // Largura da pele
    float minMoveDistance;             // Distância mínima de movimento
    bool isGrounded;                   // Está no chão
    Vector3 groundNormal;              // Normal do chão
    uint32_t groundBodyId;             // ID do corpo no chão
    TerrainType terrainType;           // Tipo de terreno
    
    PhysicsCharacter()
        : id(0)
        , body(nullptr)
        , height(2.0f)
        , radius(0.5f)
        , stepHeight(0.3f)
        , slopeLimit(45.0f)
        , skinWidth(0.08f)
        , minMoveDistance(0.001f)
        , isGrounded(false)
        , groundNormal(0.0f, 1.0f, 0.0f)
        , groundBodyId(0)
        , terrainType(TerrainType::FLAT)
    {}
};

/**
 * @brief Evento de colisão
 */
struct CollisionEvent {
    CollisionEventType type;           // Tipo
    uint32_t bodyIdA;                  // ID do corpo A
    uint32_t bodyIdB;                  // ID do corpo B
    Vector3 point;                     // Ponto
    Vector3 normal;                    // Normal
    float impulse;                     // Impulso
    
    CollisionEvent()
        : type(CollisionEventType::NONE)
        , bodyIdA(0)
        , bodyIdB(0)
        , point()
        , normal()
        , impulse(0.0f)
    {}
};

/**
 * @brief Gatilho
 */
struct Trigger {
    uint32_t id;                       // ID
    CollisionShape* shape;             // Forma
    bool isEnabled;                    // Está habilitado
    std::string name;                  // Nome
    void* userData;                    // Dados do usuário
    
    Trigger()
        : id(0)
        , shape(nullptr)
        , isEnabled(true)
        , name("")
        , userData(nullptr)
    {}
};

/**
 * @brief Estatísticas do motor de física
 */
struct PhysicsStats {
    uint32_t numBodies;                // Número de corpos
    uint32_t numDynamicBodies;         // Número de corpos dinâmicos
    uint32_t numStaticBodies;          // Número de corpos estáticos
    uint32_t numKinematicBodies;       // Número de corpos cinemáticos
    uint32_t numShapes;                // Número de formas
    uint32_t numConstraints;           // Número de restrições
    uint32_t numContacts;              // Número de contatos
    uint32_t numTriggers;              // Número de gatilhos
    uint32_t numCharacters;            // Número de personagens
    uint32_t numCollisionTests;        // Número de testes de colisão
    uint32_t numCollisions;            // Número de colisões
    uint32_t numRaycasts;              // Número de raycasts
    float simulationTime;              // Tempo de simulação
    float broadPhaseTime;              // Tempo de fase ampla
    float narrowPhaseTime;             // Tempo de fase estreita
    float solverTime;                  // Tempo do solver
    float integrationTime;             // Tempo de integração
    
    PhysicsStats()
        : numBodies(0)
        , numDynamicBodies(0)
        , numStaticBodies(0)
        , numKinematicBodies(0)
        , numShapes(0)
        , numConstraints(0)
        , numContacts(0)
        , numTriggers(0)
        , numCharacters(0)
        , numCollisionTests(0)
        , numCollisions(0)
        , numRaycasts(0)
        , simulationTime(0.0f)
        , broadPhaseTime(0.0f)
        , narrowPhaseTime(0.0f)
        , solverTime(0.0f)
        , integrationTime(0.0f)
    {}
};

/**
 * @brief Configuração do motor de física
 */
struct PhysicsConfig {
    Vector3 gravity;                   // Gravidade
    float fixedTimeStep;               // Passo de tempo fixo
    uint32_t maxSubSteps;              // Máximo de subpassos
    uint32_t solverIterations;         // Iterações do solver
    float broadphaseCellSize;          // Tamanho da célula de fase ampla
    float defaultFriction;             // Fricção padrão
    float defaultRestitution;          // Restituição padrão
    float sleepThreshold;              // Limite de sono
    float linearSleepThreshold;        // Limite de sono linear
    float angularSleepThreshold;       // Limite de sono angular
    bool enableSleeping;               // Habilitar sono
    bool enableCCD;                    // Habilitar CCD
    bool enableDebugDraw;              // Habilitar desenho de depuração
    bool enableTriangleMeshCompression; // Habilitar compressão de malha de triângulos
    bool enableHeightFieldCompression; // Habilitar compressão de campo de altura
    float contactBreakingThreshold;    // Limite de quebra de contato
    float contactOffset;               // Offset de contato
    float restOffset;                  // Offset de repouso
    bool enableAdaptiveForce;          // Habilitar força adaptativa
    bool enableFrictionCombine;        // Habilitar combinação de fricção
    uint32_t maxContactPoints;         // Máximo de pontos de contato
    
    PhysicsConfig()
        : gravity(0.0f, -9.8f, 0.0f)
        , fixedTimeStep(1.0f / 60.0f)
        , maxSubSteps(10)
        , solverIterations(10)
        , broadphaseCellSize(BROAD_PHASE_CELL_SIZE)
        , defaultFriction(DEFAULT_FRICTION)
        , defaultRestitution(DEFAULT_RESTITUTION)
        , sleepThreshold(0.005f)
        , linearSleepThreshold(0.01f)
        , angularSleepThreshold(0.01f)
        , enableSleeping(true)
        , enableCCD(true)
        , enableDebugDraw(false)
        , enableTriangleMeshCompression(true)
        , enableHeightFieldCompression(true)
        , contactBreakingThreshold(0.02f)
        , contactOffset(0.01f)
        , restOffset(0.0f)
        , enableAdaptiveForce(true)
        , enableFrictionCombine(true)
        , maxContactPoints(MAX_COLLISION_PAIRS)
    {}
};

/**
 * @brief Tipo de callback de colisão
 */
using CollisionCallback = std::function<void(const CollisionEvent& event)>;

/**
 * @brief Tipo de callback de trigger
 */
using TriggerCallback = std::function<void(const Trigger& trigger, const RigidBody& body, bool isEnter)>;

/**
 * @brief Motor de física
 */
/**
 * Classe WYDPhysicsEngine
 * 
 * Esta classe implementa a funcionalidade WYDPhysicsEngine conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
/**
 * Classe WYDPhysicsEngine
 * 
 * Esta classe implementa a funcionalidade WYDPhysicsEngine conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class PhysicsEngine {
public:
    /**
     * @brief Obtém a instância singleton
     * @return Referência para a instância
     */
    static PhysicsEngine& GetInstance();
    
    /**
     * @brief Inicializa o motor
     * @param config Configuração
     * @return true se inicializado com sucesso
     */
    bool Initialize(const PhysicsConfig& config = PhysicsConfig());
    
    /**
     * @brief Finaliza o motor
     */
    void Shutdown();
    
    /**
     * @brief Atualiza o motor
     * @param deltaTime Tempo delta
     * @param fixedTimeStep Passo de tempo fixo (opcional)
     */
    void Update(float deltaTime, float fixedTimeStep = 0.0f);
    
    /**
     * @brief Cria um corpo rígido
     * @param type Tipo
     * @param position Posição
     * @param rotation Rotação
     * @return ID do corpo, ou 0 se falhar
     */
    uint32_t CreateRigidBody(BodyType type, const Vector3& position, const Quaternion& rotation = Quaternion());
    
    /**
     * @brief Destrói um corpo rígido
     * @param bodyId ID do corpo
     * @return true se destruído com sucesso
     */
    bool DestroyRigidBody(uint32_t bodyId);
    
    /**
     * @brief Cria uma forma de colisão
     * @param bodyId ID do corpo
     * @param type Tipo
     * @return ID da forma, ou 0 se falhar
     */
    uint32_t CreateCollisionShape(uint32_t bodyId, ShapeType type);
    
    /**
     * @brief Destrói uma forma de colisão
     * @param shapeId ID da forma
     * @return true se destruído com sucesso
     */
    bool DestroyCollisionShape(uint32_t shapeId);
    
    /**
     * @brief Cria um gatilho
     * @param shape Forma
     * @return ID do gatilho, ou 0 se falhar
     */
    uint32_t CreateTrigger(CollisionShape* shape);
    
    /**
     * @brief Destrói um gatilho
     * @param triggerId ID do gatilho
     * @return true se destruído com sucesso
     */
    bool DestroyTrigger(uint32_t triggerId);
    
    /**
     * @brief Cria uma restrição
     * @param type Tipo
     * @param bodyIdA ID do corpo A
     * @param bodyIdB ID do corpo B
     * @return ID da restrição, ou 0 se falhar
     */
    uint32_t CreateConstraint(ConstraintType type, uint32_t bodyIdA, uint32_t bodyIdB);
    
    /**
     * @brief Destrói uma restrição
     * @param constraintId ID da restrição
     * @return true se destruído com sucesso
     */
    bool DestroyConstraint(uint32_t constraintId);
    
    /**
     * @brief Cria um personagem
     * @param height Altura
     * @param radius Raio
     * @param position Posição
     * @return ID do personagem, ou 0 se falhar
     */
    uint32_t CreateCharacter(float height, float radius, const Vector3& position);
    
    /**
     * @brief Destrói um personagem
     * @param characterId ID do personagem
     * @return true se destruído com sucesso
     */
    bool DestroyCharacter(uint32_t characterId);
    
    /**
     * @brief Obtém um corpo rígido
     * @param bodyId ID do corpo
     * @return Ponteiro para o corpo, ou nullptr se não encontrado
     */
    RigidBody* GetRigidBody(uint32_t bodyId);
    
    /**
     * @brief Obtém uma forma de colisão
     * @param shapeId ID da forma
     * @return Ponteiro para a forma, ou nullptr se não encontrada
     */
    CollisionShape* GetCollisionShape(uint32_t shapeId);
    
    /**
     * @brief Obtém um gatilho
     * @param triggerId ID do gatilho
     * @return Ponteiro para o gatilho, ou nullptr se não encontrado
     */
    Trigger* GetTrigger(uint32_t triggerId);
    
    /**
     * @brief Obtém uma restrição
     * @param constraintId ID da restrição
     * @return Ponteiro para a restrição, ou nullptr se não encontrada
     */
    Constraint* GetConstraint(uint32_t constraintId);
    
    /**
     * @brief Obtém um personagem
     * @param characterId ID do personagem
     * @return Ponteiro para o personagem, ou nullptr se não encontrado
     */
    PhysicsCharacter* GetCharacter(uint32_t characterId);
    
    /**
     * @brief Move um personagem
     * @param characterId ID do personagem
     * @param direction Direção
     * @param deltaTime Tempo delta
     * @return true se movido com sucesso
     */
    bool MoveCharacter(uint32_t characterId, const Vector3& direction, float deltaTime);
    
    /**
     * @brief Faz um ray cast
     * @param ray Raio
     * @param result Resultado
     * @param collisionMask Máscara de colisão
     * @return true se o raio atingiu algo
     */
    bool Raycast(const Ray& ray, RaycastResult& result, uint32_t collisionMask = 0xFFFFFFFF);
    
    /**
     * @brief Faz um ray cast contra todos os objetos
     * @param ray Raio
     * @param results Resultados
     * @param collisionMask Máscara de colisão
     * @param maxResults Máximo de resultados
     * @return Número de hits
     */
    uint32_t RaycastAll(const Ray& ray, std::vector<RaycastResult>& results, uint32_t collisionMask = 0xFFFFFFFF, uint32_t maxResults = 16);
    
    /**
     * @brief Verifica colisão entre dois corpos
     * @param bodyIdA ID do corpo A
     * @param bodyIdB ID do corpo B
     * @param result Resultado
     * @return true se houver colisão
     */
    bool TestCollision(uint32_t bodyIdA, uint32_t bodyIdB, CollisionResult& result);
    
    /**
     * @brief Verifica quais corpos estão em uma caixa
     * @param aabb AABB
     * @param bodies Corpos (saída)
     * @param collisionMask Máscara de colisão
     * @return Número de corpos
     */
    uint32_t OverlapBox(const AABB& aabb, std::vector<uint32_t>& bodies, uint32_t collisionMask = 0xFFFFFFFF);
    
    /**
     * @brief Verifica quais corpos estão em uma esfera
     * @param center Centro
     * @param radius Raio
     * @param bodies Corpos (saída)
     * @param collisionMask Máscara de colisão
     * @return Número de corpos
     */
    uint32_t OverlapSphere(const Vector3& center, float radius, std::vector<uint32_t>& bodies, uint32_t collisionMask = 0xFFFFFFFF);
    
    /**
     * @brief Registra um callback de colisão
     * @param callback Callback
     * @return ID do callback, ou 0 se falhar
     */
    uint32_t RegisterCollisionCallback(CollisionCallback callback);
    
    /**
     * @brief Remove um callback de colisão
     * @param callbackId ID do callback
     * @return true se removido com sucesso
     */
    bool UnregisterCollisionCallback(uint32_t callbackId);
    
    /**
     * @brief Registra um callback de trigger
     * @param callback Callback
     * @return ID do callback, ou 0 se falhar
     */
    uint32_t RegisterTriggerCallback(TriggerCallback callback);
    
    /**
     * @brief Remove um callback de trigger
     * @param callbackId ID do callback
     * @return true se removido com sucesso
     */
    bool UnregisterTriggerCallback(uint32_t callbackId);
    
    /**
     * @brief Define a gravidade
     * @param gravity Gravidade
     */
    void SetGravity(const Vector3& gravity);
    
    /**
     * @brief Obtém a gravidade
     * @return Gravidade
     */
    Vector3 GetGravity() const;
    
    /**
     * @brief Define um corpo como cinemático
     * @param bodyId ID do corpo
     * @param isKinematic É cinemático
     * @return true se definido com sucesso
     */
    bool SetBodyKinematic(uint32_t bodyId, bool isKinematic);
    
    /**
     * @brief Define um corpo como estático
     * @param bodyId ID do corpo
     * @param isStatic É estático
     * @return true se definido com sucesso
     */
    bool SetBodyStatic(uint32_t bodyId, bool isStatic);
    
    /**
     * @brief Define a transformação de um corpo
     * @param bodyId ID do corpo
     * @param position Posição
     * @param rotation Rotação
     * @return true se definido com sucesso
     */
    bool SetBodyTransform(uint32_t bodyId, const Vector3& position, const Quaternion& rotation);
    
    /**
     * @brief Define a velocidade linear de um corpo
     * @param bodyId ID do corpo
     * @param velocity Velocidade
     * @return true se definido com sucesso
     */
    bool SetBodyLinearVelocity(uint32_t bodyId, const Vector3& velocity);
    
    /**
     * @brief Define a velocidade angular de um corpo
     * @param bodyId ID do corpo
     * @param velocity Velocidade
     * @return true se definido com sucesso
     */
    bool SetBodyAngularVelocity(uint32_t bodyId, const Vector3& velocity);
    
    /**
     * @brief Define a massa de um corpo
     * @param bodyId ID do corpo
     * @param mass Massa
     * @return true se definido com sucesso
     */
    bool SetBodyMass(uint32_t bodyId, float mass);
    
    /**
     * @brief Define o material de uma forma
     * @param shapeId ID da forma
     * @param material Material
     * @return true se definido com sucesso
     */
    bool SetShapeMaterial(uint32_t shapeId, const PhysicsMaterial& material);
    
    /**
     * @brief Define os parâmetros de uma caixa
     * @param shapeId ID da forma
     * @param halfExtents Metade das extensões
     * @return true se definido com sucesso
     */
    bool SetBoxParameters(uint32_t shapeId, const Vector3& halfExtents);
    
    /**
     * @brief Define os parâmetros de uma esfera
     * @param shapeId ID da forma
     * @param radius Raio
     * @return true se definido com sucesso
     */
    bool SetSphereParameters(uint32_t shapeId, float radius);
    
    /**
     * @brief Define os parâmetros de uma cápsula
     * @param shapeId ID da forma
     * @param radius Raio
     * @param height Altura
     * @return true se definido com sucesso
     */
    bool SetCapsuleParameters(uint32_t shapeId, float radius, float height);
    
    /**
     * @brief Define os parâmetros de um cilindro
     * @param shapeId ID da forma
     * @param radius Raio
     * @param height Altura
     * @return true se definido com sucesso
     */
    bool SetCylinderParameters(uint32_t shapeId, float radius, float height);
    
    /**
     * @brief Define os parâmetros de uma restrição de ponto
     * @param constraintId ID da restrição
     * @param pivotA Pivô em A
     * @param pivotB Pivô em B
     * @return true se definido com sucesso
     */
    bool SetPointConstraintParameters(uint32_t constraintId, const Vector3& pivotA, const Vector3& pivotB);
    
    /**
     * @brief Define os parâmetros de uma restrição de dobradiça
     * @param constraintId ID da restrição
     * @param pivotA Pivô em A
     * @param pivotB Pivô em B
     * @param axisA Eixo em A
     * @param axisB Eixo em B
     * @return true se definido com sucesso
     */
    bool SetHingeConstraintParameters(uint32_t constraintId, const Vector3& pivotA, const Vector3& pivotB, const Vector3& axisA, const Vector3& axisB);
    
    /**
     * @brief Obtém as estatísticas
     * @return Estatísticas
     */
    PhysicsStats GetStatistics() const;
    
    /**
     * @brief Limpa o mundo
     */
    void ClearWorld();
    
    /**
     * @brief Verifica se o motor está inicializado
     * @return true se inicializado
     */
    bool IsInitialized() const;
    
private:
    /**
     * @brief Construtor privado (singleton)
     */
    PhysicsEngine();
    
    /**
     * @brief Destrutor
     */
    ~PhysicsEngine();
    
    /**
     * @brief Desabilita o construtor de cópia
     */
    PhysicsEngine(const PhysicsEngine&) = delete;
    
    /**
     * @brief Desabilita o operador de atribuição
     */
    PhysicsEngine& operator=(const PhysicsEngine&) = delete;
    
    /**
     * @brief Passo de simulação fixo
     * @param timeStep Passo de tempo
     */
    void SimulationStep(float timeStep);
    
    /**
     * @brief Integra as velocidades dos corpos
     * @param timeStep Passo de tempo
     */
    void IntegrateVelocities(float timeStep);
    
    /**
     * @brief Integra as posições dos corpos
     * @param timeStep Passo de tempo
     */
    void IntegratePositions(float timeStep);
    
    /**
     * @brief Detecta colisões
     */
    void DetectCollisions();
    
    /**
     * @brief Fase ampla de detecção de colisão
     * @param pairs Pares de colisão (saída)
     */
    void BroadPhaseCollision(std::vector<std::pair<uint32_t, uint32_t>>& pairs);
    
    /**
     * @brief Fase estreita de detecção de colisão
     * @param pairs Pares de colisão
     * @param contacts Contatos (saída)
     */
    void NarrowPhaseCollision(const std::vector<std::pair<uint32_t, uint32_t>>& pairs, std::vector<CollisionResult>& contacts);
    
    /**
     * @brief Resolve colisões
     * @param contacts Contatos
     */
    void ResolveCollisions(std::vector<CollisionResult>& contacts);
    
    /**
     * @brief Resolve restrições
     * @param timeStep Passo de tempo
     */
    void SolveConstraints(float timeStep);
    
    /**
     * @brief Atualiza forças e torques
     */
    void UpdateForcesAndTorques();
    
    /**
     * @brief Atualiza bounding boxes
     */
    void UpdateBoundingBoxes();
    
    /**
     * @brief Verifica gatilhos
     */
    void CheckTriggers();
    
    /**
     * @brief Atualiza personagens
     * @param timeStep Passo de tempo
     */
    void UpdateCharacters(float timeStep);
    
    /**
     * @brief Notifica eventos de colisão
     * @param collisions Colisões
     */
    void NotifyCollisionEvents(const std::vector<CollisionResult>& collisions);
    
    /**
     * @brief Move corpo para resolver penetração
     * @param bodyId ID do corpo
     * @param normal Normal
     * @param penetration Penetração
     */
    void MoveBodyForPenetrationResolution(uint32_t bodyId, const Vector3& normal, float penetration);
    
    /**
     * @brief Resolve penetração
     * @param collision Colisão
     */
    void ResolvePenetration(const CollisionResult& collision);
    
    /**
     * @brief Aplica impulso para resolver colisão
     * @param collision Colisão
     */
    void ApplyCollisionImpulse(const CollisionResult& collision);
    
    /**
     * @brief Aplica fricção
     * @param collision Colisão
     */
    void ApplyFriction(const CollisionResult& collision);
    
    /**
     * @brief Detecta adormecimento
     */
    void DetectSleeping();
    
    /**
     * @brief Calcula colisão contínua
     * @param timeStep Passo de tempo
     */
    void ContinuousCollisionDetection(float timeStep);
    
    /**
     * @brief Atualiza as estatísticas
     */
    void UpdateStatistics();
    
    /**
     * @brief Notifica callbacks de trigger
     * @param trigger Gatilho
     * @param body Corpo
     * @param isEnter É entrada
     */
    void NotifyTriggerCallbacks(const Trigger& trigger, const RigidBody& body, bool isEnter);
    
    // Configuração
    PhysicsConfig config_;
    
    // Estado
    bool initialized_;
    float accumulator_;
    
    // Corpos
    std::unordered_map<uint32_t, RigidBody> bodies_;
    
    // Formas
    std::unordered_map<uint32_t, CollisionShape> shapes_;
    
    // Gatilhos
    std::unordered_map<uint32_t, Trigger> triggers_;
    
    // Restrições
    std::unordered_map<uint32_t, Constraint> constraints_;
    
    // Personagens
    std::unordered_map<uint32_t, PhysicsCharacter> characters_;
    
    // Mutexes
    std::mutex bodiesMutex_;
    std::mutex shapesMutex_;
    std::mutex triggersMutex_;
    std::mutex constraintsMutex_;
    std::mutex charactersMutex_;
    
    // Contadores de ID
    uint32_t nextBodyId_;
    uint32_t nextShapeId_;
    uint32_t nextTriggerId_;
    uint32_t nextConstraintId_;
    uint32_t nextCharacterId_;
    
    // Callbacks de colisão
    std::unordered_map<uint32_t, CollisionCallback> collisionCallbacks_;
    std::mutex collisionCallbacksMutex_;
    uint32_t nextCollisionCallbackId_;
    
    // Callbacks de trigger
    std::unordered_map<uint32_t, TriggerCallback> triggerCallbacks_;
    std::mutex triggerCallbacksMutex_;
    uint32_t nextTriggerCallbackId_;
    
    // Estatísticas
    PhysicsStats stats_;
    std::mutex statsMutex_;
    
    // Grade espacial para fase ampla
    struct SpatialGrid {
        struct Cell {
            std::vector<uint32_t> bodyIds;
        };
        
        std::unordered_map<uint64_t, Cell> cells;
        float cellSize;
        
        SpatialGrid() : cellSize(BROAD_PHASE_CELL_SIZE), cells() {}
        
        /**
 * Clear
 * 
 * Implementa a funcionalidade Clear conforme especificação original.
 */

        
        void Clear(){
            cells.clear();
        }
        
        /**
 * GetCellIndex
 * 
 * Implementa a funcionalidade GetCellIndex conforme especificação original.
 * @param x Parâmetro x
 * @param y Parâmetro y
 * @param z Parâmetro z
 * @return Retorna uint64_t
 */

        
        uint64_t GetCellIndex(int x, int y, int z) const{
            // Converte coordenadas 3D em um índice 1D
            return ((uint64_t)x & 0x1FFFFF) | 
                   (((uint64_t)y & 0x1FFFFF) << 21) | 
                   (((uint64_t)z & 0x1FFFFF) << 42);
        }
        
        /**
 * GetCellCoordinate
 * 
 * Implementa a funcionalidade GetCellCoordinate conforme especificação original.
 * @param position Parâmetro position
 * @param x Parâmetro x
 * @param y Parâmetro y
 * @param z Parâmetro z
 */

        
        void GetCellCoordinate(const Vector3& position, int& x, int& y, int& z) const{
            x = (int)std::floor(position.x / cellSize);
            y = (int)std::floor(position.y / cellSize);
            z = (int)std::floor(position.z / cellSize);
        }
        
        /**
 * InsertBody
 * 
 * Implementa a funcionalidade InsertBody conforme especificação original.
 * @param bodyId Parâmetro bodyId
 * @param aabb Parâmetro aabb
 */

        
        void InsertBody(uint32_t bodyId, const AABB& aabb){
            int minX, minY, minZ, maxX, maxY, maxZ;
            GetCellCoordinate(aabb.min, minX, minY, minZ);
            GetCellCoordinate(aabb.max, maxX, maxY, maxZ);
            
            for (int x = minX; x <= maxX; ++x) {
                for (int y = minY; y <= maxY; ++y) {
                    for (int z = minZ; z <= maxZ; ++z) {
                        uint64_t cellIndex = GetCellIndex(x, y, z);
                        cells[cellIndex].bodyIds.push_back(bodyId);
                    }
                }
            }
        }
        
        /**
 * QueryPotentialCollisions
 * 
 * Implementa a funcionalidade QueryPotentialCollisions conforme especificação original.
 * @param std::vector<std::pair<uint32_t Parâmetro std::vector<std::pair<uint32_t
 * @param pairs Parâmetro pairs
 */

        
        void QueryPotentialCollisions(std::vector<std::pair<uint32_t, uint32_t>>& pairs){
            std::set<std::pair<uint32_t, uint32_t>> uniquePairs;
            
            for (auto& cellPair : cells) {
                Cell& cell = cellPair.second;
                
                // Gera pares para todos os corpos na mesma cé/**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param cell.bodyIds.size( Parâmetro cell.bodyIds.size(
 * @return Retorna lula
 */
lula
                for(size_t i = 0; i < cell.bodyIds.size(); ++i) {
                    for (size_t j = i + 1; j < cell.bodyIds.size(); ++j) {
                        uint32_t bodyA = cell.bodyIds[i];
                        uint32_t bodyB = cell.bodyIds[j];
                        
                        // Garante que o par é ordenado pelo menor ID /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param bodyB Parâmetro bodyB
 * @return Retorna primeiro
 */
 primeiro
                        if(bodyA > bodyB){
                            std::swap(bodyA, bodyB);
                        }
                        
                        uniquePairs.insert(std::pair<uint32_t, uint32_t>(bodyA, bodyB));
                    }
                }
            }
            
            // Converte o conjunto em vetor
            pairs.assign(uniquePairs.begin(), uniquePairs.end());
        }
    };
    
    SpatialGrid spatialGrid_;
    
    // Estado do mundo e objetos de cache
    std::vector<CollisionResult> persistentContacts_;
    std::set<std::pair<uint32_t, uint32_t>> triggerPairs_;
    std::vector<CollisionEvent> collisionEvents_;
};

// Acesso global
#define g_PhysicsEngine PhysicsEngine::GetInstance()

} // namespace physics
} // namespace reverse
} // namespace wyd

#endif // PHYSICSENGINE_H

} // namespace wydbr
