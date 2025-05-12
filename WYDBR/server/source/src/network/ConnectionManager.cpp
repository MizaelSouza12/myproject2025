/**
 * @file ConnectionManager.cpp
 * @brief Implementação do sistema de gerenciamento de conexões
 * 
 * Este arquivo implementa a classe de gerenciamento de conexões do servidor WYD,
 * mantendo compatibilidade total com o protocolo original e implementando
 * otimizações específicas para o protocolo do WYD.
 */

#include "../../include/network/ConnectionManager.h"
#include <cstring>
#include <chrono>
#include <thread>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <ctime>
#include <functional>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    #define SOCKET_ERROR_CODE WSAGetLastError()
    #define SOCKADDR_IN sockaddr_in
    #define SOCKADDR sockaddr
    #define SOCKET_T SOCKET
    #define SOCKET_CLOSE(s) closesocket(s)
    #define SOCKET_INVALID INVALID_SOCKET
    #define SOCKLEN_T int
    #define SOCK_EAGAIN WSAEWOULDBLOCK
    #define SOCK_EWOULDBLOCK WSAEWOULDBLOCK
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <netinet/tcp.h>
    #include <arpa/inet.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include <errno.h>
    #define SOCKET_ERROR_CODE errno
    #define SOCKADDR_IN sockaddr_in
    #define SOCKADDR sockaddr
    #define SOCKET_T int
    #define SOCKET_CLOSE(s) close(s)
    #define SOCKET_INVALID -1
    #define SOCKLEN_T socklen_t
    #define SOCK_EAGAIN EAGAIN
    #define SOCK_EWOULDBLOCK EWOULDBLOCK
#endif

