#include "../../include/AgentFileAccessProvider.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <algorithm>
#include <regex>
#include <set>
#include <filesystem>
#include <json/json.hpp> // Usando nlohmann/json para C++

using json = nlohmann::json;
namespace fs = std::filesystem;

namespace WYDStudio {

/**
 * @brief Obtém um timestamp atual como int64
 * @return Timestamp em milissegundos
 */
static int64_t GetCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_point::time_since_epoch()).count();
}

/**
 * @brief Converte uma operação de arquivo para string
 * @param operation Operação a ser convertida
 * @return String representando a operação
 */
static std::string OperationToString(FileOperation operation) {
    switch (operation) {
        case FileOperation::CREATE:
            return "CREATE";
        case FileOperation::MODIFY:
            return "MODIFY";
        case FileOperation::DELETE:
            return "DELETE";
        case FileOperation::MOVE:
            return "MOVE";
        case FileOperation::COPY:
            return "COPY";
        case FileOperation::COMPILE:
            return "COMPILE";
        case FileOperation::DECOMPILE:
            return "DECOMPILE";
        default:
            return "UNKNOWN";
    }
}

/**
 * @brief Converte uma string para operação de arquivo
 * @param operationStr String a ser convertida
 * @return Operação correspondente
 */
static FileOperation StringToOperation(const std::string& operationStr) {
    if (operationStr == "CREATE") return FileOperation::CREATE;
    if (operationStr == "MODIFY") return FileOperation::MODIFY;
    if (operationStr == "DELETE") return FileOperation::DELETE;
    if (operationStr == "MOVE") return FileOperation::MOVE;
    if (operationStr == "COPY") return FileOperation::COPY;
    if (operationStr == "COMPILE") return FileOperation::COMPILE;
    if (operationStr == "DECOMPILE") return FileOperation::DECOMPILE;
    
    return FileOperation::MODIFY; // Default
}

/**
 * @brief Verifica se um arquivo corresponde a um padrão de extensão
 * @param path Caminho do arquivo
 * @param extensions Lista de extensões
 * @return true se corresponder a alguma extensão
 */
static bool MatchesExtension(const std::string& path, const std::vector<std::string>& extensions) {
    if (extensions.empty()) return true; // Sem restrições
    
    fs::path filePath(path);
    std::string extension = filePath.extension().string();
    
    // Garantir que começa com ponto
    if (!extension.empty() && extension[0] != '.') {
        extension = "." + extension;
    }
    
    // Normalizar para minúsculas
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    for (const auto& ext : extensions) {
        std::string normalizedExt = ext;
        
        // Garantir que começa com ponto
        if (!normalizedExt.empty() && normalizedExt[0] != '.') {
            normalizedExt = "." + normalizedExt;
        }
        
        // Normalizar para minúsculas
        std::transform(normalizedExt.begin(), normalizedExt.end(), normalizedExt.begin(), ::tolower);
        
        if (extension == normalizedExt) {
            return true;
        }
    }
    
    return false;
}

/**
 * @brief Verifica se um arquivo está em um diretório
 * @param filePath Caminho do arquivo
 * @param directory Diretório
 * @param includeSubdirectories true para incluir subdiretórios
 * @return true se o arquivo está no diretório
 */
static bool IsFileInDirectory(const std::string& filePath, 
                            const std::string& directory,
                            bool includeSubdirectories) {
    fs::path file(filePath);
    fs::path dir(directory);
    
    // Normalizar para caminhos absolutos
    file = fs::absolute(file);
    dir = fs::absolute(dir);
    
    // Verificar se o arquivo está no diretório
    std::string fileStr = file.string();
    std::string dirStr = dir.string();
    
    // Garantir que o diretório termine com separador
    if (!dirStr.empty() && dirStr.back() != fs::path::preferred_separator) {
        dirStr += fs::path::preferred_separator;
    }
    
    if (includeSubdirectories) {
        // Verificar se o arquivo está no diretório ou em um subdiretório
        return fileStr.find(dirStr) == 0;
    } else {
        // Verificar se o arquivo está diretamente no diretório
        std::string fileDir = file.parent_path().string();
        if (!fileDir.empty() && fileDir.back() != fs::path::preferred_separator) {
            fileDir += fs::path::preferred_separator;
        }
        return fileDir == dirStr;
    }
}

