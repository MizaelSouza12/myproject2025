/**
 * NetworkStateManager.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/network/NetworkStateManager.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef NETWORKSTATEMANAGER_H
#define NETWORKSTATEMANAGER_H

#include <map>
#include <vector>
#include <string>
#include <mutex>
#include <functional>
#include <memory>
#include <queue>

#include "../../../core/WYDTypes.h"
#include "protocol/PacketTypes.h"
#include "ConnectionManager.h"

namespace wydbr {


/**
 * @file NetworkStateManager.h
 * @brief Gerenciador de estado de rede
 * 
 * Este arquivo contém a definição do gerenciador de estado de rede,
 * que é responsável por gerenciar os estados de conexão e a máquina
 * de estados do protocolo do WYD.
 * No WYD original, essa funcionalidade estava espalhada em várias funções
 * e havia muitos bugs e brechas de segurança.
 */

namespace wyd {
namespace server {
namespace network {

// Forward declarations
class CConnectionManager;
class CConnection;
class CPacket;

/**
 * @brief Estado de rede
 * 
 * Esta classe representa um estado na máquina de estados de rede.
 */
/**
 * Classe WYDNetworkState
 * 
 * Esta classe implementa a funcionalidade WYDNetworkState conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
/**
 * Classe WYDNetworkState
 * 
 * Esta classe implementa a funcionalidade WYDNetworkState conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class CNetworkState {
public:
    /**
     * @brief Construtor
     * @param type Tipo de estado
     */
    CNetworkState(ConnectionState type);
    
    /**
     * @brief Destrutor virtual
     */
    virtual ~CNetworkState();
    
    /**
     * @brief Obtém o tipo de estado
     * @return Tipo de estado
     */
    ConnectionState GetType() const;
    
    /**
     * @brief Entrada no estado
     * @param connection Conexão
     * @param prevState Estado anterior
     */
    virtual void OnEnter(std::shared_ptr<CConnection> connection, ConnectionState prevState) = 0;
    
    /**
     * @brief Saída do estado
     * @param connection Conexão
     * @param nextState Próximo estado
     */
    virtual void OnExit(std::shared_ptr<CConnection> connection, ConnectionState nextState) = 0;
    
    /**
     * @brief Atualização do estado
     * @param connection Conexão
     * @param elapsed Tempo decorrido
     */
    virtual void OnUpdate(std::shared_ptr<CConnection> connection, DWORD elapsed) = 0;
    
    /**
     * @brief Processamento de pacote
     * @param connection Conexão
     * @param packet Pacote
     * @return Próximo estado, ou INVALID_STATE para manter o estado atual
     */
    virtual ConnectionState OnPacket(std::shared_ptr<CConnection> connection, std::shared_ptr<CPacket> packet) = 0;

protected:
    ConnectionState type_;      // Tipo de estado
};

/**
 * @brief Estado desconectado
 */
/**
 * Classe WYDDisconnectedState
 * 
 * Esta classe implementa a funcionalidade WYDDisconnectedState conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
/**
 * Classe WYDDisconnectedState
 * 
 * Esta classe implementa a funcionalidade WYDDisconnectedState conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class CDisconnectedState : public CNetworkState {
/**
 * CDisconnectedState
 * 
 * Implementa a funcionalidade CDisconnectedState conforme especificação original.
 * @return Retorna public:
 */

public:
    CDisconnectedState();
    virtual ~CDisconnectedState();
    
    /**
 * OnEnter
 * 
 * Implementa a funcionalidade OnEnter conforme especificação original.
 * @param connection Parâmetro connection
 * @param prevState Parâmetro prevState
 */

    
    virtual void OnEnter(std::shared_ptr<CConnection> connection, ConnectionState prevState) override;
    /**
 * OnExit
 * 
 * Implementa a funcionalidade OnExit conforme especificação original.
 * @param connection Parâmetro connection
 * @param nextState Parâmetro nextState
 */

    virtual void OnExit(std::shared_ptr<CConnection> connection, ConnectionState nextState) override;
    /**
 * OnUpdate
 * 
 * Implementa a funcionalidade OnUpdate conforme especificação original.
 * @param connection Parâmetro connection
 * @param elapsed Parâmetro elapsed
 */

