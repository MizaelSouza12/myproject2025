/**
 * PacketTypes.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/network/PacketTypes.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef PACKETTYPES_H
#define PACKETTYPES_H

#include "../core/WYDTypes.h"
#include <vector>
#include <cstring>

namespace wydbr {


/**
 * @file PacketTypes.h
 * @brief Definição dos tipos de pacotes e estruturas do protocolo de rede do WYD
 * 
 * Este arquivo contém as definições das estruturas de pacotes e códigos de comando
 * usados na comunicação entre cliente e servidor. Segue a estrutura original do WYD,
 * mas com melhorias para evitar bugs comuns e garantir compatibilidade.
 */

namespace wyd {
namespace network {

// Tamanhos máximos e constantes
#define MAX_PACKET_SIZE 1024        // Tamanho máximo de um pacote (original: 512, aumentado para melhor suporte)
#define HEADER_SIZE 12              // Tamanho do cabeçalho de pacote (12 bytes no WYD original)
#define MAX_PACKET_ENCRYPTION 2048  // Tamanho máximo para buffer de encriptação
#define MAX_PACKET_DATA (MAX_PACKET_SIZE - HEADER_SIZE) // Tamanho máximo de dados em um pacote

/**
 * @brief Enumeração dos tipos de comando do pacote (original: COMMAND_TYPE)
 * 
 * Estes são os códigos de comando usados no WYD original para identificar
 * o tipo de pacote na comunicação cliente-servidor.
 */
enum COMMAND_TYPE : WORD {
    // Pacotes de conexão e autenticação
    PKT_INITIAL               = 0x00FB,   // Pacote inicial do cliente
    PKT_INIT_GAME             = 0x00FC,   // Inicializar jogo
    PKT_KEYCHANGE             = 0x00FD,   // Troca de chave de criptografia
    PKT_ACCOUNT_LOGIN         = 0x0FA1,   // Login de conta
    PKT_NUMERIC_PASSWORD      = 0x0FA2,   // Senha numérica (PIN)
    PKT_ACCOUNT_LOGIN_RESULT  = 0x0FA3,   // Resultado do login
    PKT_CHAR_CREATE           = 0x0FE7,   // Criar personagem
    PKT_CHAR_DELETE           = 0x0FE8,   // Deletar personagem
    PKT_CHAR_LIST             = 0x0FE9,   // Lista de personagens
    PKT_CHAR_SELECT           = 0x0FEA,   // Selecionar personagem
    
    // Pacotes de movimentação e mapa
    PKT_TELEPORT              = 0x0FB4,   // Teleporte
    PKT_TELEPORT_REPLY        = 0x0FB5,   // Resposta de teleporte
    PKT_WALK                  = 0x0FB6,   // Andar
    PKT_ATTACK                = 0x0FB7,   // Atacar
    PKT_ATTACK_ARROW          = 0x0FB8,   // Atacar com flecha
    PKT_GET_ITEM              = 0x0F54,   // Pegar item do chão
    
    // Pacotes de chat e mensagens
    PKT_CHAT                  = 0x0F31,   // Chat normal
    PKT_WHISPER               = 0x0F32,   // Sussurro (whisper)
    PKT_SHOUT                 = 0x0F33,   // Grito (shout)
    PKT_ANNOUNCE              = 0x0F34,   // Anúncio global
    
    // Pacotes de atributos e status
    PKT_REQ_STAT              = 0x0F05,   // Requisitar atributos
    PKT_SET_STAT              = 0x0F07,   // Definir atributos
    PKT_LEVELUP               = 0x0F0B,   // Subir de nível
    PKT_SKILLUP               = 0x0F0C,   // Aumentar skill
    PKT_UPDATE_SCORE          = 0x0F09,   // Atualizar pontuação/status
    PKT_ACTION                = 0x0F15,   // Ação (sentar, levantar, etc)
    
