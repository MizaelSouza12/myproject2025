/**
 * ResourceSystem.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/reverse/resources/ResourceSystem.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef RESOURCESYSTEM_H
#define RESOURCESYSTEM_H

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
#include <variant>
#include <filesystem>

namespace wydbr {


/**
 * @file ResourceSystem.h
 * @brief Sistema avançado de gerenciamento de recursos de jogo reconstruído do WYD original
 * 
 * Este arquivo contém a reconstrução do sistema de recursos do WYD,
 * incluindo carregamento, manipulação e conversão de formatos proprietários como
 * .bin, .wys, .wyt, .wyp e outros. Sistema desenvolvido a partir de engenharia
 * reversa completa dos arquivos originais, mantendo compatibilidade total.
 */

namespace wyd {
namespace reverse {
namespace resources {

// Constantes do sistema de recursos
constexpr uint32_t MAX_RESOURCE_PATH_LENGTH = 260;   // Tamanho máximo do caminho do recurso
constexpr uint32_t MAX_RESOURCE_NAME_LENGTH = 64;    // Tamanho máximo do nome do recurso
constexpr uint32_t MAX_RESOURCE_TYPES = 32;          // Máximo de tipos de recurso
constexpr uint32_t MAX_RESOURCE_LOADERS = 16;        // Máximo de carregadores de recurso
constexpr uint32_t MAX_ASYNC_REQUESTS = 64;          // Máximo de requisições assíncronas
constexpr uint32_t MAX_RESOURCE_HANDLES = 4096;      // Máximo de handles de recurso
constexpr uint32_t DEFAULT_RESOURCE_CACHE_SIZE = 256 * 1024 * 1024;  // 256 MB
constexpr uint32_t MAX_PAK_FILES = 32;               // Máximo de arquivos PAK
constexpr uint32_t MAX_CONVERSIONS = 16;             // Máximo de conversões simultâneas
constexpr uint32_t MAX_SEARCH_PATHS = 16;            // Máximo de caminhos de busca
constexpr uint32_t MAX_TEXTURE_SLOTS = 1024;         // Máximo de slots de textura
constexpr uint32_t MAX_MODEL_SLOTS = 512;            // Máximo de slots de modelo
constexpr uint32_t MAX_MAP_SLOTS = 128;              // Máximo de slots de mapa
constexpr uint32_t MAX_DATA_SLOTS = 2048;            // Máximo de slots de dados

/**
 * @brief Tipo de recurso
 */
enum class ResourceType : uint32_t {
    NONE = 0,                          // Nenhum
    TEXTURE = 1,                       // Textura (.wyt)
    MODEL = 2,                         // Modelo 3D (.wyp)
    MAP = 3,                           // Mapa (.wys)
    ITEM_LIST = 4,                     // Lista de itens (ItemList.bin)
    MOB_LIST = 5,                      // Lista de mobs (MobList.bin)
    SKILL_LIST = 6,                    // Lista de habilidades (SkillList.bin)
    QUEST_LIST = 7,                    // Lista de quests (QuestList.bin)
    DROP_LIST = 8,                     // Lista de drops (DropList.bin)
    SCRIPT = 9,                        // Script
    SOUND = 10,                        // Som
    MUSIC = 11,                        // Música
    ANIMATION = 12,                    // Animação
    EFFECT = 13,                       // Efeito
    UI = 14,                           // UI
    FONT = 15,                         // Fonte
    SHADER = 16,                       // Shader
    PACKAGE = 17,                      // Pacote (.pak)
    LOCALIZATION = 18,                 // Localização (.msg)
    BINARY = 19,                       // Binário genérico
    CUSTOM = 20,                       // Personalizado
};

/**
 * @brief Status de recurso
 */
enum class ResourceStatus : uint32_t {
    NONE = 0,                          // Nenhum
    UNLOADED = 1,                      // Não carregado
    LOADING = 2,                       // Carregando
    LOADED = 3,                        // Carregado
    FAILED = 4,                        // Falhou
    UNLOADING = 5,                     // Descarregando
    CONVERTING = 6,                    // Convertendo
    WAITING = 7,                       // Aguardando
    PROCESSING = 8,                    // Processando
    CACHED = 9,                        // Em cache
};

/**
 * @brief Prioridade de recurso
 */
enum class ResourcePriority : uint32_t {
    LOWEST = 0,                        // Mais baixa
    LOW = 1,                           // Baixa
    NORMAL = 2,                        // Normal
    HIGH = 3,                          // Alta
    HIGHEST = 4,                       // Mais alta
    CRITICAL = 5,                      // Crítica
};

/**
 * @brief Modo de cache
 */
enum class CacheMode : uint32_t {
    NONE = 0,                          // Nenhum
    MEMORY = 1,                        // Memória
    DISK = 2,                          // Disco
    HYBRID = 3,                        // Híbrido
};

/**
 * @brief Tipo de formato de mapa
 */
enum class MapFormat : uint32_t {
    UNKNOWN = 0,                       // Desconhecido
    WYS_1_0 = 1,                       // WYS 1.0
    WYS_2_0 = 2,                       // WYS 2.0
    WYS_EXTENDED = 3,                  // WYS Estendido
    WYS_OPTIMIZED = 4,                 // WYS Otimizado
    WYS_MODERN = 5,                    // WYS Moderno
};

/**
 * @brief Tipo de formato de modelo
 */
enum class ModelFormat : uint32_t {
    UNKNOWN = 0,                       // Desconhecido
    WYP_1_0 = 1,                       // WYP 1.0
    WYP_2_0 = 2,                       // WYP 2.0
    WYP_EXTENDED = 3,                  // WYP Estendido
    WYP_ANIMATED = 4,                  // WYP Animado
    WYP_OPTIMIZED = 5,                 // WYP Otimizado
    WYP_MODERN = 6,                    // WYP Moderno
};

/**
 * @brief Tipo de formato de textura
 */
enum class TextureFormat : uint32_t {
    UNKNOWN = 0,                       // Desconhecido
    WYT_1_0 = 1,                       // WYT 1.0
    WYT_2_0 = 2,                       // WYT 2.0
    WYT_COMPRESSED = 3,                // WYT Comprimido
    WYT_EXTENDED = 4,                  // WYT Estendido
    WYT_OPTIMIZED = 5,                 // WYT Otimizado
    WYT_MODERN = 6,                    // WYT Moderno
};

/**
 * @brief Tipo de formato binário
 */
enum class BinaryFormat : uint32_t {
    UNKNOWN = 0,                       // Desconhecido
    ITEM_LIST = 1,                     // Lista de itens
    MOB_LIST = 2,                      // Lista de mobs
    SKILL_LIST = 3,                    // Lista de habilidades
    QUEST_LIST = 4,                    // Lista de quests
    DROP_LIST = 5,                     // Lista de drops
    EFFECT_LIST = 6,                   // Lista de efeitos
    NPC_LIST = 7,                      // Lista de NPCs
    SHOP_LIST = 8,                     // Lista de lojas
    CUSTOM = 9,                        // Personalizado
};

/**
 * @brief Modo de compressão
 */
enum class CompressionMode : uint32_t {
    NONE = 0,                          // Nenhum
    ZLIB = 1,                          // ZLIB
    LZ4 = 2,                           // LZ4
    LZO = 3,                           // LZO
    RLE = 4,                           // RLE
    CUSTOM = 5,                        // Personalizado
};

/**
 * @brief Tipo de evento de recurso
 */
enum class ResourceEventType : uint32_t {
    NONE = 0,                          // Nenhum
    LOAD_START = 1,                    // Início de carregamento
    LOAD_COMPLETE = 2,                 // Carregamento completo
    LOAD_FAIL = 3,                     // Falha de carregamento
    UNLOAD_START = 4,                  // Início de descarregamento
    UNLOAD_COMPLETE = 5,               // Descarregamento completo
    CONVERT_START = 6,                 // Início de conversão
    CONVERT_COMPLETE = 7,              // Conversão completa
    CONVERT_FAIL = 8,                  // Falha de conversão
    CACHE_HIT = 9,                     // Hit de cache
    CACHE_MISS = 10,                   // Miss de cache
    CACHE_EVICT = 11,                  // Evicção de cache
    PAK_OPEN = 12,                     // Abertura de PAK
    PAK_CLOSE = 13,                    // Fechamento de PAK
    CUSTOM = 14,                       // Personalizado
};

/**
 * @brief Tipo de item de jogo
 */
enum class ItemType : uint32_t {
    NONE = 0,                          // Nenhum
    WEAPON = 1,                        // Arma
    ARMOR = 2,                         // Armadura
    SHIELD = 3,                        // Escudo
    HELMET = 4,                        // Elmo
    NECKLACE = 5,                      // Colar
    EARRING = 6,                       // Brinco
    RING = 7,                          // Anel
    BRACELET = 8,                      // Bracelete
    BOOT = 9,                          // Bota
    CLOAK = 10,                        // Capa
    BELT = 11,                         // Cinto
    POTION = 12,                       // Poção
    SCROLL = 13,                       // Pergaminho
    QUEST_ITEM = 14,                   // Item de quest
    MATERIAL = 15,                     // Material
    MOUNT = 16,                        // Montaria
    PET = 17,                          // Pet
    SKILLBOOK = 18,                    // Livro de habilidade
    CASH_ITEM = 19,                    // Item de cash
    CUSTOM = 20,                       // Personalizado
};

/**
 * @brief Entidade do jogo
 */
enum class EntityType : uint32_t {
    NONE = 0,                          // Nenhum
    PLAYER = 1,                        // Jogador
    NPC = 2,                           // NPC
    MONSTER = 3,                       // Monstro
    BOSS = 4,                          // Chefe
    ITEM = 5,                          // Item
    OBJECT = 6,                        // Objeto
    PROJECTILE = 7,                    // Projétil
    EFFECT = 8,                        // Efeito
    TRAP = 9,                          // Armadilha
    MOUNT = 10,                        // Montaria
    PET = 11,                          // Pet
    CUSTOM = 12,                       // Personalizado
};

/**
 * @brief Habilidade do jogo
 */
enum class SkillType : uint32_t {
    NONE = 0,                          // Nenhum
    ACTIVE = 1,                        // Ativa
    PASSIVE = 2,                       // Passiva
    BUFF = 3,                          // Buff
    DEBUFF = 4,                        // Debuff
    AOE = 5,                           // Área de efeito
    DOT = 6,                           // Dano ao longo do tempo
    HEAL = 7,                          // Cura
    TELEPORT = 8,                      // Teleporte
    SUMMON = 9,                        // Invocação
    TRANSFORMATION = 10,               // Transformação
    CUSTOM = 11,                       // Personalizado
};

/**
 * @brief Efeito do jogo
 */
enum class EffectType : uint32_t {
    NONE = 0,                          // Nenhum
    VISUAL = 1,                        // Visual
    SOUND = 2,                         // Som
    PARTICLE = 3,                      // Partícula
    LIGHT = 4,                         // Luz
    ANIMATION = 5,                     // Animação
    CUSTOM = 6,                        // Personalizado
};

/**
 * @brief Cabeçalho de mapa WYS
 */
struct WYSHeader {
    char signature[4];                 // Assinatura ('W', 'Y', 'S', '\0')
    uint32_t version;                  // Versão
    uint32_t width;                    // Largura
    uint32_t height;                   // Altura
    uint32_t tileSize;                 // Tamanho do tile
    uint32_t layerCount;               // Contagem de camadas
    uint32_t objectCount;              // Contagem de objetos
    uint32_t flags;                    // Flags
    uint32_t dataOffset;               // Offset de dados
    uint32_t dataSize;                 // Tamanho de dados
    uint32_t nameOffset;               // Offset de nome
    uint32_t nameLength;               // Comprimento de nome
    uint32_t reserved[4];              // Reservado
    