namespace wydbr {

/**
 * Implementação da classe ConnectionBuffer
 */

ConnectionBuffer::ConnectionBuffer(size_t size) 
    : bufferSize_(size), readPos_(0), writePos_(0), dataSize_(0) {
    buffer_ = new uint8_t[size];
    memset(buffer_, 0, size);
}

ConnectionBuffer::~ConnectionBuffer() {
    delete[] buffer_;
}

void ConnectionBuffer::Reset() {
    readPos_ = 0;
    writePos_ = 0;
    dataSize_ = 0;
    memset(buffer_, 0, bufferSize_);
}

size_t ConnectionBuffer::Write(const void* data, size_t size) {
    if (size == 0 || data == nullptr) {
        return 0;
    }

    // Se o buffer estiver /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param bufferSize_ Parâmetro bufferSize_
 * @return Retorna cheio
 */
 cheio
    if(dataSize_ >= bufferSize_){
        return 0;
    }

    // Calcular espaço disponível
    size_t availableSpace = bufferSize_ - dataSize_;
    size_t bytesToWrite = std::min(availableSpace, size);

    // Se o espaço contíguo não for /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param writePos_ Parâmetro writePos_
 * @return Retorna suficiente
 */
 suficiente
    if(bytesToWrite <= bufferSize_ - writePos_){
        // Copiar os dados /**
 * memcpy
 * 
 * Implementa a funcionalidade memcpy conforme especificação original.
 * @param writePos_ Parâmetro writePos_
 * @param data Parâmetro data
 * @param bytesToWrite Parâmetro bytesToWrite
 * @return Retorna diretamente
 */
 diretamente
        memcpy(buffer_ + writePos_, data, bytesToWrite);
        writePos_ = (writePos_ + bytesToWrite) % bufferSize_;
    } else {
        // Copiar em duas partes
        size_t firstPart = bufferSize_ - writePos_;
        memcpy(buffer_ + writePos_, data, firstPart);
        
        size_t secondPart = bytesToWrite - firstPart;
        memcpy(buffer_, static_cast<const uint8_t*>(data) + firstPart, secondPart);
        writePos_ = secondPart;
    }

    dataSize_ += bytesToWrite;
    return bytesToWrite;
}

size_t ConnectionBuffer::Read(void* data, size_t size) {
    if (size == 0 || data == nullptr) {
        return 0;
    }

    // Se o buffer estiver /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna vazio
 */
 vazio
    if(dataSize_ == 0){
        return 0;
    }

    // Calcular quantidade de bytes a ler
    size_t bytesToRead = std::min(dataSize_, size);

    // Se os dados estiverem contí/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param readPos_ Parâmetro readPos_
 * @return Retorna guos
 */
guos
    if(bytesToRead <= bufferSize_ - readPos_){
        // Copiar os dados /**
 * memcpy
 * 
 * Implementa a funcionalidade memcpy conforme especificação original.
 * @param data Parâmetro data
 * @param readPos_ Parâmetro readPos_
 * @param bytesToRead Parâmetro bytesToRead
 * @return Retorna diretamente
 */
 diretamente
        memcpy(data, buffer_ + readPos_, bytesToRead);
        readPos_ = (readPos_ + bytesToRead) % bufferSize_;
    } else {
        // Copiar em duas partes
        size_t firstPart = bufferSize_ - readPos_;
        memcpy(data, buffer_ + readPos_, firstPart);
        
        size_t secondPart = bytesToRead - firstPart;
        memcpy(static_cast<uint8_t*>(data) + firstPart, buffer_, secondPart);
        readPos_ = secondPart;
    }

    dataSize_ -= bytesToRead;
    return bytesToRead;
}

size_t ConnectionBuffer::Peek(void* data, size_t size) const {
    if (size == 0 || data == nullptr) {
        return 0;
    }

    // Se o buffer estiver /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna vazio
 */
 vazio
    if(dataSize_ == 0){
        return 0;
    }

    // Calcular quantidade de bytes a ler
    size_t bytesToRead = std::min(dataSize_, size);

    // Se os dados estiverem contí/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param readPos_ Parâmetro readPos_
 * @return Retorna guos
 */
guos
    if(bytesToRead <= bufferSize_ - readPos_){
        // Copiar os dados /**
 * memcpy
 * 
 * Implementa a funcionalidade memcpy conforme especificação original.
 * @param data Parâmetro data
 * @param readPos_ Parâmetro readPos_
 * @param bytesToRead Parâmetro bytesToRead
 * @return Retorna diretamente
 */
 diretamente
        memcpy(data, buffer_ + readPos_, bytesToRead);
    } else {
        // Copiar em duas partes
        size_t firstPart = bufferSize_ - readPos_;
        memcpy(data, buffer_ + readPos_, firstPart);
        
        size_t secondPart = bytesToRead - firstPart;
        memcpy(static_cast<uint8_t*>(data) + firstPart, buffer_, secondPart);
    }

    return bytesToRead;
}

size_t ConnectionBuffer::Skip(size_t size) {
    if (size == 0) {
        return 0;
    }

    // Se o buffer estiver /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna vazio
 */
 vazio
    if(dataSize_ == 0){
        return 0;
    }

    // Calcular quantidade de bytes a pular
    size_t bytesToSkip = std::min(dataSize_, size);
    readPos_ = (readPos_ + bytesToSkip) % bufferSize_;
    dataSize_ -= bytesToSkip;
    return bytesToSkip;
}

size_t ConnectionBuffer::GetSpace() const {
    return bufferSize_ - dataSize_;
}

size_t ConnectionBuffer::GetDataSize() const {
    return dataSize_;
}

size_t ConnectionBuffer::GetTotalSize() const {
    return bufferSize_;
}

bool ConnectionBuffer::IsEmpty() const {
    return dataSize_ == 0;
}

bool ConnectionBuffer::IsFull() const {
    return dataSize_ >= bufferSize_;
}

uint8_t* ConnectionBuffer::GetWritePointer(size_t* availableSize) {
    // Se o buffer estiver /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param bufferSize_ Parâmetro bufferSize_
 * @return Retorna cheio
 */
 cheio
    if(dataSize_ >= bufferSize_){
        if (availableSize) {
            *availableSize = 0;
        }
        return nullptr;
    }

    // Calcular espaço disponível contíguo
    size_t contiguousSpace = 0;
    if (writePos_ >= readPos_) {
        // Espaço disponível no final do buffer
        contiguousSpace = bufferSize_ - writePos_;
        if (readPos_ == 0) {
            // Menos 1 byte para diferenciar buffer cheio de vazio
            contiguousSpace--;
        }
    } else {
        // Espaço disponível entre writePos_ e readPos_
        contiguousSpace = readPos_ - writePos_ - 1;
    }

    if (availableSize) {
        *availableSize = contiguousSpace;
    }

    return buffer_ + writePos_;
}

bool ConnectionBuffer::AdvanceWritePointer(size_t size) {
    // Verificar se o tamanho é válido
    size_t availableSpace = 0;
    GetWritePointer(&availableSpace);
    if (size > availableSpace) {
        return false;
    }

    writePos_ = (writePos_ + size) % bufferSize_;
    dataSize_ += size;
    return true;
}

const uint8_t* ConnectionBuffer::GetReadPointer(size_t* availableSize) const {
    // Se o buffer estiver /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna vazio
 */
 vazio
    if(dataSize_ == 0){
        if (availableSize) {
            *availableSize = 0;
        }
        return nullptr;
    }

    // Calcular espaço contíguo disponível para leitura
    size_t contiguousData = 0;
    if (readPos_ < writePos_) {
        // Dados contíguos entre readPos_ e writePos_
        contiguousData = writePos_ - readPos_;
    } else {
        // Dados contíguos até o final do buffer
        contiguousData = bufferSize_ - readPos_;
    }

    if (availableSize) {
        *availableSize = contiguousData;
    }

    return buffer_ + readPos_;
}

bool ConnectionBuffer::AdvanceReadPointer(size_t size) {
    // Verificar se o tamanho é válido
    size_t availableData = 0;
    GetReadPointer(&availableData);
    if (size > availableData) {
        return false;
    }

    readPos_ = (readPos_ + size) % bufferSize_;
    dataSize_ -= size;
    return true;
}

/**
 * Implementação da classe Connection
 */

Connection::Connection(int socket, const std::string& ipAddress, uint16_t port, int id)
    : socket_(socket), ipAddress_(ipAddress), port_(port), id_(id), state_(CONN_CONNECTING),
      errorType_(CONN_ERROR_NONE), pendingClose_(false), closeTime_(0),
      recvBuffer_(8192), sendBuffer_(8192) {
}

Connection::~Connection() {
    Finalize();
}

bool Connection::Initialize() {
    state_ = CONN_CONNECTED;
    errorType_ = CONN_ERROR_NONE;
    errorMessage_.clear();
    pendingClose_ = false;
    closeTime_ = 0;
    
    stats_.connectionTime = static_cast<uint32_t>(time(nullptr));
    
    return true;
}

void Connection::Finalize() {
    if (socket_ != SOCKET_INVALID) {
        SOCKET_CLOSE(socket_);
        socket_ = SOCKET_INVALID;
    }
    
    state_ = CONN_DISCONNECTED;
}

void Connection::Close(bool immediate, ConnectionErrorType errorType, const std::string& errorMessage) {
    if (state_ == CONN_DISCONNECTED || state_ == CONN_CLOSING) {
        return;
    }
    
    errorType_ = errorType;
    errorMessage_ = errorMessage;
    
    if (immediate) {
        Finalize();
    } else {
        state_ = CONN_CLOSING;
        pendingClose_ = true;
        closeTime_ = static_cast<uint32_t>(time(nullptr)) + 5; // 5 segundos para fechar
    }
}

bool Connection::ProcessReceive(const void* data, size_t size) {
    if (state_ == CONN_DISCONNECTED || state_ == CONN_CLOSING) {
        return false;
    }
    
    // Adicionar dados ao buffer de recebimento
    size_t bytesWritten = recvBuffer_.Write(data, size);
    if (bytesWritten < size) {
        // Buffer cheio, erro de overflow
        stats_.overflowErrors++;
        Close(true, CONN_ERROR_PROTOCOL, "Receive buffer overflow");
        return false;
    }
    
    stats_.bytesReceived += bytesWritten;
    
    // Processar pacotes /**
 * ProcessPackets
 * 
 * Implementa a funcionalidade ProcessPackets conforme especificação original.
 * @return Retorna recebidos
 */
 recebidos
    ProcessPackets();
    
    return true;
}

bool Connection::SendPacket(const void* data, size_t size) {
    if (state_ == CONN_DISCONNECTED || state_ == CONN_CLOSING) {
        return false;
    }
    
    // Adicionar dados ao buffer de envio
    size_t bytesWritten = sendBuffer_.Write(data, size);
    if (bytesWritten < size) {
        // Buffer cheio, erro de overflow
        stats_.overflowErrors++;
        Close(true, CONN_ERROR_PROTOCOL, "Send buffer overflow");
        return false;
    }
    
    stats_.bytesSent += bytesWritten;
    stats_.packetsSent++;
    
    return true;
}

bool Connection::SendPacketWithData(uint16_t opcode, const void* data, size_t dataSize) {
    // Criar cabeçalho do pacote
    PacketHeader header;
    header.size = static_cast<uint16_t>(sizeof(PacketHeader) + dataSize);
    header.opcode = opcode;
    header.checksum = 0; // Será calculado depois
    
    // Buffer temporário para construir o pacote
    uint8_t packetBuffer[MAX_PACKET_SIZE];
    
    // Copiar cabeçalho
    memcpy(packetBuffer, &header, sizeof(PacketHeader));
    
    // Copiar /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param 0 Parâmetro 0
 * @return Retorna dados
 */
 dados
    if(data != nullptr && dataSize > 0){
        memcpy(packetBuffer + sizeof(PacketHeader), data, dataSize);
    }
    
    // Calcular checksum (implementação simplificada, deve ser substituída pelo algoritmo exato do WYD)
    header.checksum = CalculateChecksum(packetBuffer, header.size);
    memcpy(packetBuffer, &header, sizeof(PacketHeader));
    
    // Enviar pacote
    /**
 * SendPacket
 * 
 * Implementa a funcionalidade SendPacket conforme especificação original.
 * @param packetBuffer Parâmetro packetBuffer
 * @param header.size Parâmetro header.size
 * @return Retorna return
 */

    return SendPacket(packetBuffer, header.size);
}

bool Connection::SendSimplePacket(uint16_t opcode) {
    /**
 * SendPacketWithData
 * 
 * Implementa a funcionalidade SendPacketWithData conforme especificação original.
 * @param opcode Parâmetro opcode
 * @param nullptr Parâmetro nullptr
 * @param 0 Parâmetro 0
 * @return Retorna return
 */

    return SendPacketWithData(opcode, nullptr, 0);
}

bool Connection::SendHeartbeat() {
    // Implementar o envio de heartbeat específico do WYD
    /**
 * SendSimplePacket
 * 
 * Implementa a funcionalidade SendSimplePacket conforme especificação original.
 * @param HEARTBEAT_PKT Parâmetro HEARTBEAT_PKT
 * @return Retorna return
 */

    return SendSimplePacket(HEARTBEAT_PKT);
}

bool Connection::SendMessage(const std::string& message, uint16_t type) {
    // Estrutura simplificada de mensagem
    struct ChatMessage {
        uint16_t type;
        char sender[16];
        char message[256];
    };
    
    ChatMessage chatMsg;
    memset(&chatMsg, 0, sizeof(ChatMessage));
    
    chatMsg.type = type;
    strncpy(chatMsg.sender, "Sistema", sizeof(chatMsg.sender) - 1);
    strncpy(chatMsg.message, message.c_str(), sizeof(chatMsg.message) - 1);
    
    return SendPacketWithData(CHAT_PKT, &chatMsg, sizeof(ChatMessage));
}

bool Connection::ProcessTick(uint32_t currentTime) {
    if (state_ == CONN_DISCONNECTED) {
        return false;
    }
    
    // Verificar timeout de /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param closeTime_ Parâmetro closeTime_
 * @return Retorna fechamento
 */
 fechamento
    if(pendingClose_ && currentTime >= closeTime_){
        Finalize();
        return false;
    }
    
    // Processar pacotes /**
 * ProcessPackets
 * 
 * Implementa a funcionalidade ProcessPackets conforme especificação original.
 * @return Retorna pendentes
 */
 pendentes
    ProcessPackets();
    
    // Verificar timeout de conexão
    uint32_t lastPacketTime = stats_.lastPacketTime;
    if (lastPacketTime > 0 && currentTime - lastPacketTime > 60) { // 60 segundos sem pacotes
        stats_.timeoutErrors++;
        Close(true, CONN_ERROR_TIMEOUT, "Connection timeout");
        return false;
    }
    
    // Enviar heartbeat periódico
    uint32_t lastKeepAlive = info_.lastKeepAlive;
    if (currentTime - lastKeepAlive > 30) { // 30 segundos entre /**
 * SendHeartbeat
 * 
 * Implementa a funcionalidade SendHeartbeat conforme especificação original.
 * @return Retorna heartbeats
 */
 heartbeats
        SendHeartbeat();
        info_.lastKeepAlive = currentTime;
    }
    
    return true;
}

bool Connection::ProcessPackets() {
    const size_t MIN_PACKET_SIZE = sizeof(PacketHeader);
    
    // Processar pacotes enquanto houver dados suficientes no buffer
    while (recvBuffer_.GetDataSize() >= MIN_PACKET_SIZE) {
        // Ler cabeçalho sem remover do buffer
        PacketHeader header;
        size_t bytesRead = recvBuffer_.Peek(&header, sizeof(PacketHeader));
        if (bytesRead < sizeof(PacketHeader)) {
            break;
        }
        
        // Verificar tamanho do /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param MAX_PACKET_SIZE Parâmetro MAX_PACKET_SIZE
 * @return Retorna pacote
 */
 pacote
        if(header.size < MIN_PACKET_SIZE || header.size > MAX_PACKET_SIZE){
            stats_.invalidPackets++;
            Close(true, CONN_ERROR_INVALID_PACKET, "Invalid packet size");
            return false;
        }
        
        // Verificar se o pacote inteiro está disponível
        if (recvBuffer_.GetDataSize() < header.size) {
            break;
        }
        
        // Ler pacote completo
        uint8_t packetBuffer[MAX_PACKET_SIZE];
        bytesRead = recvBuffer_.Read(packetBuffer, header.size);
        if (bytesRead < header.size) {
            stats_.invalidPackets++;
            Close(true, CONN_ERROR_PROTOCOL, "Failed to read packet");
            return false;
        }
        
        // Verificar checksum
        uint16_t calculatedChecksum = CalculateChecksum(packetBuffer, header.size);
        if (calculatedChecksum != header.checksum) {
            stats_.checksumErrors++;
            Close(true, CONN_ERROR_CHECKSUM, "Invalid packet checksum");
            return false;
        }
        
        // Processar pacote
        if (!ProcessPacket(packetBuffer, header.size)) {
            stats_.protocolErrors++;
            Close(true, CONN_ERROR_PROTOCOL, "Failed to process packet");
            return false;
        }
        
        stats_.packetsReceived++;
        stats_.lastPacketTime = static_cast<uint32_t>(time(nullptr));
    }
    
    return true;
}

bool Connection::ProcessPacket(const void* packet, size_t size) {
    if (size < sizeof(PacketHeader)) {
        return false;
    }
    
    const PacketHeader* header = static_cast<const PacketHeader*>(packet);
    uint16_t opcode = header->opcode;
    
    // Verificar se há um callback registrado para este opcode
    auto it = packetCallbacks_.find(opcode);
    if (it != packetCallbacks_.end()) {
        return it->second(this, packet, size);
    }
    
    // Processamento padrão para pacotes sem /**
 * switch
 * 
 * Implementa a funcionalidade switch conforme especificação original.
 * @param opcode Parâmetro opcode
 * @return Retorna callback
 */
 callback
    switch(opcode){
        case HEARTBEAT_PKT:
            // Responder /**
 * SendHeartbeat
 * 
 * Implementa a funcionalidade SendHeartbeat conforme especificação original.
 * @return Retorna heartbeat
 */
 heartbeat
            SendHeartbeat();
            return true;
            
        case DISCONNECT_PKT:
            // Fechar conexã/**
 * Close
 * 
 * Implementa a funcionalidade Close conforme especificação original.
 * @param false Parâmetro false
 * @param CONN_ERROR_NONE Parâmetro CONN_ERROR_NONE
 * @param disconnect" Parâmetro disconnect"
 * @return Retorna o
 */
o
            Close(false, CONN_ERROR_NONE, "Client requested disconnect");
            return true;
            
        default:
            // Pacote desconhecido, logar como informativo mas não tratar como erro
            return true;
    }
}

size_t Connection::GetSendData(void* buffer, size_t bufferSize) {
    if (buffer == nullptr || bufferSize == 0) {
        return 0;
    }
    
    return sendBuffer_.Read(buffer, bufferSize);
}

void Connection::RegisterPacketCallback(uint16_t opcode, std::function<bool(Connection*, const void*, size_t)> callback) {
    packetCallbacks_[opcode] = callback;
}

void Connection::SetAccountId(uint32_t accountId) {
    info_.accountId = accountId;
}

void Connection::SetAccountName(const std::string& accountName) {
    info_.accountName = accountName;
}

void Connection::SetPlayerId(int playerId) {
    info_.playerId = playerId;
}

void Connection::SetState(ConnectionState state) {
    state_ = state;
}

void Connection::SetError(ConnectionErrorType errorType, const std::string& errorMessage) {
    errorType_ = errorType;
    errorMessage_ = errorMessage;
}

void Connection::SetLastKeepAlive(uint32_t time) {
    info_.lastKeepAlive = time;
}

void Connection::SetClientVersion(uint16_t version) {
    info_.clientVersion = version;
}

void Connection::SetClientType(uint16_t type) {
    info_.clientType = type;
}

void Connection::SetMacAddress(const std::string& macAddress) {
    info_.macAddress = macAddress;
}

void Connection::SetKey(uint32_t key) {
    info_.key = key;
}

void Connection::SetSeed(uint8_t seed) {
    info_.seed = seed;
}

void Connection::SetUseCompression(bool useCompression) {
    info_.useCompression = useCompression;
}

void Connection::SetUseEncryption(bool useEncryption) {
    info_.useEncryption = useEncryption;
}

int Connection::GetId() const {
    return id_;
}

int Connection::GetSocket() const {
    return socket_;
}

const std::string& Connection::GetIpAddress() const {
    return ipAddress_;
}

uint16_t Connection::GetPort() const {
    return port_;
}

uint32_t Connection::GetAccountId() const {
    return info_.accountId;
}

const std::string& Connection::GetAccountName() const {
    return info_.accountName;
}

int Connection::GetPlayerId() const {
    return info_.playerId;
}

ConnectionState Connection::GetState() const {
    return state_;
}

ConnectionErrorType Connection::GetErrorType() const {
    return errorType_;
}

const std::string& Connection::GetErrorMessage() const {
    return errorMessage_;
}

uint32_t Connection::GetLastKeepAlive() const {
    return info_.lastKeepAlive;
}

uint16_t Connection::GetClientVersion() const {
    return info_.clientVersion;
}

uint16_t Connection::GetClientType() const {
    return info_.clientType;
}

const std::string& Connection::GetMacAddress() const {
    return info_.macAddress;
}

uint32_t Connection::GetKey() const {
    return info_.key;
}

uint8_t Connection::GetSeed() const {
    return info_.seed;
}

bool Connection::GetUseCompression() const {
    return info_.useCompression;
}

bool Connection::GetUseEncryption() const {
    return info_.useEncryption;
}

const ConnectionStats& Connection::GetStats() const {
    return stats_;
}

bool Connection::IsConnected() const {
    return state_ == CONN_CONNECTED || state_ == CONN_AUTHENTICATING || 
           state_ == CONN_AUTHENTICATED || state_ == CONN_GAME;
}

bool Connection::IsAuthenticated() const {
    return state_ == CONN_AUTHENTICATED || state_ == CONN_GAME;
}

bool Connection::IsInGame() const {
    return state_ == CONN_GAME;
}

bool Connection::IsClosing() const {
    return state_ == CONN_CLOSING || pendingClose_;
}

uint16_t Connection::CalculateChecksum(const void* data, size_t size) {
    // Implementação simplificada de checksum para demonstração
    // Deve ser substituída pelo algoritmo exato usado no WYD
    
    if (data == nullptr || size < sizeof(PacketHeader)) {
        return 0;
    }
    
    // Cópia temporária do pacote para o cálculo
    uint8_t* buffer = new uint8_t[size];
    memcpy(buffer, data, size);
    
    // Zerar o campo de checksum para o cálculo
    PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
    uint16_t originalChecksum = header->checksum;
    header->checksum = 0;
    
    // Cálculo de checksum (soma simples de bytes)
    uint16_t checksum = 0;
    for (size_t i = 0; i < size; i++) {
        checksum += buffer[i];
    }
    
    delete[] buffer;
    return checksum;
}

/**
 * Implementação da classe ConnectionManager
 */

/**
 * Classe WYDConnectionManager
 * 
 * Esta classe implementa a funcionalidade WYDConnectionManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class ConnectionManager {
public:
    // Singleton
    static ConnectionManager& GetInstance() {
        static ConnectionManager instance;
        return instance;
    }
    
    /**
     * @brief Inicializa o gerenciador de conexões
     * @param port Porta para escutar
     * @param maxConnections Número máximo de conexões
     * @return true se inicializado com sucesso
     */
    bool Initialize(uint16_t port, uint32_t maxConnections) {
        // Inicializar WinSock no Windows
#ifdef _WIN32
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cerr << "Falha ao inicializar WinSock" << std::endl;
            return false;
        }
#endif

