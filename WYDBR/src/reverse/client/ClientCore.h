/**
 * ClientCore.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/reverse/client/ClientCore.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef CLIENTCORE_H
#define CLIENTCORE_H

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <array>
#include <mutex>
#include <atomic>
#include <thread>
#include <chrono>
#include <queue>
#include <bitset>
#include <cstdint>
#include <optional>

#include "../network/NetworkProtocol.h"
#include "../renderer/RenderingSystem.h"

namespace wydbr {


/**
 * @file ClientCore.h
 * @brief Sistema completo do core de cliente reconstruído do WYD original
 * 
 * Este arquivo contém a reconstrução do core de cliente do WYD,
 * incluindo sistema de inputs, eventos, comunicação entre componentes
 * e gerenciamento de memória. Todas as estruturas e comportamentos foram
 * replicados a partir da engenharia reversa do cliente original.
 */

namespace wyd {
namespace reverse {
namespace client {

// Constantes do client core
constexpr uint32_t MAX_INPUTS = 32;            // Número máximo de inputs
constexpr uint32_t MAX_INPUT_QUEUE = 64;       // Tamanho da fila de inputs
constexpr uint32_t MAX_EVENT_LISTENERS = 32;   // Número máximo de listeners de eventos
constexpr uint32_t MAX_CHARACTERS = 4;         // Número máximo de personagens
constexpr uint32_t MAX_INVENTORY_SLOTS = 64;   // Número máximo de slots de inventário
constexpr uint32_t MAX_STORAGE_SLOTS = 128;    // Número máximo de slots de armazenamento
constexpr uint32_t MAX_QUICK_SLOTS = 10;       // Número máximo de slots rápidos
constexpr uint32_t MAX_SKILL_SLOTS = 20;       // Número máximo de slots de habilidades
constexpr uint32_t MAX_FRIENDS = 50;           // Número máximo de amigos
constexpr uint32_t MAX_CHAT_MESSAGES = 100;    // Número máximo de mensagens de chat
constexpr uint32_t MAX_UI_ELEMENTS = 256;      // Número máximo de elementos de UI
constexpr uint32_t MAX_ENTITIES = 1024;        // Número máximo de entidades renderizáveis
constexpr uint32_t MAX_EFFECTS = 128;          // Número máximo de efeitos simultâneos
constexpr uint32_t MAX_SOUNDS = 32;            // Número máximo de sons simultâneos
constexpr uint32_t MAX_MEMORY_POOLS = 16;      // Número máximo de pools de memória

/**
 * @brief Código de tecla
 */
enum class KeyCode : uint32_t {
    NONE = 0,
    ESCAPE = 1,
    KEY_1 = 2,
    KEY_2 = 3,
    KEY_3 = 4,
    KEY_4 = 5,
    KEY_5 = 6,
    KEY_6 = 7,
    KEY_7 = 8,
    KEY_8 = 9,
    KEY_9 = 10,
    KEY_0 = 11,
    MINUS = 12,
    EQUALS = 13,
    BACK = 14,
    TAB = 15,
    Q = 16,
    W = 17,
    E = 18,
    R = 19,
    T = 20,
    Y = 21,
    U = 22,
    I = 23,
    O = 24,
    P = 25,
    LBRACKET = 26,
    RBRACKET = 27,
    RETURN = 28,
    LCONTROL = 29,
    A = 30,
    S = 31,
    D = 32,
    F = 33,
    G = 34,
    H = 35,
    J = 36,
    K = 37,
    L = 38,
    SEMICOLON = 39,
    APOSTROPHE = 40,
    GRAVE = 41,
    LSHIFT = 42,
    BACKSLASH = 43,
    Z = 44,
    X = 45,
    C = 46,
    V = 47,
    B = 48,
    N = 49,
    M = 50,
    COMMA = 51,
    PERIOD = 52,
    SLASH = 53,
    RSHIFT = 54,
    MULTIPLY = 55,
    LMENU = 56,
    SPACE = 57,
    CAPITAL = 58,
    F1 = 59,
    F2 = 60,
    F3 = 61,
    F4 = 62,
    F5 = 63,
    F6 = 64,
    F7 = 65,
    F8 = 66,
    F9 = 67,
    F10 = 68,
    NUMLOCK = 69,
    SCROLL = 70,
    NUMPAD7 = 71,
    NUMPAD8 = 72,
    NUMPAD9 = 73,
    SUBTRACT = 74,
    NUMPAD4 = 75,
    NUMPAD5 = 76,
    NUMPAD6 = 77,
    ADD = 78,
    NUMPAD1 = 79,
    NUMPAD2 = 80,
    NUMPAD3 = 81,
    NUMPAD0 = 82,
    DECIMAL = 83,
    F11 = 87,
    F12 = 88,
    F13 = 100,
    F14 = 101,
    F15 = 102,
    KANA = 112,
    CONVERT = 121,
    NOCONVERT = 123,
    YEN = 125,
    NUMPADEQUALS = 141,
    CIRCUMFLEX = 144,
    AT = 145,
    COLON = 146,
    UNDERLINE = 147,
    KANJI = 148,
    STOP = 149,
    AX = 150,
    UNLABELED = 151,
    NUMPADENTER = 156,
    RCONTROL = 157,
    NUMPADCOMMA = 179,
    DIVIDE = 181,
    SYSRQ = 183,
    RMENU = 184,
    PAUSE = 197,
    HOME = 199,
    UP = 200,
    PRIOR = 201,
    LEFT = 203,
    RIGHT = 205,
    END = 207,
    DOWN = 208,
    NEXT = 209,
    INSERT = 210,
    DELETE = 211,
    LWIN = 219,
    RWIN = 220,
    APPS = 221,
    POWER = 222,
    SLEEP = 223,
    LBUTTON = 1000,
    RBUTTON = 1001,
    MBUTTON = 1002,
    MOUSE4 = 1003,
    MOUSE5 = 1004,
    MOUSEWHEEL_UP = 1005,
    MOUSEWHEEL_DOWN = 1006,
};

/**
 * @brief Tipo de evento
 */
enum class EventType : uint32_t {
    NONE = 0,
    KEY_DOWN = 1,
    KEY_UP = 2,
    MOUSE_MOVE = 3,
    MOUSE_DOWN = 4,
    MOUSE_UP = 5,
    MOUSE_WHEEL = 6,
    WINDOW_RESIZE = 7,
    WINDOW_CLOSE = 8,
    WINDOW_FOCUS = 9,
    WINDOW_BLUR = 10,
    NETWORK_CONNECT = 11,
    NETWORK_DISCONNECT = 12,
    NETWORK_PACKET_RECEIVED = 13,
    RENDER_FRAME = 14,
    ENTITY_SPAWN = 15,
    ENTITY_DESPAWN = 16,
    ENTITY_MOVE = 17,
    ENTITY_ATTACK = 18,
    ENTITY_DAMAGE = 19,
    ENTITY_DEATH = 20,
    ENTITY_SKILL = 21,
    PLAYER_LEVELUP = 22,
    PLAYER_EXPERIENCE = 23,
    PLAYER_INVENTORY_CHANGE = 24,
    PLAYER_EQUIPMENT_CHANGE = 25,
    PLAYER_GOLD_CHANGE = 26,
    PLAYER_STAT_CHANGE = 27,
    PLAYER_SKILL_CHANGE = 28,
    PLAYER_QUEST_CHANGE = 29,
    PLAYER_TITLE_CHANGE = 30,
    CHAT_MESSAGE = 31,
    UI_ELEMENT_CLICK = 32,
    UI_ELEMENT_FOCUS = 33,
    UI_ELEMENT_BLUR = 34,
    UI_ELEMENT_DRAG = 35,
    GAME_STATE_CHANGE = 36,
    SYSTEM_ERROR = 37,
    SOUND_PLAY = 38,
    SOUND_STOP = 39,
    RESOURCE_LOAD = 40,
    RESOURCE_UNLOAD = 41,
    MEMORY_WARNING = 42,
    CUSTOM = 43,
};

/**
 * @brief Estado de jogo
 */
enum class GameState : uint32_t {
    NONE = 0,
    LOADING = 1,
    LOGIN = 2,
    CHARACTER_SELECTION = 3,
    CHARACTER_CREATION = 4,
    WORLD = 5,
    DIALOGUE = 6,
    SHOP = 7,
    INVENTORY = 8,
    SKILL = 9,
    QUEST = 10,
    MAP = 11,
    OPTION = 12,
    EXIT = 13,
};

/**
 * @brief Tipo de entidade
 */
enum class EntityType : uint32_t {
    NONE = 0,
    PLAYER = 1,
    NPC = 2,
    MONSTER = 3,
    ITEM = 4,
    PROJECTILE = 5,
    EFFECT = 6,
    STRUCTURE = 7,
    VEHICLE = 8,
    MOUNT = 9,
    PET = 10,
    TRAP = 11,
    CUSTOM = 12,
};

/**
 * @brief Tipo de input
 */
enum class InputType : uint32_t {
    NONE = 0,
    KEYBOARD = 1,
    MOUSE = 2,
    GAMEPAD = 3,
    TOUCH = 4,
    GESTURE = 5,
    CUSTOM = 6,
};

/**
 * @brief Modo de input
 */
enum class InputMode : uint32_t {
    NORMAL = 0,
    TEXT = 1,
    DIRECT = 2,
    CUSTOM = 3,
};

/**
 * @brief Tipo de recurso
 */
enum class ResourceType : uint32_t {
    NONE = 0,
    TEXTURE = 1,
    MESH = 2,
    SOUND = 3,
    MUSIC = 4,
    SCRIPT = 5,
    FONT = 6,
    SHADER = 7,
    EFFECT = 8,
    UI = 9,
    MAP = 10,
    BINARY = 11,
    CUSTOM = 12,
};

/**
 * @brief Prioridade de memória
 */
enum class MemoryPriority : uint32_t {
    LOWEST = 0,
    LOW = 1,
    NORMAL = 2,
    HIGH = 3,
    HIGHEST = 4,
    CRITICAL = 5,
};

/**
 * @brief Evento
 */
struct Event {
    EventType type;                     // Tipo
    uint64_t timestamp;                 // Timestamp
    void* userData;                     // Dados do usuário
    