/**
 * @struct PermissionEntry
 * @brief Representa uma entrada de permissão para agentes
 */
struct PermissionEntry {
    std::vector<std::string> fileExtensions;    // Extensões de arquivo com permissão
    std::vector<std::string> directories;       // Diretórios com permissão
    std::vector<FileOperation> operations;      // Operações permitidas
    bool includeSubdirectories;                 // Incluir subdiretórios
};

/**
 * @struct AgentRegistration
 * @brief Informações de registro de um agente
 */
struct AgentRegistration {
    std::shared_ptr<BaseAgent> agent;       // Ponteiro para o agente
    AgentType type;                         // Tipo do agente
    std::string name;                       // Nome do agente
};

class AgentFileAccessProvider::Impl {
public:
    Impl(std::shared_ptr<GameFileManager> fileManager) 
        : m_fileManager(fileManager) {
        
        // Carregar histórico de ações se existir
        LoadActionHistory();
    }
    
    ~Impl() {
        // Salvar histórico de ações ao destruir
        SaveActionHistory();
    }
    
    // Gerenciador de arquivos
    std::shared_ptr<GameFileManager> m_fileManager;
    
    // Agentes registrados
    std::map<BaseAgent*, AgentRegistration> m_registeredAgents;
    
    // Permissões por tipo de agente
    std::map<AgentType, std::vector<PermissionEntry>> m_permissions;
    
    // Histórico de ações
    std::vector<AgentFileAction> m_actionHistory;
    
    // Função de log
    std::function<void(const std::string&)> m_logCallback;
    
    /**
     * @brief Registra uma mensagem no log
     * @param message Mensagem a ser registrada
     */
    void Log(const std::string& message) {
        if (m_logCallback) {
            m_logCallback(message);
        } else {
            std::cout << "[AgentFileAccessProvider] " << message << std::endl;
        }
    }
    
    /**
     * @brief Salva o histórico de ações em um arquivo
     */
    void SaveActionHistory() {
        try {
            // Verificar se temos o diretório de logs
            std::string logDir = "logs";
            if (!fs::exists(logDir)) {
                fs::create_directories(logDir);
            }
            
            // Caminho para o arquivo de histórico
            std::string historyPath = logDir + "/agent_file_actions.json";
            
            // Criar array JSON com todas as ações
            json actionsJson = json::array();
            
            // Converter cada ação para JSON
            for (const auto& action : m_actionHistory) {
                json actionJson;
                actionJson["agentType"] = static_cast<int>(action.agentType);
                actionJson["agentName"] = action.agentName;
                actionJson["operation"] = OperationToString(action.operation);
                actionJson["targetPath"] = action.targetPath;
                actionJson["description"] = action.description;
                actionJson["timestamp"] = action.timestamp;
                actionJson["backupPath"] = action.backupPath;
                actionJson["success"] = action.success;
                
                actionsJson.push_back(actionJson);
            }
            
            // Escrever no arquivo
            std::ofstream file(historyPath);
            if (!file.is_open()) {
                Log("Erro: Não foi possível abrir arquivo para escrita: " + historyPath);
                return;
            }
            
            file << actionsJson.dump(4); // Pretty print com 4 espaços
            file.close();
            
            Log("Histórico de ações salvo com sucesso: " + std::to_string(m_actionHistory.size()) + " ações");
        }
        catch (const std::exception& e) {
            Log("Erro ao salvar histórico de ações: " + std::string(e.what()));
        }
    }
    