    WYSHeader()
        : version(0)
        , width(0)
        , height(0)
        , tileSize(0)
        , layerCount(0)
        , objectCount(0)
        , flags(0)
        , dataOffset(0)
        , dataSize(0)
        , nameOffset(0)
        , nameLength(0)
    {
        signature[0] = 'W';
        signature[1] = 'Y';
        signature[2] = 'S';
        signature[3] = '\0';
        for (int i = 0; i < 4; i++) {
            reserved[i] = 0;
        }
    }
};

/**
 * @brief Cabeçalho de modelo WYP
 */
struct WYPHeader {
    char signature[4];                 // Assinatura ('W', 'Y', 'P', '\0')
    uint32_t version;                  // Versão
    uint32_t vertexCount;              // Contagem de vértices
    uint32_t indexCount;               // Contagem de índices
    uint32_t textureCount;             // Contagem de texturas
    uint32_t boneCount;                // Contagem de ossos
    uint32_t animationCount;           // Contagem de animações
    uint32_t flags;                    // Flags
    uint32_t vertexOffset;             // Offset de vértices
    uint32_t indexOffset;              // Offset de índices
    uint32_t textureOffset;            // Offset de texturas
    uint32_t boneOffset;               // Offset de ossos
    uint32_t animationOffset;          // Offset de animações
    uint32_t reserved[3];              // Reservado
    