    Event()
        : type(EventType::NONE)
        , timestamp(0)
        , userData(nullptr)
    {}
    
    Event(EventType t, uint64_t ts, void* ud)
        : type(t)
        , timestamp(ts)
        , userData(ud)
    {}
};

/**
 * @brief Evento de teclado
 */
struct KeyboardEvent : public Event {
    KeyCode keyCode;                    // Código da tecla
    uint32_t scanCode;                  // Código de scan
    uint32_t modifiers;                 // Modificadores
    bool repeat;                        // Repetição
    
    KeyboardEvent()
        : Event(EventType::KEY_DOWN, 0, nullptr)
        , keyCode(KeyCode::NONE)
        , scanCode(0)
        , modifiers(0)
        , repeat(false)
    {}
};

/**
 * @brief Evento de mouse
 */
struct MouseEvent : public Event {
    int32_t x;                          // Posição X
    int32_t y;                          // Posição Y
    int32_t deltaX;                     // Delta X
    int32_t deltaY;                     // Delta Y
    int32_t button;                     // Botão
    uint32_t modifiers;                 // Modificadores
    
    MouseEvent()
        : Event(EventType::MOUSE_MOVE, 0, nullptr)
        , x(0)
        , y(0)
        , deltaX(0)
        , deltaY(0)
        , button(0)
        , modifiers(0)
    {}
};

/**
 * @brief Evento de rede
 */
struct NetworkEvent : public Event {
    network::PacketType packetType;     // Tipo de pacote
    network::Packet packet;             // Pacote
    uint32_t connectionId;              // ID de conexão
    
    NetworkEvent()
        : Event(EventType::NETWORK_PACKET_RECEIVED, 0, nullptr)
        , packetType(network::PacketType::NONE)
        , packet()
        , connectionId(0)
    {}
};

/**
 * @brief Evento de entidade
 */
struct EntityEvent : public Event {
    uint32_t entityId;                  // ID da entidade
    EntityType entityType;              // Tipo de entidade
    int32_t x;                          // Posição X
    int32_t y;                          // Posição Y
    int32_t z;                          // Posição Z
    
    EntityEvent()
        : Event(EventType::ENTITY_SPAWN, 0, nullptr)
        , entityId(0)
        , entityType(EntityType::NONE)
        , x(0)
        , y(0)
        , z(0)
    {}
};

/**
 * @brief Evento de jogador
 */
struct PlayerEvent : public Event {
    uint32_t playerId;                  // ID do jogador
    uint32_t value;                     // Valor
    std::string parameter;              // Parâmetro
    
    PlayerEvent()
        : Event(EventType::PLAYER_LEVELUP, 0, nullptr)
        , playerId(0)
        , value(0)
        , parameter("")
    {}
};

/**
 * @brief Evento de chat
 */
struct ChatEvent : public Event {
    uint32_t senderId;                  // ID do remetente
    std::string senderName;             // Nome do remetente
    std::string message;                // Mensagem
    uint32_t channel;                   // Canal
    
    ChatEvent()
        : Event(EventType::CHAT_MESSAGE, 0, nullptr)
        , senderId(0)
        , senderName("")
        , message("")
        , channel(0)
    {}
};

/**
 * @brief Evento de UI
 */
struct UIEvent : public Event {
    uint32_t elementId;                 // ID do elemento
    std::string elementName;            // Nome do elemento
    int32_t x;                          // Posição X
    int32_t y;                          // Posição Y
    
    UIEvent()
        : Event(EventType::UI_ELEMENT_CLICK, 0, nullptr)
        , elementId(0)
        , elementName("")
        , x(0)
        , y(0)
    {}
};

/**
 * @brief Evento de sistema
 */
struct SystemEvent : public Event {
    std::string message;                // Mensagem
    int32_t code;                       // Código
    
    SystemEvent()
        : Event(EventType::SYSTEM_ERROR, 0, nullptr)
        , message("")
        , code(0)
    {}
};

/**
 * @brief Input
 */
struct Input {
    InputType type;                     // Tipo
    uint32_t code;                      // Código
    uint32_t modifiers;                 // Modificadores
    float value;                        // Valor
    uint64_t timestamp;                 // Timestamp
    
    Input()
        : type(InputType::NONE)
        , code(0)
        , modifiers(0)
        , value(0.0f)
        , timestamp(0)
    {}
    
    Input(InputType t, uint32_t c, uint32_t m, float v, uint64_t ts)
        : type(t)
        , code(c)
        , modifiers(m)
        , value(v)
        , timestamp(ts)
    {}
};

/**
 * @brief Mapeamento de input
 */
struct InputMapping {
    std::string name;                   // Nome
    InputType type;                     // Tipo
    uint32_t code;                      // Código
    uint32_t modifiers;                 // Modificadores
    float scale;                        // Escala
    
    InputMapping()
        : name("")
        , type(InputType::NONE)
        , code(0)
        , modifiers(0)
        , scale(1.0f)
    {}
    
    InputMapping(const std::string& n, InputType t, uint32_t c, uint32_t m, float s)
        : name(n)
        , type(t)
        , code(c)
        , modifiers(m)
        , scale(s)
    {}
};

/**
 * @brief Entidade
 */
struct Entity {
    uint32_t id;                        // ID
    EntityType type;                    // Tipo
    std::string name;                   // Nome
    float x;                            // Posição X
    float y;                            // Posição Y
    float z;                            // Posição Z
    float rotationX;                    // Rotação X
    float rotationY;                    // Rotação Y
    float rotationZ;                    // Rotação Z
    float scaleX;                       // Escala X
    float scaleY;                       // Escala Y
    float scaleZ;                       // Escala Z
    uint32_t modelId;                   // ID do modelo
    uint32_t textureId;                 // ID da textura
    bool visible;                       // Visível
    void* userData;                     // Dados do usuário
    