    /**
     * @brief Carrega o histórico de ações de um arquivo
     */
    void LoadActionHistory() {
        try {
            // Caminho para o arquivo de histórico
            std::string historyPath = "logs/agent_file_actions.json";
            
            // Verificar se o arquivo existe
            if (!fs::exists(historyPath)) {
                Log("Arquivo de histórico de ações não encontrado. Criando novo histórico.");
                m_actionHistory.clear();
                return;
            }
            
            // Abrir o arquivo
            std::ifstream file(historyPath);
            if (!file.is_open()) {
                Log("Erro ao abrir arquivo de histórico de ações.");
                return;
            }
            
            // Ler o JSON
            json actionsJson;
            file >> actionsJson;
            file.close();
            
            // Limpar histórico atual
            m_actionHistory.clear();
            
            // Processar cada entrada
            for (const auto& actionJson : actionsJson) {
                AgentFileAction action;
                action.agentType = static_cast<AgentType>(actionJson["agentType"].get<int>());
                action.agentName = actionJson["agentName"].get<std::string>();
                action.operation = StringToOperation(actionJson["operation"].get<std::string>());
                action.targetPath = actionJson["targetPath"].get<std::string>();
                action.description = actionJson["description"].get<std::string>();
                action.timestamp = actionJson["timestamp"].get<int64_t>();
                action.backupPath = actionJson["backupPath"].get<std::string>();
                action.success = actionJson["success"].get<bool>();
                
                m_actionHistory.push_back(action);
            }
            
            Log("Histórico de ações carregado com sucesso: " + std::to_string(m_actionHistory.size()) + " ações");
        }
        catch (const std::exception& e) {
            Log("Erro ao carregar histórico de ações: " + std::string(e.what()));
            // Em caso de erro, criar um novo histórico vazio
            m_actionHistory.clear();
        }
    }
    
    /**
     * @brief Adiciona uma ação ao histórico
     * @param action Ação a ser adicionada
     */
    void AddActionToHistory(const AgentFileAction& action) {
        m_actionHistory.push_back(action);
        
        // Se o histórico ficar muito grande, limitar
        const size_t MAX_HISTORY_SIZE = 10000;
        if (m_actionHistory.size() > MAX_HISTORY_SIZE) {
            // Remover as ações mais antigas
            m_actionHistory.erase(m_actionHistory.begin(), 
                               m_actionHistory.begin() + (m_actionHistory.size() - MAX_HISTORY_SIZE));
        }
        
        // Salvar o histórico periodicamente (a cada 100 ações)
        if (m_actionHistory.size() % 100 == 0) {
            SaveActionHistory();
        }
    }
    
    /**
     * @brief Verifica se um agente tem permissão para uma operação em um arquivo
     * @param agent Agente a verificar
     * @param path Caminho do arquivo
     * @param operation Operação desejada
     * @return true se o agente tem permissão
     */
    bool HasPermission(BaseAgent* agent, const std::string& path, FileOperation operation) {
        // Verificar se o agente está registrado
        auto agentIt = m_registeredAgents.find(agent);
        if (agentIt == m_registeredAgents.end()) {
            Log("Agente não registrado tentando acessar arquivo: " + path);
            return false;
        }
        
        // Obter tipo do agente
        AgentType agentType = agentIt->second.type;
        
        // Verificar se o tipo de agente tem permissões definidas
        auto permIt = m_permissions.find(agentType);
        if (permIt == m_permissions.end()) {
            Log("Agente sem permissões definidas: " + std::to_string(static_cast<int>(agentType)));
            return false;
        }
        
        // Verificar cada entrada de permissão para este tipo de agente
        for (const auto& permEntry : permIt->second) {
            // Verificar se a operação está permitida
            if (std::find(permEntry.operations.begin(), 
                        permEntry.operations.end(), 
                        operation) == permEntry.operations.end()) {
                continue;
            }
            
            // Verificar extensão do arquivo
            if (!MatchesExtension(path, permEntry.fileExtensions)) {
                continue;
            }
            
            // Verificar diretório
            bool inAllowedDirectory = false;
            for (const auto& dir : permEntry.directories) {
                if (IsFileInDirectory(path, dir, permEntry.includeSubdirectories)) {
                    inAllowedDirectory = true;
                    break;
                }
            }
            
            if (!inAllowedDirectory) {
                continue;
            }
            
            // Se chegou aqui, todas as verificações passaram
            return true;
        }
        
        // Nenhuma permissão encontrada
        return false;
    }
    