    virtual void OnUpdate(std::shared_ptr<CConnection> connection, DWORD elapsed) override;
    /**
 * OnPacket
 * 
 * Implementa a funcionalidade OnPacket conforme especificação original.
 * @param connection Parâmetro connection
 * @param packet Parâmetro packet
 * @return Retorna ConnectionState
 */

    virtual ConnectionState OnPacket(std::shared_ptr<CConnection> connection, std::shared_ptr<CPacket> packet) override;
};

/**
 * @brief Estado conectando
 */
/**
 * Classe WYDConnectingState
 * 
 * Esta classe implementa a funcionalidade WYDConnectingState conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
/**
 * Classe WYDConnectingState
 * 
 * Esta classe implementa a funcionalidade WYDConnectingState conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class CConnectingState : public CNetworkState {
/**
 * CConnectingState
 * 
 * Implementa a funcionalidade CConnectingState conforme especificação original.
 * @return Retorna public:
 */

public:
    CConnectingState();
    virtual ~CConnectingState();
    
    /**
 * OnEnter
 * 
 * Implementa a funcionalidade OnEnter conforme especificação original.
 * @param connection Parâmetro connection
 * @param prevState Parâmetro prevState
 */

    
    virtual void OnEnter(std::shared_ptr<CConnection> connection, ConnectionState prevState) override;
    /**
 * OnExit
 * 
 * Implementa a funcionalidade OnExit conforme especificação original.
 * @param connection Parâmetro connection
 * @param nextState Parâmetro nextState
 */

    virtual void OnExit(std::shared_ptr<CConnection> connection, ConnectionState nextState) override;
    /**
 * OnUpdate
 * 
 * Implementa a funcionalidade OnUpdate conforme especificação original.
 * @param connection Parâmetro connection
 * @param elapsed Parâmetro elapsed
 */

    virtual void OnUpdate(std::shared_ptr<CConnection> connection, DWORD elapsed) override;
    /**
 * OnPacket
 * 
 * Implementa a funcionalidade OnPacket conforme especificação original.
 * @param connection Parâmetro connection
 * @param packet Parâmetro packet
 * @return Retorna ConnectionState
 */

    virtual ConnectionState OnPacket(std::shared_ptr<CConnection> connection, std::shared_ptr<CPacket> packet) override;
};

/**
 * @brief Estado conectado
 */
/**
 * Classe WYDConnectedState
 * 
 * Esta classe implementa a funcionalidade WYDConnectedState conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
/**
 * Classe WYDConnectedState
 * 
 * Esta classe implementa a funcionalidade WYDConnectedState conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class CConnectedState : public CNetworkState {
/**
 * CConnectedState
 * 
 * Implementa a funcionalidade CConnectedState conforme especificação original.
 * @return Retorna public:
 */

public:
    CConnectedState();
    virtual ~CConnectedState();
    
    /**
 * OnEnter
 * 
 * Implementa a funcionalidade OnEnter conforme especificação original.
 * @param connection Parâmetro connection
 * @param prevState Parâmetro prevState
 */

    
    virtual void OnEnter(std::shared_ptr<CConnection> connection, ConnectionState prevState) override;
    /**
 * OnExit
 * 
 * Implementa a funcionalidade OnExit conforme especificação original.
 * @param connection Parâmetro connection
 * @param nextState Parâmetro nextState
 */

    virtual void OnExit(std::shared_ptr<CConnection> connection, ConnectionState nextState) override;
    /**
 * OnUpdate
 * 
 * Implementa a funcionalidade OnUpdate conforme especificação original.
 * @param connection Parâmetro connection
 * @param elapsed Parâmetro elapsed
 */

    virtual void OnUpdate(std::shared_ptr<CConnection> connection, DWORD elapsed) override;
    /**
 * OnPacket
 * 
 * Implementa a funcionalidade OnPacket conforme especificação original.
 * @param connection Parâmetro connection
 * @param packet Parâmetro packet
 * @return Retorna ConnectionState
 */

    virtual ConnectionState OnPacket(std::shared_ptr<CConnection> connection, std::shared_ptr<CPacket> packet) override;
};

/**
 * @brief Estado autenticando
 */
/**
 * Classe WYDAuthenticatingState
 * 
 * Esta classe implementa a funcionalidade WYDAuthenticatingState conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
/**
 * Classe WYDAuthenticatingState
 * 
 * Esta classe implementa a funcionalidade WYDAuthenticatingState conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class CAuthenticatingState : public CNetworkState {
/**
 * CAuthenticatingState
 * 
 * Implementa a funcionalidade CAuthenticatingState conforme especificação original.
 * @return Retorna public:
 */