    Entity()
        : id(0)
        , type(EntityType::NONE)
        , name("")
        , x(0.0f)
        , y(0.0f)
        , z(0.0f)
        , rotationX(0.0f)
        , rotationY(0.0f)
        , rotationZ(0.0f)
        , scaleX(1.0f)
        , scaleY(1.0f)
        , scaleZ(1.0f)
        , modelId(0)
        , textureId(0)
        , visible(true)
        , userData(nullptr)
    {}
};

/**
 * @brief Personagem do jogador
 */
struct Character {
    uint32_t id;                        // ID
    std::string name;                   // Nome
    uint32_t level;                     // Nível
    uint32_t classId;                   // ID da classe
    uint32_t experience;                // Experiência
    uint32_t gold;                      // Ouro
    uint32_t hp;                        // HP
    uint32_t maxHp;                     // HP máximo
    uint32_t mp;                        // MP
    uint32_t maxMp;                     // MP máximo
    uint32_t sp;                        // SP
    uint32_t maxSp;                     // SP máximo
    uint32_t strength;                  // Força
    uint32_t dexterity;                 // Destreza
    uint32_t constitution;              // Constituição
    uint32_t intelligence;              // Inteligência
    uint32_t wisdom;                    // Sabedoria
    uint32_t charisma;                  // Carisma
    uint32_t statPoints;                // Pontos de status
    uint32_t skillPoints;               // Pontos de habilidade
    std::array<uint32_t, MAX_INVENTORY_SLOTS> inventory; // Inventário
    std::array<uint32_t, 16> equipment; // Equipamento
    std::array<uint32_t, MAX_SKILL_SLOTS> skills; // Habilidades
    
    Character()
        : id(0)
        , name("")
        , level(1)
        , classId(0)
        , experience(0)
        , gold(0)
        , hp(100)
        , maxHp(100)
        , mp(100)
        , maxMp(100)
        , sp(100)
        , maxSp(100)
        , strength(10)
        , dexterity(10)
        , constitution(10)
        , intelligence(10)
        , wisdom(10)
        , charisma(10)
        , statPoints(0)
        , skillPoints(0)
    {
        inventory.fill(0);
        equipment.fill(0);
        skills.fill(0);
    }
};

/**
 * @brief Item
 */
struct Item {
    uint32_t id;                        // ID
    std::string name;                   // Nome
    uint32_t type;                      // Tipo
    uint32_t subType;                   // Subtipo
    uint32_t level;                     // Nível
    uint32_t quality;                   // Qualidade
    uint32_t price;                     // Preço
    uint32_t sellPrice;                 // Preço de venda
    uint32_t durability;                // Durabilidade
    uint32_t maxDurability;             // Durabilidade máxima
    uint32_t quantity;                  // Quantidade
    uint32_t maxQuantity;               // Quantidade máxima
    bool tradable;                      // Comercializável
    bool droppable;                     // Descartável
    bool usable;                        // Utilizável
    uint32_t modelId;                   // ID do modelo
    uint32_t textureId;                 // ID da textura
    std::string description;            // Descrição
    std::unordered_map<std::string, int32_t> attributes; // Atributos
    
    Item()
        : id(0)
        , name("")
        , type(0)
        , subType(0)
        , level(1)
        , quality(0)
        , price(0)
        , sellPrice(0)
        , durability(100)
        , maxDurability(100)
        , quantity(1)
        , maxQuantity(1)
        , tradable(true)
        , droppable(true)
        , usable(false)
        , modelId(0)
        , textureId(0)
        , description("")
        , attributes()
    {}
};

/**
 * @brief Mensagem de chat
 */
struct ChatMessage {
    uint32_t senderId;                  // ID do remetente
    std::string senderName;             // Nome do remetente
    std::string message;                // Mensagem
    uint32_t channel;                   // Canal
    uint64_t timestamp;                 // Timestamp
    
    ChatMessage()
        : senderId(0)
        , senderName("")
        , message("")
        , channel(0)
        , timestamp(0)
    {}
};

/**
 * @brief Elemento de UI
 */
struct UIElement {
    uint32_t id;                        // ID
    std::string name;                   // Nome
    int32_t x;                          // Posição X
    int32_t y;                          // Posição Y
    int32_t width;                      // Largura
    int32_t height;                     // Altura
    bool visible;                       // Visível
    bool enabled;                       // Habilitado
    uint32_t parentId;                  // ID do pai
    uint32_t textureId;                 // ID da textura
    std::string text;                   // Texto
    uint32_t textColor;                 // Cor do texto
    uint32_t backgroundColor;           // Cor de fundo
    uint32_t borderColor;               // Cor da borda
    int32_t borderWidth;                // Largura da borda
    void* userData;                     // Dados do usuário
    
    UIElement()
        : id(0)
        , name("")
        , x(0)
        , y(0)
        , width(0)
        , height(0)
        , visible(true)
        , enabled(true)
        , parentId(0)
        , textureId(0)
        , text("")
        , textColor(0xFFFFFFFF)
        , backgroundColor(0)
        , borderColor(0)
        , borderWidth(0)
        , userData(nullptr)
    {}
};

/**
 * @brief Recurso
 */
struct Resource {
    uint32_t id;                        // ID
    std::string name;                   // Nome
    ResourceType type;                  // Tipo
    std::string path;                   // Caminho
    bool loaded;                        // Carregado
    uint32_t size;                      // Tamanho
    void* data;                         // Dados
    uint32_t referenceCount;            // Contagem de referências
    MemoryPriority priority;            // Prioridade
    
    Resource()
        : id(0)
        , name("")
        , type(ResourceType::NONE)
        , path("")
        , loaded(false)
        , size(0)
        , data(nullptr)
        , referenceCount(0)
        , priority(MemoryPriority::NORMAL)
    {}
};

/**
 * @brief Pool de memória
 */
struct MemoryPool {
    std::string name;                   // Nome
    uint32_t blockSize;                 // Tamanho do bloco
    uint32_t blockCount;                // Contagem de blocos
    uint32_t usedBlocks;                // Blocos usados
    void* memory;                       // Memória
    std::vector<bool> blockStatus;      // Status dos blocos
    
    MemoryPool()
        : name("")
        , blockSize(0)
        , blockCount(0)
        , usedBlocks(0)
        , memory(nullptr)
        , blockStatus()
    {}
};

/**
 * @brief Configuração de cliente
 */
struct ClientConfig {
    std::string windowTitle;            // Título da janela
    int32_t windowWidth;                // Largura da janela
    int32_t windowHeight;               // Altura da janela
    bool fullscreen;                    // Tela cheia
    bool vsync;                         // VSync
    int32_t maxFPS;                     // FPS máximo
    int32_t soundVolume;                // Volume de som
    int32_t musicVolume;                // Volume de música
    bool showFPS;                       // Mostrar FPS
    bool showPing;                      // Mostrar ping
    std::string language;               // Idioma
    std::string dataPath;               // Caminho de dados
    std::string savePath;               // Caminho de salvamento
    uint32_t maxEntities;               // Máximo de entidades
    uint32_t maxResources;              // Máximo de recursos
    uint32_t memoryLimit;               // Limite de memória (MB)
    