    // Pacotes de itens e equipamentos
    PKT_ITEM_PLACE            = 0x0F55,   // Posicionar item no inventário
    PKT_ITEM_REMOVE           = 0x0F56,   // Remover item do inventário
    PKT_ITEM_DROP             = 0x0F57,   // Dropar item no chão
    PKT_ITEM_EQUIP            = 0x0F6A,   // Equipar item
    PKT_ITEM_USE              = 0x0F88,   // Usar item
    PKT_ITEM_BUY              = 0x0F66,   // Comprar item
    PKT_ITEM_SELL             = 0x0F67,   // Vender item
    PKT_DECAY_TIME            = 0x0F6F,   // Tempo de decaimento do item
    
    // Pacotes de skill e combate
    PKT_CAST_SPELL            = 0x0F41,   // Lançar magia/skill
    PKT_CAST_FAIL             = 0x0F42,   // Falha ao lançar magia/skill
    PKT_CAST_RECLAIM          = 0x0F43,   // Reclamar cast
    PKT_APPLY_AFFECT          = 0x0F44,   // Aplicar afetador/efeito
    PKT_REMOVE_AFFECT         = 0x0F45,   // Remover afetador/efeito
    
    // Pacotes de guildas
    PKT_GUILD_CREATE          = 0x0F71,   // Criar guilda
    PKT_GUILD_INFO            = 0x0F72,   // Informação de guilda
    PKT_GUILD_INVITE          = 0x0F73,   // Convidar para guilda
    PKT_GUILD_MEMBER_LIST     = 0x0F74,   // Lista de membros da guilda
    PKT_GUILD_DELETE          = 0x0F75,   // Deletar guilda
    PKT_GUILD_DISBAND         = 0x0F76,   // Disbanda guilda
    
    // Pacotes de comércio
    PKT_TRADE_REQUEST         = 0x0F6E,   // Solicitar comércio
    PKT_TRADE_REPLY           = 0x0F70,   // Responder comércio
    PKT_TRADE_ADD             = 0x0F71,   // Adicionar item ao comércio
    PKT_TRADE_DEL             = 0x0F72,   // Remover item do comércio
    PKT_TRADE_GOLD            = 0x0F73,   // Adicionar gold ao comércio
    PKT_TRADE_CONFIRM         = 0x0F74,   // Confirmar comércio
    PKT_TRADE_CANCEL          = 0x0F75,   // Cancelar comércio
    
    // Pacotes de PK
    PKT_PK_MODE               = 0x0F91,   // Modo PK
    PKT_PK_PENALTY            = 0x0F92,   // Penalidade PK
    PKT_PK_ITEM_DROP          = 0x0F93,   // Item dropado por PK
    
    // Pacotes de eventos de mundo/especiais
    PKT_WEATHER               = 0x0F9F,   // Clima
    PKT_TIME                  = 0x0FA0,   // Tempo no jogo
    PKT_EVENT                 = 0x0F26,   // Evento especial
    PKT_QUEST                 = 0x0FAF,   // Quest/missão
    
    // Pacotes administrativos
    PKT_GM_COMMAND            = 0x0F95,   // Comando de GM
    PKT_SERVER_CONTROL        = 0x0F96,   // Controle de servidor
    PKT_SERVER_DATA           = 0x0F97,   // Dados de servidor
    