    /**
     * @brief Obtém o tipo de um agente registrado
     * @param agent Agente
     * @return Tipo do agente, ou UNKNOWN se não registrado
     */
    AgentType GetAgentType(BaseAgent* agent) {
        auto it = m_registeredAgents.find(agent);
        if (it != m_registeredAgents.end()) {
            return it->second.type;
        }
        return AgentType::UNKNOWN;
    }
    
    /**
     * @brief Obtém o nome de um agente registrado
     * @param agent Agente
     * @return Nome do agente, ou "Desconhecido" se não registrado
     */
    std::string GetAgentName(BaseAgent* agent) {
        auto it = m_registeredAgents.find(agent);
        if (it != m_registeredAgents.end()) {
            return it->second.name;
        }
        return "Desconhecido";
    }
    
    /**
     * @brief Registra uma ação de agente
     * @param agent Agente que realizou a ação
     * @param operation Operação realizada
     * @param targetPath Caminho do arquivo alvo
     * @param description Descrição da ação
     * @param success Indica se a operação foi bem-sucedida
     * @param backupPath Caminho do backup (se houver)
     */
    void LogAgentAction(BaseAgent* agent, 
                       FileOperation operation,
                       const std::string& targetPath,
                       const std::string& description,
                       bool success,
                       const std::string& backupPath = "") {
        AgentFileAction action;
        action.agentType = GetAgentType(agent);
        action.agentName = GetAgentName(agent);
        action.operation = operation;
        action.targetPath = targetPath;
        action.description = description;
        action.timestamp = GetCurrentTimestamp();
        action.backupPath = backupPath;
        action.success = success;
        
        // Adicionar ao histórico
        AddActionToHistory(action);
        
        // Registrar no log
        std::string logMessage = "Agente '" + action.agentName + "' (" + 
                              std::to_string(static_cast<int>(action.agentType)) + ") " +
                              (success ? "Sucesso" : "Falha") + " " +
                              OperationToString(operation) + " " +
                              targetPath;
        
        Log(logMessage);
    }
};

// Implementação da classe AgentFileAccessProvider

AgentFileAccessProvider::AgentFileAccessProvider(std::shared_ptr<GameFileManager> fileManager)
    : m_pImpl(std::make_unique<Impl>(fileManager))
    , m_fileManager(fileManager) {
}

AgentFileAccessProvider::~AgentFileAccessProvider() = default;

bool AgentFileAccessProvider::RegisterAgent(std::shared_ptr<BaseAgent> agent, 
                                          AgentType type,
                                          const std::string& name) {
    if (!agent) {
        m_pImpl->Log("Erro: Tentativa de registrar agente nulo");
        return false;
    }
    
    // Registrar o agente
    m_pImpl->m_registeredAgents[agent.get()] = {agent, type, name};
    
    m_pImpl->Log("Agente '" + name + "' registrado com sucesso (Tipo: " + 
               std::to_string(static_cast<int>(type)) + ")");
    
    return true;
}

bool AgentFileAccessProvider::SetFileTypePermissions(AgentType agentType, 
                                                  const std::vector<std::string>& fileExtensions,
                                                  const std::vector<FileOperation>& operations) {
    if (fileExtensions.empty() || operations.empty()) {
        m_pImpl->Log("Erro: Lista de extensões ou operações vazia");
        return false;
    }
    
    // Criar uma nova entrada de permissão
    PermissionEntry entry;
    entry.fileExtensions = fileExtensions;
    entry.operations = operations;
    entry.includeSubdirectories = true;
    
    // Permitir todos os diretórios (controle apenas por extensão)
    entry.directories.push_back("./"); // Diretório atual
    
    // Adicionar à lista de permissões
    m_pImpl->m_permissions[agentType].push_back(entry);
    
    m_pImpl->Log("Permissões por tipo de arquivo definidas para agente tipo " + 
               std::to_string(static_cast<int>(agentType)));
    
    return true;
}

