#include "../include/AdvancedAdminAgent.h"
#include "../include/NaturalLanguageProcessor.h"
#include "../include/WYDSpecificKnowledge.h"
#include "../include/GameStateMonitor.h"
#include "../include/SecurityManager.h"
#include "../include/AgentTrainer.h"

#include <sstream>
#include <iomanip>
#include <algorithm>
#include <random>
#include <chrono>
#include <thread>
#include <functional>
#include <future>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace WYDStudio {

// Implementação da classe AdvancedAdminAgent

AdvancedAdminAgent::AdvancedAdminAgent(
    std::shared_ptr<NaturalLanguageProcessor> nlProcessor,
    std::shared_ptr<WYDSpecificKnowledge> knowledgeBase)
    : m_nlProcessor(nlProcessor), 
      m_knowledgeBase(knowledgeBase),
      m_isActive(false),
      m_currentMode(AdminAgentMode::NORMAL),
      m_securityClearance(5),  // Nível máximo de segurança para admin
      m_stopBackgroundThread(false)
{
    // Inicializar capacidades
    m_capabilities.push_back(AgentCapability::FILE_ACCESS);
    m_capabilities.push_back(AgentCapability::DATABASE_ACCESS);
    m_capabilities.push_back(AgentCapability::NETWORK_ACCESS);
    m_capabilities.push_back(AgentCapability::GAME_KNOWLEDGE);
    m_capabilities.push_back(AgentCapability::PLAYER_INTERACTION);
    m_capabilities.push_back(AgentCapability::SCRIPT_EXECUTION);
    
    // Inicializar estatísticas
    m_statistics.totalQueries = 0;
    m_statistics.successfulActions = 0;
    m_statistics.failedActions = 0;
    
    // Gerar ID de sessão único
    auto now = std::chrono::system_clock::now();
    auto nowTime = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << "admin_" << std::put_time(std::localtime(&nowTime), "%Y%m%d_%H%M%S_");
    
    // Adicionar números aleatórios ao ID de sessão
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(1000, 9999);
    ss << distrib(gen);
    
    m_sessionId = ss.str();
}

AdvancedAdminAgent::~AdvancedAdminAgent() {
    // Parar a thread de processamento em segundo plano
    m_stopBackgroundThread = true;
    if (m_backgroundThread.valid()) {
        try {
            m_backgroundThread.wait();
        } catch (...) {
            // Ignorar erros durante o encerramento
        }
    }
    
    // Exportar dados de aprendizado e estatísticas
    try {
        ExportLearningData("./admin_agent_learning_data_" + m_sessionId + ".json");
    } catch (...) {
        // Ignorar erros durante o encerramento
    }
}

bool AdvancedAdminAgent::Initialize(const std::string& modelPath) {
    Log("Inicializando AdvancedAdminAgent...");
    
    if (!m_nlProcessor) {
        Log("Erro: Processador de linguagem natural não fornecido");
        return false;
    }
    
    if (!m_knowledgeBase) {
        Log("Erro: Base de conhecimento não fornecida");
        return false;
    }
    
    // Inicializar operações padrão do agente
    InitializeDefaultOperations();
    
    // Iniciar thread de processamento em segundo plano
    m_stopBackgroundThread = false;
    m_backgroundThread = std::async(std::launch::async, 
                                   &AdvancedAdminAgent::BackgroundProcessingThread, 
                                   this);
    
    m_isActive = true;
    m_initialized = true;
    Log("AdvancedAdminAgent inicializado com sucesso. ID de sessão: " + m_sessionId);
    
    return true;
}

std::string AdvancedAdminAgent::ProcessQuery(const std::string& query) {
    if (!m_initialized) {
        return "Agente não inicializado";
    }
    
    if (!m_isActive) {
        return "Agente desativado temporariamente. Modo atual: " + 
               std::to_string(static_cast<int>(m_currentMode));
    }
    
    Log("Processando consulta: '" + query + "'");
    
    // Incrementar contador de consultas
    std::lock_guard<std::mutex> statLock(m_statsMutex);
    m_statistics.totalQueries++;
    
    // Pré-processar o comando para o contexto atual
    std::string processedQuery = PreprocessCommand(query, m_sessionId);
    
    // Verificar se é uma consulta de conhecimento do jogo
    if (query.find("?") != std::string::npos || 
        query.find("quanto") != std::string::npos || 
        query.find("onde") != std::string::npos || 
        query.find("como") != std::string::npos || 
        query.find("qual") != std::string::npos || 
        query.find("quem") != std::string::npos) {
        
        // Processamento de consulta de conhecimento
        std::string knowledgeResponse = ProcessKnowledgeQuery(processedQuery);
        if (!knowledgeResponse.empty()) {
            // Atualizar memória de contexto
            UpdateContextMemory(m_sessionId, query, knowledgeResponse);
            return knowledgeResponse;
        }
    }
    
    // Caso contrário, processar como comando administrativo
    std::string response = m_nlProcessor->ProcessAndExecute(processedQuery, m_sessionId);
    
    // Atualizar memória de contexto
    UpdateContextMemory(m_sessionId, query, response);
    
    return response;
}

std::string AdvancedAdminAgent::ExecuteAction(
    const std::string& actionName, 
    const std::map<std::string, std::string>& parameters) {
    
    if (!m_initialized) {
        return "Agente não inicializado";
    }
    
    if (!m_isActive) {
        return "Agente desativado temporariamente. Modo atual: " + 
               std::to_string(static_cast<int>(m_currentMode));
    }
    
    Log("Executando ação: " + actionName);
    
    // Verificar se a ação corresponde a uma operação registrada
    return ExecuteOperation(actionName, parameters);
}

std::vector<AgentCapability> AdvancedAdminAgent::GetCapabilities() const {
    return m_capabilities;
}

bool AdvancedAdminAgent::SetOperationMode(AdminAgentMode mode, const std::string& reason) {
    std::lock_guard<std::mutex> lock(m_modeMutex);
    
    AdminAgentMode oldMode = m_currentMode;
    m_currentMode = mode;
    m_currentModeReason = reason;
    
    Log("Modo de operação alterado: " + std::to_string(static_cast<int>(oldMode)) + 
        " -> " + std::to_string(static_cast<int>(mode)) + 
        " (Motivo: " + reason + ")");
    
    // Atualizar o status ativo/inativo com base no modo
    if (mode == AdminAgentMode::MAINTENANCE || mode == AdminAgentMode::EMERGENCY) {
        m_isActive = false;
    } else {
        m_isActive = true;
    }
    
    return true;
}

AdminAgentMode AdvancedAdminAgent::GetOperationMode() const {
    std::lock_guard<std::mutex> lock(m_modeMutex);
    return m_currentMode;
}