    // Pacotes especiais/sistema
    PKT_KEEPALIVE             = 0x0FFF,   // Keep-alive/heartbeat
    PKT_DISCONNECT            = 0x00FF,   // Desconectar
    PKT_CRYPTION_INIT         = 0x00FA,   // Inicialização de criptografia
};

/**
 * @brief Enumeração dos resultados de login (original: LOGIN_RESULT)
 * 
 * Estes são os códigos de resultado usados no WYD original para informar
 * o resultado do login.
 */
enum LOGIN_RESULT : BYTE {
    LOGIN_SUCCESS             = 0,    // Login bem-sucedido
    LOGIN_INVALID_ID          = 1,    // ID inválido
    LOGIN_INVALID_PW          = 2,    // Senha inválida
    LOGIN_ALREADY_CONNECTED   = 3,    // Já conectado
    LOGIN_TOO_MANY_CONNECTION = 4,    // Muitas conexões
    LOGIN_INVALID_VERSION     = 5,    // Versão inválida
    LOGIN_BANNED              = 6,    // Conta banida
    LOGIN_MAINTENANCE         = 7,    // Servidor em manutenção
    LOGIN_FULL                = 8,    // Servidor cheio
    LOGIN_ACCOUNT_EXPIRED     = 9,    // Conta expirada
    LOGIN_TIMEOUT             = 10,   // Tempo esgotado
    LOGIN_NEED_CHARGE         = 11,   // Precisa recarregar (para sistemas de pagamento)
    LOGIN_SYSTEM_ERROR        = 12,   // Erro de sistema
    LOGIN_IP_BANNED           = 13,   // IP banido
    LOGIN_INVALID_PIN         = 14,   // PIN inválido
    LOGIN_BLOCK_COUNTRY       = 15,   // País bloqueado
};

/**
 * @brief Estrutura de cabeçalho de pacote do WYD (original: PACKET_HEADER)
 * 
 * Esta estrutura representa o cabeçalho de 12 bytes usado em todos os pacotes do WYD.
 * No original, esta estrutura tinha problemas de alinhamento e packing que causavam bugs.
 */
#pragma pack(push, 1) // Força alinhamento de 1 byte para compatibilidade com a estrutura original
struct STRUCT_PACKET_HEADER {
    WORD wSize;        // Tamanho total do pacote (cabeçalho + dados)
    WORD wCommand;     // Código do comando (tipo de pacote)
    DWORD dwClientKey; // Chave do cliente (ID único de sessão)
    DWORD dwChecksum;  // Checksum para verificação de integridade
    
    // Construtor com inicialização segura
    STRUCT_PACKET_HEADER() 
        : wSize(HEADER_SIZE)
        , wCommand(0)
        , dwClientKey(0)
        , dwChecksum(0)
    {
    }
    
    // Construtor com parâmetros
    STRUCT_PACKET_HEADER(WORD command, DWORD clientKey = 0)
        : wSize(HEADER_SIZE)
        , wCommand(command)
        , dwClientKey(clientKey)
        , dwChecksum(0)
    {
    }
    
    // Inicializa o cabeçalho
    /**
 * Initialize
 * 
 * Implementa a funcionalidade Initialize conforme especificação original.
 * @param command Parâmetro command
 * @param 0 Parâmetro 0
 */

    void Initialize(WORD command, DWORD clientKey = 0){
        wSize = HEADER_SIZE;
        wCommand = command;
        dwClientKey = clientKey;
        dwChecksum = 0;
    }
    
    // Calcula o checksum
    /**
 * CalculateChecksum
 * 
 * Implementa a funcionalidade CalculateChecksum conforme especificação original.
 * @param packetData Parâmetro packetData
 * @param packetSize Parâmetro packetSize
 */

    void CalculateChecksum(const BYTE* packetData, WORD packetSize){
        // Implementação original do WYD para checksum (simplificada)
        dwChecksum = 0;
        
        // Se não tiver dados adicionais, usa só o cabeçalho para /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param !packetData Parâmetro !packetData
 * @return Retorna calcular
 */
 calcular
        if(packetSize <= HEADER_SIZE || !packetData){
            for (int i = 0; i < 8; i++) {
                BYTE* ptr = (BYTE*)this;
                dwChecksum += ptr[i];
            }
            return;
        }
        
        // Se tiver dados adicionais, inclui no cá/**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param i++ Parâmetro i++
 * @return Retorna lculo
 */
lculo
        for(int i = 0; i < 8; i++){
            BYTE* ptr = (BYTE*)this;
            dwChecksum += ptr[i];
        }
        
        for (int i = HEADER_SIZE; i < packetSize; i++) {
            dwChecksum += packetData[i];
        }
    }
    
    // Valida o checksum
    /**
 * ValidateChecksum
 * 
 * Implementa a funcionalidade ValidateChecksum conforme especificação original.
 * @param packetData Parâmetro packetData
 * @param packetSize Parâmetro packetSize
 * @return Retorna bool
 */

