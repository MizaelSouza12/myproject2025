/**
 * ChatManager.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/include/chat/ChatManager.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef _CHAT_MANAGER_H_
#define _CHAT_MANAGER_H_

/**
 * @file ChatManager.h
 * @brief Sistema de chat do servidor WYD - Baseado no código original
 * 
 * Implementa um sistema completo de gerenciamento de mensagens de chat,
 * incluindo chat normal, sussurros, grupos, guildas e anúncios.
 * Mantém compatibilidade com o protocolo original do WYD.
 */

#include <string>
#include <map>
#include <vector>
#include <queue>
#include <mutex>
#include <unordered_map>
#include <functional>

#include "../GlobalDef.h"
#include "../TM_Protocol.h"

namespace wydbr {

class WYDPlayer;
class Guild;

/**
 * @brief Estrutura de mensagem de chat
 */
struct ChatMessage {
    int senderId;                   // ID do remetente (0 para sistema)
    std::string senderName;         // Nome do remetente
    std::string targetName;         // Nome do destinatário (para sussurros)
    std::string message;            // Conteúdo da mensagem
    ChatType type;                  // Tipo de mensagem
    uint32_t timestamp;             // Timestamp da mensagem
    int guildId;                    // ID da guilda (para mensagens de guilda)
    int partyId;                    // ID de grupo (para mensagens de grupo)
    int channelId;                  // ID do canal (para mensagens de canal)
    
    /**
     * @brief Construtor padrão
     */
    ChatMessage() : senderId(0), type(CHAT_NORMAL), timestamp(0), 
                   guildId(0), partyId(0), channelId(0) {}
    
    /**
     * @brief Construtor completo
     */
    ChatMessage(int _senderId, const std::string& _senderName, 
                const std::string& _targetName, const std::string& _message,
                ChatType _type, uint32_t _timestamp = 0, 
                int _guildId = 0, int _partyId = 0, int _channelId = 0)
        : senderId(_senderId), senderName(_senderName), targetName(_targetName),
          message(_message), type(_type), timestamp(_timestamp),
          guildId(_guildId), partyId(_partyId), channelId(_channelId) {
        
        if (timestamp == 0) {
            timestamp = static_cast<uint32_t>(time(nullptr));
        }
    }
};

/**
 * @brief Sistema de filtro de linguagem
 */
/**
 * Classe WYDLanguageFilter
 * 
 * Esta classe implementa a funcionalidade WYDLanguageFilter conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class LanguageFilter {
public:
    /**
     * @brief Inicializa o filtro de linguagem
     * @param filePath Caminho para o arquivo de palavras proibidas
     * @return true se inicializado com sucesso
     */
    bool Initialize(const std::string& filePath);
    
    /**
     * @brief Filtra uma mensagem, substituindo palavras proibidas
     * @param message Mensagem a ser filtrada
     * @return Mensagem filtrada
     */
    std::string FilterMessage(const std::string& message);
    
    /**
     * @brief Verifica se uma mensagem contém palavras proibidas
     * @param message Mensagem a verificar
     * @return true se a mensagem contiver palavras proibidas
     */
    bool ContainsBannedWords(const std::string& message);
    
    /**
     * @brief Adiciona uma palavra ao filtro
     * @param word Palavra a adicionar
     */
    void AddBannedWord(const std::string& word);
    
    /**
     * @brief Remove uma palavra do filtro
     * @param word Palavra a remover
     */
    void RemoveBannedWord(const std::string& word);
    
private:
    std::vector<std::string> bannedWords;  // Lista de palavras proibidas
    std::mutex filterMutex;                // Mutex para acesso ao filtro
};

/**
 * @brief Gerenciador de chat global
 * 
 * Classe singleton responsável pelo gerenciamento de todas as
 * mensagens de chat no servidor WYD.
 */