    ClientConfig()
        : windowTitle("WYD Client")
        , windowWidth(1024)
        , windowHeight(768)
        , fullscreen(false)
        , vsync(true)
        , maxFPS(60)
        , soundVolume(100)
        , musicVolume(80)
        , showFPS(true)
        , showPing(true)
        , language("en")
        , dataPath("./data")
        , savePath("./save")
        , maxEntities(MAX_ENTITIES)
        , maxResources(1024)
        , memoryLimit(1024)
    {}
};

/**
 * @brief Estatísticas de cliente
 */
struct ClientStats {
    uint32_t fps;                       // FPS
    float frameTime;                    // Tempo de quadro
    uint32_t ping;                      // Ping
    uint32_t memoryUsage;               // Uso de memória
    uint32_t entityCount;               // Contagem de entidades
    uint32_t drawCalls;                 // Chamadas de desenho
    uint32_t triangleCount;             // Contagem de triângulos
    uint32_t resourceCount;             // Contagem de recursos
    uint32_t resourceMemory;            // Memória de recursos
    uint32_t networkBytesIn;            // Bytes recebidos
    uint32_t networkBytesOut;           // Bytes enviados
    uint32_t packetsSent;               // Pacotes enviados
    uint32_t packetsReceived;           // Pacotes recebidos
    
    ClientStats()
        : fps(0)
        , frameTime(0.0f)
        , ping(0)
        , memoryUsage(0)
        , entityCount(0)
        , drawCalls(0)
        , triangleCount(0)
        , resourceCount(0)
        , resourceMemory(0)
        , networkBytesIn(0)
        , networkBytesOut(0)
        , packetsSent(0)
        , packetsReceived(0)
    {}
};

/**
 * @brief Tipo de callback de evento
 */
using EventCallback = std::function<void(const Event&)>;

/**
 * @brief Tipo de callback de input
 */
using InputCallback = std::function<void(const Input&)>;

/**
 * @brief Gerenciador de eventos
 */
/**
 * Classe WYDEventManager
 * 
 * Esta classe implementa a funcionalidade WYDEventManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class EventManager {
public:
    /**
     * @brief Obtém a instância singleton
     * @return Referência para a instância
     */
    static EventManager& GetInstance();
    
    /**
     * @brief Inicializa o gerenciador
     * @return true se inicializado com sucesso
     */
    bool Initialize();
    
    /**
     * @brief Finaliza o gerenciador
     */
    void Shutdown();
    
    /**
     * @brief Adiciona um evento à fila
     * @param event Evento
     */
    void QueueEvent(const Event& event);
    
    /**
     * @brief Despacha um evento imediatamente
     * @param event Evento
     * @return true se o evento foi tratado por algum listener
     */
    bool DispatchEvent(const Event& event);
    
    /**
     * @brief Processa todos os eventos na fila
     * @param timeoutMs Timeout em ms (0 para processar todos)
     * @return Número de eventos processados
     */
    uint32_t ProcessEvents(uint32_t timeoutMs = 0);
    
    /**
     * @brief Registra um callback para um tipo de evento
     * @param type Tipo de evento
     * @param callback Callback
     * @return ID do listener, ou 0 se falhar
     */
    uint32_t AddListener(EventType type, EventCallback callback);
    
    /**
     * @brief Remove um listener
     * @param listenerId ID do listener
     * @return true se removido com sucesso
     */
    bool RemoveListener(uint32_t listenerId);
    
    /**
     * @brief Cria um evento de teclado
     * @param type Tipo (KEY_DOWN ou KEY_UP)
     * @param keyCode Código da tecla
     * @param scanCode Código de scan
     * @param modifiers Modificadores
     * @param repeat Repetição
     * @return Evento criado
     */
    KeyboardEvent CreateKeyboardEvent(EventType type, KeyCode keyCode, uint32_t scanCode, uint32_t modifiers, bool repeat);
    
    /**
     * @brief Cria um evento de mouse
     * @param type Tipo (MOUSE_MOVE, MOUSE_DOWN, MOUSE_UP ou MOUSE_WHEEL)
     * @param x Posição X
     * @param y Posição Y
     * @param deltaX Delta X
     * @param deltaY Delta Y
     * @param button Botão
     * @param modifiers Modificadores
     * @return Evento criado
     */
    MouseEvent CreateMouseEvent(EventType type, int32_t x, int32_t y, int32_t deltaX, int32_t deltaY, int32_t button, uint32_t modifiers);
    
    /**
     * @brief Cria um evento de rede
     * @param type Tipo (NETWORK_CONNECT, NETWORK_DISCONNECT ou NETWORK_PACKET_RECEIVED)
     * @param packetType Tipo de pacote
     * @param packet Pacote
     * @param connectionId ID de conexão
     * @return Evento criado
     */
    NetworkEvent CreateNetworkEvent(EventType type, network::PacketType packetType, const network::Packet& packet, uint32_t connectionId);
    
    /**
     * @brief Cria um evento de entidade
     * @param type Tipo (ENTITY_SPAWN, ENTITY_DESPAWN, ENTITY_MOVE, ENTITY_ATTACK, ENTITY_DAMAGE, ENTITY_DEATH ou ENTITY_SKILL)
     * @param entityId ID da entidade
     * @param entityType Tipo de entidade
     * @param x Posição X
     * @param y Posição Y
     * @param z Posição Z
     * @return Evento criado
     */
    EntityEvent CreateEntityEvent(EventType type, uint32_t entityId, EntityType entityType, int32_t x, int32_t y, int32_t z);
    
    /**
     * @brief Cria um evento de jogador
     * @param type Tipo (PLAYER_LEVELUP, PLAYER_EXPERIENCE, PLAYER_INVENTORY_CHANGE, PLAYER_EQUIPMENT_CHANGE, PLAYER_GOLD_CHANGE, PLAYER_STAT_CHANGE, PLAYER_SKILL_CHANGE, PLAYER_QUEST_CHANGE ou PLAYER_TITLE_CHANGE)
     * @param playerId ID do jogador
     * @param value Valor
     * @param parameter Parâmetro
     * @return Evento criado
     */
    PlayerEvent CreatePlayerEvent(EventType type, uint32_t playerId, uint32_t value, const std::string& parameter);
    
    /**
     * @brief Cria um evento de chat
     * @param senderId ID do remetente
     * @param senderName Nome do remetente
     * @param message Mensagem
     * @param channel Canal
     * @return Evento criado
     */
    ChatEvent CreateChatEvent(uint32_t senderId, const std::string& senderName, const std::string& message, uint32_t channel);
    
    /**
     * @brief Cria um evento de UI
     * @param type Tipo (UI_ELEMENT_CLICK, UI_ELEMENT_FOCUS, UI_ELEMENT_BLUR ou UI_ELEMENT_DRAG)
     * @param elementId ID do elemento
     * @param elementName Nome do elemento
     * @param x Posição X
     * @param y Posição Y
     * @return Evento criado
     */
    UIEvent CreateUIEvent(EventType type, uint32_t elementId, const std::string& elementName, int32_t x, int32_t y);
    
    /**
     * @brief Cria um evento de sistema
     * @param message Mensagem
     * @param code Código
     * @return Evento criado
     */
    SystemEvent CreateSystemEvent(const std::string& message, int32_t code);
    
private:
    /**
     * @brief Construtor privado (singleton)
     */
    EventManager();
    
    /**
     * @brief Destrutor
     */
    ~EventManager();
    
    /**
     * @brief Desabilita o construtor de cópia
     */
    EventManager(const EventManager&) = delete;
    
    /**
     * @brief Desabilita o operador de atribuição
     */
    EventManager& operator=(const EventManager&) = delete;
    
    // Fila de eventos
    std::queue<Event> eventQueue_;
    
    // Mutex para a fila de eventos
    std::mutex eventQueueMutex_;
    
    // Listeners de eventos
    std::unordered_map<EventType, std::unordered_map<uint32_t, EventCallback>> listeners_;
    
    // Mutex para os listeners
    std::mutex listenersMutex_;
    
    // Contador de IDs de listeners
    uint32_t nextListenerId_;
    
    // Estado
    bool initialized_;
};

/**
 * @brief Gerenciador de input
 */
/**
 * Classe WYDInputManager
 * 
 * Esta classe implementa a funcionalidade WYDInputManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class InputManager {
public:
    /**
     * @brief Obtém a instância singleton
     * @return Referência para a instância
     */
    static InputManager& GetInstance();
    