    bool ValidateChecksum(const BYTE* packetData, WORD packetSize) const{
        DWORD calculatedChecksum = 0;
        
        // Se não tiver dados adicionais, usa só o cabeçalho para /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param !packetData Parâmetro !packetData
 * @return Retorna calcular
 */
 calcular
        if(packetSize <= HEADER_SIZE || !packetData){
            for (int i = 0; i < 8; i++) {
                const BYTE* ptr = (const BYTE*)this;
                calculatedChecksum += ptr[i];
            }
            return (calculatedChecksum == dwChecksum);
        }
        
        // Se tiver dados adicionais, inclui no cá/**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param i++ Parâmetro i++
 * @return Retorna lculo
 */
lculo
        for(int i = 0; i < 8; i++){
            const BYTE* ptr = (const BYTE*)this;
            calculatedChecksum += ptr[i];
        }
        
        for (int i = HEADER_SIZE; i < packetSize; i++) {
            calculatedChecksum += packetData[i];
        }
        
        return (calculatedChecksum == dwChecksum);
    }
};
#pragma pack(pop)

/**
 * @brief Estrutura que representa um pacote completo (original: PACKET)
 * 
 * Esta estrutura encapsula um pacote completo do WYD, incluindo cabeçalho e dados.
 * No original, esta estrutura tinha problemas de gerenciamento de memória que causavam vazamentos.
 */
/**
 * Classe WYDPacket
 * 
 * Esta classe implementa a funcionalidade WYDPacket conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
/**
 * Classe WYDPacket
 * 
 * Esta classe implementa a funcionalidade WYDPacket conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class Packet {
public:
    // /**
 * Packet
 * 
 * Implementa a funcionalidade Packet conforme especificação original.
 * @return Retorna Construtores
 */
 Construtores
    Packet(){
        Clear();
    }
    
    // Construtor com /**
 * Packet
 * 
 * Implementa a funcionalidade Packet conforme especificação original.
 * @param command Parâmetro command
 * @param 0 Parâmetro 0
 * @return Retorna comando
 */
 comando
    Packet(WORD command, DWORD clientKey = 0){
        Clear();
        header_.Initialize(command, clientKey);
    }
    
    // Construtor de có/**
 * Packet
 * 
 * Implementa a funcionalidade Packet conforme especificação original.
 * @param other Parâmetro other
 * @return Retorna pia
 */
pia
    Packet(const Packet& other){
        header_ = other.header_;
        data_.resize(other.data_.size());
        if (!other.data_.empty()) {
            memcpy(data_.data(), other.data_.data(), other.data_.size());
        }
    }
    
    // Operador de atribuição
    Packet& operator=(const Packet& other) {
        if (this != &other) {
            header_ = other.header_;
            data_.resize(other.data_.size());
            if (!other.data_.empty()) {
                memcpy(data_.data(), other.data_.data(), other.data_.size());
            }
        }
        return *this;
    }
    
    // Limpa o pacote
    /**
 * Clear
 * 
 * Implementa a funcionalidade Clear conforme especificação original.
 */

    void Clear(){
        memset(&header_, 0, sizeof(header_));
        header_.wSize = HEADER_SIZE;
        data_.clear();
    }
    
    // Define o comando do pacote
    /**
 * SetCommand
 * 
 * Implementa a funcionalidade SetCommand conforme especificação original.
 * @param command Parâmetro command
 */

    void SetCommand(WORD command){
        header_.wCommand = command;
    }
    
    // Obtém o comando do pacote
    /**
 * GetCommand
 * 
 * Implementa a funcionalidade GetCommand conforme especificação original.
 * @return Retorna WORD
 */

    WORD GetCommand() const{
        return header_.wCommand;
    }
    
    // Define a chave do cliente
    /**
 * SetClientKey
 * 
 * Implementa a funcionalidade SetClientKey conforme especificação original.
 * @param clientKey Parâmetro clientKey
 */

    void SetClientKey(DWORD clientKey){
        header_.dwClientKey = clientKey;
    }
    
    // Obtém a chave do cliente
    /**
 * GetClientKey
 * 
 * Implementa a funcionalidade GetClientKey conforme especificação original.
 * @return Retorna DWORD
 */

    DWORD GetClientKey() const{
        return header_.dwClientKey;
    }
    
    // Obtém o tamanho total do pacote
    /**
 * GetSize
 * 
 * Implementa a funcionalidade GetSize conforme especificação original.
 * @return Retorna WORD
 */