bool AdvancedAdminAgent::RegisterOperation(const AdminOperation& operation) {
    std::lock_guard<std::mutex> lock(m_operationsMutex);
    
    // Verificar se a operação já existe
    if (m_operations.find(operation.operationId) != m_operations.end()) {
        Log("Operação '" + operation.operationId + "' já registrada");
        return false;
    }
    
    // Verificar se a função executora foi fornecida
    if (!operation.executor) {
        Log("Erro: Tentativa de registrar operação '" + operation.operationId + 
            "' sem função executora");
        return false;
    }
    
    m_operations[operation.operationId] = operation;
    Log("Operação registrada: " + operation.name + " (ID: " + operation.operationId + ")");
    
    return true;
}

std::string AdvancedAdminAgent::ExecuteOperation(
    const std::string& operationId,
    const std::map<std::string, std::string>& params) {
    
    std::lock_guard<std::mutex> lock(m_operationsMutex);
    
    auto it = m_operations.find(operationId);
    if (it == m_operations.end()) {
        Log("Operação não encontrada: " + operationId);
        return "Operação não encontrada: " + operationId;
    }
    
    const AdminOperation& operation = it->second;
    
    // Verificar nível de segurança
    if (operation.securityLevel > m_securityClearance) {
        Log("Acesso negado: Nível de segurança insuficiente para executar " + operation.name);
        return "Acesso negado: Você não tem permissão para executar esta operação.";
    }
    
    // Validar parâmetros
    if (!ValidateParameters(operation, params)) {
        std::stringstream missingParams;
        missingParams << "Parâmetros insuficientes para executar esta operação. ";
        missingParams << "Parâmetros necessários: ";
        
        for (size_t i = 0; i < operation.requiredParams.size(); ++i) {
            missingParams << operation.requiredParams[i];
            if (i < operation.requiredParams.size() - 1) {
                missingParams << ", ";
            }
        }
        
        Log("Parâmetros inválidos para " + operation.name + ": " + missingParams.str());
        return missingParams.str();
    }
    
    // Criar registro de histórico
    AdminActionHistory actionHistory;
    actionHistory.operationId = operationId;
    actionHistory.params = params;
    actionHistory.timestamp = std::chrono::system_clock::now();
    actionHistory.adminName = "System"; // Placeholder, em uma implementação real seria o nome do admin
    actionHistory.sessionId = m_sessionId;
    actionHistory.originalCommand = ""; // Placeholder, seria o comando original em linguagem natural
    
    // Medir tempo de execução
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Executar a operação
    try {
        std::string result = operation.executor(params);
        
        // Registrar sucesso
        actionHistory.status = "success";
        actionHistory.result = result;
        
        // Calcular duração
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        // Atualizar estatísticas
        UpdateStatistics(operationId, true, duration);
        
        // Registrar no histórico
        RecordActionHistory(actionHistory);
        
        Log("Operação executada com sucesso: " + operation.name);
        return result;
    }
    catch (const std::exception& e) {
        // Registrar falha
        actionHistory.status = "error";
        actionHistory.result = e.what();
        
        // Calcular duração
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        // Atualizar estatísticas
        UpdateStatistics(operationId, false, duration);
        
        // Registrar no histórico
        RecordActionHistory(actionHistory);
        
        Log("Erro ao executar operação " + operation.name + ": " + e.what());
        return "Erro ao executar operação: " + std::string(e.what());
    }
    catch (...) {
        // Registrar falha
        actionHistory.status = "error";
        actionHistory.result = "Erro desconhecido";
        
        // Calcular duração
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        // Atualizar estatísticas
        UpdateStatistics(operationId, false, duration);
        
        // Registrar no histórico
        RecordActionHistory(actionHistory);
        
        Log("Erro desconhecido ao executar operação " + operation.name);
        return "Erro desconhecido ao executar a operação.";
    }
}

std::vector<AdminOperation> AdvancedAdminAgent::ListAvailableOperations(
    const std::string& filter) {
    
    std::lock_guard<std::mutex> lock(m_operationsMutex);
    
    std::vector<AdminOperation> result;
    
    for (const auto& [id, operation] : m_operations) {
        // Aplicar filtro se fornecido
        if (!filter.empty()) {
            // Filtrar por ID, nome ou descrição
            bool matchesFilter = 
                operation.operationId.find(filter) != std::string::npos ||
                operation.name.find(filter) != std::string::npos ||
                operation.description.find(filter) != std::string::npos;
            
            if (!matchesFilter) {
                continue;
            }
        }
        
        // Verificar nível de segurança
        if (operation.securityLevel <= m_securityClearance) {
            result.push_back(operation);
        }
    }
    
    return result;
}

std::vector<AdminActionHistory> AdvancedAdminAgent::GetActionHistory(
    int maxItems, 
    const std::string& filter) {
    
    std::lock_guard<std::mutex> lock(m_historyMutex);
    
    std::vector<AdminActionHistory> result;
    
    // Aplicar filtro se fornecido
    if (!filter.empty()) {
        for (const auto& action : m_actionHistory) {
            // Filtrar por ID de operação, admin ou status
            bool matchesFilter = 
                action.operationId.find(filter) != std::string::npos ||
                action.adminName.find(filter) != std::string::npos ||
                action.status.find(filter) != std::string::npos;
            
            if (matchesFilter) {
                result.push_back(action);
            }
        }
    } else {
        result = m_actionHistory;
    }
    
    // Limitar o número de itens se necessário
    if (maxItems > 0 && result.size() > static_cast<size_t>(maxItems)) {
        // Retornar os itens mais recentes
        result.erase(result.begin(), result.end() - maxItems);
    }
    
    return result;
}

void AdvancedAdminAgent::SetGameStateMonitor(std::shared_ptr<GameStateMonitor> monitor) {
    m_gameStateMonitor = monitor;
    Log("GameStateMonitor definido");
}

void AdvancedAdminAgent::SetSecurityManager(std::shared_ptr<SecurityManager> securityManager) {
    m_securityManager = securityManager;
    Log("SecurityManager definido");
}

void AdvancedAdminAgent::SetAgentTrainer(std::shared_ptr<AgentTrainer> trainer) {
    m_agentTrainer = trainer;
    Log("AgentTrainer definido");
}

void AdvancedAdminAgent::SetLogCallback(std::function<void(const std::string&)> logCallback) {
    m_logCallback = logCallback;
}

void AdvancedAdminAgent::SetProgressCallback(
    std::function<void(const std::string&, float)> progressCallback) {
    
    m_progressCallback = progressCallback;
}