bool AgentFileAccessProvider::SetDirectoryPermissions(AgentType agentType, 
                                                   const std::vector<std::string>& directoryPaths,
                                                   const std::vector<FileOperation>& operations,
                                                   bool includeSubdirectories) {
    if (directoryPaths.empty() || operations.empty()) {
        m_pImpl->Log("Erro: Lista de diretórios ou operações vazia");
        return false;
    }
    
    // Criar uma nova entrada de permissão
    PermissionEntry entry;
    entry.directories = directoryPaths;
    entry.operations = operations;
    entry.includeSubdirectories = includeSubdirectories;
    
    // Permitir todas as extensões (controle apenas por diretório)
    // Usar vetor vazio para indicar "todas as extensões"
    
    // Adicionar à lista de permissões
    m_pImpl->m_permissions[agentType].push_back(entry);
    
    m_pImpl->Log("Permissões por diretório definidas para agente tipo " + 
               std::to_string(static_cast<int>(agentType)));
    
    return true;
}

FileOperationResult AgentFileAccessProvider::AgentCreateFile(BaseAgent* agent, 
                                                          const std::string& path, 
                                                          const std::string& content,
                                                          const std::string& description) {
    // Verificar permissão
    if (!m_pImpl->HasPermission(agent, path, FileOperation::CREATE)) {
        m_pImpl->LogAgentAction(agent, FileOperation::CREATE, path, description, false);
        
        FileOperationResult result;
        result.success = false;
        result.message = "Permissão negada para criar arquivo";
        result.timestamp = GetCurrentTimestamp();
        result.originalPath = path;
        
        return result;
    }
    
    // Executar a operação
    FileOperationResult result = m_fileManager->CreateFile(path, content, description);
    
    // Registrar a ação
    m_pImpl->LogAgentAction(agent, FileOperation::CREATE, path, description, 
                          result.success, result.backupPath);
    
    return result;
}

FileOperationResult AgentFileAccessProvider::AgentModifyFile(BaseAgent* agent, 
                                                          const std::string& path, 
                                                          const std::string& newContent,
                                                          const std::string& description) {
    // Verificar permissão
    if (!m_pImpl->HasPermission(agent, path, FileOperation::MODIFY)) {
        m_pImpl->LogAgentAction(agent, FileOperation::MODIFY, path, description, false);
        
        FileOperationResult result;
        result.success = false;
        result.message = "Permissão negada para modificar arquivo";
        result.timestamp = GetCurrentTimestamp();
        result.originalPath = path;
        
        return result;
    }
    
    // Executar a operação
    FileOperationResult result = m_fileManager->ModifyFile(path, newContent, description);
    
    // Registrar a ação
    m_pImpl->LogAgentAction(agent, FileOperation::MODIFY, path, description, 
                          result.success, result.backupPath);
    
    return result;
}

FileOperationResult AgentFileAccessProvider::AgentModifyFilePart(BaseAgent* agent, 
                                                              const std::string& path, 
                                                              const std::string& oldContent,
                                                              const std::string& newContent,
                                                              const std::string& description) {
    // Verificar permissão
    if (!m_pImpl->HasPermission(agent, path, FileOperation::MODIFY)) {
        m_pImpl->LogAgentAction(agent, FileOperation::MODIFY, path, description, false);
        
        FileOperationResult result;
        result.success = false;
        result.message = "Permissão negada para modificar arquivo";
        result.timestamp = GetCurrentTimestamp();
        result.originalPath = path;
        
        return result;
    }
    
    // Executar a operação
    FileOperationResult result = m_fileManager->ModifyFilePart(path, oldContent, newContent, description);
    
    // Registrar a ação
    m_pImpl->LogAgentAction(agent, FileOperation::MODIFY, path, description, 
                          result.success, result.backupPath);
    
    return result;
}