/**
 * Classe WYDChatManager
 * 
 * Esta classe implementa a funcionalidade WYDChatManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class ChatManager {
public:
    /**
     * @brief Obtém a instância única do ChatManager
     * @return Referência para a instância
     */
    static ChatManager& GetInstance();

    /**
     * @brief Inicializa o gerenciador de chat
     * @param configPath Caminho para arquivo de configuração
     * @return true se inicializado com sucesso
     */
    bool Initialize(const std::string& configPath = "");

    /**
     * @brief Finaliza o gerenciador de chat
     */
    void Finalize();
    
    /**
     * @brief Processa mensagens pendentes
     * @param maxMessages Número máximo de mensagens a processar
     */
    void ProcessMessages(int maxMessages = 100);
    
    /**
     * @brief Envia uma mensagem de chat
     * @param message Mensagem a enviar
     * @return true se enviada com sucesso
     */
    bool SendMessage(const ChatMessage& message);
    
    /**
     * @brief Envia mensagem de chat local (em área)
     * @param playerId ID do jogador remetente
     * @param message Texto da mensagem
     * @param mapId ID do mapa
     * @param x Coordenada X
     * @param y Coordenada Y
     * @param radius Raio da área
     * @return true se enviada com sucesso
     */
    bool SendLocalMessage(int playerId, const std::string& message,
                         int mapId, int x, int y, int radius);
    
    /**
     * @brief Envia mensagem de sussurro
     * @param senderId ID do jogador remetente
     * @param senderName Nome do remetente
     * @param targetName Nome do destinatário
     * @param message Texto da mensagem
     * @return true se enviada com sucesso
     */
    bool SendWhisper(int senderId, const std::string& senderName,
                    const std::string& targetName, const std::string& message);
    
    /**
     * @brief Envia mensagem de grupo
     * @param playerId ID do jogador remetente
     * @param partyId ID do grupo
     * @param message Texto da mensagem
     * @return true se enviada com sucesso
     */
    bool SendPartyMessage(int playerId, int partyId, const std::string& message);
    
    /**
     * @brief Envia mensagem de guilda
     * @param playerId ID do jogador remetente
     * @param guildId ID da guilda
     * @param message Texto da mensagem
     * @return true se enviada com sucesso
     */
    bool SendGuildMessage(int playerId, int guildId, const std::string& message);
    
    /**
     * @brief Envia mensagem global
     * @param playerId ID do jogador remetente (0 para sistema)
     * @param message Texto da mensagem
     * @return true se enviada com sucesso
     */
    bool SendGlobalMessage(int playerId, const std::string& message);
    
    /**
     * @brief Envia anúncio do sistema
     * @param message Texto da mensagem
     * @param color Cor do anúncio
     * @return true se enviado com sucesso
     */
    bool SendAnnouncement(const std::string& message, uint32_t color = 0xFFFFFF);
    
    /**
     * @brief Envia mensagem do sistema para um jogador
     * @param playerId ID do jogador destinatário
     * @param message Texto da mensagem
     * @param color Cor da mensagem
     * @return true se enviada com sucesso
     */
    bool SendSystemMessage(int playerId, const std::string& message, uint32_t color = 0xFFFFFF);
    
    /**
     * @brief Envia mensagem do sistema para todos os jogadores
     * @param message Texto da mensagem
     * @param color Cor da mensagem
     * @return true se enviada com sucesso
     */
    bool SendSystemMessageToAll(const std::string& message, uint32_t color = 0xFFFFFF);
    
    /**
     * @brief Processa uma mensagem de entrada
     * @param playerId ID do jogador remetente
     * @param packet Pacote de mensagem
     * @param size Tamanho do pacote
     * @return true se processada com sucesso
     */
    bool ProcessChatPacket(int playerId, const void* packet, int size);
    
    /**
     * @brief Silencia um jogador
     * @param playerId ID do jogador
     * @param duration Duração em segundos (0 = permanente)
     * @param reason Motivo do silenciamento
     * @return true se silenciado com sucesso
     */
    bool MutePlayer(int playerId, uint32_t duration, const std::string& reason);
    
    /**
     * @brief Remove o silenciamento de um jogador
     * @param playerId ID do jogador
     * @return true se dessilenciado com sucesso
     */
    bool UnmutePlayer(int playerId);
    
    /**
     * @brief Verifica se um jogador está silenciado
     * @param playerId ID do jogador
     * @return true se o jogador estiver silenciado
     */
    bool IsPlayerMuted(int playerId);
    
    /**
     * @brief Obtém o tempo restante de silenciamento
     * @param playerId ID do jogador
     * @return Tempo restante em segundos (0 se não estiver silenciado)
     */
    uint32_t GetPlayerMuteTime(int playerId);
    
    /**
     * @brief Registra um canal de chat
     * @param channelId ID do canal
     * @param name Nome do canal
     * @param joinCallback Callback para quando um jogador entrar no canal
     * @param leaveCallback Callback para quando um jogador sair do canal
     * @param messageCallback Callback para quando uma mensagem for enviada no canal
     * @return true se registrado com sucesso
     */
    bool RegisterChannel(int channelId, const std::string& name,
                      std::function<void(int)> joinCallback,
                      std::function<void(int)> leaveCallback,
                      std::function<void(const ChatMessage&)> messageCallback);
    
    /**
     * @brief Remove um canal de chat
     * @param channelId ID do canal
     * @return true se removido com sucesso
     */
    bool UnregisterChannel(int channelId);
    
    /**
     * @brief Adiciona um jogador a um canal
     * @param playerId ID do jogador
     * @param channelId ID do canal
     * @return true se adicionado com sucesso
     */
    bool AddPlayerToChannel(int playerId, int channelId);
    
    /**
     * @brief Remove um jogador de um canal
     * @param playerId ID do jogador
     * @param channelId ID do canal
     * @return true se removido com sucesso
     */
    bool RemovePlayerFromChannel(int playerId, int channelId);
    
    /**
     * @brief Envia mensagem para um canal
     * @param playerId ID do jogador remetente
     * @param channelId ID do canal
     * @param message Texto da mensagem
     * @return true se enviada com sucesso
     */
    bool SendChannelMessage(int playerId, int channelId, const std::string& message);
    
    /**
     * @brief Define um limitador de taxa de mensagens
     * @param messageLimit Número máximo de mensagens por intervalo
     * @param interval Intervalo em segundos
     */
    void SetRateLimit(int messageLimit, int interval);
    
    /**
     * @brief Define a profundidade máxima do histórico de chat
     * @param depth Número máximo de mensagens a guardar por tipo
     */
    void SetHistoryDepth(int depth);
    
    /**
     * @brief Obtém o histórico de mensagens por tipo
     * @param type Tipo de mensagem
     * @param maxMessages Número máximo de mensagens a retornar
     * @return Vector de mensagens
     */
    std::vector<ChatMessage> GetChatHistory(ChatType type, int maxMessages = 50);
    
    /**
     * @brief Obtém o histórico de mensagens de um jogador
     * @param playerId ID do jogador
     * @param type Tipo de mensagem (CHAT_NORMAL para todos)
     * @param maxMessages Número máximo de mensagens a retornar
     * @return Vector de mensagens
     */
    std::vector<ChatMessage> GetPlayerChatHistory(int playerId, ChatType type, int maxMessages = 50);
    
    /**
     * @brief Limpa o histórico de mensagens
     * @param type Tipo de mensagem (CHAT_NORMAL para todos)
     */
    void ClearChatHistory(ChatType type = CHAT_NORMAL);
    
    /**
     * @brief Obtém o filtro de linguagem
     * @return Referência para o filtro de linguagem
     */
    LanguageFilter& GetLanguageFilter();