std::string AdvancedAdminAgent::AnalyzeGameState() {
    if (!m_gameStateMonitor) {
        return "Monitor de estado do jogo não disponível.";
    }
    
    // Em uma implementação real, usaríamos o monitor para obter dados
    // e analisar o estado atual do jogo, gerando um relatório detalhado.
    
    // Simulação de análise
    std::stringstream report;
    report << "=== Análise do Estado do Jogo ===\n\n";
    
    // População de jogadores
    report << "População: 128 jogadores online\n";
    report << "Distribuição: 45% em Armia, 25% em masmorras, 30% em outras áreas\n\n";
    
    // Economia
    report << "Economia:\n";
    report << "- Preços de armaduras +10%\n";
    report << "- Preços de armas estáveis\n";
    report << "- Pedras de Composição +25% (possível especulação)\n\n";
    
    // Segurança
    report << "Segurança:\n";
    report << "- 3 alertas de possível uso de hack\n";
    report << "- 2 jogadores com comportamento suspeito em RockyVale\n\n";
    
    // Desempenho do servidor
    report << "Servidor:\n";
    report << "- CPU: 45% utilização\n";
    report << "- Memória: 3.2 GB / 8 GB\n";
    report << "- Latência média: 85ms\n\n";
    
    // Eventos ativos
    report << "Eventos:\n";
    report << "- Drop rate aumentado em Noatun (restam 2h)\n";
    report << "- Guerra de Castelo programada para amanhã\n\n";
    
    return report.str();
}

std::vector<std::string> AdvancedAdminAgent::GenerateProactiveSuggestions() {
    std::vector<std::string> suggestions;
    
    // Em uma implementação real, usaríamos os dados do jogo
    // para gerar sugestões relevantes baseadas na situação atual.
    
    // Exemplos de sugestões
    suggestions.push_back("Iniciar evento de drop em áreas menos populadas para distribuir jogadores");
    suggestions.push_back("Verificar possível exploração de recursos em RockyVale");
    suggestions.push_back("Aumentar limite de jogadores na Dungeon 3 devido ao alto tráfego");
    suggestions.push_back("Verificar possível desequilíbrio de classes em PVP (70% FM)");
    suggestions.push_back("Realizar manutenção preventiva no banco de dados (última há 7 dias)");
    suggestions.push_back("Implementar evento temporário para item Aura Celestial (baixa circulação)");
    
    return suggestions;
}

bool AdvancedAdminAgent::LearnFromInteraction(
    const std::string& query, 
    const std::string& response, 
    bool wasHelpful) {
    
    if (!m_agentTrainer) {
        Log("AgentTrainer não disponível para aprendizado");
        return false;
    }
    
    // Em uma implementação real, enviaríamos estes dados para o treinador
    // e melhoraríamos a base de conhecimento do agente.
    
    // Simulação de aprendizado
    Log("Aprendendo com interação: query='" + query + "', helpful=" + 
        (wasHelpful ? "true" : "false"));
    
    // Se o agente possui processador de linguagem natural, pode
    // aprender novos padrões ou corrigir interpretações erradas
    if (m_nlProcessor && !wasHelpful) {
        // Tentar aprender com esta interação como um novo padrão
        return m_nlProcessor->LearnFromCorrection(query, query, true);
    }
    
    return true;
}

bool AdvancedAdminAgent::ExportLearningData(const std::string& outputPath) {
    try {
        // Criar estrutura JSON com dados de aprendizado
        json learningData;
        
        // Adicionar estatísticas de uso
        std::lock_guard<std::mutex> statLock(m_statsMutex);
        learningData["statistics"]["totalQueries"] = m_statistics.totalQueries;
        learningData["statistics"]["successfulActions"] = m_statistics.successfulActions;
        learningData["statistics"]["failedActions"] = m_statistics.failedActions;
        
        // Adicionar dados de uso de operações
        json operationUsage = json::object();
        for (const auto& [operationId, count] : m_statistics.operationUsage) {
            operationUsage[operationId] = count;
        }
        learningData["statistics"]["operationUsage"] = operationUsage;
        
        // Adicionar histórico de ações (versão simplificada para salvar espaço)
        json actionHistoryJson = json::array();
        {
            std::lock_guard<std::mutex> historyLock(m_historyMutex);
            for (const auto& action : m_actionHistory) {
                json actionJson;
                actionJson["operationId"] = action.operationId;
                actionJson["status"] = action.status;
                actionJson["timestamp"] = std::chrono::system_clock::to_time_t(action.timestamp);
                actionHistoryJson.push_back(actionJson);
            }
        }
        learningData["actionHistory"] = actionHistoryJson;
        
        // Adicionar dados de memória de contexto (versão simplificada)
        json contextMemoryJson = json::object();
        {
            std::lock_guard<std::mutex> contextLock(m_contextMutex);
            for (const auto& [contextId, memory] : m_contextMemory) {
                json memoryJson;
                memoryJson["queries"] = memory.recentQueries;
                memoryJson["responses"] = memory.recentResponses;
                contextMemoryJson[contextId] = memoryJson;
            }
        }
        learningData["contextMemory"] = contextMemoryJson;
        
        // Adicionar metadados
        learningData["metadata"]["sessionId"] = m_sessionId;
        learningData["metadata"]["exportTimestamp"] = 
            std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        
        // Salvar no arquivo
        std::ofstream outFile(outputPath);
        if (!outFile.is_open()) {
            Log("Erro ao abrir arquivo para exportar dados de aprendizado: " + outputPath);
            return false;
        }
        
        outFile << learningData.dump(4);  // Pretty print com 4 espaços
        outFile.close();
        
        Log("Dados de aprendizado exportados para: " + outputPath);
        return true;
    }
    catch (const std::exception& e) {
        Log("Erro ao exportar dados de aprendizado: " + std::string(e.what()));
        return false;
    }
}

bool AdvancedAdminAgent::IsAuthorized(const std::string& operationId, const std::string& adminName) {
    // Em uma implementação real, verificaríamos as permissões
    // específicas do administrador para esta operação.
    
    // Por simplicidade, verificamos apenas o nível de segurança
    std::lock_guard<std::mutex> lock(m_operationsMutex);
    
    auto it = m_operations.find(operationId);
    if (it == m_operations.end()) {
        return false;
    }
    
    // Verificar nível de segurança
    return m_securityClearance >= it->second.securityLevel;
}

std::string AdvancedAdminAgent::ExecuteServerDiagnostic(bool deep) {
    // Em uma implementação real, realizaríamos diagnóstico real do servidor
    
    std::stringstream report;
    report << "=== Diagnóstico do Servidor ===\n\n";
    
    // Verificações básicas
    report << "Verificações básicas:\n";
    report << "- Conectividade: OK\n";
    report << "- Autenticação: OK\n";
    report << "- Serviços: OK\n";
    report << "- Banco de dados: OK\n\n";
    
    // Métricas de performance
    report << "Performance:\n";
    report << "- CPU: 45% utilização\n";
    report << "- Memória: 3.2 GB / 8 GB\n";
    report << "- Rede: 25 Mbps / 100 Mbps\n";
    report << "- Tempo de resposta: 85ms (média)\n\n";
    
    // Verificações de saúde
    report << "Saúde do sistema:\n";
    report << "- Estado do banco de dados: Saudável\n";
    report << "- Cache: Funcionando normalmente\n";
    report << "- Sistema de arquivos: 65% utilizado\n\n";
    
    // Se diagnóstico profundo foi solicitado
    if (deep) {
        report << "Verificações profundas:\n";
        report << "- Consistência de banco de dados: 100%\n";
        report << "- Verificação de arquivos do jogo: 100% íntegros\n";
        report << "- Verificação de segurança: Sem vulnerabilidades detectadas\n";
        report << "- Análise de logs: 3 avisos encontrados (detalhes abaixo)\n\n";
        
        report << "Avisos encontrados:\n";
        report << "1. Alto tráfego em zona Armia às 20:00 (possível evento não programado)\n";
        report << "2. Tentativas de login inválidas repetidas para conta 'GameMaster5'\n";
        report << "3. Diminuição anormal de itens 'Pedra de Composição' no mercado\n\n";
    }
    
    return report.str();
}