    WYPHeader()
        : version(0)
        , vertexCount(0)
        , indexCount(0)
        , textureCount(0)
        , boneCount(0)
        , animationCount(0)
        , flags(0)
        , vertexOffset(0)
        , indexOffset(0)
        , textureOffset(0)
        , boneOffset(0)
        , animationOffset(0)
    {
        signature[0] = 'W';
        signature[1] = 'Y';
        signature[2] = 'P';
        signature[3] = '\0';
        for (int i = 0; i < 3; i++) {
            reserved[i] = 0;
        }
    }
};

/**
 * @brief Cabeçalho de textura WYT
 */
struct WYTHeader {
    char signature[4];                 // Assinatura ('W', 'Y', 'T', '\0')
    uint32_t version;                  // Versão
    uint32_t width;                    // Largura
    uint32_t height;                   // Altura
    uint32_t format;                   // Formato
    uint32_t mipmapCount;              // Contagem de mipmaps
    uint32_t flags;                    // Flags
    uint32_t dataOffset;               // Offset de dados
    uint32_t dataSize;                 // Tamanho de dados
    uint32_t reserved[7];              // Reservado
    
    WYTHeader()
        : version(0)
        , width(0)
        , height(0)
        , format(0)
        , mipmapCount(0)
        , flags(0)
        , dataOffset(0)
        , dataSize(0)
    {
        signature[0] = 'W';
        signature[1] = 'Y';
        signature[2] = 'T';
        signature[3] = '\0';
        for (int i = 0; i < 7; i++) {
            reserved[i] = 0;
        }
    }
};

/**
 * @brief Cabeçalho de pacote PAK
 */
struct PAKHeader {
    char signature[4];                 // Assinatura ('P', 'A', 'K', '\0')
    uint32_t version;                  // Versão
    uint32_t fileCount;                // Contagem de arquivos
    uint32_t indexOffset;              // Offset de índice
    uint32_t indexSize;                 // Tamanho de índice
    uint32_t flags;                    // Flags
    uint32_t dataOffset;               // Offset de dados
    uint32_t dataSize;                 // Tamanho de dados
    uint32_t reserved[8];              // Reservado
    
    PAKHeader()
        : version(0)
        , fileCount(0)
        , indexOffset(0)
        , indexSize(0)
        , flags(0)
        , dataOffset(0)
        , dataSize(0)
    {
        signature[0] = 'P';
        signature[1] = 'A';
        signature[2] = 'K';
        signature[3] = '\0';
        for (int i = 0; i < 8; i++) {
            reserved[i] = 0;
        }
    }
};

/**
 * @brief Entrada de índice PAK
 */
struct PAKIndexEntry {
    char name[MAX_RESOURCE_PATH_LENGTH]; // Nome
    uint32_t offset;                   // Offset
    uint32_t size;                     // Tamanho
    uint32_t compressedSize;           // Tamanho comprimido
    uint32_t flags;                    // Flags
    uint32_t checksum;                 // Checksum
    uint32_t timestamp;                // Timestamp
    
    PAKIndexEntry()
        : offset(0)
        , size(0)
        , compressedSize(0)
        , flags(0)
        , checksum(0)
        , timestamp(0)
    {
        for (int i = 0; i < MAX_RESOURCE_PATH_LENGTH; i++) {
            name[i] = 0;
        }
    }
};

/**
 * @brief Arquivo PAK
 */
struct PAKFile {
    std::string path;                  // Caminho
    PAKHeader header;                  // Cabeçalho
    std::vector<PAKIndexEntry> index;  // Índice
    std::fstream file;                 // Arquivo
    bool isOpen;                       // Está aberto
    
    PAKFile()
        : path("")
        , header()
        , index()
        , isOpen(false)
    {}
};

/**
 * @brief Item de jogo
 */
struct GameItem {
    uint32_t id;                       // ID
    std::string name;                  // Nome
    ItemType type;                     // Tipo
    uint32_t subType;                  // Subtipo
    uint32_t level;                    // Nível
    uint32_t requiredLevel;            // Nível requerido
    uint32_t requiredClass;            // Classe requerida
    uint32_t attack;                   // Ataque
    uint32_t defense;                  // Defesa
    uint32_t price;                    // Preço
    uint32_t sellPrice;                // Preço de venda
    uint32_t modelId;                  // ID do modelo
    uint32_t textureId;                // ID da textura
    uint32_t maxStack;                 // Pilha máxima
    bool tradable;                     // Comercializável
    bool droppable;                    // Descartável
    bool storable;                     // Armazenável
    std::string description;           // Descrição
    std::unordered_map<std::string, int32_t> attributes; // Atributos
    std::unordered_map<std::string, std::string> metadata; // Metadados
    uint32_t iconIndex;                // Índice de ícone
    
    GameItem()
        : id(0)
        , name("")
        , type(ItemType::NONE)
        , subType(0)
        , level(0)
        , requiredLevel(0)
        , requiredClass(0)
        , attack(0)
        , defense(0)
        , price(0)
        , sellPrice(0)
        , modelId(0)
        , textureId(0)
        , maxStack(1)
        , tradable(true)
        , droppable(true)
        , storable(true)
        , description("")
        , attributes()
        , metadata()
        , iconIndex(0)
    {}
};

/**
 * @brief Entidade de jogo
 */
struct GameEntity {
    uint32_t id;                       // ID
    std::string name;                  // Nome
    EntityType type;                   // Tipo
    uint32_t level;                    // Nível
    uint32_t hp;                       // HP
    uint32_t mp;                       // MP
    uint32_t attack;                   // Ataque
    uint32_t defense;                  // Defesa
    uint32_t agility;                  // Agilidade
    uint32_t intelligence;             // Inteligência
    uint32_t modelId;                  // ID do modelo
    uint32_t textureId;                // ID da textura
    uint32_t dropGroup;                // Grupo de drop
    uint32_t expReward;                // Recompensa de experiência
    std::vector<uint32_t> skills;      // Habilidades
    std::unordered_map<std::string, int32_t> attributes; // Atributos
    std::unordered_map<std::string, std::string> metadata; // Metadados
    
    GameEntity()
        : id(0)
        , name("")
        , type(EntityType::NONE)
        , level(1)
        , hp(100)
        , mp(100)
        , attack(10)
        , defense(10)
        , agility(10)
        , intelligence(10)
        , modelId(0)
        , textureId(0)
        , dropGroup(0)
        , expReward(0)
        , skills()
        , attributes()
        , metadata()
    {}
};

/**
 * @brief Habilidade de jogo
 */
struct GameSkill {
    uint32_t id;                       // ID
    std::string name;                  // Nome
    SkillType type;                    // Tipo
    uint32_t level;                    // Nível
    uint32_t requiredLevel;            // Nível requerido
    uint32_t requiredClass;            // Classe requerida
    uint32_t requiredMP;               // MP requerido
    uint32_t cooldown;                 // Cooldown
    uint32_t damage;                   // Dano
    uint32_t range;                    // Alcance
    uint32_t areaOfEffect;             // Área de efeito
    uint32_t duration;                 // Duração
    uint32_t effectId;                 // ID do efeito
    std::string description;           // Descrição
    std::unordered_map<std::string, int32_t> attributes; // Atributos
    std::unordered_map<std::string, std::string> metadata; // Metadados
    uint32_t iconIndex;                // Índice de ícone
    
