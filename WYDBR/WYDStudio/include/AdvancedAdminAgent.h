#pragma once

#include "NaturalLanguageProcessor.h"
#include <string>
#include <memory>
#include <vector>
#include <functional>
#include <map>

// Forward declarations
namespace WYDStudio {
    class GameKnowledgeBase;
    class WYDSpecificKnowledge;
}

namespace WYDStudio {

/**
 * @enum AdminActionType
 * @brief Tipos de ações administrativas
 */
enum class AdminActionType {
    UNKNOWN,
    ITEM_MANAGEMENT,        // Criar, modificar, remover itens
    PLAYER_MANAGEMENT,      // Banir, silenciar, teleportar jogadores
    EVENT_MANAGEMENT,       // Iniciar, modificar, encerrar eventos
    SERVER_MANAGEMENT,      // Controlar servidor, reiniciar, manutenção
    MAP_MANAGEMENT,         // Modificar mapas, áreas, territórios
    GUILD_MANAGEMENT,       // Gerenciar guildas, alianças, guerras
    ECONOMY_MANAGEMENT,     // Economia, drops, taxas de experiência
    QUERY_EXECUTION,        // Consultas informativas
    SYSTEM_CONFIGURATION    // Configuração de parâmetros do sistema
};

/**
 * @struct AdminAction
 * @brief Ação administrativa identificada
 */
struct AdminAction {
    AdminActionType type;                   // Tipo da ação
    std::string actionName;                 // Nome da ação
    std::map<std::string, std::string> params; // Parâmetros da ação
    std::string originalText;               // Texto original do comando
    float confidence;                       // Confiança na identificação (0.0-1.0)
    bool requiresConfirmation;              // Se requer confirmação adicional
    
    // Métodos de conveniência
    bool HasParam(const std::string& key) const {
        return params.find(key) != params.end();
    }
    
    std::string GetParam(const std::string& key, const std::string& defaultValue = "") const {
        auto it = params.find(key);
        return it != params.end() ? it->second : defaultValue;
    }
};

/**
 * @struct ActionResponse
 * @brief Resposta a uma ação administrativa
 */
struct ActionResponse {
    bool success;                       // Se a ação foi executada com sucesso
    std::string message;                // Mensagem de retorno
    std::map<std::string, std::string> data; // Dados adicionais
};

/**
 * @typedef ActionHandler
 * @brief Tipo para manipulador de ação
 */
using ActionHandler = std::function<ActionResponse(const AdminAction&)>;

/**
 * @typedef LogCallback
 * @brief Tipo para callback de log
 */
using LogCallback = std::function<void(const std::string&)>;

/**
 * @class AdvancedAdminAgent
 * @brief Agente administrativo avançado com processamento de linguagem natural
 */
class AdvancedAdminAgent {
public:
    /**
     * @brief Construtor
     * @param nlProcessor Processador de linguagem natural
     * @param gameKnowledge Base de conhecimento específico do jogo
     */
    AdvancedAdminAgent(
        std::shared_ptr<NaturalLanguageProcessor> nlProcessor,
        std::shared_ptr<WYDSpecificKnowledge> gameKnowledge);
    
    ~AdvancedAdminAgent();
    
    /**
     * @brief Inicializa o agente
     * @param configPath Caminho opcional para configuração
     * @return True se inicializado com sucesso
     */
    bool Initialize(const std::string& configPath = "");
    
    /**
     * @brief Define callback para logs
     * @param callback Função de callback
     */
    void SetLogCallback(LogCallback callback);
    
    /**
     * @brief Processa uma consulta em linguagem natural
     * @param query Consulta em texto
     * @param userId ID do usuário opcional
     * @return Resposta à consulta
     */
    std::string ProcessQuery(const std::string& query, const std::string& userId = "");
    
    /**
     * @brief Registra um manipulador para um tipo de ação
     * @param actionType Tipo de ação
     * @param handler Manipulador de ação
     * @return True se registrado com sucesso
     */
    bool RegisterActionHandler(AdminActionType actionType, ActionHandler handler);
    
    /**
     * @brief Executa uma ação administrativa
     * @param action Ação a executar
     * @return Resposta à ação
     */
    ActionResponse ExecuteAction(const AdminAction& action);
    
    /**
     * @brief Obtém sugestões de comando com base na entrada atual
     * @param currentInput Entrada atual
     * @param maxSuggestions Número máximo de sugestões
     * @return Lista de sugestões
     */
    std::vector<std::string> GetCommandSuggestions(
        const std::string& currentInput,
        int maxSuggestions = 5);
    
    /**
     * @brief Exporta estatísticas de uso de comandos
     * @param filePath Caminho para o arquivo
     * @return True se exportado com sucesso
     */
    bool ExportCommandStatistics(const std::string& filePath) const;
    
    /**
     * @brief Verifica se um usuário tem permissão para executar uma ação
     * @param userId ID do usuário
     * @param actionType Tipo de ação
     * @return True se tem permissão
     */
    bool UserHasPermission(const std::string& userId, AdminActionType actionType) const;
    
    /**
     * @brief Registra uma ação no histórico do usuário
     * @param userId ID do usuário
     * @param action Ação executada
     * @param response Resposta à ação
     */
    void LogUserAction(
        const std::string& userId,
        const AdminAction& action,
        const ActionResponse& response);
    
private:
    // Tipos internos
    struct CommandPattern;
    struct UserContext;
    struct ActionStatistics;
    
    // Componentes principais
    std::shared_ptr<NaturalLanguageProcessor> m_nlProcessor;
    std::shared_ptr<WYDSpecificKnowledge> m_gameKnowledge;
    
    // Manipuladores de ações
    std::map<AdminActionType, ActionHandler> m_actionHandlers;
    
    // Contextos de usuário
    std::map<std::string, std::shared_ptr<UserContext>> m_userContexts;
    
    // Padrões de comando
    std::vector<std::shared_ptr<CommandPattern>> m_commandPatterns;
    
    // Estatísticas de uso
    std::shared_ptr<ActionStatistics> m_statistics;
    
    // Callback de log
    LogCallback m_logCallback;
    
    // Implementações privadas
    void Log(const std::string& message);
    AdminAction IdentifyAction(const std::string& query, const std::string& userId);
    void InitializeDefaultActionHandlers();
    void InitializeDefaultCommandPatterns();
    bool MatchCommandPattern(const std::string& text, const std::string& pattern) const;
    std::string ResolveEntities(const std::string& text) const;
    void UpdateUserContext(const std::string& userId, const AdminAction& action, const ActionResponse& response);
    std::string FormatActionResponse(const ActionResponse& response) const;
    void TrackCommandUsage(const AdminAction& action);
};

} // namespace WYDStudio