    /**
     * @brief Inicializa o gerenciador
     * @return true se inicializado com sucesso
     */
    bool Initialize();
    
    /**
     * @brief Finaliza o gerenciador
     */
    void Shutdown();
    
    /**
     * @brief Atualiza o gerenciador
     */
    void Update();
    
    /**
     * @brief Define o modo de input
     * @param mode Modo
     */
    void SetInputMode(InputMode mode);
    
    /**
     * @brief Obtém o modo de input
     * @return Modo
     */
    InputMode GetInputMode() const;
    
    /**
     * @brief Adiciona um mapeamento de input
     * @param mapping Mapeamento
     * @return true se adicionado com sucesso
     */
    bool AddInputMapping(const InputMapping& mapping);
    
    /**
     * @brief Remove um mapeamento de input
     * @param name Nome
     * @return true se removido com sucesso
     */
    bool RemoveInputMapping(const std::string& name);
    
    /**
     * @brief Obtém um mapeamento de input
     * @param name Nome
     * @return Mapeamento, ou nullptr se não encontrado
     */
    const InputMapping* GetInputMapping(const std::string& name) const;
    
    /**
     * @brief Verifica se uma tecla está pressionada
     * @param keyCode Código da tecla
     * @return true se pressionada
     */
    bool IsKeyDown(KeyCode keyCode) const;
    
    /**
     * @brief Verifica se uma tecla foi pressionada neste quadro
     * @param keyCode Código da tecla
     * @return true se pressionada neste quadro
     */
    bool IsKeyPressed(KeyCode keyCode) const;
    
    /**
     * @brief Verifica se uma tecla foi liberada neste quadro
     * @param keyCode Código da tecla
     * @return true se liberada neste quadro
     */
    bool IsKeyReleased(KeyCode keyCode) const;
    
    /**
     * @brief Verifica se um botão do mouse está pressionado
     * @param button Botão
     * @return true se pressionado
     */
    bool IsMouseButtonDown(int32_t button) const;
    
    /**
     * @brief Verifica se um botão do mouse foi pressionado neste quadro
     * @param button Botão
     * @return true se pressionado neste quadro
     */
    bool IsMouseButtonPressed(int32_t button) const;
    
    /**
     * @brief Verifica se um botão do mouse foi liberado neste quadro
     * @param button Botão
     * @return true se liberado neste quadro
     */
    bool IsMouseButtonReleased(int32_t button) const;
    
    /**
     * @brief Obtém a posição do mouse
     * @param x Posição X (saída)
     * @param y Posição Y (saída)
     */
    void GetMousePosition(int32_t& x, int32_t& y) const;
    
    /**
     * @brief Obtém o movimento do mouse
     * @param deltaX Delta X (saída)
     * @param deltaY Delta Y (saída)
     */
    void GetMouseDelta(int32_t& deltaX, int32_t& deltaY) const;
    
    /**
     * @brief Obtém o valor de um input mapeado
     * @param name Nome
     * @return Valor, ou 0.0f se não encontrado
     */
    float GetMappedInputValue(const std::string& name) const;
    
    /**
     * @brief Processa um evento de teclado
     * @param event Evento
     */
    void ProcessKeyboardEvent(const KeyboardEvent& event);
    
    /**
     * @brief Processa um evento de mouse
     * @param event Evento
     */
    void ProcessMouseEvent(const MouseEvent& event);
    
    /**
     * @brief Registra um callback para input mapeado
     * @param name Nome do mapeamento
     * @param callback Callback
     * @param valueThreshold Limiar de valor
     * @param callOnRepeat Chamar em repetição
     * @return ID do callback, ou 0 se falhar
     */
    uint32_t RegisterInputCallback(const std::string& name, InputCallback callback, float valueThreshold = 0.5f, bool callOnRepeat = false);
    
    /**
     * @brief Remove um callback de input
     * @param callbackId ID do callback
     * @return true se removido com sucesso
     */
    bool UnregisterInputCallback(uint32_t callbackId);
    
    /**
     * @brief Limpa todo o estado de input
     */
    void ClearState();
    
private:
    /**
     * @brief Construtor privado (singleton)
     */
    InputManager();
    
    /**
     * @brief Destrutor
     */
    ~InputManager();
    
    /**
     * @brief Desabilita o construtor de cópia
     */
    InputManager(const InputManager&) = delete;
    
    /**
     * @brief Desabilita o operador de atribuição
     */
    InputManager& operator=(const InputManager&) = delete;
    
    /**
     * @brief Notifica os callbacks de input
     * @param name Nome do mapeamento
     * @param value Valor
     */
    void NotifyInputCallbacks(const std::string& name, float value);
    
    /**
     * @brief Atualiza o estado de input mapeado
     * @param name Nome do mapeamento
     * @param value Valor
     */
    void UpdateMappedInput(const std::string& name, float value);
    
    // Modo de input
    InputMode inputMode_;
    
    // Estado das teclas
    std::bitset<256> keyState_;
    std::bitset<256> keyPressedState_;
    std::bitset<256> keyReleasedState_;
    
    // Estado do mouse
    std::bitset<8> mouseButtonState_;
    std::bitset<8> mouseButtonPressedState_;
    std::bitset<8> mouseButtonReleasedState_;
    int32_t mouseX_;
    int32_t mouseY_;
    int32_t mouseDeltaX_;
    int32_t mouseDeltaY_;
    
    // Mapeamentos de input
    std::unordered_map<std::string, InputMapping> inputMappings_;
    
    // Valores de input mapeado
    std::unordered_map<std::string, float> mappedInputValues_;
    
    // Callbacks de input
    struct InputCallbackInfo {
        InputCallback callback;
        float valueThreshold;
        bool callOnRepeat;
        bool lastCallState;
    };
    
    std::unordered_map<std::string, std::unordered_map<uint32_t, InputCallbackInfo>> inputCallbacks_;
    
    // Contador de IDs de callbacks
    uint32_t nextCallbackId_;
    
    // Mutex para os mapeamentos
    std::mutex mappingMutex_;
    
    // Mutex para os callbacks
    std::mutex callbackMutex_;
    
    // Estado
    bool initialized_;
};

/**
 * @brief Gerenciador de recursos
 */
/**
 * Classe WYDResourceManager
 * 
 * Esta classe implementa a funcionalidade WYDResourceManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class ResourceManager {
public:
    /**
     * @brief Obtém a instância singleton
     * @return Referência para a instância
     */
    static ResourceManager& GetInstance();
    
    /**
     * @brief Inicializa o gerenciador
     * @param basePath Caminho base
     * @return true se inicializado com sucesso
     */
    bool Initialize(const std::string& basePath);
    
    /**
     * @brief Finaliza o gerenciador
     */
    void Shutdown();
    
    /**
     * @brief Carrega um recurso
     * @param name Nome
     * @param type Tipo
     * @param path Caminho (relativo ao caminho base)
     * @param priority Prioridade
     * @return ID do recurso, ou 0 se falhar
     */
    uint32_t LoadResource(const std::string& name, ResourceType type, const std::string& path, MemoryPriority priority = MemoryPriority::NORMAL);
    
    /**
     * @brief Descarrega um recurso
     * @param resourceId ID do recurso
     * @return true se descarregado com sucesso
     */
    bool UnloadResource(uint32_t resourceId);
    
    /**
     * @brief Obtém um recurso pelo ID
     * @param resourceId ID do recurso
     * @return Ponteiro para o recurso, ou nullptr se não encontrado
     */
    const Resource* GetResource(uint32_t resourceId) const;
    
    /**
     * @brief Obtém um recurso pelo nome
     * @param name Nome
     * @return Ponteiro para o recurso, ou nullptr se não encontrado
     */
    const Resource* GetResourceByName(const std::string& name) const;
    