    GameSkill()
        : id(0)
        , name("")
        , type(SkillType::NONE)
        , level(1)
        , requiredLevel(1)
        , requiredClass(0)
        , requiredMP(0)
        , cooldown(0)
        , damage(0)
        , range(0)
        , areaOfEffect(0)
        , duration(0)
        , effectId(0)
        , description("")
        , attributes()
        , metadata()
        , iconIndex(0)
    {}
};

/**
 * @brief Mapa de jogo
 */
struct GameMap {
    uint32_t id;                       // ID
    std::string name;                  // Nome
    uint32_t width;                    // Largura
    uint32_t height;                   // Altura
    uint32_t tileSize;                 // Tamanho do tile
    uint32_t flags;                    // Flags
    uint32_t minLevel;                 // Nível mínimo
    uint32_t maxLevel;                 // Nível máximo
    std::vector<std::vector<uint8_t>> layers; // Camadas
    std::vector<uint32_t> objects;     // Objetos
    std::vector<uint32_t> entities;    // Entidades
    std::vector<uint32_t> portals;     // Portais
    std::vector<uint32_t> spawns;      // Spawns
    std::vector<uint32_t> events;      // Eventos
    std::unordered_map<std::string, std::string> metadata; // Metadados
    
    GameMap()
        : id(0)
        , name("")
        , width(0)
        , height(0)
        , tileSize(0)
        , flags(0)
        , minLevel(0)
        , maxLevel(0)
        , layers()
        , objects()
        , entities()
        , portals()
        , spawns()
        , events()
        , metadata()
    {}
};

/**
 * @brief Drop de jogo
 */
struct GameDrop {
    uint32_t id;                       // ID
    uint32_t itemId;                   // ID do item
    uint32_t groupId;                  // ID do grupo
    uint32_t minQuantity;              // Quantidade mínima
    uint32_t maxQuantity;              // Quantidade máxima
    uint32_t chance;                   // Chance
    uint32_t flags;                    // Flags
    
    GameDrop()
        : id(0)
        , itemId(0)
        , groupId(0)
        , minQuantity(1)
        , maxQuantity(1)
        , chance(0)
        , flags(0)
    {}
};

/**
 * @brief Evento de jogo
 */
struct GameEvent {
    uint32_t id;                       // ID
    std::string name;                  // Nome
    uint32_t type;                     // Tipo
    uint32_t minLevel;                 // Nível mínimo
    uint32_t maxLevel;                 // Nível máximo
    uint32_t mapId;                    // ID do mapa
    uint32_t startTime;                // Tempo de início
    uint32_t endTime;                  // Tempo de fim
    uint32_t duration;                 // Duração
    uint32_t cooldown;                 // Cooldown
    std::vector<uint32_t> rewards;     // Recompensas
    
    GameEvent()
        : id(0)
        , name("")
        , type(0)
        , minLevel(0)
        , maxLevel(0)
        , mapId(0)
        , startTime(0)
        , endTime(0)
        , duration(0)
        , cooldown(0)
        , rewards()
    {}
};

/**
 * @brief Quest de jogo
 */
struct GameQuest {
    uint32_t id;                       // ID
    std::string name;                  // Nome
    uint32_t type;                     // Tipo
    uint32_t minLevel;                 // Nível mínimo
    uint32_t maxLevel;                 // Nível máximo
    uint32_t startNpcId;               // ID do NPC de início
    uint32_t endNpcId;                 // ID do NPC de fim
    std::vector<uint32_t> objectives;  // Objetivos
    std::vector<uint32_t> rewards;     // Recompensas
    std::string description;           // Descrição
    
    GameQuest()
        : id(0)
        , name("")
        , type(0)
        , minLevel(0)
        , maxLevel(0)
        , startNpcId(0)
        , endNpcId(0)
        , objectives()
        , rewards()
        , description("")
    {}
};

/**
 * @brief Manipulador de recurso
 */
struct ResourceHandle {
    uint32_t id;                       // ID
    ResourceType type;                 // Tipo
    std::string path;                  // Caminho
    ResourceStatus status;             // Status
    uint32_t size;                     // Tamanho
    uint32_t referenceCount;           // Contagem de referências
    ResourcePriority priority;         // Prioridade
    void* data;                        // Dados
    std::string name;                  // Nome
    uint64_t lastAccessTime;           // Último tempo de acesso
    uint64_t loadTime;                 // Tempo de carregamento
    std::string error;                 // Erro
    
    ResourceHandle()
        : id(0)
        , type(ResourceType::NONE)
        , path("")
        , status(ResourceStatus::UNLOADED)
        , size(0)
        , referenceCount(0)
        , priority(ResourcePriority::NORMAL)
        , data(nullptr)
        , name("")
        , lastAccessTime(0)
        , loadTime(0)
        , error("")
    {}
};

/**
 * @brief Requisição de recurso assíncrona
 */
struct AsyncRequest {
    uint32_t id;                       // ID
    ResourceType type;                 // Tipo
    std::string path;                  // Caminho
    ResourcePriority priority;         // Prioridade
    bool isLoading;                    // Está carregando
    std::function<void(uint32_t, bool)> callback; // Callback
    uint64_t requestTime;              // Tempo de requisição
    std::string userData;              // Dados do usuário
    
    AsyncRequest()
        : id(0)
        , type(ResourceType::NONE)
        , path("")
        , priority(ResourcePriority::NORMAL)
        , isLoading(false)
        , callback(nullptr)
        , requestTime(0)
        , userData("")
    {}
};

/**
 * @brief Estatísticas do sistema de recursos
 */
struct ResourceStats {
    uint32_t totalResources;           // Total de recursos
    uint32_t loadedResources;          // Recursos carregados
    uint32_t failedResources;          // Recursos falhos
    uint32_t cachedResources;          // Recursos em cache
    uint32_t totalMemoryUsage;         // Uso total de memória
    uint32_t pendingRequests;          // Requisições pendentes
    uint32_t processedRequests;        // Requisições processadas
    uint32_t cacheHits;                // Hits de cache
    uint32_t cacheMisses;              // Misses de cache
    uint32_t diskReads;                // Leituras de disco
    uint32_t diskWrites;               // Escritas de disco
    uint32_t compressionRatio;         // Taxa de compressão
    uint32_t decompressionTime;        // Tempo de descompressão
    uint32_t totalLoadTime;            // Tempo total de carregamento
    float averageLoadTime;             // Tempo médio de carregamento
    