std::string AdvancedAdminAgent::ScheduleAdminTask(
    const std::string& operationId,
    const std::map<std::string, std::string>& params,
    const std::chrono::system_clock::time_point& scheduledTime,
    const std::string& adminName) {
    
    std::lock_guard<std::mutex> lock(m_scheduleMutex);
    
    // Verificar se a operação existe
    if (m_operations.find(operationId) == m_operations.end()) {
        return "Erro: Operação não encontrada";
    }
    
    // Verificar autorização
    if (!IsAuthorized(operationId, adminName)) {
        return "Erro: Não autorizado a agendar esta operação";
    }
    
    // Gerar ID único para a tarefa
    std::stringstream taskIdStream;
    taskIdStream << "task_" << m_scheduledTasks.size() + 1 << "_" 
                 << std::chrono::system_clock::to_time_t(scheduledTime);
    std::string taskId = taskIdStream.str();
    
    // Criar tarefa agendada
    ScheduledTask task;
    task.taskId = taskId;
    task.operationId = operationId;
    task.params = params;
    task.scheduledTime = scheduledTime;
    task.adminName = adminName;
    task.isCompleted = false;
    
    // Adicionar à lista de tarefas
    m_scheduledTasks.push_back(task);
    
    // Formatar mensagem de confirmação
    auto timeT = std::chrono::system_clock::to_time_t(scheduledTime);
    char timeBuffer[100];
    std::strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", std::localtime(&timeT));
    
    std::string confirmMessage = "Tarefa agendada com sucesso. ID: " + taskId + 
                                ", Data/Hora: " + timeBuffer;
    
    Log(confirmMessage);
    return confirmMessage;
}

bool AdvancedAdminAgent::CancelScheduledTask(const std::string& taskId) {
    std::lock_guard<std::mutex> lock(m_scheduleMutex);
    
    // Procurar a tarefa
    for (auto it = m_scheduledTasks.begin(); it != m_scheduledTasks.end(); ++it) {
        if (it->taskId == taskId) {
            // Verificar se já foi completada
            if (it->isCompleted) {
                Log("Não é possível cancelar tarefa já completada: " + taskId);
                return false;
            }
            
            // Remover da lista
            m_scheduledTasks.erase(it);
            Log("Tarefa cancelada: " + taskId);
            return true;
        }
    }
    
    Log("Tarefa não encontrada: " + taskId);
    return false;
}

std::vector<std::string> AdvancedAdminAgent::ListScheduledTasks() {
    std::lock_guard<std::mutex> lock(m_scheduleMutex);
    
    std::vector<std::string> taskDescriptions;
    
    for (const auto& task : m_scheduledTasks) {
        // Pular tarefas já completadas
        if (task.isCompleted) continue;
        
        // Formatar data/hora
        auto timeT = std::chrono::system_clock::to_time_t(task.scheduledTime);
        char timeBuffer[100];
        std::strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", std::localtime(&timeT));
        
        // Verificar se a operação ainda existe
        std::string operationName;
        auto opIt = m_operations.find(task.operationId);
        if (opIt != m_operations.end()) {
            operationName = opIt->second.name;
        } else {
            operationName = task.operationId + " (operação desconhecida)";
        }
        
        // Formatar descrição da tarefa
        std::stringstream taskDesc;
        taskDesc << "ID: " << task.taskId << " | ";
        taskDesc << "Operação: " << operationName << " | ";
        taskDesc << "Agendada para: " << timeBuffer << " | ";
        taskDesc << "Agendada por: " << task.adminName;
        
        taskDescriptions.push_back(taskDesc.str());
    }
    
    return taskDescriptions;
}

void AdvancedAdminAgent::Log(const std::string& message) {
    if (m_logCallback) {
        m_logCallback("[AdminAgent] " + message);
    }
}

void AdvancedAdminAgent::ReportProgress(const std::string& operationName, float progress) {
    if (m_progressCallback) {
        m_progressCallback(operationName, progress);
    }
}

void AdvancedAdminAgent::RecordActionHistory(const AdminActionHistory& action) {
    std::lock_guard<std::mutex> lock(m_historyMutex);
    
    m_actionHistory.push_back(action);
    
    // Limitar tamanho do histórico (manter últimos 1000 itens)
    const size_t maxHistorySize = 1000;
    if (m_actionHistory.size() > maxHistorySize) {
        m_actionHistory.erase(m_actionHistory.begin(), 
                           m_actionHistory.begin() + (m_actionHistory.size() - maxHistorySize));
    }
}

std::string AdvancedAdminAgent::PreprocessCommand(const std::string& command, const std::string& contextId) {
    // Este método realiza pré-processamento do comando antes de passá-lo
    // para o processador de linguagem natural
    
    // Verificar contexto ativo para entender referências
    std::map<std::string, std::string> activeContext;
    {
        std::lock_guard<std::mutex> lock(m_contextMutex);
        auto it = m_contextMemory.find(contextId);
        if (it != m_contextMemory.end()) {
            activeContext = it->second.activeContext;
        }
    }
    
    // Em uma implementação real, realizaríamos processamento sofisticado
    // para resolver referências anafóricas, elipses, etc.
    
    // Exemplo simples: resolver "ele", "ela", "isso" baseado no contexto
    std::string processedCommand = command;
    
    // Verificar se o comando contém referências que precisam ser resolvidas
    if (processedCommand.find("ele") != std::string::npos || 
        processedCommand.find("ela") != std::string::npos || 
        processedCommand.find("isso") != std::string::npos) {
        
        // Verificar se temos contexto de item
        if (activeContext.find("item") != activeContext.end()) {
            std::string item = activeContext["item"];
            processedCommand = std::regex_replace(processedCommand, 
                                               std::regex("\\bele\\b|\\bela\\b|\\bisso\\b"), 
                                               item);
        }
        // Verificar se temos contexto de jogador
        else if (activeContext.find("player") != activeContext.end()) {
            std::string player = activeContext["player"];
            processedCommand = std::regex_replace(processedCommand, 
                                               std::regex("\\bele\\b|\\bela\\b"), 
                                               player);
        }
    }
    
    return processedCommand;
}