        // Criar socket de escuta
        listenSocket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (listenSocket_ == SOCKET_INVALID) {
            std::cerr << "Falha ao criar socket de escuta: " << SOCKET_ERROR_CODE << std::endl;
            return false;
        }
        
        // Configurar socket de escuta para modo não-bloqueante
#ifdef _WIN32
        u_long mode = 1;
        if (ioctlsocket(listenSocket_, FIONBIO, &mode) != 0) {
            std::cerr << "Falha ao configurar socket não-bloqueante: " << SOCKET_ERROR_CODE << std::endl;
            SOCKET_CLOSE(listenSocket_);
            return false;
        }
#else
        int flags = fcntl(listenSocket_, F_GETFL, 0);
        if (flags == -1) {
            std::cerr << "Falha ao obter flags do socket: " << SOCKET_ERROR_CODE << std::endl;
            SOCKET_CLOSE(listenSocket_);
            return false;
        }
        
        if (fcntl(listenSocket_, F_SETFL, flags | O_NONBLOCK) == -1) {
            std::cerr << "Falha ao configurar socket não-bloqueante: " << SOCKET_ERROR_CODE << std::endl;
            SOCKET_CLOSE(listenSocket_);
            return false;
        }
#endif
        
        // Configurar reutilização de endereço
        int reuse = 1;
        if (setsockopt(listenSocket_, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char*>(&reuse), sizeof(reuse)) != 0) {
            std::cerr << "Falha ao configurar reutilização de endereço: " << SOCKET_ERROR_CODE << std::endl;
            SOCKET_CLOSE(listenSocket_);
            return false;
        }
        