    /**
     * @brief Adiciona uma referência a um recurso
     * @param resourceId ID do recurso
     * @return Contagem de referências atualizada, ou 0 se falhar
     */
    uint32_t AddResourceReference(uint32_t resourceId);
    
    /**
     * @brief Remove uma referência de um recurso
     * @param resourceId ID do recurso
     * @return Contagem de referências atualizada, ou 0 se falhar
     */
    uint32_t RemoveResourceReference(uint32_t resourceId);
    
    /**
     * @brief Define a prioridade de um recurso
     * @param resourceId ID do recurso
     * @param priority Prioridade
     * @return true se definido com sucesso
     */
    bool SetResourcePriority(uint32_t resourceId, MemoryPriority priority);
    
    /**
     * @brief Recarrega um recurso
     * @param resourceId ID do recurso
     * @return true se recarregado com sucesso
     */
    bool ReloadResource(uint32_t resourceId);
    
    /**
     * @brief Gerencia a memória (descarrega recursos de baixa prioridade se necessário)
     * @param targetMemory Memória alvo (bytes)
     * @return true se a memória está abaixo do alvo
     */
    bool ManageMemory(uint32_t targetMemory);
    
    /**
     * @brief Obtém o uso de memória total
     * @return Uso de memória (bytes)
     */
    uint32_t GetTotalMemoryUsage() const;
    
    /**
     * @brief Obtém a contagem de recursos
     * @return Contagem
     */
    uint32_t GetResourceCount() const;
    
    /**
     * @brief Obtém a contagem de recursos carregados
     * @return Contagem
     */
    uint32_t GetLoadedResourceCount() const;
    
    /**
     * @brief Obtém o caminho base
     * @return Caminho base
     */
    const std::string& GetBasePath() const;
    
private:
    /**
     * @brief Construtor privado (singleton)
     */
    ResourceManager();
    
    /**
     * @brief Destrutor
     */
    ~ResourceManager();
    
    /**
     * @brief Desabilita o construtor de cópia
     */
    ResourceManager(const ResourceManager&) = delete;
    
    /**
     * @brief Desabilita o operador de atribuição
     */
    ResourceManager& operator=(const ResourceManager&) = delete;
    
    /**
     * @brief Carrega um recurso de textura
     * @param resource Recurso
     * @return true se carregado com sucesso
     */
    bool LoadTextureResource(Resource& resource);
    
    /**
     * @brief Carrega um recurso de malha
     * @param resource Recurso
     * @return true se carregado com sucesso
     */
    bool LoadMeshResource(Resource& resource);
    
    /**
     * @brief Carrega um recurso de som
     * @param resource Recurso
     * @return true se carregado com sucesso
     */
    bool LoadSoundResource(Resource& resource);
    
    /**
     * @brief Carrega um recurso de música
     * @param resource Recurso
     * @return true se carregado com sucesso
     */
    bool LoadMusicResource(Resource& resource);
    
    /**
     * @brief Carrega um recurso de script
     * @param resource Recurso
     * @return true se carregado com sucesso
     */
    bool LoadScriptResource(Resource& resource);
    
    /**
     * @brief Carrega um recurso de fonte
     * @param resource Recurso
     * @return true se carregado com sucesso
     */
    bool LoadFontResource(Resource& resource);
    
    /**
     * @brief Carrega um recurso de shader
     * @param resource Recurso
     * @return true se carregado com sucesso
     */
    bool LoadShaderResource(Resource& resource);
    
    /**
     * @brief Carrega um recurso de efeito
     * @param resource Recurso
     * @return true se carregado com sucesso
     */
    bool LoadEffectResource(Resource& resource);
    
    /**
     * @brief Carrega um recurso de UI
     * @param resource Recurso
     * @return true se carregado com sucesso
     */
    bool LoadUIResource(Resource& resource);
    
    /**
     * @brief Carrega um recurso de mapa
     * @param resource Recurso
     * @return true se carregado com sucesso
     */
    bool LoadMapResource(Resource& resource);
    
    /**
     * @brief Carrega um recurso binário
     * @param resource Recurso
     * @return true se carregado com sucesso
     */
    bool LoadBinaryResource(Resource& resource);
    
    /**
     * @brief Descarrega um recurso de textura
     * @param resource Recurso
     * @return true se descarregado com sucesso
     */
    bool UnloadTextureResource(Resource& resource);
    
    /**
     * @brief Descarrega um recurso de malha
     * @param resource Recurso
     * @return true se descarregado com sucesso
     */
    bool UnloadMeshResource(Resource& resource);
    
    /**
     * @brief Descarrega um recurso de som
     * @param resource Recurso
     * @return true se descarregado com sucesso
     */
    bool UnloadSoundResource(Resource& resource);
    
    /**
     * @brief Descarrega um recurso de música
     * @param resource Recurso
     * @return true se descarregado com sucesso
     */
    bool UnloadMusicResource(Resource& resource);
    
    /**
     * @brief Descarrega um recurso de script
     * @param resource Recurso
     * @return true se descarregado com sucesso
     */
    bool UnloadScriptResource(Resource& resource);
    
    /**
     * @brief Descarrega um recurso de fonte
     * @param resource Recurso
     * @return true se descarregado com sucesso
     */
    bool UnloadFontResource(Resource& resource);
    
    /**
     * @brief Descarrega um recurso de shader
     * @param resource Recurso
     * @return true se descarregado com sucesso
     */
    bool UnloadShaderResource(Resource& resource);
    
    /**
     * @brief Descarrega um recurso de efeito
     * @param resource Recurso
     * @return true se descarregado com sucesso
     */
    bool UnloadEffectResource(Resource& resource);
    
    /**
     * @brief Descarrega um recurso de UI
     * @param resource Recurso
     * @return true se descarregado com sucesso
     */
    bool UnloadUIResource(Resource& resource);
    
    /**
     * @brief Descarrega um recurso de mapa
     * @param resource Recurso
     * @return true se descarregado com sucesso
     */
    bool UnloadMapResource(Resource& resource);
    
    /**
     * @brief Descarrega um recurso binário
     * @param resource Recurso
     * @return true se descarregado com sucesso
     */
    bool UnloadBinaryResource(Resource& resource);
    
    // Caminho base
    std::string basePath_;
    
    // Recursos
    std::unordered_map<uint32_t, Resource> resources_;
    
    // Mapa de nome para ID
    std::unordered_map<std::string, uint32_t> resourceNames_;
    
    // Mutex para os recursos
    std::mutex resourceMutex_;
    
    // Contador de IDs de recursos
    uint32_t nextResourceId_;
    
    // Estado
    bool initialized_;
};

/**
 * @brief Gerenciador de memória
 */