std::string AdvancedAdminAgent::MapIntentToOperation(const RecognizedIntent& intent) {
    // Mapear uma intenção reconhecida para uma operação administrativa
    
    // Mapeamentos diretos
    static const std::map<std::string, std::string> intentToOperationMap = {
        {"create_item", "create_game_item"},
        {"drop_event", "start_drop_event"},
        {"player_management", "manage_player"},
        {"broadcast", "send_broadcast"},
        {"server_status", "check_server_status"},
        {"diagnostic", "run_diagnostic"},
        {"analyze_state", "analyze_game_state"}
    };
    
    // Verificar mapeamento direto
    auto it = intentToOperationMap.find(intent.intentName);
    if (it != intentToOperationMap.end()) {
        return it->second;
    }
    
    // Sem mapeamento direto, tentar determinar com base nos parâmetros
    if (intent.HasParam("itemType") && intent.HasParam("playerClass")) {
        return "create_game_item";
    }
    
    if (intent.HasParam("playerName")) {
        // Determinar o tipo de ação com base em outros parâmetros
        if (intent.HasParam("action")) {
            std::string action = intent.GetParam("action");
            if (action.find("ban") != std::string::npos || 
                action.find("kick") != std::string::npos) {
                return "restrict_player";
            }
            if (action.find("tp") != std::string::npos || 
                action.find("teleport") != std::string::npos) {
                return "teleport_player";
            }
        }
        return "manage_player";
    }
    
    // Sem mapeamento claro
    return "";
}

void AdvancedAdminAgent::BackgroundProcessingThread() {
    Log("Thread de processamento em segundo plano iniciada");
    
    while (!m_stopBackgroundThread) {
        try {
            // Verificar tarefas agendadas
            CheckScheduledTasks();
            
            // Verificar se é necessário monitorar anomalias
            if (m_currentMode == AdminAgentMode::SILENT_MONITOR && m_securityManager) {
                auto anomalies = MonitorPlayerAnomalies();
                
                if (!anomalies.empty()) {
                    // Registrar anomalias detectadas
                    for (const auto& anomaly : anomalies) {
                        Log("Anomalia detectada: " + anomaly);
                    }
                }
            }
            
            // Aguardar antes da próxima verificação
            std::this_thread::sleep_for(std::chrono::seconds(5));
        }
        catch (const std::exception& e) {
            Log("Erro na thread de processamento: " + std::string(e.what()));
        }
        catch (...) {
            Log("Erro desconhecido na thread de processamento");
        }
    }
    
    Log("Thread de processamento em segundo plano encerrada");
}

void AdvancedAdminAgent::CheckScheduledTasks() {
    std::lock_guard<std::mutex> lock(m_scheduleMutex);
    
    auto now = std::chrono::system_clock::now();
    
    for (auto& task : m_scheduledTasks) {
        // Pular tarefas já completadas
        if (task.isCompleted) continue;
        
        // Verificar se é hora de executar
        if (task.scheduledTime <= now) {
            Log("Executando tarefa agendada: " + task.taskId);
            
            try {
                // Executar a operação
                std::string result = ExecuteOperation(task.operationId, task.params);
                
                // Marcar como completada
                task.isCompleted = true;
                task.result = result;
                
                Log("Tarefa agendada completada: " + task.taskId + " - " + result);
            }
            catch (const std::exception& e) {
                // Registrar falha
                task.isCompleted = true;
                task.result = "Erro: " + std::string(e.what());
                
                Log("Erro ao executar tarefa agendada: " + task.taskId + " - " + e.what());
            }
            catch (...) {
                // Registrar falha
                task.isCompleted = true;
                task.result = "Erro desconhecido";
                
                Log("Erro desconhecido ao executar tarefa agendada: " + task.taskId);
            }
        }
    }
}

void AdvancedAdminAgent::InitializeDefaultOperations() {
    Log("Inicializando operações padrão...");
    
    // Implementar operações padrão
    ImplementStandardOperations();
    
    Log("Operações padrão inicializadas: " + std::to_string(m_operations.size()));
}

std::string AdvancedAdminAgent::GenerateGameStateReport() {
    // Em uma implementação real, usaríamos o GameStateMonitor
    // para obter dados reais do jogo
    
    std::stringstream report;
    report << "=== Estado Atual do Jogo ===\n\n";
    
    // Dados de jogadores
    report << "Jogadores online: 128\n";
    report << "Distribuição por classes: 35% FM, 30% BM, 20% HT, 15% outros\n";
    report << "Novos jogadores nas últimas 24h: 15\n\n";
    
    // Estado do servidor
    report << "Saúde do servidor: Boa\n";
    report << "Tempo online: 72h 15m\n";
    report << "Uso de recursos: 45% CPU, 50% RAM\n\n";
    
    // Eventos
    report << "Eventos ativos: 2\n";
    report << "- Drop rate aumentado (global)\n";
    report << "- XP bônus em Saphira\n\n";
    
    // Economia
    report << "Estado da economia: Estável\n";
    report << "Itens mais comercializados: Poeira de Lac, Pedras +9, Armaduras C\n";
    report << "Inflação de preços: +5% (semanal)\n\n";
    
    return report.str();
}

std::vector<std::string> AdvancedAdminAgent::MonitorPlayerAnomalies() {
    std::vector<std::string> anomalies;
    
    // Em uma implementação real, usaríamos o SecurityManager
    // para detectar anomalias no comportamento dos jogadores
    
    // Simulação
    if ((std::rand() % 100) < 10) {  // 10% de chance de detectar anomalia
        // Exemplos de anomalias
        anomalies.push_back("Jogador 'DarkMage55' movendo-se em velocidade anormal");
        anomalies.push_back("Acúmulo suspeito de itens raros por 'GoldHunter23'");
        anomalies.push_back("Tentativas repetidas de exploração de duplicação por 'Shadow12'");
    }
    
    return anomalies;
}

void AdvancedAdminAgent::UpdateStatistics(
    const std::string& operationId, 
    bool success, 
    const std::chrono::milliseconds& duration) {
    
    std::lock_guard<std::mutex> lock(m_statsMutex);
    
    // Incrementar contadores
    m_statistics.operationUsage[operationId]++;
    
    if (success) {
        m_statistics.successfulActions++;
    } else {
        m_statistics.failedActions++;
    }
    
    // Atualizar duração média
    auto& avgDuration = m_statistics.averageDuration[operationId];
    if (avgDuration.count() == 0) {
        // Primeira execução
        avgDuration = duration;
    } else {
        // Atualizar média móvel
        int count = m_statistics.operationUsage[operationId];
        avgDuration = std::chrono::milliseconds(
            (avgDuration.count() * (count - 1) + duration.count()) / count);
    }
}