    WORD GetSize() const{
        return header_.wSize;
    }
    
    // Obtém o cabeçalho do pacote
    const STRUCT_PACKET_HEADER& GetHeader() const {
        return header_;
    }
    
    // Obtém referência para o cabeçalho do pacote
    STRUCT_PACKET_HEADER& GetHeader() {
        return header_;
    }
    
    // Obtém ponteiro para os dados do pacote
    /**
 * GetData
 * 
 * Implementa a funcionalidade GetData conforme especificação original.
 * @return Retorna BYTE
 */

    const BYTE* GetData() const{
        return data_.empty() ? nullptr : data_.data();
    }
    
    // Obtém ponteiro para os dados do pacote (não const)
    /**
 * GetData
 * 
 * Implementa a funcionalidade GetData conforme especificação original.
 * @return Retorna BYTE
 */

    BYTE* GetData(){
        return data_.empty() ? nullptr : data_.data();
    }
    
    // Obtém o tamanho dos dados do pacote
    /**
 * GetDataSize
 * 
 * Implementa a funcionalidade GetDataSize conforme especificação original.
 * @return Retorna WORD
 */

    WORD GetDataSize() const{
        return static_cast<WORD>(data_.size());
    }
    
    // Adiciona dados ao pacote
    /**
 * AppendData
 * 
 * Implementa a funcionalidade AppendData conforme especificação original.
 * @param data Parâmetro data
 * @param size Parâmetro size
 * @return Retorna bool
 */

    bool AppendData(const void* data, WORD size){
        if (!data || size == 0) {
            return false;
        }
        
        // Verifica se excede o tamanho máximo
        if (data_.size() + size > MAX_PACKET_DATA) {
            return false;
        }
        
        // Posição atual no buffer de dados
        size_t currentPos = data_.size();
        
        // Redimensiona o buffer para acomodar os novos dados
        data_.resize(currentPos + size);
        
        // Copia os dados para o buffer
        memcpy(data_.data() + currentPos, data, size);
        
        // Atualiza o tamanho no cabeçalho
        header_.wSize = HEADER_SIZE + static_cast<WORD>(data_.size());
        
        return true;
    }
    
    // Define os dados do pacote (substitui dados existentes)
    /**
 * SetData
 * 
 * Implementa a funcionalidade SetData conforme especificação original.
 * @param data Parâmetro data
 * @param size Parâmetro size
 * @return Retorna bool
 */

    bool SetData(const void* data, WORD size){
        if (!data || size == 0 || size > MAX_PACKET_DATA) {
            return false;
        }
        
        data_.resize(size);
        memcpy(data_.data(), data, size);
        
        // Atualiza o tamanho no cabeçalho
        header_.wSize = HEADER_SIZE + size;
        
        return true;
    }
    
    // Serializa o pacote para um buffer
    /**
 * Serialize
 * 
 * Implementa a funcionalidade Serialize conforme especificação original.
 * @param buffer Parâmetro buffer
 * @param bufferSize Parâmetro bufferSize
 * @return Retorna bool
 */

    bool Serialize(BYTE* buffer, WORD& bufferSize) const{
        if (!buffer || bufferSize < header_.wSize) {
            return false;
        }
        
        // Copia o cabeç/**
 * memcpy
 * 
 * Implementa a funcionalidade memcpy conforme especificação original.
 * @param buffer Parâmetro buffer
 * @param header_ Parâmetro header_
 * @param HEADER_SIZE Parâmetro HEADER_SIZE
 * @return Retorna alho
 */
alho
        memcpy(buffer, &header_, HEADER_SIZE);
        
        // Copia os dados (se houver)
        if (!data_.empty()) {
            memcpy(buffer + HEADER_SIZE, data_.data(), data_.size());
        }
        
        // Atualiza o tamanho do buffer
        bufferSize = header_.wSize;
        
        return true;
    }
    
    // Deserializa um pacote a partir de um buffer
    /**
 * Deserialize
 * 
 * Implementa a funcionalidade Deserialize conforme especificação original.
 * @param buffer Parâmetro buffer
 * @param bufferSize Parâmetro bufferSize
 * @return Retorna bool
 */