/**
 * ChatManager
 * 
 * Implementa a funcionalidade ChatManager conforme especificação original.
 * @return Retorna private:
 */


private:
    ChatManager();  // Construtor privado (singleton)
    ~ChatManager(); // Destrutor privado (singleton)
    
    /**
     * @brief Cria um pacote de chat
     * @param message Mensagem de chat
     * @return Pacote criado
     */
    PACKET_CHAT CreateChatPacket(const ChatMessage& message);
    
    /**
     * @brief Verifica se um jogador pode enviar mensagem
     * @param playerId ID do jogador
     * @return true se o jogador puder enviar mensagem
     */
    bool CanPlayerSendMessage(int playerId);
    
    /**
     * @brief Atualiza estatísticas de mensagens do jogador
     * @param playerId ID do jogador
     */
    void UpdatePlayerMessageStats(int playerId);
    
    /**
     * @brief Processa mensagens de comandos
     * @param playerId ID do jogador
     * @param message Texto da mensagem
     * @return true se for um comando válido
     */
    bool ProcessCommandMessage(int playerId, const std::string& message);
    
    /**
     * @brief Salva o histórico de mensagens
     * @param filePath Caminho para o arquivo
     * @return true se salvo com sucesso
     */
    bool SaveChatHistory(const std::string& filePath);
    
    // Atributos privados
    static ChatManager* instance_;                   // Instância única (singleton)
    bool initialized_;                              // Flag de inicialização
    std::queue<ChatMessage> messageQueue_;          // Fila de mensagens
    std::map<ChatType, std::vector<ChatMessage>> chatHistory_; // Histórico por tipo
    std::map<int, std::vector<ChatMessage>> playerChatHistory_; // Histórico por jogador
    std::map<int, uint32_t> mutedPlayers_;          // Jogadores silenciados (ID -> timestamp de fim)
    std::map<int, std::pair<int, uint32_t>> messageStats_; // Estatísticas de mensagens por jogador (ID -> (contagem, timestamp))
    std::map<int, std::string> channels_;           // Canais de chat (ID -> nome)
    std::map<int, std::vector<int>> channelPlayers_; // Jogadores por canal (ID do canal -> IDs de jogadores)
    
    // Callbacks para canais
    std::map<int, std::function<void(int)>> channelJoinCallbacks_;
    std::map<int, std::function<void(int)>> channelLeaveCallbacks_;
    std::map<int, std::function<void(const ChatMessage&)>> channelMessageCallbacks_;
    
    // Configurações
    int historyDepth_;                             // Profundidade do histórico
    int messageRateLimit_;                         // Limite de taxa de mensagens
    int messageRateInterval_;                      // Intervalo para o limite de taxa
    int maxMessageLength_;                         // Tamanho máximo de mensagem
    bool filterEnabled_;                           // Filtro de linguagem habilitado
    
    // Filtro de linguagem
    LanguageFilter languageFilter_;                // Filtro de linguagem
    
    // Mutex para proteção de dados
    std::mutex chatMutex_;                         // Mutex para acesso aos dados de chat
    
    // Desabilita cópias (singleton)
    ChatManager(const ChatManager&) = delete;
    ChatManager& operator=(const ChatManager&) = delete;
};

} // namespace wydbr

#endif // _CHAT_MANAGER_H_