    ResourceStats()
        : totalResources(0)
        , loadedResources(0)
        , failedResources(0)
        , cachedResources(0)
        , totalMemoryUsage(0)
        , pendingRequests(0)
        , processedRequests(0)
        , cacheHits(0)
        , cacheMisses(0)
        , diskReads(0)
        , diskWrites(0)
        , compressionRatio(0)
        , decompressionTime(0)
        , totalLoadTime(0)
        , averageLoadTime(0.0f)
    {}
};

/**
 * @brief Configuração do sistema de recursos
 */
struct ResourceConfig {
    std::string basePath;              // Caminho base
    std::vector<std::string> searchPaths; // Caminhos de busca
    std::vector<std::string> pakFiles; // Arquivos PAK
    uint32_t cacheSizeBytes;           // Tamanho de cache em bytes
    CacheMode cacheMode;               // Modo de cache
    CompressionMode compressionMode;   // Modo de compressão
    uint32_t compressionLevel;         // Nível de compressão
    uint32_t maxAsyncRequests;         // Máximo de requisições assíncronas
    uint32_t workerThreads;            // Threads de trabalho
    bool enablePreloading;             // Habilitar pré-carregamento
    bool enableHotReloading;           // Habilitar recarregamento a quente
    bool trackResourceHistory;         // Rastrear histórico de recursos
    bool showDebugInfo;                // Mostrar informações de depuração
    bool useMemoryMappedFiles;         // Usar arquivos mapeados em memória
    std::string tempDirectory;         // Diretório temporário
    
    ResourceConfig()
        : basePath("./data")
        , searchPaths()
        , pakFiles()
        , cacheSizeBytes(DEFAULT_RESOURCE_CACHE_SIZE)
        , cacheMode(CacheMode::HYBRID)
        , compressionMode(CompressionMode::ZLIB)
        , compressionLevel(6)
        , maxAsyncRequests(MAX_ASYNC_REQUESTS)
        , workerThreads(2)
        , enablePreloading(true)
        , enableHotReloading(false)
        , trackResourceHistory(false)
        , showDebugInfo(false)
        , useMemoryMappedFiles(true)
        , tempDirectory("./temp")
    {}
};

/**
 * @brief Evento de recurso
 */
struct ResourceEvent {
    ResourceEventType type;            // Tipo
    ResourceType resourceType;         // Tipo de recurso
    uint32_t resourceId;               // ID do recurso
    std::string path;                  // Caminho
    uint64_t timestamp;                // Timestamp
    uint32_t size;                     // Tamanho
    std::string message;               // Mensagem
    
    ResourceEvent()
        : type(ResourceEventType::NONE)
        , resourceType(ResourceType::NONE)
        , resourceId(0)
        , path("")
        , timestamp(0)
        , size(0)
        , message("")
    {}
};

/**
 * @brief Tipo de callback de evento de recurso
 */
using ResourceEventCallback = std::function<void(const ResourceEvent&)>;

/**
 * @brief Tipo de callback de carregamento de recurso
 */
using ResourceLoadCallback = std::function<void(uint32_t, bool)>;

/**
 * @brief Sistema de recursos
 */
/**
 * Classe WYDResourceSystem
 * 
 * Esta classe implementa a funcionalidade WYDResourceSystem conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class ResourceSystem {
public:
    /**
     * @brief Obtém a instância singleton
     * @return Referência para a instância
     */
    static ResourceSystem& GetInstance();
    
    /**
     * @brief Inicializa o sistema
     * @param config Configuração
     * @return true se inicializado com sucesso
     */
    bool Initialize(const ResourceConfig& config = ResourceConfig());
    
    /**
     * @brief Finaliza o sistema
     */
    void Shutdown();
    
    /**
     * @brief Atualiza o sistema
     * @param deltaTime Tempo delta
     */
    void Update(float deltaTime);
    
    /**
     * @brief Carrega um recurso
     * @param path Caminho
     * @param type Tipo
     * @param priority Prioridade
     * @return ID do recurso, ou 0 se falhar
     */
    uint32_t LoadResource(const std::string& path, ResourceType type, ResourcePriority priority = ResourcePriority::NORMAL);
    
    /**
     * @brief Carrega um recurso de forma assíncrona
     * @param path Caminho
     * @param type Tipo
     * @param callback Callback
     * @param priority Prioridade
     * @param userData Dados do usuário
     * @return ID da requisição, ou 0 se falhar
     */
    uint32_t LoadResourceAsync(const std::string& path, ResourceType type, ResourceLoadCallback callback, 
                            ResourcePriority priority = ResourcePriority::NORMAL, const std::string& userData = "");
    
    /**
     * @brief Descarrega um recurso
     * @param resourceId ID do recurso
     * @return true se descarregado com sucesso
     */
    bool UnloadResource(uint32_t resourceId);
    
    /**
     * @brief Obtém um manipulador de recurso
     * @param resourceId ID do recurso
     * @return Manipulador, ou nullptr se não encontrado
     */
    const ResourceHandle* GetResource(uint32_t resourceId) const;
    
    /**
     * @brief Obtém dados brutos do recurso
     * @param resourceId ID do recurso
     * @return Dados, ou nullptr se não encontrado
     */
    void* GetResourceData(uint32_t resourceId);
    
    /**
     * @brief Adiciona uma referência ao recurso
     * @param resourceId ID do recurso
     * @return Nova contagem de referências, ou 0 se falhar
     */
    uint32_t AddResourceReference(uint32_t resourceId);
    
    /**
     * @brief Remove uma referência do recurso
     * @param resourceId ID do recurso
     * @return Nova contagem de referências, ou 0 se falhar
     */
    uint32_t RemoveResourceReference(uint32_t resourceId);
    
    /**
     * @brief Obtém o status de um recurso
     * @param resourceId ID do recurso
     * @return Status
     */
    ResourceStatus GetResourceStatus(uint32_t resourceId) const;
    
    /**
     * @brief Adiciona um caminho de busca
     * @param path Caminho
     * @return true se adicionado com sucesso
     */
    bool AddSearchPath(const std::string& path);
    
    /**
     * @brief Remove um caminho de busca
     * @param path Caminho
     * @return true se removido com sucesso
     */
    bool RemoveSearchPath(const std::string& path);
    
    /**
     * @brief Abre um arquivo PAK
     * @param path Caminho
     * @return true se aberto com sucesso
     */
    bool OpenPAK(const std::string& path);
    
    /**
     * @brief Fecha um arquivo PAK
     * @param path Caminho
     * @return true se fechado com sucesso
     */
    bool ClosePAK(const std::string& path);
    
    /**
     * @brief Extrai um arquivo de um PAK
     * @param pakPath Caminho do PAK
     * @param filePath Caminho do arquivo
     * @param outputPath Caminho de saída
     * @return true se extraído com sucesso
     */
    bool ExtractFromPAK(const std::string& pakPath, const std::string& filePath, const std::string& outputPath);
    