/**
 * Classe WYDMemoryManager
 * 
 * Esta classe implementa a funcionalidade WYDMemoryManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class MemoryManager {
public:
    /**
     * @brief Obtém a instância singleton
     * @return Referência para a instância
     */
    static MemoryManager& GetInstance();
    
    /**
     * @brief Inicializa o gerenciador
     * @return true se inicializado com sucesso
     */
    bool Initialize();
    
    /**
     * @brief Finaliza o gerenciador
     */
    void Shutdown();
    
    /**
     * @brief Cria uma pool de memória
     * @param name Nome
     * @param blockSize Tamanho do bloco
     * @param blockCount Contagem de blocos
     * @return true se criado com sucesso
     */
    bool CreateMemoryPool(const std::string& name, uint32_t blockSize, uint32_t blockCount);
    
    /**
     * @brief Destrói uma pool de memória
     * @param name Nome
     * @return true se destruído com sucesso
     */
    bool DestroyMemoryPool(const std::string& name);
    
    /**
     * @brief Aloca memória de uma pool
     * @param poolName Nome da pool
     * @return Ponteiro para a memória alocada, ou nullptr se falhar
     */
    void* AllocFromPool(const std::string& poolName);
    
    /**
     * @brief Libera memória de uma pool
     * @param poolName Nome da pool
     * @param ptr Ponteiro para a memória
     * @return true se liberado com sucesso
     */
    bool FreeToPool(const std::string& poolName, void* ptr);
    
    /**
     * @brief Aloca memória
     * @param size Tamanho
     * @return Ponteiro para a memória alocada, ou nullptr se falhar
     */
    void* Alloc(uint32_t size);
    
    /**
     * @brief Libera memória
     * @param ptr Ponteiro para a memória
     */
    void Free(void* ptr);
    
    /**
     * @brief Obtém o uso de memória total
     * @return Uso de memória (bytes)
     */
    uint32_t GetTotalMemoryUsage() const;
    
    /**
     * @brief Obtém o uso de memória de pools
     * @return Uso de memória (bytes)
     */
    uint32_t GetPoolMemoryUsage() const;
    
    /**
     * @brief Obtém o uso de memória de alocações gerais
     * @return Uso de memória (bytes)
     */
    uint32_t GetGeneralMemoryUsage() const;
    
    /**
     * @brief Obtém informações de uma pool de memória
     * @param name Nome
     * @param blockSize Tamanho do bloco (saída)
     * @param blockCount Contagem de blocos (saída)
     * @param usedBlocks Blocos usados (saída)
     * @return true se a pool existe
     */
    bool GetPoolInfo(const std::string& name, uint32_t& blockSize, uint32_t& blockCount, uint32_t& usedBlocks) const;
    
private:
    /**
     * @brief Construtor privado (singleton)
     */
    MemoryManager();
    
    /**
     * @brief Destrutor
     */
    ~MemoryManager();
    
    /**
     * @brief Desabilita o construtor de cópia
     */
    MemoryManager(const MemoryManager&) = delete;
    
    /**
     * @brief Desabilita o operador de atribuição
     */
    MemoryManager& operator=(const MemoryManager&) = delete;
    
    // Pools de memória
    std::unordered_map<std::string, MemoryPool> memoryPools_;
    
    // Mutex para as pools
    std::mutex poolMutex_;
    
    // Uso de memória geral
    std::atomic<uint32_t> generalMemoryUsage_;
    
    // Estado
    bool initialized_;
};

/**
 * @brief Sistema de cliente
 */
/**
 * Classe WYDClientSystem
 * 
 * Esta classe implementa a funcionalidade WYDClientSystem conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class ClientSystem {
public:
    /**
     * @brief Obtém a instância singleton
     * @return Referência para a instância
     */
    static ClientSystem& GetInstance();
    
    /**
     * @brief Inicializa o sistema
     * @param config Configuração
     * @return true se inicializado com sucesso
     */
    bool Initialize(const ClientConfig& config = ClientConfig());
    
    /**
     * @brief Finaliza o sistema
     */
    void Shutdown();
    
    /**
     * @brief Executa o loop principal
     * @return Código de saída
     */
    int Run();
    
    /**
     * @brief Solicita a saída do loop principal
     */
    void Exit();
    
    /**
     * @brief Conecta ao servidor
     * @param host Endereço
     * @param port Porta
     * @return true se conectado com sucesso
     */
    bool Connect(const std::string& host, uint16_t port);
    
    /**
     * @brief Desconecta do servidor
     */
    void Disconnect();
    
    /**
     * @brief Verifica se está conectado
     * @return true se conectado
     */
    bool IsConnected() const;
    
    /**
     * @brief Envia um pacote
     * @param packetType Tipo de pacote
     * @param data Dados
     * @param size Tamanho
     * @return true se enviado com sucesso
     */
    bool SendPacket(network::PacketType packetType, const void* data, uint16_t size);
    
    /**
     * @brief Cria uma entidade
     * @param type Tipo
     * @param name Nome
     * @param modelId ID do modelo
     * @param textureId ID da textura
     * @param x Posição X
     * @param y Posição Y
     * @param z Posição Z
     * @return ID da entidade, ou 0 se falhar
     */
    uint32_t CreateEntity(EntityType type, const std::string& name, uint32_t modelId, uint32_t textureId, float x, float y, float z);
    
    /**
     * @brief Destrói uma entidade
     * @param entityId ID da entidade
     * @return true se destruído com sucesso
     */
    bool DestroyEntity(uint32_t entityId);
    
    /**
     * @brief Obtém uma entidade
     * @param entityId ID da entidade
     * @return Ponteiro para a entidade, ou nullptr se não encontrada
     */
    const Entity* GetEntity(uint32_t entityId) const;
    
    /**
     * @brief Atualiza a posição de uma entidade
     * @param entityId ID da entidade
     * @param x Posição X
     * @param y Posição Y
     * @param z Posição Z
     * @return true se atualizado com sucesso
     */
    bool UpdateEntityPosition(uint32_t entityId, float x, float y, float z);
    
    /**
     * @brief Atualiza a rotação de uma entidade
     * @param entityId ID da entidade
     * @param rotationX Rotação X
     * @param rotationY Rotação Y
     * @param rotationZ Rotação Z
     * @return true se atualizado com sucesso
     */
    bool UpdateEntityRotation(uint32_t entityId, float rotationX, float rotationY, float rotationZ);
    
    /**
     * @brief Atualiza a escala de uma entidade
     * @param entityId ID da entidade
     * @param scaleX Escala X
     * @param scaleY Escala Y
     * @param scaleZ Escala Z
     * @return true se atualizado com sucesso
     */
    bool UpdateEntityScale(uint32_t entityId, float scaleX, float scaleY, float scaleZ);
    
    /**
     * @brief Define a visibilidade de uma entidade
     * @param entityId ID da entidade
     * @param visible Visível
     * @return true se definido com sucesso
     */
    bool SetEntityVisibility(uint32_t entityId, bool visible);
    
    /**
     * @brief Define o personagem do jogador
     * @param characterId ID do personagem
     * @return true se definido com sucesso
     */
    bool SetPlayerCharacter(uint32_t characterId);
    
    /**
     * @brief Obtém o personagem do jogador
     * @return Ponteiro para o personagem, ou nullptr se não definido
     */
    const Character* GetPlayerCharacter() const;
    
    /**
     * @brief Obtém o ID do personagem do jogador
     * @return ID do personagem, ou 0 se não definido
     */
    uint32_t GetPlayerCharacterId() const;
    
    /**
     * @brief Obtém a posição do jogador
     * @param x Posição X (saída)
     * @param y Posição Y (saída)
     * @param z Posição Z (saída)
     * @return true se o jogador está definido
     */
    bool GetPlayerPosition(float& x, float& y, float& z) const;
    
    /**
     * @brief Obtém a entidade do jogador
     * @return Ponteiro para a entidade, ou nullptr se não definido
     */
    const Entity* GetPlayerEntity() const;
    
    /**
     * @brief Adiciona uma mensagem de chat
     * @param senderId ID do remetente
     * @param senderName Nome do remetente
     * @param message Mensagem
     * @param channel Canal
     * @return true se adicionado com sucesso
     */
    bool AddChatMessage(uint32_t senderId, const std::string& senderName, const std::string& message, uint32_t channel);
    
    /**
     * @brief Obtém mensagens de chat
     * @param channel Canal (0 para todos)
     * @param count Contagem máxima
     * @return Lista de mensagens
     */
    std::vector<ChatMessage> GetChatMessages(uint32_t channel = 0, uint32_t count = MAX_CHAT_MESSAGES) const;
    
    /**
     * @brief Cria um elemento de UI
     * @param name Nome
     * @param x Posição X
     * @param y Posição Y
     * @param width Largura
     * @param height Altura
     * @param parentId ID do pai (0 para nenhum)
     * @return ID do elemento, ou 0 se falhar
     */
    uint32_t CreateUIElement(const std::string& name, int32_t x, int32_t y, int32_t width, int32_t height, uint32_t parentId = 0);
    
    /**
     * @brief Destrói um elemento de UI
     * @param elementId ID do elemento
     * @return true se destruído com sucesso
     */
    bool DestroyUIElement(uint32_t elementId);
    
    /**
     * @brief Obtém um elemento de UI
     * @param elementId ID do elemento
     * @return Ponteiro para o elemento, ou nullptr se não encontrado
     */
    const UIElement* GetUIElement(uint32_t elementId) const;
    
    /**
     * @brief Obtém um elemento de UI pelo nome
     * @param name Nome
     * @return Ponteiro para o elemento, ou nullptr se não encontrado
     */
    const UIElement* GetUIElementByName(const std::string& name) const;
    
    /**
     * @brief Define a posição de um elemento de UI
     * @param elementId ID do elemento
     * @param x Posição X
     * @param y Posição Y
     * @return true se definido com sucesso
     */
    bool SetUIElementPosition(uint32_t elementId, int32_t x, int32_t y);
    
    /**
     * @brief Define o tamanho de um elemento de UI
     * @param elementId ID do elemento
     * @param width Largura
     * @param height Altura
     * @return true se definido com sucesso
     */
    bool SetUIElementSize(uint32_t elementId, int32_t width, int32_t height);
    
    /**
     * @brief Define a visibilidade de um elemento de UI
     * @param elementId ID do elemento
     * @param visible Visível
     * @return true se definido com sucesso
     */
    bool SetUIElementVisibility(uint32_t elementId, bool visible);
    
    /**
     * @brief Define o texto de um elemento de UI
     * @param elementId ID do elemento
     * @param text Texto
     * @return true se definido com sucesso
     */
    bool SetUIElementText(uint32_t elementId, const std::string& text);
    
    /**
     * @brief Define a textura de um elemento de UI
     * @param elementId ID do elemento
     * @param textureId ID da textura
     * @return true se definido com sucesso
     */
    bool SetUIElementTexture(uint32_t elementId, uint32_t textureId);
    
    /**
     * @brief Define o estado do jogo
     * @param state Estado
     */
    void SetGameState(GameState state);
    
    /**
     * @brief Obtém o estado do jogo
     * @return Estado
     */
    GameState GetGameState() const;
    
    /**
     * @brief Obtém o EventManager
     * @return Referência para o EventManager
     */
    EventManager& GetEventManager();
    
    /**
     * @brief Obtém o InputManager
     * @return Referência para o InputManager
     */
    InputManager& GetInputManager();
    
    /**
     * @brief Obtém o ResourceManager
     * @return Referência para o ResourceManager
     */
    ResourceManager& GetResourceManager();
    
    /**
     * @brief Obtém o MemoryManager
     * @return Referência para o MemoryManager
     */
    MemoryManager& GetMemoryManager();
    
    /**
     * @brief Obtém o RenderingSystem
     * @return Referência para o RenderingSystem
     */
    renderer::RenderingSystem& GetRenderingSystem();
    
    /**
     * @brief Obtém o NetworkProtocol
     * @return Referência para o NetworkProtocol
     */
    network::NetworkProtocol& GetNetworkProtocol();
    
    /**
     * @brief Obtém o ConnectionManager
     * @return Referência para o ConnectionManager
     */
    network::ConnectionManager& GetConnectionManager();
    
    /**
     * @brief Obtém as estatísticas do cliente
     * @return Estatísticas
     */
    const ClientStats& GetStats() const;
    
    /**
     * @brief Obtém a configuração do cliente
     * @return Configuração
     */
    const ClientConfig& GetConfig() const;
    
    /**
     * @brief Carrega um item
     * @param itemId ID do item
     * @return Ponteiro para o item, ou nullptr se não encontrado
     */
    const Item* LoadItem(uint32_t itemId);
    
    /**
     * @brief Verifica se o sistema está inicializado
     * @return true se inicializado
     */
    bool IsInitialized() const;
    