    bool Deserialize(const BYTE* buffer, WORD bufferSize){
        if (!buffer || bufferSize < HEADER_SIZE) {
            return false;
        }
        
        // Limpa o pacote /**
 * Clear
 * 
 * Implementa a funcionalidade Clear conforme especificação original.
 * @return Retorna atual
 */
 atual
        Clear();
        
        // Copia o cabeç/**
 * memcpy
 * 
 * Implementa a funcionalidade memcpy conforme especificação original.
 * @param header_ Parâmetro header_
 * @param buffer Parâmetro buffer
 * @param HEADER_SIZE Parâmetro HEADER_SIZE
 * @return Retorna alho
 */
alho
        memcpy(&header_, buffer, HEADER_SIZE);
        
        // Verifica consistência do /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param HEADER_SIZE Parâmetro HEADER_SIZE
 * @return Retorna tamanho
 */
 tamanho
        if(header_.wSize > bufferSize || header_.wSize < HEADER_SIZE){
            Clear();
            return false;
        }
        
        // Copia os dados (se houver)
        WORD dataSize = header_.wSize - HEADER_SIZE;
        if (dataSize > 0) {
            data_.resize(dataSize);
            memcpy(data_.data(), buffer + HEADER_SIZE, dataSize);
        }
        
        return true;
    }
    
    // Calcula e define o checksum do pacote
    /**
 * CalculateChecksum
 * 
 * Implementa a funcionalidade CalculateChecksum conforme especificação original.
 */

    void CalculateChecksum(){
        header_.CalculateChecksum(data_.data(), header_.wSize);
    }
    
    // Valida o checksum do pacote
    /**
 * ValidateChecksum
 * 
 * Implementa a funcionalidade ValidateChecksum conforme especificação original.
 * @return Retorna bool
 */

    bool ValidateChecksum() const{
        return header_.ValidateChecksum(data_.data(), header_.wSize);
    }
    
private:
    STRUCT_PACKET_HEADER header_;  // Cabeçalho do pacote
    std::vector<BYTE> data_;       // Dados do pacote
};

/**
 * @brief Estrutura para pacote de login de conta (original: PACKET_ACCOUNTLOGIN)
 * 
 * Esta estrutura representa o pacote usado para login de conta no WYD.
 * No original, este pacote não tinha validações de segurança adequadas.
 */
#pragma pack(push, 1)
struct STRUCT_PACKET_ACCOUNTLOGIN {
    char szAccountName[32];    // Nome da conta
    char szPassword[32];       // Senha da conta
    char szClientVersion[12];  // Versão do cliente
    BYTE byUnique[9];          // ID único do /**
 * STRUCT_PACKET_ACCOUNTLOGIN
 * 
 * Implementa a funcionalidade STRUCT_PACKET_ACCOUNTLOGIN conforme especificação original.
 * @return Retorna computador
 */
 computador
    
    STRUCT_PACKET_ACCOUNTLOGIN(){
        memset(szAccountName, 0, sizeof(szAccountName));
        memset(szPassword, 0, sizeof(szPassword));
        memset(szClientVersion, 0, sizeof(szClientVersion));
        memset(byUnique, 0, sizeof(byUnique));
    }
    
    // Define o nome da conta com segurança
    /**
 * SetAccountName
 * 
 * Implementa a funcionalidade SetAccountName conforme especificação original.
 * @param accountName Parâmetro accountName
 */

    void SetAccountName(const char* accountName){
        if (!accountName) {
            return;
        }
        
        strncpy(szAccountName, accountName, sizeof(szAccountName) - 1);
        szAccountName[sizeof(szAccountName) - 1] = '\0';
    }
    
    // Define a senha com segurança
    /**
 * SetPassword
 * 
 * Implementa a funcionalidade SetPassword conforme especificação original.
 * @param password Parâmetro password
 */

    void SetPassword(const char* password){
        if (!password) {
            return;
        }
        
        strncpy(szPassword, password, sizeof(szPassword) - 1);
        szPassword[sizeof(szPassword) - 1] = '\0';
    }
    