    /**
     * @brief Lista arquivos em um PAK
     * @param pakPath Caminho do PAK
     * @return Lista de arquivos
     */
    std::vector<std::string> ListFilesInPAK(const std::string& pakPath);
    
    /**
     * @brief Converte um arquivo para outro formato
     * @param inputPath Caminho de entrada
     * @param outputPath Caminho de saída
     * @param fromType Tipo de origem
     * @param toType Tipo de destino
     * @return true se convertido com sucesso
     */
    bool ConvertResource(const std::string& inputPath, const std::string& outputPath, 
                       ResourceType fromType, ResourceType toType);
    
    /**
     * @brief Cria um novo arquivo PAK
     * @param outputPath Caminho de saída
     * @param files Arquivos
     * @param compress Comprimir
     * @return true se criado com sucesso
     */
    bool CreatePAK(const std::string& outputPath, const std::vector<std::string>& files, bool compress = true);
    
    /**
     * @brief Carrega um arquivo binário
     * @param path Caminho
     * @param format Formato
     * @return true se carregado com sucesso
     */
    bool LoadBinaryFile(const std::string& path, BinaryFormat format);
    
    /**
     * @brief Salva um arquivo binário
     * @param path Caminho
     * @param format Formato
     * @return true se salvo com sucesso
     */
    bool SaveBinaryFile(const std::string& path, BinaryFormat format);
    
    /**
     * @brief Registra um callback de evento
     * @param callback Callback
     * @param eventType Tipo de evento
     * @return ID do callback, ou 0 se falhar
     */
    uint32_t RegisterEventCallback(ResourceEventCallback callback, ResourceEventType eventType = ResourceEventType::NONE);
    
    /**
     * @brief Remove um callback de evento
     * @param callbackId ID do callback
     * @return true se removido com sucesso
     */
    bool UnregisterEventCallback(uint32_t callbackId);
    
    /**
     * @brief Obtém uma lista de itens
     * @return Lista de itens
     */
    const std::vector<GameItem>& GetItemList() const;
    
    /**
     * @brief Obtém uma lista de entidades
     * @return Lista de entidades
     */
    const std::vector<GameEntity>& GetEntityList() const;
    
    /**
     * @brief Obtém uma lista de habilidades
     * @return Lista de habilidades
     */
    const std::vector<GameSkill>& GetSkillList() const;
    
    /**
     * @brief Obtém uma lista de mapas
     * @return Lista de mapas
     */
    const std::vector<GameMap>& GetMapList() const;
    
    /**
     * @brief Obtém um item pelo ID
     * @param itemId ID do item
     * @return Ponteiro para o item, ou nullptr se não encontrado
     */
    const GameItem* GetItemById(uint32_t itemId) const;
    
    /**
     * @brief Obtém uma entidade pelo ID
     * @param entityId ID da entidade
     * @return Ponteiro para a entidade, ou nullptr se não encontrada
     */
    const GameEntity* GetEntityById(uint32_t entityId) const;
    
    /**
     * @brief Obtém uma habilidade pelo ID
     * @param skillId ID da habilidade
     * @return Ponteiro para a habilidade, ou nullptr se não encontrada
     */
    const GameSkill* GetSkillById(uint32_t skillId) const;
    
    /**
     * @brief Obtém um mapa pelo ID
     * @param mapId ID do mapa
     * @return Ponteiro para o mapa, ou nullptr se não encontrado
     */
    const GameMap* GetMapById(uint32_t mapId) const;
    
    /**
     * @brief Obtém um drop pelo ID
     * @param dropId ID do drop
     * @return Ponteiro para o drop, ou nullptr se não encontrado
     */
    const GameDrop* GetDropById(uint32_t dropId) const;
    
    /**
     * @brief Obtém um evento pelo ID
     * @param eventId ID do evento
     * @return Ponteiro para o evento, ou nullptr se não encontrado
     */
    const GameEvent* GetEventById(uint32_t eventId) const;
    
    /**
     * @brief Obtém uma quest pelo ID
     * @param questId ID da quest
     * @return Ponteiro para a quest, ou nullptr se não encontrada
     */
    const GameQuest* GetQuestById(uint32_t questId) const;
    
    /**
     * @brief Obtém estatísticas do sistema
     * @return Estatísticas
     */
    ResourceStats GetStatistics() const;
    
    /**
     * @brief Limpa o cache
     * @return true se limpo com sucesso
     */
    bool ClearCache();
    
    /**
     * @brief Pré-carrega recursos
     * @param paths Caminhos
     * @param types Tipos
     * @return Número de recursos pré-carregados
     */
    uint32_t Preload(const std::vector<std::string>& paths, const std::vector<ResourceType>& types);
    
    /**
     * @brief Recarrega um recurso
     * @param resourceId ID do recurso
     * @return true se recarregado com sucesso
     */
    bool ReloadResource(uint32_t resourceId);
    
    /**
     * @brief Busca recursos por padrão
     * @param pattern Padrão
     * @param type Tipo
     * @return Lista de recursos
     */
    std::vector<ResourceHandle> FindResources(const std::string& pattern, ResourceType type = ResourceType::NONE);
    
    /**
     * @brief Carrega um mapa
     * @param path Caminho
     * @return ID do recurso, ou 0 se falhar
     */
    uint32_t LoadMap(const std::string& path);
    
    /**
     * @brief Carrega um modelo
     * @param path Caminho
     * @return ID do recurso, ou 0 se falhar
     */
    uint32_t LoadModel(const std::string& path);
    
    /**
     * @brief Carrega uma textura
     * @param path Caminho
     * @return ID do recurso, ou 0 se falhar
     */
    uint32_t LoadTexture(const std::string& path);
    
    /**
     * @brief Exporta um recurso para um formato comum
     * @param resourceId ID do recurso
     * @param outputPath Caminho de saída
     * @return true se exportado com sucesso
     */
    bool ExportResource(uint32_t resourceId, const std::string& outputPath);
    
    /**
     * @brief Importa um recurso de um formato comum
     * @param inputPath Caminho de entrada
     * @param outputPath Caminho de saída
     * @param type Tipo
     * @return true se importado com sucesso
     */
    bool ImportResource(const std::string& inputPath, const std::string& outputPath, ResourceType type);
    
    /**
     * @brief Descarrega todos os recursos
     */
    void UnloadAllResources();
    
    /**
     * @brief Sincroniza atualizações de recursos
     * @return true se sincronizado com sucesso
     */
    bool SyncResourceUpdates();
    
    /**
     * @brief Verifica a integridade dos recursos
     * @return true se os recursos estão íntegros
     */
    bool VerifyResourceIntegrity();
    
    /**
     * @brief Verifica se o sistema está inicializado
     * @return true se inicializado
     */
    bool IsInitialized() const;
    
private:
    /**
     * @brief Construtor privado (singleton)
     */
    ResourceSystem();
    