public:
    CAuthenticatingState();
    virtual ~CAuthenticatingState();
    
    /**
 * OnEnter
 * 
 * Implementa a funcionalidade OnEnter conforme especificação original.
 * @param connection Parâmetro connection
 * @param prevState Parâmetro prevState
 */

    
    virtual void OnEnter(std::shared_ptr<CConnection> connection, ConnectionState prevState) override;
    /**
 * OnExit
 * 
 * Implementa a funcionalidade OnExit conforme especificação original.
 * @param connection Parâmetro connection
 * @param nextState Parâmetro nextState
 */

    virtual void OnExit(std::shared_ptr<CConnection> connection, ConnectionState nextState) override;
    /**
 * OnUpdate
 * 
 * Implementa a funcionalidade OnUpdate conforme especificação original.
 * @param connection Parâmetro connection
 * @param elapsed Parâmetro elapsed
 */

    virtual void OnUpdate(std::shared_ptr<CConnection> connection, DWORD elapsed) override;
    /**
 * OnPacket
 * 
 * Implementa a funcionalidade OnPacket conforme especificação original.
 * @param connection Parâmetro connection
 * @param packet Parâmetro packet
 * @return Retorna ConnectionState
 */

    virtual ConnectionState OnPacket(std::shared_ptr<CConnection> connection, std::shared_ptr<CPacket> packet) override;
};

/**
 * @brief Estado autenticado
 */
/**
 * Classe WYDAuthenticatedState
 * 
 * Esta classe implementa a funcionalidade WYDAuthenticatedState conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
/**
 * Classe WYDAuthenticatedState
 * 
 * Esta classe implementa a funcionalidade WYDAuthenticatedState conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class CAuthenticatedState : public CNetworkState {
/**
 * CAuthenticatedState
 * 
 * Implementa a funcionalidade CAuthenticatedState conforme especificação original.
 * @return Retorna public:
 */

public:
    CAuthenticatedState();
    virtual ~CAuthenticatedState();
    
    /**
 * OnEnter
 * 
 * Implementa a funcionalidade OnEnter conforme especificação original.
 * @param connection Parâmetro connection
 * @param prevState Parâmetro prevState
 */

    
    virtual void OnEnter(std::shared_ptr<CConnection> connection, ConnectionState prevState) override;
    /**
 * OnExit
 * 
 * Implementa a funcionalidade OnExit conforme especificação original.
 * @param connection Parâmetro connection
 * @param nextState Parâmetro nextState
 */

    virtual void OnExit(std::shared_ptr<CConnection> connection, ConnectionState nextState) override;
    /**
 * OnUpdate
 * 
 * Implementa a funcionalidade OnUpdate conforme especificação original.
 * @param connection Parâmetro connection
 * @param elapsed Parâmetro elapsed
 */

    virtual void OnUpdate(std::shared_ptr<CConnection> connection, DWORD elapsed) override;
    /**
 * OnPacket
 * 
 * Implementa a funcionalidade OnPacket conforme especificação original.
 * @param connection Parâmetro connection
 * @param packet Parâmetro packet
 * @return Retorna ConnectionState
 */

    virtual ConnectionState OnPacket(std::shared_ptr<CConnection> connection, std::shared_ptr<CPacket> packet) override;
};

/**
 * @brief Estado entrando no mundo
 */
/**
 * Classe WYDEnteringWorldState
 * 
 * Esta classe implementa a funcionalidade WYDEnteringWorldState conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
/**
 * Classe WYDEnteringWorldState
 * 
 * Esta classe implementa a funcionalidade WYDEnteringWorldState conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class CEnteringWorldState : public CNetworkState {
/**
 * CEnteringWorldState
 * 
 * Implementa a funcionalidade CEnteringWorldState conforme especificação original.
 * @return Retorna public:
 */