    // Define a versão do cliente com segurança
    /**
 * SetClientVersion
 * 
 * Implementa a funcionalidade SetClientVersion conforme especificação original.
 * @param clientVersion Parâmetro clientVersion
 */

    void SetClientVersion(const char* clientVersion){
        if (!clientVersion) {
            return;
        }
        
        strncpy(szClientVersion, clientVersion, sizeof(szClientVersion) - 1);
        szClientVersion[sizeof(szClientVersion) - 1] = '\0';
    }
};
#pragma pack(pop)

/**
 * @brief Estrutura para resposta de login de conta (original: PACKET_ACCOUNTLOGINRESULT)
 * 
 * Esta estrutura representa o pacote de resposta para login de conta no WYD.
 */
#pragma pack(push, 1)
struct STRUCT_PACKET_ACCOUNTLOGINRESULT {
    BYTE byResult;                 // Resultado do login (LOGIN_RESULT)
    BYTE byBlockYear;              // Ano de bloqueio (para contas banidas)
    BYTE byBlockMonth;             // Mês de bloqueio
    BYTE byBlockDay;               // Dia de bloqueio
    BYTE byBlockHour;              // Hora de bloqueio
    BYTE byBlockMinute;            // Minuto de bloqueio
    char szMessage[256];           // Mensagem (erro ou informação)
    
    STRUCT_PACKET_ACCOUNTLOGINRESULT() 
        : byResult(LOGIN_SYSTEM_ERROR)
        , byBlockYear(0)
        , byBlockMonth(0)
        , byBlockDay(0)
        , byBlockHour(0)
        , byBlockMinute(0)
    {
        memset(szMessage, 0, sizeof(szMessage));
    }
    
    // Define o resultado do login
    /**
 * SetResult
 * 
 * Implementa a funcionalidade SetResult conforme especificação original.
 * @param result Parâmetro result
 */

    void SetResult(LOGIN_RESULT result){
        byResult = static_cast<BYTE>(result);
    }
    
    // Define a mensagem com segurança
    /**
 * SetMessage
 * 
 * Implementa a funcionalidade SetMessage conforme especificação original.
 * @param message Parâmetro message
 */

    void SetMessage(const char* message){
        if (!message) {
            return;
        }
        
        strncpy(szMessage, message, sizeof(szMessage) - 1);
        szMessage[sizeof(szMessage) - 1] = '\0';
    }
};
#pragma pack(pop)

/**
 * @brief Estrutura para pacote de movimentação (original: PACKET_WALK)
 * 
 * Esta estrutura representa o pacote usado para movimentação no WYD.
 */
#pragma pack(push, 1)
struct STRUCT_PACKET_WALK {
    WORD wPosX;          // Posição X
    WORD wPosY;          // Posição Y
    BYTE byType;         // Tipo de movimento (0=andar, 1=correr)
    BYTE byDirection;    // Direção (0-7, onde 0=Norte, 2=Leste, 4=Sul, 6=Oeste)
    
    STRUCT_PACKET_WALK() 
        : wPosX(0)
        , wPosY(0)
        , byType(0)
        , byDirection(0)
    {
    }
};
#pragma pack(pop)

/**
 * @brief Estrutura para pacote de ataque (original: PACKET_ATTACK)
 * 
 * Esta estrutura representa o pacote usado para ataque no WYD.
 */
#pragma pack(push, 1)
struct STRUCT_PACKET_ATTACK {
    WORD wPosX;          // Posição X
    WORD wPosY;          // Posição Y
    BYTE byDirection;    // Direção (0-7)
    BYTE byType;         // Tipo de ataque (0=normal, 1=habilidade)
    DWORD dwTargetID;    // ID do alvo (0 se não tiver alvo específico)
    WORD wSkillID;       // ID da habilidade (0 se ataque normal)
    
    STRUCT_PACKET_ATTACK() 
        : wPosX(0)
        , wPosY(0)
        , byDirection(0)
        , byType(0)
        , dwTargetID(0)
        , wSkillID(0)
    {
    }
};
#pragma pack(pop)

/**
 * @brief Estrutura para pacote de chat (original: PACKET_CHAT)
 * 
 * Esta estrutura representa o pacote usado para chat no WYD.
 */
#pragma pack(push, 1)
struct STRUCT_PACKET_CHAT {
    char szMessage[256];     // Mensagem de /**
 * STRUCT_PACKET_CHAT
 * 
 * Implementa a funcionalidade STRUCT_PACKET_CHAT conforme especificação original.
 * @return Retorna chat
 */
 chat
    