    /**
     * @brief Destrutor
     */
    ~ResourceSystem();
    
    /**
     * @brief Desabilita o construtor de cópia
     */
    ResourceSystem(const ResourceSystem&) = delete;
    
    /**
     * @brief Desabilita o operador de atribuição
     */
    ResourceSystem& operator=(const ResourceSystem&) = delete;
    
    /**
     * @brief Thread de carregamento assíncrono
     */
    void AsyncLoadThread();
    
    /**
     * @brief Carrega um recurso internamente
     * @param path Caminho
     * @param type Tipo
     * @param priority Prioridade
     * @param handle Manipulador (saída)
     * @return true se carregado com sucesso
     */
    bool LoadResourceInternal(const std::string& path, ResourceType type, ResourcePriority priority, ResourceHandle& handle);
    
    /**
     * @brief Carrega um arquivo PAK
     * @param path Caminho
     * @param pak PAK (saída)
     * @return true se carregado com sucesso
     */
    bool LoadPAKFile(const std::string& path, PAKFile& pak);
    
    /**
     * @brief Carrega um recurso de um PAK
     * @param pakFile PAK
     * @param filePath Caminho do arquivo
     * @param data Dados (saída)
     * @param size Tamanho (saída)
     * @return true se carregado com sucesso
     */
    bool LoadResourceFromPAK(const PAKFile& pakFile, const std::string& filePath, void*& data, size_t& size);
    
    /**
     * @brief Carrega uma textura
     * @param path Caminho
     * @param handle Manipulador (saída)
     * @return true se carregada com sucesso
     */
    bool LoadTextureResource(const std::string& path, ResourceHandle& handle);
    
    /**
     * @brief Carrega um modelo
     * @param path Caminho
     * @param handle Manipulador (saída)
     * @return true se carregado com sucesso
     */
    bool LoadModelResource(const std::string& path, ResourceHandle& handle);
    
    /**
     * @brief Carrega um mapa
     * @param path Caminho
     * @param handle Manipulador (saída)
     * @return true se carregado com sucesso
     */
    bool LoadMapResource(const std::string& path, ResourceHandle& handle);
    
    /**
     * @brief Carrega uma lista binária
     * @param path Caminho
     * @param format Formato
     * @param handle Manipulador (saída)
     * @return true se carregada com sucesso
     */
    bool LoadBinaryResource(const std::string& path, BinaryFormat format, ResourceHandle& handle);
    
    /**
     * @brief Verifica o cache para um recurso
     * @param path Caminho
     * @param type Tipo
     * @param handle Manipulador (saída)
     * @return true se encontrado no cache
     */
    bool CheckResourceCache(const std::string& path, ResourceType type, ResourceHandle& handle);
    
    /**
     * @brief Adiciona um recurso ao cache
     * @param handle Manipulador
     * @return true se adicionado com sucesso
     */
    bool CacheResource(const ResourceHandle& handle);
    
    /**
     * @brief Remove um recurso do cache
     * @param resourceId ID do recurso
     * @return true se removido com sucesso
     */
    bool RemoveFromCache(uint32_t resourceId);
    
    /**
     * @brief Localiza um arquivo
     * @param path Caminho
     * @param absolutePath Caminho absoluto (saída)
     * @param inPak Em PAK (saída)
     * @param pakPath Caminho do PAK (saída)
     * @return true se encontrado
     */
    bool LocateFile(const std::string& path, std::string& absolutePath, bool& inPak, std::string& pakPath);
    
    /**
     * @brief Processa requisições assíncronas
     */
    void ProcessAsyncRequests();
    
    /**
     * @brief Gerencia o cache de memória
     */
    void ManageMemoryCache();
    
    /**
     * @brief Notifica callbacks de evento
     * @param event Evento
     */
    void NotifyEventCallbacks(const ResourceEvent& event);
    
    /**
     * @brief Cria um evento de recurso
     * @param type Tipo
     * @param resourceType Tipo de recurso
     * @param resourceId ID do recurso
     * @param path Caminho
     * @param message Mensagem
     * @return Evento
     */
    ResourceEvent CreateEvent(ResourceEventType type, ResourceType resourceType, uint32_t resourceId, 
                           const std::string& path, const std::string& message = "");
    
    /**
     * @brief Aloca memória para um recurso
     * @param size Tamanho
     * @return Ponteiro para a memória alocada, ou nullptr se falhar
     */
    void* AllocateResourceMemory(size_t size);
    
    /**
     * @brief Libera memória de um recurso
     * @param ptr Ponteiro
     */
    void FreeResourceMemory(void* ptr);
    
    /**
     * @brief Lê um arquivo completo
     * @param path Caminho
     * @param data Dados (saída)
     * @param size Tamanho (saída)
     * @return true se lido com sucesso
     */
    bool ReadEntireFile(const std::string& path, void*& data, size_t& size);
    
    /**
     * @brief Escreve um arquivo completo
     * @param path Caminho
     * @param data Dados
     * @param size Tamanho
     * @return true se escrito com sucesso
     */
    bool WriteEntireFile(const std::string& path, const void* data, size_t size);
    
    /**
     * @brief Analisa um arquivo binário para determinar o tipo
     * @param data Dados
     * @param size Tamanho
     * @return Tipo
     */
    ResourceType DetectResourceType(const void* data, size_t size);
    
    /**
     * @brief Comprime dados
     * @param data Dados
     * @param size Tamanho
     * @param compressedData Dados comprimidos (saída)
     * @param compressedSize Tamanho comprimido (saída)
     * @return true se comprimido com sucesso
     */
    bool CompressData(const void* data, size_t size, void*& compressedData, size_t& compressedSize);
    
    /**
     * @brief Descomprime dados
     * @param data Dados
     * @param size Tamanho
     * @param originalSize Tamanho original
     * @param decompressedData Dados descomprimidos (saída)
     * @param decompressedSize Tamanho descomprimido (saída)
     * @return true se descomprimido com sucesso
     */
    bool DecompressData(const void* data, size_t size, size_t originalSize, void*& decompressedData, size_t& decompressedSize);
    
    /**
     * @brief Converte um mapa
     * @param sourceData Dados fonte
     * @param sourceSize Tamanho fonte
     * @param sourceFormat Formato fonte
     * @param targetFormat Formato alvo
     * @param targetData Dados alvo (saída)
     * @param targetSize Tamanho alvo (saída)
     * @return true se convertido com sucesso
     */
    bool ConvertMap(const void* sourceData, size_t sourceSize, MapFormat sourceFormat, 
                   MapFormat targetFormat, void*& targetData, size_t& targetSize);
    