public:
    CEnteringWorldState();
    virtual ~CEnteringWorldState();
    
    /**
 * OnEnter
 * 
 * Implementa a funcionalidade OnEnter conforme especificação original.
 * @param connection Parâmetro connection
 * @param prevState Parâmetro prevState
 */

    
    virtual void OnEnter(std::shared_ptr<CConnection> connection, ConnectionState prevState) override;
    /**
 * OnExit
 * 
 * Implementa a funcionalidade OnExit conforme especificação original.
 * @param connection Parâmetro connection
 * @param nextState Parâmetro nextState
 */

    virtual void OnExit(std::shared_ptr<CConnection> connection, ConnectionState nextState) override;
    /**
 * OnUpdate
 * 
 * Implementa a funcionalidade OnUpdate conforme especificação original.
 * @param connection Parâmetro connection
 * @param elapsed Parâmetro elapsed
 */

    virtual void OnUpdate(std::shared_ptr<CConnection> connection, DWORD elapsed) override;
    /**
 * OnPacket
 * 
 * Implementa a funcionalidade OnPacket conforme especificação original.
 * @param connection Parâmetro connection
 * @param packet Parâmetro packet
 * @return Retorna ConnectionState
 */

    virtual ConnectionState OnPacket(std::shared_ptr<CConnection> connection, std::shared_ptr<CPacket> packet) override;
};

/**
 * @brief Estado no jogo
 */
/**
 * Classe WYDInGameState
 * 
 * Esta classe implementa a funcionalidade WYDInGameState conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
/**
 * Classe WYDInGameState
 * 
 * Esta classe implementa a funcionalidade WYDInGameState conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class CInGameState : public CNetworkState {
/**
 * CInGameState
 * 
 * Implementa a funcionalidade CInGameState conforme especificação original.
 * @return Retorna public:
 */

public:
    CInGameState();
    virtual ~CInGameState();
    
    /**
 * OnEnter
 * 
 * Implementa a funcionalidade OnEnter conforme especificação original.
 * @param connection Parâmetro connection
 * @param prevState Parâmetro prevState
 */

    
    virtual void OnEnter(std::shared_ptr<CConnection> connection, ConnectionState prevState) override;
    /**
 * OnExit
 * 
 * Implementa a funcionalidade OnExit conforme especificação original.
 * @param connection Parâmetro connection
 * @param nextState Parâmetro nextState
 */

    virtual void OnExit(std::shared_ptr<CConnection> connection, ConnectionState nextState) override;
    /**
 * OnUpdate
 * 
 * Implementa a funcionalidade OnUpdate conforme especificação original.
 * @param connection Parâmetro connection
 * @param elapsed Parâmetro elapsed
 */

    virtual void OnUpdate(std::shared_ptr<CConnection> connection, DWORD elapsed) override;
    /**
 * OnPacket
 * 
 * Implementa a funcionalidade OnPacket conforme especificação original.
 * @param connection Parâmetro connection
 * @param packet Parâmetro packet
 * @return Retorna ConnectionState
 */

    virtual ConnectionState OnPacket(std::shared_ptr<CConnection> connection, std::shared_ptr<CPacket> packet) override;
};

/**
 * @brief Estado desconectando
 */
/**
 * Classe WYDDisconnectingState
 * 
 * Esta classe implementa a funcionalidade WYDDisconnectingState conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
/**
 * Classe WYDDisconnectingState
 * 
 * Esta classe implementa a funcionalidade WYDDisconnectingState conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class CDisconnectingState : public CNetworkState {
/**
 * CDisconnectingState
 * 
 * Implementa a funcionalidade CDisconnectingState conforme especificação original.
 * @return Retorna public:
 */

public:
    CDisconnectingState();
    virtual ~CDisconnectingState();
    
    /**
 * OnEnter
 * 
 * Implementa a funcionalidade OnEnter conforme especificação original.
 * @param connection Parâmetro connection
 * @param prevState Parâmetro prevState
 */

    
    virtual void OnEnter(std::shared_ptr<CConnection> connection, ConnectionState prevState) override;
    /**
 * OnExit
 * 
 * Implementa a funcionalidade OnExit conforme especificação original.
 * @param connection Parâmetro connection
 * @param nextState Parâmetro nextState
 */

    virtual void OnExit(std::shared_ptr<CConnection> connection, ConnectionState nextState) override;
    /**
 * OnUpdate
 * 
 * Implementa a funcionalidade OnUpdate conforme especificação original.
 * @param connection Parâmetro connection
 * @param elapsed Parâmetro elapsed
 */

    virtual void OnUpdate(std::shared_ptr<CConnection> connection, DWORD elapsed) override;
    /**
 * OnPacket
 * 
 * Implementa a funcionalidade OnPacket conforme especificação original.
 * @param connection Parâmetro connection
 * @param packet Parâmetro packet
 * @return Retorna ConnectionState
 */

    virtual ConnectionState OnPacket(std::shared_ptr<CConnection> connection, std::shared_ptr<CPacket> packet) override;
};