FileOperationResult AgentFileAccessProvider::AgentDeleteFile(BaseAgent* agent, 
                                                          const std::string& path,
                                                          const std::string& description) {
    // Verificar permissão
    if (!m_pImpl->HasPermission(agent, path, FileOperation::DELETE)) {
        m_pImpl->LogAgentAction(agent, FileOperation::DELETE, path, description, false);
        
        FileOperationResult result;
        result.success = false;
        result.message = "Permissão negada para excluir arquivo";
        result.timestamp = GetCurrentTimestamp();
        result.originalPath = path;
        
        return result;
    }
    
    // Executar a operação
    FileOperationResult result = m_fileManager->DeleteFile(path, description);
    
    // Registrar a ação
    m_pImpl->LogAgentAction(agent, FileOperation::DELETE, path, description, 
                          result.success, result.backupPath);
    
    return result;
}

FileOperationResult AgentFileAccessProvider::AgentMoveFile(BaseAgent* agent, 
                                                        const std::string& sourcePath,
                                                        const std::string& destinationPath,
                                                        const std::string& description) {
    // Verificar permissão para origem e destino
    if (!m_pImpl->HasPermission(agent, sourcePath, FileOperation::DELETE) || 
        !m_pImpl->HasPermission(agent, destinationPath, FileOperation::CREATE)) {
        
        m_pImpl->LogAgentAction(agent, FileOperation::MOVE, 
                             sourcePath + " -> " + destinationPath, 
                             description, false);
        
        FileOperationResult result;
        result.success = false;
        result.message = "Permissão negada para mover arquivo";
        result.timestamp = GetCurrentTimestamp();
        result.originalPath = sourcePath;
        result.newPath = destinationPath;
        
        return result;
    }
    
    // Executar a operação
    FileOperationResult result = m_fileManager->MoveFile(sourcePath, destinationPath, description);
    
    // Registrar a ação
    m_pImpl->LogAgentAction(agent, FileOperation::MOVE, 
                         sourcePath + " -> " + destinationPath, 
                         description, result.success, result.backupPath);
    
    return result;
}

FileOperationResult AgentFileAccessProvider::AgentCopyFile(BaseAgent* agent, 
                                                        const std::string& sourcePath,
                                                        const std::string& destinationPath,
                                                        const std::string& description) {
    // Verificar permissão para origem e destino
    if (!m_pImpl->HasPermission(agent, sourcePath, FileOperation::MODIFY) || 
        !m_pImpl->HasPermission(agent, destinationPath, FileOperation::CREATE)) {
        
        m_pImpl->LogAgentAction(agent, FileOperation::COPY, 
                             sourcePath + " -> " + destinationPath, 
                             description, false);
        
        FileOperationResult result;
        result.success = false;
        result.message = "Permissão negada para copiar arquivo";
        result.timestamp = GetCurrentTimestamp();
        result.originalPath = sourcePath;
        result.newPath = destinationPath;
        
        return result;
    }
    
    // Executar a operação
    FileOperationResult result = m_fileManager->CopyFile(sourcePath, destinationPath, description);
    
    // Registrar a ação
    m_pImpl->LogAgentAction(agent, FileOperation::COPY, 
                         sourcePath + " -> " + destinationPath, 
                         description, result.success, result.backupPath);
    
    return result;
}

FileOperationResult AgentFileAccessProvider::AgentCompileJsonToBinary(BaseAgent* agent, 
                                                                   const std::string& jsonPath,
                                                                   const std::string& binPath,
                                                                   const std::string& description) {
    // Determinar caminho do binário se não fornecido
    std::string outputBinPath;
    if (binPath.empty()) {
        // Usar o mesmo caminho, mas com extensão .bin
        fs::path jsonPathObj(jsonPath);
        outputBinPath = (jsonPathObj.parent_path() / (jsonPathObj.stem().string() + ".bin")).string();
    } else {
        outputBinPath = binPath;
    }
    
    // Verificar permissão para ler JSON e escrever binário
    if (!m_pImpl->HasPermission(agent, jsonPath, FileOperation::MODIFY) || 
        !m_pImpl->HasPermission(agent, outputBinPath, FileOperation::CREATE)) {
        
        m_pImpl->LogAgentAction(agent, FileOperation::COMPILE, 
                             jsonPath + " -> " + outputBinPath, 
                             description, false);
        
        FileOperationResult result;
        result.success = false;
        result.message = "Permissão negada para compilar JSON para binário";
        result.timestamp = GetCurrentTimestamp();
        result.originalPath = jsonPath;
        result.newPath = outputBinPath;
        
        return result;
    }
    
    // Executar a operação
    FileOperationResult result = m_fileManager->CompileJsonToBinary(jsonPath, outputBinPath, description);
    
    // Registrar a ação
    m_pImpl->LogAgentAction(agent, FileOperation::COMPILE, 
                         jsonPath + " -> " + outputBinPath, 
                         description, result.success, result.backupPath);
    
    return result;
}