    /**
     * @brief Converte um modelo
     * @param sourceData Dados fonte
     * @param sourceSize Tamanho fonte
     * @param sourceFormat Formato fonte
     * @param targetFormat Formato alvo
     * @param targetData Dados alvo (saída)
     * @param targetSize Tamanho alvo (saída)
     * @return true se convertido com sucesso
     */
    bool ConvertModel(const void* sourceData, size_t sourceSize, ModelFormat sourceFormat, 
                    ModelFormat targetFormat, void*& targetData, size_t& targetSize);
    
    /**
     * @brief Converte uma textura
     * @param sourceData Dados fonte
     * @param sourceSize Tamanho fonte
     * @param sourceFormat Formato fonte
     * @param targetFormat Formato alvo
     * @param targetData Dados alvo (saída)
     * @param targetSize Tamanho alvo (saída)
     * @return true se convertido com sucesso
     */
    bool ConvertTexture(const void* sourceData, size_t sourceSize, TextureFormat sourceFormat, 
                      TextureFormat targetFormat, void*& targetData, size_t& targetSize);
    
    /**
     * @brief Converte dados binários
     * @param sourceData Dados fonte
     * @param sourceSize Tamanho fonte
     * @param sourceFormat Formato fonte
     * @param targetFormat Formato alvo
     * @param targetData Dados alvo (saída)
     * @param targetSize Tamanho alvo (saída)
     * @return true se convertido com sucesso
     */
    bool ConvertBinary(const void* sourceData, size_t sourceSize, BinaryFormat sourceFormat, 
                     BinaryFormat targetFormat, void*& targetData, size_t& targetSize);
    
    /**
     * @brief Analisa a lista de itens
     * @param data Dados
     * @param size Tamanho
     * @return true se analisado com sucesso
     */
    bool ParseItemList(const void* data, size_t size);
    
    /**
     * @brief Analisa a lista de entidades
     * @param data Dados
     * @param size Tamanho
     * @return true se analisado com sucesso
     */
    bool ParseEntityList(const void* data, size_t size);
    
    /**
     * @brief Analisa a lista de habilidades
     * @param data Dados
     * @param size Tamanho
     * @return true se analisado com sucesso
     */
    bool ParseSkillList(const void* data, size_t size);
    
    /**
     * @brief Analisa a lista de quests
     * @param data Dados
     * @param size Tamanho
     * @return true se analisado com sucesso
     */
    bool ParseQuestList(const void* data, size_t size);
    
    /**
     * @brief Analisa a lista de drops
     * @param data Dados
     * @param size Tamanho
     * @return true se analisado com sucesso
     */
    bool ParseDropList(const void* data, size_t size);
    
    /**
     * @brief Analisa a lista de eventos
     * @param data Dados
     * @param size Tamanho
     * @return true se analisado com sucesso
     */
    bool ParseEventList(const void* data, size_t size);
    
    /**
     * @brief Gera a lista de itens
     * @param data Dados (saída)
     * @param size Tamanho (saída)
     * @return true se gerado com sucesso
     */
    bool GenerateItemList(void*& data, size_t& size);
    
    /**
     * @brief Gera a lista de entidades
     * @param data Dados (saída)
     * @param size Tamanho (saída)
     * @return true se gerado com sucesso
     */
    bool GenerateEntityList(void*& data, size_t& size);
    
    /**
     * @brief Gera a lista de habilidades
     * @param data Dados (saída)
     * @param size Tamanho (saída)
     * @return true se gerado com sucesso
     */
    bool GenerateSkillList(void*& data, size_t& size);
    
    /**
     * @brief Gera a lista de quests
     * @param data Dados (saída)
     * @param size Tamanho (saída)
     * @return true se gerado com sucesso
     */
    bool GenerateQuestList(void*& data, size_t& size);
    
    /**
     * @brief Gera a lista de drops
     * @param data Dados (saída)
     * @param size Tamanho (saída)
     * @return true se gerado com sucesso
     */
    bool GenerateDropList(void*& data, size_t& size);
    
    /**
     * @brief Gera a lista de eventos
     * @param data Dados (saída)
     * @param size Tamanho (saída)
     * @return true se gerado com sucesso
     */
    bool GenerateEventList(void*& data, size_t& size);
    
    // Configuração
    ResourceConfig config_;
    
    // Estado
    bool initialized_;
    
    // Manipuladores de recurso
    std::unordered_map<uint32_t, ResourceHandle> resources_;
    
    // Mapeamento de caminho para ID
    std::unordered_map<std::string, uint32_t> pathToResourceId_;
    
    // Cache de recursos
    std::unordered_map<uint32_t, ResourceHandle> resourceCache_;
    
    // Arquivos PAK
    std::vector<PAKFile> pakFiles_;
    
    // Requisições assíncronas
    std::queue<AsyncRequest> asyncRequests_;
    
    // Threads de carregamento assíncrono
    std::vector<std::thread> loadThreads_;
    std::atomic<bool> running_;
    
    // Mutexes
    std::mutex resourcesMutex_;
    std::mutex cacheMutex_;
    std::mutex pakFilesMutex_;
    std::mutex asyncRequestsMutex_;
    
    // Contadores de ID
    uint32_t nextResourceId_;
    uint32_t nextRequestId_;
    
    // Callbacks de evento
    std::unordered_map<uint32_t, std::pair<ResourceEventCallback, ResourceEventType>> eventCallbacks_;
    std::mutex eventCallbacksMutex_;
    uint32_t nextCallbackId_;
    
    // Estatísticas
    ResourceStats stats_;
    std::mutex statsMutex_;
    
    // Dados de jogo
    std::vector<GameItem> items_;
    std::vector<GameEntity> entities_;
    std::vector<GameSkill> skills_;
    std::vector<GameMap> maps_;
    std::vector<GameDrop> drops_;
    std::vector<GameEvent> events_;
    std::vector<GameQuest> quests_;
    
    // Mutexes de dados de jogo
    std::mutex itemsMutex_;
    std::mutex entitiesMutex_;
    std::mutex skillsMutex_;
    std::mutex mapsMutex_;
    std::mutex dropsMutex_;
    std::mutex eventsMutex_;
    std::mutex questsMutex_;
    
    // Tabelas de mapeamento de ID para índice
    std::unordered_map<uint32_t, size_t> itemIdToIndex_;
    std::unordered_map<uint32_t, size_t> entityIdToIndex_;
    std::unordered_map<uint32_t, size_t> skillIdToIndex_;
    std::unordered_map<uint32_t, size_t> mapIdToIndex_;
    std::unordered_map<uint32_t, size_t> dropIdToIndex_;
    std::unordered_map<uint32_t, size_t> eventIdToIndex_;
    std::unordered_map<uint32_t, size_t> questIdToIndex_;
};

// Acesso global
#define g_ResourceSystem ResourceSystem::GetInstance()

} // namespace resources
} // namespace reverse
} // namespace wyd

#endif // RESOURCESYSTEM_H

} // namespace wydbr