/**
 * @brief Estado reconectando
 */
/**
 * Classe WYDReconnectingState
 * 
 * Esta classe implementa a funcionalidade WYDReconnectingState conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
/**
 * Classe WYDReconnectingState
 * 
 * Esta classe implementa a funcionalidade WYDReconnectingState conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class CReconnectingState : public CNetworkState {
/**
 * CReconnectingState
 * 
 * Implementa a funcionalidade CReconnectingState conforme especificação original.
 * @return Retorna public:
 */

public:
    CReconnectingState();
    virtual ~CReconnectingState();
    
    /**
 * OnEnter
 * 
 * Implementa a funcionalidade OnEnter conforme especificação original.
 * @param connection Parâmetro connection
 * @param prevState Parâmetro prevState
 */

    
    virtual void OnEnter(std::shared_ptr<CConnection> connection, ConnectionState prevState) override;
    /**
 * OnExit
 * 
 * Implementa a funcionalidade OnExit conforme especificação original.
 * @param connection Parâmetro connection
 * @param nextState Parâmetro nextState
 */

    virtual void OnExit(std::shared_ptr<CConnection> connection, ConnectionState nextState) override;
    /**
 * OnUpdate
 * 
 * Implementa a funcionalidade OnUpdate conforme especificação original.
 * @param connection Parâmetro connection
 * @param elapsed Parâmetro elapsed
 */

    virtual void OnUpdate(std::shared_ptr<CConnection> connection, DWORD elapsed) override;
    /**
 * OnPacket
 * 
 * Implementa a funcionalidade OnPacket conforme especificação original.
 * @param connection Parâmetro connection
 * @param packet Parâmetro packet
 * @return Retorna ConnectionState
 */

    virtual ConnectionState OnPacket(std::shared_ptr<CConnection> connection, std::shared_ptr<CPacket> packet) override;
};

/**
 * @brief Gerenciador de estado de rede
 * 
 * Esta classe gerencia a máquina de estados de rede para todas as conexões.
 */