bool AdvancedAdminAgent::ValidateParameters(
    const AdminOperation& operation,
    const std::map<std::string, std::string>& params) {
    
    // Verificar se todos os parâmetros obrigatórios estão presentes
    for (const auto& requiredParam : operation.requiredParams) {
        if (params.find(requiredParam) == params.end()) {
            return false;
        }
    }
    
    // Consideramos válido se todos os parâmetros obrigatórios estiverem presentes
    return true;
}

void AdvancedAdminAgent::ImplementStandardOperations() {
    // Criar item no jogo
    {
        AdminOperation operation;
        operation.operationId = "create_game_item";
        operation.name = "Criar Item";
        operation.description = "Cria um item no jogo para um jogador específico";
        operation.requiredParams = {"itemType", "playerName"};
        operation.optionalParams = {"refineLevel", "playerClass"};
        operation.executor = [this](const std::map<std::string, std::string>& params) -> std::string {
            // Em uma implementação real, chamaríamos a API do jogo
            // para criar o item para o jogador
            
            std::string itemType = params.at("itemType");
            std::string playerName = params.at("playerName");
            
            // Obter nível de refinamento, se fornecido
            int refineLevel = 0;
            auto refineLevelIt = params.find("refineLevel");
            if (refineLevelIt != params.end() && !refineLevelIt->second.empty()) {
                try {
                    refineLevel = std::stoi(refineLevelIt->second);
                } catch (...) {
                    // Ignorar erros de conversão
                }
            }
            
            // Obter classe, se fornecida
            std::string playerClass = "";
            auto playerClassIt = params.find("playerClass");
            if (playerClassIt != params.end()) {
                playerClass = playerClassIt->second;
            }
            
            // Construir descrição do item
            std::string itemDescription = itemType;
            if (refineLevel > 0) {
                itemDescription += " +" + std::to_string(refineLevel);
            }
            if (!playerClass.empty()) {
                itemDescription += " para " + playerClass;
            }
            
            std::string result = "Item '" + itemDescription + "' criado para o jogador '" + 
                               playerName + "'";
            
            Log("Operação create_game_item executada: " + result);
            return result;
        };
        operation.securityLevel = 3;  // Nível de segurança moderado
        operation.requiresConfirmation = false;
        operation.isLogged = true;
        operation.expectedDuration = std::chrono::milliseconds(500);
        
        RegisterOperation(operation);
    }
    
    // Iniciar evento de drop
    {
        AdminOperation operation;
        operation.operationId = "start_drop_event";
        operation.name = "Iniciar Evento de Drop";
        operation.description = "Inicia um evento de drop de itens em uma área específica";
        operation.requiredParams = {"itemName", "targetLocation"};
        operation.optionalParams = {"duration", "dropRate"};
        operation.executor = [this](const std::map<std::string, std::string>& params) -> std::string {
            // Em uma implementação real, chamaríamos a API do jogo
            // para iniciar o evento
            
            std::string itemName = params.at("itemName");
            std::string targetLocation = params.at("targetLocation");
            
            // Obter duração, se fornecida
            int duration = 60;  // Minutos, padrão 60
            auto durationIt = params.find("duration");
            if (durationIt != params.end() && !durationIt->second.empty()) {
                try {
                    duration = std::stoi(durationIt->second);
                } catch (...) {
                    // Ignorar erros de conversão
                }
            }
            
            // Obter taxa, se fornecida
            int dropRate = 200;  // 200%, padrão dobrar
            auto dropRateIt = params.find("dropRate");
            if (dropRateIt != params.end() && !dropRateIt->second.empty()) {
                try {
                    dropRate = std::stoi(dropRateIt->second);
                } catch (...) {
                    // Ignorar erros de conversão
                }
            }
            
            std::string result = "Evento de drop iniciado: Item '" + itemName + 
                               "' em '" + targetLocation + "' " +
                               "por " + std::to_string(duration) + " minutos " +
                               "com taxa de " + std::to_string(dropRate) + "%";
            
            Log("Operação start_drop_event executada: " + result);
            return result;
        };
        operation.securityLevel = 4;  // Nível de segurança alto
        operation.requiresConfirmation = true;
        operation.isLogged = true;
        operation.expectedDuration = std::chrono::milliseconds(1000);
        
        RegisterOperation(operation);
    }
    
    // Gerenciar jogador
    {
        AdminOperation operation;
        operation.operationId = "manage_player";
        operation.name = "Gerenciar Jogador";
        operation.description = "Gerencia um jogador (banir, silenciar, teleportar, etc.)";
        operation.requiredParams = {"action", "playerName"};
        operation.optionalParams = {"duration", "reason", "targetLocation"};
        operation.executor = [this](const std::map<std::string, std::string>& params) -> std::string {
            // Em uma implementação real, chamaríamos a API do jogo
            // para gerenciar o jogador
            
            std::string action = params.at("action");
            std::string playerName = params.at("playerName");
            
            // Obter motivo, se fornecido
            std::string reason = "";
            auto reasonIt = params.find("reason");
            if (reasonIt != params.end()) {
                reason = reasonIt->second;
            }
            
            // Resultado padrão
            std::string result = "Ação '" + action + "' executada para o jogador '" + playerName + "'";
            
            // Personalizar com base na ação
            if (action == "ban" || action == "banir") {
                // Obter duração, se fornecida
                int duration = 0;  // 0 = permanente
                auto durationIt = params.find("duration");
                if (durationIt != params.end() && !durationIt->second.empty()) {
                    try {
                        duration = std::stoi(durationIt->second);
                    } catch (...) {
                        // Ignorar erros de conversão
                    }
                }
                
                if (duration > 0) {
                    result = "Jogador '" + playerName + "' banido por " + 
                           std::to_string(duration) + " dias";
                } else {
                    result = "Jogador '" + playerName + "' banido permanentemente";
                }
                
                if (!reason.empty()) {
                    result += ". Motivo: " + reason;
                }
            }
            else if (action == "teleport" || action == "tp" || action == "teleportar") {
                // Obter localização alvo, se fornecida
                std::string targetLocation = "Armia";  // Padrão
                auto targetIt = params.find("targetLocation");
                if (targetIt != params.end() && !targetIt->second.empty()) {
                    targetLocation = targetIt->second;
                }
                
                result = "Jogador '" + playerName + "' teleportado para '" + targetLocation + "'";
            }
            else if (action == "silence" || action == "silenciar" || action == "mute") {
                // Obter duração, se fornecida
                int duration = 60;  // Minutos, padrão 60
                auto durationIt = params.find("duration");
                if (durationIt != params.end() && !durationIt->second.empty()) {
                    try {
                        duration = std::stoi(durationIt->second);
                    } catch (...) {
                        // Ignorar erros de conversão
                    }
                }
                
                result = "Jogador '" + playerName + "' silenciado por " + 
                       std::to_string(duration) + " minutos";
                
                if (!reason.empty()) {
                    result += ". Motivo: " + reason;
                }
            }
            
            Log("Operação manage_player executada: " + result);
            return result;
        };
        operation.securityLevel = 4;  // Nível de segurança alto
        operation.requiresConfirmation = true;
        operation.isLogged = true;
        operation.expectedDuration = std::chrono::milliseconds(800);
        
        RegisterOperation(operation);
    }
    
    // Enviar broadcast
    {
        AdminOperation operation;
        operation.operationId = "send_broadcast";
        operation.name = "Enviar Broadcast";
        operation.description = "Envia uma mensagem de broadcast para todos os jogadores";
        operation.requiredParams = {"message"};
        operation.optionalParams = {"color", "repeat"};
        operation.executor = [this](const std::map<std::string, std::string>& params) -> std::string {
            // Em uma implementação real, chamaríamos a API do jogo
            // para enviar o broadcast
            
            std::string message = params.at("message");
            
            // Obter cor, se fornecida
            std::string color = "white";  // Padrão
            auto colorIt = params.find("color");
            if (colorIt != params.end() && !colorIt->second.empty()) {
                color = colorIt->second;
            }
            
            // Obter repetições, se fornecidas
            int repeat = 1;
            auto repeatIt = params.find("repeat");
            if (repeatIt != params.end() && !repeatIt->second.empty()) {
                try {
                    repeat = std::stoi(repeatIt->second);
                    // Limitar a 5 repetições para evitar spam
                    repeat = std::min(repeat, 5);
                } catch (...) {
                    // Ignorar erros de conversão
                }
            }
            
            std::string result = "Broadcast enviado: '" + message + "' ";
            if (repeat > 1) {
                result += "(" + std::to_string(repeat) + " vezes) ";
            }
            result += "na cor " + color;
            
            Log("Operação send_broadcast executada: " + result);
            return result;
        };
        operation.securityLevel = 3;  // Nível de segurança moderado
        operation.requiresConfirmation = false;
        operation.isLogged = true;
        operation.expectedDuration = std::chrono::milliseconds(300);
        
        RegisterOperation(operation);
    }
    
    // Verificar status do servidor
    {
        AdminOperation operation;
        operation.operationId = "check_server_status";
        operation.name = "Verificar Status do Servidor";
        operation.description = "Verifica o status atual do servidor";
        operation.requiredParams = {};
        operation.optionalParams = {"detailLevel"};
        operation.executor = [this](const std::map<std::string, std::string>& params) -> std::string {
            // Em uma implementação real, chamaríamos as APIs do servidor
            // para obter o status atual
            
            // Obter nível de detalhe, se fornecido
            int detailLevel = 1;  // Padrão: básico
            auto detailIt = params.find("detailLevel");
            if (detailIt != params.end() && !detailIt->second.empty()) {
                try {
                    detailLevel = std::stoi(detailIt->second);
                } catch (...) {
                    // Ignorar erros de conversão
                }
            }
            
            // Gerar relatório com base no nível de detalhe
            std::stringstream report;
            report << "=== Status do Servidor ===\n\n";
            
            // Informações básicas (sempre incluídas)
            report << "Estado: Online\n";
            report << "Uptime: 72h 15m\n";
            report << "Jogadores online: 128\n";
            
            // Detalhes adicionais (nível 2+)
            if (detailLevel >= 2) {
                report << "\nPerformance:\n";
                report << "- CPU: 45% utilização\n";
                report << "- Memória: 3.2 GB / 8 GB\n";
                report << "- Rede: 25 Mbps / 100 Mbps\n";
            }
            
            // Detalhes avançados (nível 3+)
            if (detailLevel >= 3) {
                report << "\nServiços:\n";
                report << "- Autenticação: Operacional\n";
                report << "- Banco de dados: Operacional\n";
                report << "- Cache: Operacional\n";
                report << "- Sistemas de jogo: Operacional\n";
            }
            
            Log("Operação check_server_status executada");
            return report.str();
        };
        operation.securityLevel = 1;  // Nível de segurança baixo (apenas visualização)
        operation.requiresConfirmation = false;
        operation.isLogged = false;  // Não precisa registrar operações de apenas leitura
        operation.expectedDuration = std::chrono::milliseconds(200);
        
        RegisterOperation(operation);
    }
    
    // Executar diagnóstico
    {
        AdminOperation operation;
        operation.operationId = "run_diagnostic";
        operation.name = "Executar Diagnóstico";
        operation.description = "Executa diagnóstico completo do servidor e jogo";
        operation.requiredParams = {};
        operation.optionalParams = {"deep"};
        operation.executor = [this](const std::map<std::string, std::string>& params) -> std::string {
            // Verificar se é diagnóstico profundo
            bool deep = false;
            auto deepIt = params.find("deep");
            if (deepIt != params.end()) {
                deep = (deepIt->second == "true" || deepIt->second == "1");
            }
            
            // Executar diagnóstico
            return ExecuteServerDiagnostic(deep);
        };
        operation.securityLevel = 2;  // Nível de segurança médio-baixo
        operation.requiresConfirmation = false;
        operation.isLogged = true;
        operation.expectedDuration = std::chrono::milliseconds(1500);  // Pode demorar mais
        
        RegisterOperation(operation);
    }
    
    // Analisar estado do jogo
    {
        AdminOperation operation;
        operation.operationId = "analyze_game_state";
        operation.name = "Analisar Estado do Jogo";
        operation.description = "Analisa o estado atual do jogo e gera relatório";
        operation.requiredParams = {};
        operation.optionalParams = {};
        operation.executor = [this](const std::map<std::string, std::string>& params) -> std::string {
            return AnalyzeGameState();
        };
        operation.securityLevel = 2;  // Nível de segurança médio-baixo
        operation.requiresConfirmation = false;
        operation.isLogged = false;  // Não precisa registrar operações de apenas leitura
        operation.expectedDuration = std::chrono::milliseconds(500);
        
        RegisterOperation(operation);
    }
    
    // Gerar sugestões
    {
        AdminOperation operation;
        operation.operationId = "generate_suggestions";
        operation.name = "Gerar Sugestões";
        operation.description = "Gera sugestões proativas para administração";
        operation.requiredParams = {};
        operation.optionalParams = {"count"};
        operation.executor = [this](const std::map<std::string, std::string>& params) -> std::string {
            // Obter número de sugestões, se fornecido
            int count = 5;  // Padrão
            auto countIt = params.find("count");
            if (countIt != params.end() && !countIt->second.empty()) {
                try {
                    count = std::stoi(countIt->second);
                } catch (...) {
                    // Ignorar erros de conversão
                }
            }
            
            // Gerar sugestões
            std::vector<std::string> suggestions = GenerateProactiveSuggestions();
            
            // Limitar ao número solicitado
            if (static_cast<int>(suggestions.size()) > count) {
                suggestions.resize(count);
            }
            
            // Formatar resultado
            std::stringstream result;
            result << "=== Sugestões Proativas ===\n\n";
            
            for (size_t i = 0; i < suggestions.size(); ++i) {
                result << (i+1) << ". " << suggestions[i] << "\n";
            }
            
            return result.str();
        };
        operation.securityLevel = 1;  // Nível de segurança baixo (apenas visualização)
        operation.requiresConfirmation = false;
        operation.isLogged = false;  // Não precisa registrar operações de apenas leitura
        operation.expectedDuration = std::chrono::milliseconds(300);
        
        RegisterOperation(operation);
    }
}