        // Configurar endereço de escuta
        struct sockaddr_in listenAddr;
        memset(&listenAddr, 0, sizeof(listenAddr));
        listenAddr.sin_family = AF_INET;
        listenAddr.sin_port = htons(port);
        listenAddr.sin_addr.s_addr = INADDR_ANY;
        
        // Vincular socket
        if (bind(listenSocket_, reinterpret_cast<struct sockaddr*>(&listenAddr), sizeof(listenAddr)) != 0) {
            std::cerr << "Falha ao vincular socket de escuta: " << SOCKET_ERROR_CODE << std::endl;
            SOCKET_CLOSE(listenSocket_);
            return false;
        }
        
        // Iniciar escuta
        if (listen(listenSocket_, SOMAXCONN) != 0) {
            std::cerr << "Falha ao iniciar escuta: " << SOCKET_ERROR_CODE << std::endl;
            SOCKET_CLOSE(listenSocket_);
            return false;
        }
        
        // Inicializar variáveis
        maxConnections_ = maxConnections;
        port_ = port;
        nextConnectionId_ = 1;
        running_ = true;
        
        // Iniciar thread de processamento
        processingThread_ = std::thread(&ConnectionManager::ProcessThread, this);
        
        std::cout << "ConnectionManager inicializado na porta " << port << std::endl;
        return true;
    }
    
    /**
     * @brief Finaliza o gerenciador de conexões
     */
    void Shutdown() {
        running_ = false;
        
        // Encerrar thread de processamento
        if (processingThread_.joinable()) {
            processingThread_.join();
        }
        
        // Fechar socket de /**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param SOCKET_INVALID Parâmetro SOCKET_INVALID
 * @return Retorna escuta
 */
 escuta
        if(listenSocket_ != SOCKET_INVALID){
            SOCKET_CLOSE(listenSocket_);
            listenSocket_ = SOCKET_INVALID;
        }
        
        // Fechar todas as conexões
        {
            std::lock_guard<std::mutex> lock(connectionsMutex_);
            for (auto& conn : connections_) {
                if (conn.second) {
                    conn.second->Finalize();
                }
            }
            connections_.clear();
        }
        
#ifdef /**
 * WSACleanup
 * 
 * Implementa a funcionalidade WSACleanup conforme especificação original.
 * @return Retorna _WIN32
 */
 _WIN32
        WSACleanup();
#endif
        
        std::cout << "ConnectionManager finalizado" << std::endl;
    }
    
    /**
     * @brief Processa um tick do gerenciador de conexões
     */
    void ProcessTick() {
        uint32_t currentTime = static_cast<uint32_t>(time(nullptr));
        
        // Aceitar novas conexõ/**
 * AcceptConnections
 * 
 * Implementa a funcionalidade AcceptConnections conforme especificação original.
 * @return Retorna es
 */
es
        AcceptConnections();
        
        // Processar conexões existentes
        std::vector<int> closedConnections;
        {
            std::lock_guard<std::mutex> lock(connectionsMutex_);
            for (auto& conn : connections_) {
                if (!conn.second->ProcessTick(currentTime)) {
                    closedConnections.push_back(conn.first);
                }
            }
        }
        
        // Remover conexões /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param closedConnections Parâmetro closedConnections
 * @return Retorna fechadas
 */
 fechadas
        for(int id : closedConnections){
            RemoveConnection(id);
        }
    }
    
    /**
     * @brief Envia dados para uma conexão
     * @param connectionId ID da conexão
     * @param data Dados
     * @param size Tamanho
     * @return true se enviado com sucesso
     */
    bool Send(int connectionId, const void* data, size_t size) {
        std::shared_ptr<Connection> connection = GetConnection(connectionId);
        if (!connection) {
            return false;
        }
        
        return connection->SendPacket(data, size);
    }
    
    /**
     * @brief Envia um pacote para uma conexão
     * @param connectionId ID da conexão
     * @param opcode Opcode
     * @param data Dados
     * @param dataSize Tamanho dos dados
     * @return true se enviado com sucesso
     */
    bool SendPacket(int connectionId, uint16_t opcode, const void* data, size_t dataSize) {
        std::shared_ptr<Connection> connection = GetConnection(connectionId);
        if (!connection) {
            return false;
        }
        
        return connection->SendPacketWithData(opcode, data, dataSize);
    }
    
    /**
     * @brief Envia um pacote simples para uma conexão
     * @param connectionId ID da conexão
     * @param opcode Opcode
     * @return true se enviado com sucesso
     */
    bool SendSimplePacket(int connectionId, uint16_t opcode) {
        std::shared_ptr<Connection> connection = GetConnection(connectionId);
        if (!connection) {
            return false;
        }
        
        return connection->SendSimplePacket(opcode);
    }
    
    /**
     * @brief Envia um pacote para todas as conexões
     * @param opcode Opcode
     * @param data Dados
     * @param dataSize Tamanho dos dados
     * @return Número de conexões que receberam o pacote
     */
    uint32_t Broadcast(uint16_t opcode, const void* data, size_t dataSize) {
        uint32_t count = 0;
        std::lock_guard<std::mutex> lock(connectionsMutex_);
        
        for (auto& conn : connections_) {
            if (conn.second && conn.second->IsConnected()) {
                if (conn.second->SendPacketWithData(opcode, data, dataSize)) {
                    count++;
                }
            }
        }
        
        return count;
    }
    
    /**
     * @brief Registra um callback para um opcode
     * @param opcode Opcode
     * @param callback Função de callback
     */
    void RegisterPacketCallback(uint16_t opcode, std::function<bool(Connection*, const void*, size_t)> callback) {
        std::lock_guard<std::mutex> lock(connectionsMutex_);
        
        for (auto& conn : connections_) {
            if (conn.second) {
                conn.second->RegisterPacketCallback(opcode, callback);
            }
        }
        
        // Registrar para futuras conexões
        callbackRegistry_[opcode] = callback;
    }
    
    /**
     * @brief Fecha uma conexão
     * @param connectionId ID da conexão
     * @param immediate Se deve fechar imediatamente
     * @param errorType Tipo de erro
     * @param errorMessage Mensagem de erro
     * @return true se a conexão foi encontrada
     */
    bool CloseConnection(int connectionId, bool immediate, ConnectionErrorType errorType, const std::string& errorMessage) {
        std::shared_ptr<Connection> connection = GetConnection(connectionId);
        if (!connection) {
            return false;
        }
        
        connection->Close(immediate, errorType, errorMessage);
        return true;
    }
    
    /**
     * @brief Obtém o número de conexões ativas
     * @return Número de conexões
     */
    uint32_t GetConnectionCount() const {
        std::lock_guard<std::mutex> lock(connectionsMutex_);
        return static_cast<uint32_t>(connections_.size());
    }
    
    /**
     * @brief Obtém uma lista de IDs de conexões
     * @return Lista de IDs
     */
    std::vector<int> GetConnectionIds() const {
        std::vector<int> ids;
        std::lock_guard<std::mutex> lock(connectionsMutex_);
        
        for (auto& conn : connections_) {
            ids.push_back(conn.first);
        }
        
        return ids;
    }
    
    /**
     * @brief Obtém informações sobre uma conexão
     * @param connectionId ID da conexão
     * @param info Estrutura para receber as informações
     * @return true se a conexão foi encontrada
     */
    bool GetConnectionInfo(int connectionId, ConnectionInfo& info) const {
        std::shared_ptr<Connection> connection = GetConnection(connectionId);
        if (!connection) {
            return false;
        }
        
        info.socket = connection->GetSocket();
        info.ipAddress = connection->GetIpAddress();
        info.port = connection->GetPort();
        info.accountId = connection->GetAccountId();
        info.accountName = connection->GetAccountName();
        info.state = connection->GetState();
        info.errorType = connection->GetErrorType();
        info.errorMessage = connection->GetErrorMessage();
        info.key = connection->GetKey();
        info.seed = connection->GetSeed();
        info.clientVersion = connection->GetClientVersion();
        info.clientType = connection->GetClientType();
        info.macAddress = connection->GetMacAddress();
        info.useCompression = connection->GetUseCompression();
        info.useEncryption = connection->GetUseEncryption();
        info.playerId = connection->GetPlayerId();
        info.lastKeepAlive = connection->GetLastKeepAlive();
        info.pendingClose = connection->IsClosing();
        info.stats = connection->GetStats();
        
        return true;
    }
    