/**
 * Classe WYDNetworkStateManager
 * 
 * Esta classe implementa a funcionalidade WYDNetworkStateManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
/**
 * Classe WYDNetworkStateManager
 * 
 * Esta classe implementa a funcionalidade WYDNetworkStateManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class CNetworkStateManager {
public:
    /**
     * @brief Construtor
     * @param connectionManager Gerenciador de conexões
     */
    CNetworkStateManager(CConnectionManager& connectionManager);
    
    /**
     * @brief Destrutor
     */
    ~CNetworkStateManager();
    
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
     * @param elapsed Tempo decorrido
     */
    void Update(DWORD elapsed);
    
    /**
     * @brief Processa um pacote
     * @param connection Conexão
     * @param packet Pacote
     * @return true se processado com sucesso
     */
    bool ProcessPacket(std::shared_ptr<CConnection> connection, std::shared_ptr<CPacket> packet);
    
    /**
     * @brief Define o estado de uma conexão
     * @param connection Conexão
     * @param state Estado
     * @return true se definido com sucesso
     */
    bool SetConnectionState(std::shared_ptr<CConnection> connection, ConnectionState state);
    
    /**
     * @brief Adiciona uma transição de estado
     * @param currentState Estado atual
     * @param packet Tipo de pacote
     * @param nextState Próximo estado
     */
    void AddStateTransition(ConnectionState currentState, PacketType packet, ConnectionState nextState);
    
    /**
     * @brief Remove uma transição de estado
     * @param currentState Estado atual
     * @param packet Tipo de pacote
     */
    void RemoveStateTransition(ConnectionState currentState, PacketType packet);
    
    /**
     * @brief Registra um handler para um tipo de pacote
     * @param type Tipo de pacote
     * @param handler Handler
     */
    void RegisterPacketHandler(PacketType type, PacketHandler handler);
    
    /**
     * @brief Remove um handler para um tipo de pacote
     * @param type Tipo de pacote
     */
    void UnregisterPacketHandler(PacketType type);
    
    /**
     * @brief Verifica se um pacote é permitido no estado atual
     * @param state Estado atual
     * @param packet Tipo de pacote
     * @return true se permitido
     */
    bool IsPacketAllowedInState(ConnectionState state, PacketType packet);
    
    /**
     * @brief Adiciona um tipo de pacote permitido em um estado
     * @param state Estado
     * @param packet Tipo de pacote
     */
    void AddAllowedPacket(ConnectionState state, PacketType packet);
    
    /**
     * @brief Remove um tipo de pacote permitido em um estado
     * @param state Estado
     * @param packet Tipo de pacote
     */
    void RemoveAllowedPacket(ConnectionState state, PacketType packet);
    
    /**
     * @brief Obtém o estado de uma conexão
     * @param connection Conexão
     * @return Estado
     */
    CNetworkState* GetState(std::shared_ptr<CConnection> connection);
    
    /**
     * @brief Cria um pacote
     * @param type Tipo de pacote
     * @param data Dados do pacote
     * @param dataSize Tamanho dos dados
     * @param encryptSecure Se deve encriptar com segurança adicional
     * @return Pacote criado
     */
    std::shared_ptr<CPacket> CreatePacket(PacketType type, const BYTE* data = nullptr, int dataSize = 0, bool encryptSecure = false);
    
    /**
     * @brief Formata dados para uma estrutura de pacote
     * @param packet Referência para o pacote
     * @param data Dados a adicionar
     */
    template<typename T>
    /**
 * FormatPacketData
 * 
 * Implementa a funcionalidade FormatPacketData conforme especificação original.
 * @param packet Parâmetro packet
 * @param data Parâmetro data
 */

    void FormatPacketData(std::shared_ptr<CPacket>& packet, const T& data){
        // Verifica se o pacote é vá/**
 * if
 * 
 * Implementa a funcionalidade if conforme especificação original.
 * @param !packet Parâmetro !packet
 * @return Retorna lido
 */
lido
        if(!packet){
            return;
        }
        
        // Calcula o tamanho total do pacote
        int totalSize = sizeof(STRUCT_PACKET_HEADER) + sizeof(T);
        
        // Redimensiona o pacote para comportar os dados
        packet->SetData(reinterpret_cast<const BYTE*>(&data), sizeof(T));
        
        // Atualiza o tamanho no cabeçalho
        packet->GetHeader().wSize = totalSize;
    }
    
private:
    /**
     * @brief Cria e registra um estado
     * @param state Estado a registrar
     * @return Estado criado
     */
    template<typename T>
    /**
 * RegisterState
 * 
 * Implementa a funcionalidade RegisterState conforme especificação original.
 * @return Retorna T
 */

    T* RegisterState(){
        T* state = /**
 * T
 * 
 * Implementa a funcionalidade T conforme especificação original.
 * @return Retorna new
 */
 new T();
        states_[state->GetType()] = state;
        return state;
    }
    
    /**
     * @brief Envia uma notificação de erro
     * @param connection Conexão
     * @param errorCode Código de erro
     * @param message Mensagem de erro
     */
    void SendErrorNotification(std::shared_ptr<CConnection> connection, WORD errorCode, const std::string& message);
    
    /**
     * @brief Inicializa as transições de estado padrão
     */
    void InitializeDefaultTransitions();
    
    /**
     * @brief Inicializa os pacotes permitidos por estado
     */
    void InitializeAllowedPackets();
    
    // Gerenciador de conexões
    CConnectionManager& connectionManager_;
    
    // Estados
    std::map<ConnectionState, CNetworkState*> states_;
    
    // Transições de estado
    struct StateTransition {
        ConnectionState fromState;    // Estado de origem
        PacketType packetType;        // Tipo de pacote
        ConnectionState toState;      // Estado de destino
    };
    std::vector<StateTransition> transitions_;
    
    // Pacotes permitidos por estado
    std::map<ConnectionState, std::set<PacketType>> allowedPackets_;
    
    // Mutex para transições
    std::mutex transitionMutex_;
    
    // Handlers específicos para pacotes
    std::map<PacketType, PacketHandler> packetHandlers_;
    std::mutex handlersMutex_;
};

} // namespace network
} // namespace server
} // namespace wyd

#endif // NETWORKSTATEMANAGER_H

} // namespace wydbr