FileOperationResult AgentFileAccessProvider::AgentDecompileBinaryToJson(BaseAgent* agent, 
                                                                     const std::string& binPath,
                                                                     const std::string& jsonPath,
                                                                     const std::string& description) {
    // Determinar caminho do JSON se não fornecido
    std::string outputJsonPath;
    if (jsonPath.empty()) {
        // Usar o mesmo caminho, mas com extensão .json
        fs::path binPathObj(binPath);
        outputJsonPath = (binPathObj.parent_path() / (binPathObj.stem().string() + ".json")).string();
    } else {
        outputJsonPath = jsonPath;
    }
    
    // Verificar permissão para ler binário e escrever JSON
    if (!m_pImpl->HasPermission(agent, binPath, FileOperation::MODIFY) || 
        !m_pImpl->HasPermission(agent, outputJsonPath, FileOperation::CREATE)) {
        
        m_pImpl->LogAgentAction(agent, FileOperation::DECOMPILE, 
                             binPath + " -> " + outputJsonPath, 
                             description, false);
        
        FileOperationResult result;
        result.success = false;
        result.message = "Permissão negada para descompilar binário para JSON";
        result.timestamp = GetCurrentTimestamp();
        result.originalPath = binPath;
        result.newPath = outputJsonPath;
        
        return result;
    }
    
    // Executar a operação
    FileOperationResult result = m_fileManager->DecompileBinaryToJson(binPath, outputJsonPath, description);
    
    // Registrar a ação
    m_pImpl->LogAgentAction(agent, FileOperation::DECOMPILE, 
                         binPath + " -> " + outputJsonPath, 
                         description, result.success, result.backupPath);
    
    return result;
}

std::string AgentFileAccessProvider::AgentReadFile(BaseAgent* agent, 
                                                const std::string& path) {
    // Verificar permissão para ler o arquivo
    if (!m_pImpl->HasPermission(agent, path, FileOperation::MODIFY)) {
        m_pImpl->LogAgentAction(agent, FileOperation::MODIFY, path, "Leitura de arquivo", false);
        return "";
    }
    
    // Executar a operação
    std::string content = m_fileManager->ReadFileAsText(path);
    
    // Registrar a ação
    m_pImpl->LogAgentAction(agent, FileOperation::MODIFY, path, "Leitura de arquivo", !content.empty());
    
    return content;
}

DirectoryInfo AgentFileAccessProvider::AgentListDirectory(BaseAgent* agent, 
                                                       const std::string& directory,
                                                       bool recursive,
                                                       const std::string& filter) {
    // Verificar permissão para listar o diretório
    if (!m_pImpl->HasPermission(agent, directory, FileOperation::MODIFY)) {
        m_pImpl->LogAgentAction(agent, FileOperation::MODIFY, directory, "Listagem de diretório", false);
        
        DirectoryInfo emptyInfo;
        emptyInfo.path = directory;
        return emptyInfo;
    }
    
    // Executar a operação
    DirectoryInfo info = m_fileManager->ListDirectory(directory, recursive, filter);
    
    // Registrar a ação
    m_pImpl->LogAgentAction(agent, FileOperation::MODIFY, directory, "Listagem de diretório", 
                         info.fileCount > 0 || info.directoryCount > 0);
    
    return info;
}