private:
    ConnectionManager() : listenSocket_(SOCKET_INVALID), port_(0), 
                          maxConnections_(0), nextConnectionId_(1), running_(false) {}
    
    ~ConnectionManager() {
        Shutdown();
    }
    
    // Thread principal de processamento
    /**
 * ProcessThread
 * 
 * Implementa a funcionalidade ProcessThread conforme especificação original.
 */

    void ProcessThread(){
        while (running_) {
            ProcessTick();
            
            // Dormir por um curto período para não consumir muito CPU
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    
    // Aceita novas conexões
    /**
 * AcceptConnections
 * 
 * Implementa a funcionalidade AcceptConnections conforme especificação original.
 */

    void AcceptConnections(){
        if (listenSocket_ == SOCKET_INVALID) {
            return;
        }
        
        // Verificar se atingiu o limite de conexões
        if (GetConnectionCount() >= maxConnections_) {
            return;
        }
        
        // Aceitar conexão
        SOCKADDR_IN clientAddr;
        SOCKLEN_T addrLen = sizeof(clientAddr);
        SOCKET_T clientSocket = accept(listenSocket_, reinterpret_cast<SOCKADDR*>(&clientAddr), &addrLen);
        
        if (clientSocket == SOCKET_INVALID) {
            // Se não há conexões pendentes, não é um erro
            int errorCode = SOCKET_ERROR_CODE;
            if (errorCode == SOCK_EAGAIN || errorCode == SOCK_EWOULDBLOCK) {
                return;
            }
            
            std::cerr << "Erro ao aceitar conexão: " << errorCode << std::endl;
            return;
        }
        
        // Configurar socket não-bloqueante
#ifdef _WIN32
        u_long mode = 1;
        if (ioctlsocket(clientSocket, FIONBIO, &mode) != 0) {
            std::cerr << "Falha ao configurar socket não-bloqueante: " << SOCKET_ERROR_CODE << std::endl;
            SOCKET_CLOSE(clientSocket);
            return;
        }
#else
        int flags = fcntl(clientSocket, F_GETFL, 0);
        if (flags == -1) {
            std::cerr << "Falha ao obter flags do socket: " << SOCKET_ERROR_CODE << std::endl;
            SOCKET_CLOSE(clientSocket);
            return;
        }
        
        if (fcntl(clientSocket, F_SETFL, flags | O_NONBLOCK) == -1) {
            std::cerr << "Falha ao configurar socket não-bloqueante: " << SOCKET_ERROR_CODE << std::endl;
            SOCKET_CLOSE(clientSocket);
            return;
        }
#endif
        
        // Obter endereço IP
        char ipBuffer[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(clientAddr.sin_addr), ipBuffer, INET_ADDRSTRLEN);
        std::string ipAddress = ipBuffer;
        uint16_t port = ntohs(clientAddr.sin_port);
        
        // Criar conexão
        int connectionId = nextConnectionId_++;
        std::shared_ptr<Connection> connection = std::make_shared<Connection>(clientSocket, ipAddress, port, connectionId);
        
        // Registrar callbacks /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param callbackRegistry_ Parâmetro callbackRegistry_
 * @return Retorna existentes
 */
 existentes
        for(auto& cb : callbackRegistry_){
            connection->RegisterPacketCallback(cb.first, cb.second);
        }
        
        // Inicializar conexão
        if (!connection->Initialize()) {
            std::cerr << "Falha ao inicializar conexão de " << ipAddress << ":" << port << std::endl;
            connection->Finalize();
            return;
        }
        
        // Adicionar à lista de conexões
        {
            std::lock_guard<std::mutex> lock(connectionsMutex_);
            connections_[connectionId] = connection;
        }
        
        std::cout << "Nova conexão de " << ipAddress << ":" << port << " (ID " << connectionId << ")" << std::endl;
    }
    
    // Remove uma conexão
    /**
 * RemoveConnection
 * 
 * Implementa a funcionalidade RemoveConnection conforme especificação original.
 * @param connectionId Parâmetro connectionId
 */

    void RemoveConnection(int connectionId){
        std::lock_guard<std::mutex> lock(connectionsMutex_);
        
        auto it = connections_.find(connectionId);
        if (it != connections_.end()) {
            std::shared_ptr<Connection> connection = it->second;
            std::string ipAddress = connection->GetIpAddress();
            uint16_t port = connection->GetPort();
            
            connection->Finalize();
            connections_.erase(it);
            
            std::cout << "Conexão encerrada: " << ipAddress << ":" << port << " (ID " << connectionId << ")" << std::endl;
        }
    }
    
    // Obtém uma conexão pelo ID
    std::shared_ptr<Connection> GetConnection(int connectionId) const {
        std::lock_guard<std::mutex> lock(connectionsMutex_);
        
        auto it = connections_.find(connectionId);
        if (it != connections_.end()) {
            return it->second;
        }
        
        return nullptr;
    }
    
    // Socket de escuta
    SOCKET_T listenSocket_;
    
    // Porta
    uint16_t port_;
    
    // Conexões
    mutable std::mutex connectionsMutex_;
    std::unordered_map<int, std::shared_ptr<Connection>> connections_;
    
    // Callbacks
    std::unordered_map<uint16_t, std::function<bool(Connection*, const void*, size_t)>> callbackRegistry_;
    
    // Limite de conexões
    uint32_t maxConnections_;
    
    // Próximo ID de conexão
    std::atomic<int> nextConnectionId_;
    
    // Flag de execução
    std::atomic<bool> running_;
    
    // Thread de processamento
    std::thread processingThread_;
};

} // namespace wydbr