private:
    /**
     * @brief Construtor privado (singleton)
     */
    ClientSystem();
    
    /**
     * @brief Destrutor
     */
    ~ClientSystem();
    
    /**
     * @brief Desabilita o construtor de cópia
     */
    ClientSystem(const ClientSystem&) = delete;
    
    /**
     * @brief Desabilita o operador de atribuição
     */
    ClientSystem& operator=(const ClientSystem&) = delete;
    
    /**
     * @brief Atualiza um quadro
     * @param deltaTime Tempo delta
     */
    void UpdateFrame(float deltaTime);
    
    /**
     * @brief Renderiza um quadro
     */
    void RenderFrame();
    
    /**
     * @brief Processa pacotes de rede
     */
    void ProcessNetworkPackets();
    
    /**
     * @brief Processa um pacote de rede
     * @param packet Pacote
     */
    void ProcessPacket(const network::Packet& packet);
    
    /**
     * @brief Atualiza as estatísticas
     * @param deltaTime Tempo delta
     */
    void UpdateStats(float deltaTime);
    
    /**
     * @brief Atualiza o gerenciamento de memória
     */
    void UpdateMemory();
    
    /**
     * @brief Atualiza as entidades
     * @param deltaTime Tempo delta
     */
    void UpdateEntities(float deltaTime);
    
    /**
     * @brief Atualiza a UI
     * @param deltaTime Tempo delta
     */
    void UpdateUI(float deltaTime);
    
    /**
     * @brief Carrega configuração
     * @param filePath Caminho do arquivo
     * @return true se carregado com sucesso
     */
    bool LoadConfig(const std::string& filePath);
    
    /**
     * @brief Salva configuração
     * @param filePath Caminho do arquivo
     * @return true se salvo com sucesso
     */
    bool SaveConfig(const std::string& filePath);
    
    /**
     * @brief Carrega os personagens
     * @return true se carregado com sucesso
     */
    bool LoadCharacters();
    
    /**
     * @brief Salva os personagens
     * @return true se salvo com sucesso
     */
    bool SaveCharacters();
    
    /**
     * @brief Carrega os mapeamentos de input
     * @return true se carregado com sucesso
     */
    bool LoadInputMappings();
    
    /**
     * @brief Inicializa a UI
     * @return true se inicializado com sucesso
     */
    bool InitializeUI();
    
    // Configuração
    ClientConfig config_;
    
    // Estatísticas
    ClientStats stats_;
    
    // Estado do jogo
    GameState gameState_;
    
    // Loop principal
    bool running_;
    
    // Personagens
    std::unordered_map<uint32_t, Character> characters_;
    
    // ID do personagem do jogador
    uint32_t playerCharacterId_;
    
    // Entidades
    std::unordered_map<uint32_t, Entity> entities_;
    
    // Elementos de UI
    std::unordered_map<uint32_t, UIElement> uiElements_;
    
    // Mapa de nome para ID de UI
    std::unordered_map<std::string, uint32_t> uiElementNames_;
    
    // Mensagens de chat
    std::vector<ChatMessage> chatMessages_;
    
    // Itens
    std::unordered_map<uint32_t, Item> items_;
    
    // Mutex para personagens
    std::mutex charactersMutex_;
    
    // Mutex para entidades
    std::mutex entitiesMutex_;
    
    // Mutex para UI
    std::mutex uiMutex_;
    
    // Mutex para chat
    std::mutex chatMutex_;
    
    // Mutex para itens
    std::mutex itemsMutex_;
    
    // Contadores de ID
    uint32_t nextEntityId_;
    uint32_t nextUIElementId_;
    
    // Estado
    bool initialized_;
    
    // Tempo
    uint64_t lastFrameTime_;
    float deltaTime_;
};

// Acesso global
#define g_ClientSystem ClientSystem::GetInstance()
#define g_EventManager EventManager::GetInstance()
#define g_InputManager InputManager::GetInstance()
#define g_ResourceManager ResourceManager::GetInstance()
#define g_MemoryManager MemoryManager::GetInstance()

} // namespace client
} // namespace reverse
} // namespace wyd

#endif // CLIENTCORE_H

} // namespace wydbr