FileOperationResult AgentFileAccessProvider::AgentRestoreBackup(BaseAgent* agent, 
                                                             const std::string& backupPath,
                                                             bool restoreToOriginalPath,
                                                             const std::string& alternativePath,
                                                             const std::string& description) {
    // Verificar permissão
    // Precisamos primeiro descobrir o caminho original ou alternativo
    std::string targetPath;
    if (restoreToOriginalPath) {
        // Encontrar o caminho original no registro de backups
        auto backups = m_fileManager->GetAllBackups();
        bool found = false;
        
        for (const auto& entry : backups) {
            for (const auto& backup : entry.second) {
                if (backup.backupPath == backupPath) {
                    targetPath = backup.originalPath;
                    found = true;
                    break;
                }
            }
            if (found) break;
        }
        
        if (!found) {
            m_pImpl->LogAgentAction(agent, FileOperation::MODIFY, backupPath, description, false);
            
            FileOperationResult result;
            result.success = false;
            result.message = "Backup não encontrado no registro";
            result.timestamp = GetCurrentTimestamp();
            result.originalPath = backupPath;
            
            return result;
        }
    } else {
        targetPath = alternativePath;
    }
    
    // Agora verificamos a permissão para o caminho alvo
    if (!m_pImpl->HasPermission(agent, targetPath, FileOperation::MODIFY)) {
        m_pImpl->LogAgentAction(agent, FileOperation::MODIFY, 
                             backupPath + " -> " + targetPath, 
                             description, false);
        
        FileOperationResult result;
        result.success = false;
        result.message = "Permissão negada para restaurar backup";
        result.timestamp = GetCurrentTimestamp();
        result.originalPath = backupPath;
        result.newPath = targetPath;
        
        return result;
    }
    
    // Executar a operação
    FileOperationResult result = m_fileManager->RestoreFromBackup(backupPath, restoreToOriginalPath, alternativePath);
    
    // Registrar a ação
    m_pImpl->LogAgentAction(agent, FileOperation::MODIFY, 
                         backupPath + " -> " + targetPath, 
                         description, result.success, result.backupPath);
    
    return result;
}

std::vector<AgentFileAction> AgentFileAccessProvider::GetAgentFileActionHistory(AgentType agentType,
                                                                            int maxEntries) {
    std::vector<AgentFileAction> filteredHistory;
    
    // Se agentType for UNKNOWN, retornar todo o histórico
    if (agentType == AgentType::UNKNOWN) {
        filteredHistory = m_pImpl->m_actionHistory;
    } else {
        // Filtrar por tipo de agente
        for (const auto& action : m_pImpl->m_actionHistory) {
            if (action.agentType == agentType) {
                filteredHistory.push_back(action);
            }
        }
    }
    
    // Ordenar do mais recente para o mais antigo
    std::sort(filteredHistory.begin(), filteredHistory.end(), 
             [](const AgentFileAction& a, const AgentFileAction& b) {
                 return a.timestamp > b.timestamp;
             });
    
    // Limitar o número de entradas se necessário
    if (maxEntries > 0 && filteredHistory.size() > static_cast<size_t>(maxEntries)) {
        filteredHistory.resize(maxEntries);
    }
    
    return filteredHistory;
}

std::vector<BackupInfo> AgentFileAccessProvider::GetBackupsForFile(const std::string& path) {
    return m_fileManager->GetBackupsForFile(path);
}

void AgentFileAccessProvider::SetLogCallback(std::function<void(const std::string&)> logCallback) {
    m_pImpl->m_logCallback = logCallback;
}

bool AgentFileAccessProvider::HasPermission(BaseAgent* agent, 
                                          const std::string& path, 
                                          FileOperation operation) {
    return m_pImpl->HasPermission(agent, path, operation);
}

AgentType AgentFileAccessProvider::GetAgentType(BaseAgent* agent) {
    return m_pImpl->GetAgentType(agent);
}

std::string AgentFileAccessProvider::GetAgentName(BaseAgent* agent) {
    return m_pImpl->GetAgentName(agent);
}

void AgentFileAccessProvider::LogAgentAction(BaseAgent* agent, 
                                           FileOperation operation,
                                           const std::string& targetPath,
                                           const std::string& description,
                                           bool success,
                                           const std::string& backupPath) {
    m_pImpl->LogAgentAction(agent, operation, targetPath, description, success, backupPath);
}

} // namespace WYDStudio