std::string AdvancedAdminAgent::ProcessKnowledgeQuery(const std::string& query) {
    if (!m_knowledgeBase) {
        return "";
    }
    
    // Em uma implementação real, usaríamos a base de conhecimento
    // para processar consultas sobre o jogo
    
    // Usar a base de conhecimento para responder à pergunta
    // Isso normalmente envolveria entidades como itens, NPCs, mapas, etc.
    return m_knowledgeBase->AnswerGameQuestion(query);
}

std::string AdvancedAdminAgent::GenerateContextualHelp(
    const std::string& contextId, 
    const std::string& topic) {
    
    // Verificar se é sobre um tópico específico
    if (!topic.empty()) {
        // Ajuda sobre operações
        if (topic == "operations" || topic == "commands" || topic == "ações") {
            std::stringstream help;
            help << "=== Ajuda: Operações Administrativas ===\n\n";
            
            help << "Operações disponíveis:\n";
            auto operations = ListAvailableOperations();
            
            for (const auto& op : operations) {
                help << "- " << op.name << ": " << op.description << "\n";
            }
            
            help << "\nExemplos de uso:\n";
            help << "- Criar um set D +11 para o jogador TestUser\n";
            help << "- Iniciar evento de drop de pedras em trolls\n";
            help << "- Banir o jogador BadUser por 3 dias\n";
            help << "- Verificar status do servidor\n";
            
            return help.str();
        }
        
        // Ajuda sobre itens
        if (topic == "items" || topic == "itens") {
            std::stringstream help;
            help << "=== Ajuda: Gerenciamento de Itens ===\n\n";
            
            help << "Comandos para itens:\n";
            help << "- Criar item: 'criar <tipo_item> +<nível> para <classe> para <jogador>'\n";
            help << "- Exemplos:\n";
            help << "  * criar set D +11 para FM para TestUser\n";
            help << "  * criar arma celestial +10 para BM para TestUser\n";
            
            return help.str();
        }
        
        // Ajuda sobre eventos
        if (topic == "events" || topic == "eventos") {
            std::stringstream help;
            help << "=== Ajuda: Gerenciamento de Eventos ===\n\n";
            
            help << "Comandos para eventos:\n";
            help << "- Iniciar evento: 'iniciar evento de <item> em <local>'\n";
            help << "- Exemplos:\n";
            help << "  * iniciar evento de drop de poeira de lac em trolls\n";
            help << "  * começar drop rate de armadura D em noatun\n";
            
            return help.str();
        }
    }
    
    // Ajuda geral (quando nenhum tópico específico é fornecido)
    std::stringstream help;
    help << "=== Ajuda do AdvancedAdminAgent ===\n\n";
    
    help << "Este agente permite administrar o servidor e jogo através de comandos\n";
    help << "em linguagem natural. Você pode fazer perguntas sobre o jogo ou executar\n";
    help << "ações administrativas.\n\n";
    
    help << "Categorias de ajuda disponíveis:\n";
    help << "- operations: Operações administrativas disponíveis\n";
    help << "- items: Gerenciamento de itens\n";
    help << "- events: Gerenciamento de eventos\n";
    help << "- players: Gerenciamento de jogadores\n";
    help << "- server: Administração do servidor\n\n";
    
    help << "Para ajuda específica, digite 'ajuda <categoria>'.\n";
    
    // Se temos contexto, adicionar sugestões com base no histórico
    if (!contextId.empty()) {
        std::lock_guard<std::mutex> lock(m_contextMutex);
        auto it = m_contextMemory.find(contextId);
        
        if (it != m_contextMemory.end() && !it->second.recentQueries.empty()) {
            help << "\nCom base em suas interações recentes, você pode estar interessado em:\n";
            
            // Sugestões baseadas nas consultas recentes
            if (std::find(it->second.recentQueries.begin(), 
                        it->second.recentQueries.end(), 
                        "item") != it->second.recentQueries.end()) {
                help << "- ajuda items\n";
            }
            
            if (std::find(it->second.recentQueries.begin(), 
                        it->second.recentQueries.end(), 
                        "evento") != it->second.recentQueries.end()) {
                help << "- ajuda events\n";
            }
        }
    }
    
    return help.str();
}