    STRUCT_PACKET_CHAT(){
        memset(szMessage, 0, sizeof(szMessage));
    }
    
    // Define a mensagem com segurança
    /**
 * SetMessage
 * 
 * Implementa a funcionalidade SetMessage conforme especificação original.
 * @param message Parâmetro message
 */

    void SetMessage(const char* message){
        if (!message) {
            return;
        }
        
        strncpy(szMessage, message, sizeof(szMessage) - 1);
        szMessage[sizeof(szMessage) - 1] = '\0';
    }
};
#pragma pack(pop)

/**
 * @brief Estrutura para pacote de teleporte (original: PACKET_TELEPORT)
 * 
 * Esta estrutura representa o pacote usado para teleporte no WYD.
 */
#pragma pack(push, 1)
struct STRUCT_PACKET_TELEPORT {
    WORD wMapID;         // ID do mapa
    WORD wPosX;          // Posição X
    WORD wPosY;          // Posição Y
    DWORD dwIsValid;     // Flag de validação (no original, usado para verificar se o teleporte é válido)
    
    STRUCT_PACKET_TELEPORT() 
        : wMapID(0)
        , wPosX(0)
        , wPosY(0)
        , dwIsValid(0)
    {
    }
};
#pragma pack(pop)

/**
 * @brief Estrutura para pacote de uso de habilidade/skill (original: PACKET_SKILLCAST)
 * 
 * Esta estrutura representa o pacote usado para lançar habilidades no WYD.
 */
#pragma pack(push, 1)
struct STRUCT_PACKET_SKILLCAST {
    WORD wSkillID;       // ID da habilidade
    WORD wPosX;          // Posição X
    WORD wPosY;          // Posição Y
    DWORD dwTargetID;    // ID do alvo (0 se não tiver alvo específico)
    BYTE byDirection;    // Direção (0-7)
    
    STRUCT_PACKET_SKILLCAST() 
        : wSkillID(0)
        , wPosX(0)
        , wPosY(0)
        , dwTargetID(0)
        , byDirection(0)
    {
    }
};
#pragma pack(pop)

// Utilitários para pacotes

/**
 * @brief Converte um pacote para um buffer de bytes
 * @param packet Pacote a converter
 * @param buffer Buffer para receber os dados
 * @param bufferSize Tamanho do buffer (entrada/saída)
 * @return true se convertido com sucesso, false caso contrário
 */
inline /**
 * PacketToBuffer
 * 
 * Implementa a funcionalidade PacketToBuffer conforme especificação original.
 * @param packet Parâmetro packet
 * @param buffer Parâmetro buffer
 * @param bufferSize Parâmetro bufferSize
 * @return Retorna bool
 */
 bool PacketToBuffer(const Packet& packet, BYTE* buffer, WORD& bufferSize){
    if (!buffer) {
        return false;
    }
    
    return packet.Serialize(buffer, bufferSize);
}

/**
 * @brief Converte um buffer de bytes para um pacote
 * @param buffer Buffer contendo os dados
 * @param bufferSize Tamanho do buffer
 * @param packet Pacote para receber os dados
 * @return true se convertido com sucesso, false caso contrário
 */
inline /**
 * BufferToPacket
 * 
 * Implementa a funcionalidade BufferToPacket conforme especificação original.
 * @param buffer Parâmetro buffer
 * @param bufferSize Parâmetro bufferSize
 * @param packet Parâmetro packet
 * @return Retorna bool
 */
 bool BufferToPacket(const BYTE* buffer, WORD bufferSize, Packet& packet){
    if (!buffer || bufferSize < HEADER_SIZE) {
        return false;
    }
    
    return packet.Deserialize(buffer, bufferSize);
}

} // namespace network
} // namespace wyd

#endif // PACKETTYPES_H

} // namespace wydbr