void AdvancedAdminAgent::UpdateContextMemory(
    const std::string& contextId,
    const std::string& query,
    const std::string& response) {
    
    if (contextId.empty()) return;
    
    std::lock_guard<std::mutex> lock(m_contextMutex);
    
    // Obter ou criar memória para este contexto
    ContextMemory& memory = m_contextMemory[contextId];
    
    // Atualizar timestamp
    memory.lastUpdated = std::chrono::system_clock::now();
    
    // Adicionar consulta e resposta
    memory.recentQueries.push_back(query);
    memory.recentResponses.push_back(response);
    
    // Limitar tamanho (manter últimos 10 itens)
    const size_t maxMemorySize = 10;
    if (memory.recentQueries.size() > maxMemorySize) {
        memory.recentQueries.erase(memory.recentQueries.begin());
        memory.recentResponses.erase(memory.recentResponses.begin());
    }
    
    // Atualizar contexto ativo com base na consulta
    
    // Exemplo: identificar referências a itens
    if (query.find("set") != std::string::npos || 
        query.find("armadura") != std::string::npos || 
        query.find("arma") != std::string::npos) {
        
        // Extrair qual o item mencionado (simplificado)
        size_t pos = query.find("set");
        if (pos != std::string::npos && pos + 4 < query.length()) {
            memory.activeContext["item"] = "set " + query.substr(pos + 4, 1);
        }
    }
    
    // Exemplo: identificar referências a jogadores
    size_t playerPos = query.find("jogador");
    if (playerPos != std::string::npos && playerPos + 8 < query.length()) {
        // Procurar pelo nome do jogador após "jogador"
        size_t nameStart = query.find_first_not_of(" ", playerPos + 8);
        if (nameStart != std::string::npos) {
            size_t nameEnd = query.find_first_of(" ,.", nameStart);
            if (nameEnd != std::string::npos) {
                memory.activeContext["player"] = query.substr(nameStart, nameEnd - nameStart);
            } else {
                memory.activeContext["player"] = query.substr(nameStart);
            }
        }
    }
}

} // namespace WYDStudio