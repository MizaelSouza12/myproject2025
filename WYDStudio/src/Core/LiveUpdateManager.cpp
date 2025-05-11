#include "LiveUpdateManager.h"
#include "../decompilers/AdvancedBinaryAnalyzer.h"
#include <fstream>
#include <chrono>
#include <thread>
#include <filesystem>
#include <algorithm>
#include <ctime>
#include <random>
#include <sstream>
#include <iomanip>
#include <nlohmann/json.hpp>

namespace wydstudio {
namespace core {

class LiveUpdateManager::Impl {
public:
    Impl(LiveUpdateManager* parent) : m_parent(parent) {}
    
    // Implementações internas
    bool loadDefinitions(const std::string& path) {
        try {
            std::ifstream file(path);
            if (!file.is_open()) {
                return false;
            }
            
            json data = json::parse(file);
            if (data.contains("fileMappings") && data["fileMappings"].is_array()) {
                for (const auto& mapping : data["fileMappings"]) {
                    FileMapping fileMapping;
                    fileMapping.originalPath = mapping.value("originalPath", "");
                    fileMapping.decompPath = mapping.value("decompPath", "");
                    fileMapping.fileType = stringToFileType(mapping.value("fileType", "UNKNOWN"));
                    fileMapping.format = mapping.value("format", "json");
                    fileMapping.canEditCompiled = mapping.value("canEditCompiled", false);
                    fileMapping.needsServerReload = mapping.value("needsServerReload", true);
                    fileMapping.needsClientReload = mapping.value("needsClientReload", true);
                    fileMapping.decompilerModule = mapping.value("decompilerModule", "");
                    fileMapping.compilerModule = mapping.value("compilerModule", "");
                    
                    if (mapping.contains("metadata") && mapping["metadata"].is_object()) {
                        for (auto it = mapping["metadata"].begin(); it != mapping["metadata"].end(); ++it) {
                            fileMapping.metadata[it.key()] = it.value().get<std::string>();
                        }
                    }
                    
                    m_fileMappings[fileMapping.originalPath] = fileMapping;
                }
            }
            
            return true;
        } catch (const std::exception& e) {
            return false;
        }
    }
    
    bool saveDefinitions(const std::string& path) {
        try {
            json data;
            json mappings = json::array();
            
            for (const auto& [_, mapping] : m_fileMappings) {
                json mappingJson;
                mappingJson["originalPath"] = mapping.originalPath;
                mappingJson["decompPath"] = mapping.decompPath;
                mappingJson["fileType"] = fileTypeToString(mapping.fileType);
                mappingJson["format"] = mapping.format;
                mappingJson["canEditCompiled"] = mapping.canEditCompiled;
                mappingJson["needsServerReload"] = mapping.needsServerReload;
                mappingJson["needsClientReload"] = mapping.needsClientReload;
                mappingJson["decompilerModule"] = mapping.decompilerModule;
                mappingJson["compilerModule"] = mapping.compilerModule;
                
                json metadataJson = json::object();
                for (const auto& [key, value] : mapping.metadata) {
                    metadataJson[key] = value;
                }
                mappingJson["metadata"] = metadataJson;
                
                mappings.push_back(mappingJson);
            }
            
            data["fileMappings"] = mappings;
            
            std::ofstream file(path);
            if (!file.is_open()) {
                return false;
            }
            
            file << std::setw(4) << data;
            return true;
        } catch (const std::exception& e) {
            return false;
        }
    }
    
    // Monitoramento de arquivos
    void startFileWatcher() {
        m_running = true;
        m_lastFileCheckTime = std::chrono::system_clock::now();
        
        m_fileWatcherThread = std::thread([this]() {
            while (m_running) {
                checkFilesForChanges();
                std::this_thread::sleep_for(std::chrono::milliseconds(m_config.pollIntervalMs));
            }
        });
    }
    
    void stopFileWatcher() {
        m_running = false;
        
        if (m_fileWatcherThread.joinable()) {
            m_fileWatcherThread.join();
        }
    }
    
    void checkFilesForChanges() {
        auto now = std::chrono::system_clock::now();
        
        for (const auto& path : m_config.watchPaths) {
            if (!fs::exists(path)) {
                continue;
            }
            
            for (const auto& entry : fs::recursive_directory_iterator(path)) {
                if (!entry.is_regular_file()) {
                    continue;
                }
                
                // Verifica se o arquivo está na lista de exclusões
                bool excluded = false;
                for (const auto& excludePath : m_config.excludePaths) {
                    if (entry.path().string().find(excludePath) != std::string::npos) {
                        excluded = true;
                        break;
                    }
                }
                
                if (excluded) {
                    continue;
                }
                
                try {
                    auto lastWriteTime = fs::last_write_time(entry.path());
                    auto lastWriteTimePoint = std::chrono::system_clock::now() - std::chrono::seconds(1);
                    
                    // Se o arquivo foi modificado desde a última verificação
                    if (lastWriteTimePoint > m_lastFileCheckTime) {
                        handleModifiedFile(entry.path().string());
                    }
                } catch (const std::exception& e) {
                    // Ignora erros de acesso a arquivos
                }
            }
        }
        
        m_lastFileCheckTime = now;
    }
    
    void handleModifiedFile(const std::string& filePath) {
        // Verifica se temos um mapeamento para este arquivo
        auto it = m_fileMappings.find(filePath);
        if (it != m_fileMappings.end()) {
            // É um arquivo original (compilado)
            notifyFileChanged(filePath, it->second.fileType, true);
        } else {
            // Verifica se é um arquivo decompilado
            for (const auto& [origPath, mapping] : m_fileMappings) {
                if (mapping.decompPath == filePath) {
                    // É um arquivo decompilado
                    notifyFileChanged(filePath, mapping.fileType, false);
                    
                    // Recompila o arquivo automaticamente se configurado
                    if (m_config.autoRecompile) {
                        recompileFile(filePath, mapping.fileType, origPath);
                    }
                    
                    break;
                }
            }
        }
    }
    
    void notifyFileChanged(const std::string& filePath, FileType fileType, bool isCompiled) {
        UpdateNotification notification;
        notification.type = UpdateNotificationType::INFO;
        notification.message = "Arquivo " + filePath + " foi modificado.";
        notification.componentId = "";
        notification.entityId = "";
        notification.progress = 0;
        notification.timestamp = std::chrono::system_clock::now();
        
        m_parent->addNotification(notification);
        
        // Se o arquivo requer reload do servidor/cliente, adiciona à fila de operações
        bool needsReload = false;
        bool isServerFile = false;
        
        auto it = m_fileMappings.find(filePath);
        if (it != m_fileMappings.end()) {
            needsReload = it->second.needsServerReload || it->second.needsClientReload;
            isServerFile = it->second.needsServerReload;
        } else {
            // Procura nos arquivos decompilados
            for (const auto& [origPath, mapping] : m_fileMappings) {
                if (mapping.decompPath == filePath) {
                    needsReload = mapping.needsServerReload || mapping.needsClientReload;
                    isServerFile = mapping.needsServerReload;
                    break;
                }
            }
        }
        
        if (needsReload) {
            LiveOperationData operation;
            operation.id = generateOperationId();
            operation.name = "Auto-reload: " + fs::path(filePath).filename().string();
            operation.description = "Recarregar recurso após modificação de arquivo";
            operation.type = LiveOperationType::RELOAD_RESOURCE;
            operation.targetFile = filePath;
            operation.targetId = "";
            operation.fileType = fileType;
            operation.requestedBy = "LiveUpdateManager";
            operation.requestTime = std::chrono::system_clock::now();
            operation.status = LiveOperationStatus::PENDING;
            operation.progress = 0;
            
            std::lock_guard<std::mutex> lock(m_queueMutex);
            m_operationQueue.push(operation);
            m_queueCondition.notify_one();
        }
    }
    
    // Operações com arquivos
    bool recompileFile(const std::string& filePath, FileType fileType, const std::string& targetPath) {
        CompilationResult result = m_parent->compileFile(filePath, fileType, targetPath);
        
        if (result.success) {
            UpdateNotification notification;
            notification.type = UpdateNotificationType::SUCCESS;
            notification.message = "Arquivo " + filePath + " foi recompilado com sucesso.";
            notification.componentId = "";
            notification.entityId = "";
            notification.progress = 100;
            notification.timestamp = std::chrono::system_clock::now();
            
            m_parent->addNotification(notification);
            return true;
        } else {
            UpdateNotification notification;
            notification.type = UpdateNotificationType::ERROR;
            notification.message = "Falha ao recompilar arquivo " + filePath + ": " + result.errorMessage;
            notification.componentId = "";
            notification.entityId = "";
            notification.progress = 0;
            notification.timestamp = std::chrono::system_clock::now();
            
            m_parent->addNotification(notification);
            return false;
        }
    }
    
    bool decompileFile(const std::string& filePath, FileType fileType, const std::string& targetFormat) {
        DecompilationResult result = m_parent->decompileFile(filePath, fileType, targetFormat);
        
        if (result.success) {
            UpdateNotification notification;
            notification.type = UpdateNotificationType::SUCCESS;
            notification.message = "Arquivo " + filePath + " foi decompilado com sucesso.";
            notification.componentId = "";
            notification.entityId = "";
            notification.progress = 100;
            notification.timestamp = std::chrono::system_clock::now();
            
            m_parent->addNotification(notification);
            
            // Atualiza o mapeamento
            auto it = m_fileMappings.find(filePath);
            if (it != m_fileMappings.end()) {
                it->second.decompPath = result.filePath;
                it->second.format = result.format;
            } else {
                FileMapping mapping;
                mapping.originalPath = filePath;
                mapping.decompPath = result.filePath;
                mapping.fileType = fileType;
                mapping.format = result.format;
                mapping.canEditCompiled = false;
                mapping.needsServerReload = true;
                mapping.needsClientReload = (fileType == FileType::UI_COMPONENT || 
                                            fileType == FileType::UI_TEXTURE || 
                                            fileType == FileType::UI_CONFIG);
                
                m_fileMappings[filePath] = mapping;
            }
            
            return true;
        } else {
            UpdateNotification notification;
            notification.type = UpdateNotificationType::ERROR;
            notification.message = "Falha ao decompiler arquivo " + filePath + ": " + result.errorMessage;
            notification.componentId = "";
            notification.entityId = "";
            notification.progress = 0;
            notification.timestamp = std::chrono::system_clock::now();
            
            m_parent->addNotification(notification);
            return false;
        }
    }
    
    // Processamento de operações
    void startOperationProcessor() {
        m_running = true;
        
        m_operationProcessorThread = std::thread([this]() {
            while (m_running) {
                LiveOperationData operation;
                
                {
                    std::unique_lock<std::mutex> lock(m_queueMutex);
                    m_queueCondition.wait(lock, [this]() {
                        return !m_operationQueue.empty() || !m_running;
                    });
                    
                    if (!m_running) {
                        break;
                    }
                    
                    operation = m_operationQueue.front();
                    m_operationQueue.pop();
                }
                
                processOperation(operation);
            }
        });
    }
    
    void stopOperationProcessor() {
        m_running = false;
        m_queueCondition.notify_all();
        
        if (m_operationProcessorThread.joinable()) {
            m_operationProcessorThread.join();
        }
    }
    
    void processOperation(LiveOperationData& operation) {
        // Marca como em progresso
        {
            std::lock_guard<std::mutex> lock(m_operationsMutex);
            operation.status = LiveOperationStatus::IN_PROGRESS;
            operation.executionTime = std::chrono::system_clock::now();
            m_activeOperations[operation.id] = operation;
        }
        
        // Notifica início da operação
        UpdateNotification notification;
        notification.type = UpdateNotificationType::INFO;
        notification.message = "Iniciando operação: " + operation.name;
        notification.componentId = "";
        notification.entityId = operation.targetId;
        notification.progress = 0;
        notification.timestamp = std::chrono::system_clock::now();
        m_parent->addNotification(notification);
        
        // Processa baseado no tipo
        bool success = false;
        
        switch (operation.type) {
            case LiveOperationType::ADD:
                success = processAddOperation(operation);
                break;
            case LiveOperationType::MODIFY:
                success = processModifyOperation(operation);
                break;
            case LiveOperationType::REMOVE:
                success = processRemoveOperation(operation);
                break;
            case LiveOperationType::REPLACE_FILE:
                success = processReplaceFileOperation(operation);
                break;
            case LiveOperationType::PATCH_FILE:
                success = processPatchFileOperation(operation);
                break;
            case LiveOperationType::RELOAD_RESOURCE:
                success = processReloadResourceOperation(operation);
                break;
            case LiveOperationType::EXECUTE_SCRIPT:
                success = processExecuteScriptOperation(operation);
                break;
            case LiveOperationType::BROADCAST_NOTICE:
                success = processBroadcastNoticeOperation(operation);
                break;
            case LiveOperationType::RESTART_SERVICE:
                success = processRestartServiceOperation(operation);
                break;
        }
        
        // Atualiza status da operação
        {
            std::lock_guard<std::mutex> lock(m_operationsMutex);
            operation.status = success ? LiveOperationStatus::COMPLETED : LiveOperationStatus::FAILED;
            operation.progress = success ? 100 : 0;
            
            // Move para histórico de operações completas
            m_completedOperations.push_back(operation);
            
            // Remove das operações ativas
            m_activeOperations.erase(operation.id);
        }
        
        // Notifica conclusão
        notification.type = success ? UpdateNotificationType::SUCCESS : UpdateNotificationType::ERROR;
        notification.message = success ? "Operação concluída: " + operation.name : "Falha na operação: " + operation.name;
        notification.progress = success ? 100 : 0;
        notification.timestamp = std::chrono::system_clock::now();
        m_parent->addNotification(notification);
    }
    
    // Implementações de processamento de operações
    bool processAddOperation(LiveOperationData& operation) {
        // Implementação para adicionar item/entidade
        return false;  // Implementação placeholder
    }
    
    bool processModifyOperation(LiveOperationData& operation) {
        // Implementação para modificar item/entidade
        return false;  // Implementação placeholder
    }
    
    bool processRemoveOperation(LiveOperationData& operation) {
        // Implementação para remover item/entidade
        return false;  // Implementação placeholder
    }
    
    bool processReplaceFileOperation(LiveOperationData& operation) {
        // Implementação para substituir arquivo
        return false;  // Implementação placeholder
    }
    
    bool processPatchFileOperation(LiveOperationData& operation) {
        // Implementação para aplicar patch a arquivo
        return false;  // Implementação placeholder
    }
    
    bool processReloadResourceOperation(LiveOperationData& operation) {
        // Implementação para recarregar recurso
        
        // Verifica se o servidor está conectado
        if (!m_connectedToServer) {
            operation.statusMessage = "Não foi possível recarregar recurso: servidor não conectado";
            return false;
        }
        
        // Obtém informações do arquivo
        FileType fileType = operation.fileType;
        std::string resourceType = fileTypeToString(fileType);
        
        // Envia comando para o servidor recarregar o recurso
        json message;
        message["command"] = "reload_resource";
        message["resource_type"] = resourceType;
        message["file_path"] = operation.targetFile;
        
        // Envia para o servidor
        if (!sendToServer(message.dump())) {
            operation.statusMessage = "Falha ao enviar comando para o servidor";
            return false;
        }
        
        // Aguarda resposta (implementação simplificada)
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        operation.statusMessage = "Recurso recarregado com sucesso";
        return true;
    }
    
    bool processExecuteScriptOperation(LiveOperationData& operation) {
        // Implementação para executar script
        return false;  // Implementação placeholder
    }
    
    bool processBroadcastNoticeOperation(LiveOperationData& operation) {
        // Implementação para enviar notificação a jogadores
        
        // Verifica se o servidor está conectado
        if (!m_connectedToServer) {
            operation.statusMessage = "Não foi possível enviar notificação: servidor não conectado";
            return false;
        }
        
        // Extrai a mensagem da operação
        std::string message;
        if (std::holds_alternative<std::string>(operation.data)) {
            message = std::get<std::string>(operation.data);
        } else if (std::holds_alternative<json>(operation.data)) {
            const json& data = std::get<json>(operation.data);
            if (data.contains("message") && data["message"].is_string()) {
                message = data["message"].get<std::string>();
            } else {
                operation.statusMessage = "Formato de dados inválido para notificação";
                return false;
            }
        } else {
            operation.statusMessage = "Tipo de dados inválido para notificação";
            return false;
        }
        
        // Envia comando para o servidor
        json serverMessage;
        serverMessage["command"] = "broadcast_notice";
        serverMessage["message"] = message;
        
        // Parâmetros opcionais
        for (const auto& [key, value] : operation.parameters) {
            if (key == "color" && std::holds_alternative<std::string>(value)) {
                serverMessage["color"] = std::get<std::string>(value);
            } else if (key == "duration" && std::holds_alternative<int>(value)) {
                serverMessage["duration"] = std::get<int>(value);
            } else if (key == "type" && std::holds_alternative<std::string>(value)) {
                serverMessage["type"] = std::get<std::string>(value);
            }
        }
        
        // Envia para o servidor
        if (!sendToServer(serverMessage.dump())) {
            operation.statusMessage = "Falha ao enviar comando para o servidor";
            return false;
        }
        
        operation.statusMessage = "Notificação enviada com sucesso";
        return true;
    }
    
    bool processRestartServiceOperation(LiveOperationData& operation) {
        // Implementação para reiniciar serviço
        
        // Verifica se o servidor está conectado
        if (!m_connectedToServer) {
            operation.statusMessage = "Não foi possível reiniciar serviço: servidor não conectado";
            return false;
        }
        
        // Extrai o nome do serviço da operação
        std::string serviceName;
        if (std::holds_alternative<std::string>(operation.data)) {
            serviceName = std::get<std::string>(operation.data);
        } else if (std::holds_alternative<json>(operation.data)) {
            const json& data = std::get<json>(operation.data);
            if (data.contains("service") && data["service"].is_string()) {
                serviceName = data["service"].get<std::string>();
            } else {
                operation.statusMessage = "Formato de dados inválido para reinício de serviço";
                return false;
            }
        } else {
            operation.statusMessage = "Tipo de dados inválido para reinício de serviço";
            return false;
        }
        
        // Envia comando para o servidor
        json serverMessage;
        serverMessage["command"] = "restart_service";
        serverMessage["service"] = serviceName;
        
        // Envia para o servidor
        if (!sendToServer(serverMessage.dump())) {
            operation.statusMessage = "Falha ao enviar comando para o servidor";
            return false;
        }
        
        // Aguarda resposta (implementação simplificada)
        std::this_thread::sleep_for(std::chrono::seconds(2));
        
        operation.statusMessage = "Serviço reiniciado com sucesso";
        return true;
    }
    
    // Comunicação com o servidor
    void startServerConnection() {
        m_running = true;
        
        m_serverConnectionThread = std::thread([this]() {
            while (m_running) {
                if (!m_connectedToServer) {
                    if (connectToServer()) {
                        UpdateNotification notification;
                        notification.type = UpdateNotificationType::SUCCESS;
                        notification.message = "Conectado ao servidor em " + m_config.serverApiUrl;
                        notification.timestamp = std::chrono::system_clock::now();
                        m_parent->addNotification(notification);
                    } else {
                        UpdateNotification notification;
                        notification.type = UpdateNotificationType::WARNING;
                        notification.message = "Falha ao conectar ao servidor em " + m_config.serverApiUrl + 
                                              ". Tentando novamente em " + std::to_string(m_config.reconnectIntervalSec) + " segundos";
                        notification.timestamp = std::chrono::system_clock::now();
                        m_parent->addNotification(notification);
                        
                        // Espera antes de tentar novamente
                        std::this_thread::sleep_for(std::chrono::seconds(m_config.reconnectIntervalSec));
                        continue;
                    }
                }
                
                // Processa mensagens do servidor
                std::string message;
                if (receiveFromServer(message)) {
                    handleServerMessage(message);
                }
                
                // Pequena pausa para não sobrecarregar a CPU
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        });
    }
    
    void stopServerConnection() {
        m_running = false;
        
        if (m_serverConnectionThread.joinable()) {
            m_serverConnectionThread.join();
        }
    }
    
    bool connectToServer() {
        // Implementação simplificada para fins de demonstração
        // Em um produto real, esta função estabeleceria uma conexão de websocket ou HTTP
        m_connectedToServer = true;
        return m_connectedToServer;
    }
    
    bool sendToServer(const std::string& message) {
        // Implementação simplificada para fins de demonstração
        // Em um produto real, esta função enviaria dados através da conexão estabelecida
        return m_connectedToServer;
    }
    
    bool receiveFromServer(std::string& message) {
        // Implementação simplificada para fins de demonstração
        // Em um produto real, esta função receberia dados do servidor
        return false;
    }
    
    void handleServerMessage(const std::string& message) {
        try {
            json data = json::parse(message);
            
            // Processa comandos do servidor
            if (data.contains("command")) {
                std::string command = data["command"].get<std::string>();
                
                if (command == "resource_reloaded") {
                    // Servidor notifica que um recurso foi recarregado
                    std::string resourceType = data.value("resource_type", "unknown");
                    std::string filePath = data.value("file_path", "unknown");
                    
                    UpdateNotification notification;
                    notification.type = UpdateNotificationType::INFO;
                    notification.message = "Servidor recarregou recurso: " + resourceType + " (" + filePath + ")";
                    notification.timestamp = std::chrono::system_clock::now();
                    m_parent->addNotification(notification);
                }
                else if (command == "file_changed") {
                    // Servidor notifica que um arquivo foi alterado
                    std::string filePath = data.value("file_path", "unknown");
                    
                    UpdateNotification notification;
                    notification.type = UpdateNotificationType::INFO;
                    notification.message = "Servidor detectou alteração em: " + filePath;
                    notification.timestamp = std::chrono::system_clock::now();
                    m_parent->addNotification(notification);
                }
                else if (command == "service_restarted") {
                    // Servidor notifica que um serviço foi reiniciado
                    std::string serviceName = data.value("service", "unknown");
                    
                    UpdateNotification notification;
                    notification.type = UpdateNotificationType::SUCCESS;
                    notification.message = "Servidor reiniciou serviço: " + serviceName;
                    notification.timestamp = std::chrono::system_clock::now();
                    m_parent->addNotification(notification);
                }
            }
        } catch (const std::exception& e) {
            UpdateNotification notification;
            notification.type = UpdateNotificationType::ERROR;
            notification.message = "Erro ao processar mensagem do servidor: " + std::string(e.what());
            notification.timestamp = std::chrono::system_clock::now();
            m_parent->addNotification(notification);
        }
    }
    
    // Utilitários
    std::string generateOperationId() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 15);
        std::uniform_int_distribution<> dis2(8, 11);
        
        std::stringstream ss;
        ss << std::hex;
        
        for (int i = 0; i < 8; i++) {
            ss << dis(gen);
        }
        ss << "-";
        for (int i = 0; i < 4; i++) {
            ss << dis(gen);
        }
        ss << "-4";
        for (int i = 0; i < 3; i++) {
            ss << dis(gen);
        }
        ss << "-";
        ss << dis2(gen);
        for (int i = 0; i < 3; i++) {
            ss << dis(gen);
        }
        ss << "-";
        for (int i = 0; i < 12; i++) {
            ss << dis(gen);
        }
        
        return ss.str();
    }
    
    FileType stringToFileType(const std::string& typeStr) {
        if (typeStr == "ITEM_LIST") return FileType::ITEM_LIST;
        if (typeStr == "ITEM_NAME") return FileType::ITEM_NAME;
        if (typeStr == "ITEM_HELP") return FileType::ITEM_HELP;
        if (typeStr == "ITEM_PRICE") return FileType::ITEM_PRICE;
        if (typeStr == "MAP_DATA") return FileType::MAP_DATA;
        if (typeStr == "MAP_HEIGHT") return FileType::MAP_HEIGHT;
        if (typeStr == "MAP_ATTRIBUTE") return FileType::MAP_ATTRIBUTE;
        if (typeStr == "NPC_DATA") return FileType::NPC_DATA;
        if (typeStr == "MOB_DATA") return FileType::MOB_DATA;
        if (typeStr == "SKILL_DATA") return FileType::SKILL_DATA;
        if (typeStr == "QUEST_DATA") return FileType::QUEST_DATA;
        if (typeStr == "EVENT_DATA") return FileType::EVENT_DATA;
        if (typeStr == "UI_COMPONENT") return FileType::UI_COMPONENT;
        if (typeStr == "UI_TEXTURE") return FileType::UI_TEXTURE;
        if (typeStr == "UI_CONFIG") return FileType::UI_CONFIG;
        if (typeStr == "GAME_CONFIG") return FileType::GAME_CONFIG;
        if (typeStr == "SERVER_CONFIG") return FileType::SERVER_CONFIG;
        if (typeStr == "TEXT_RESOURCE") return FileType::TEXT_RESOURCE;
        if (typeStr == "CUSTOM_BINARY") return FileType::CUSTOM_BINARY;
        if (typeStr == "JSON_DATA") return FileType::JSON_DATA;
        if (typeStr == "XML_DATA") return FileType::XML_DATA;
        if (typeStr == "INI_DATA") return FileType::INI_DATA;
        if (typeStr == "CSV_DATA") return FileType::CSV_DATA;
        if (typeStr == "TEXT_DATA") return FileType::TEXT_DATA;
        return FileType::CUSTOM_BINARY;
    }
    
    std::string fileTypeToString(FileType type) {
        switch (type) {
            case FileType::ITEM_LIST: return "ITEM_LIST";
            case FileType::ITEM_NAME: return "ITEM_NAME";
            case FileType::ITEM_HELP: return "ITEM_HELP";
            case FileType::ITEM_PRICE: return "ITEM_PRICE";
            case FileType::MAP_DATA: return "MAP_DATA";
            case FileType::MAP_HEIGHT: return "MAP_HEIGHT";
            case FileType::MAP_ATTRIBUTE: return "MAP_ATTRIBUTE";
            case FileType::NPC_DATA: return "NPC_DATA";
            case FileType::MOB_DATA: return "MOB_DATA";
            case FileType::SKILL_DATA: return "SKILL_DATA";
            case FileType::QUEST_DATA: return "QUEST_DATA";
            case FileType::EVENT_DATA: return "EVENT_DATA";
            case FileType::UI_COMPONENT: return "UI_COMPONENT";
            case FileType::UI_TEXTURE: return "UI_TEXTURE";
            case FileType::UI_CONFIG: return "UI_CONFIG";
            case FileType::GAME_CONFIG: return "GAME_CONFIG";
            case FileType::SERVER_CONFIG: return "SERVER_CONFIG";
            case FileType::TEXT_RESOURCE: return "TEXT_RESOURCE";
            case FileType::CUSTOM_BINARY: return "CUSTOM_BINARY";
            case FileType::JSON_DATA: return "JSON_DATA";
            case FileType::XML_DATA: return "XML_DATA";
            case FileType::INI_DATA: return "INI_DATA";
            case FileType::CSV_DATA: return "CSV_DATA";
            case FileType::TEXT_DATA: return "TEXT_DATA";
            default: return "UNKNOWN";
        }
    }
    
    LiveOperationType stringToOperationType(const std::string& typeStr) {
        if (typeStr == "ADD") return LiveOperationType::ADD;
        if (typeStr == "MODIFY") return LiveOperationType::MODIFY;
        if (typeStr == "REMOVE") return LiveOperationType::REMOVE;
        if (typeStr == "REPLACE_FILE") return LiveOperationType::REPLACE_FILE;
        if (typeStr == "PATCH_FILE") return LiveOperationType::PATCH_FILE;
        if (typeStr == "RELOAD_RESOURCE") return LiveOperationType::RELOAD_RESOURCE;
        if (typeStr == "EXECUTE_SCRIPT") return LiveOperationType::EXECUTE_SCRIPT;
        if (typeStr == "BROADCAST_NOTICE") return LiveOperationType::BROADCAST_NOTICE;
        if (typeStr == "RESTART_SERVICE") return LiveOperationType::RESTART_SERVICE;
        return LiveOperationType::RELOAD_RESOURCE;
    }
    
    std::string operationTypeToString(LiveOperationType type) {
        switch (type) {
            case LiveOperationType::ADD: return "ADD";
            case LiveOperationType::MODIFY: return "MODIFY";
            case LiveOperationType::REMOVE: return "REMOVE";
            case LiveOperationType::REPLACE_FILE: return "REPLACE_FILE";
            case LiveOperationType::PATCH_FILE: return "PATCH_FILE";
            case LiveOperationType::RELOAD_RESOURCE: return "RELOAD_RESOURCE";
            case LiveOperationType::EXECUTE_SCRIPT: return "EXECUTE_SCRIPT";
            case LiveOperationType::BROADCAST_NOTICE: return "BROADCAST_NOTICE";
            case LiveOperationType::RESTART_SERVICE: return "RESTART_SERVICE";
            default: return "UNKNOWN";
        }
    }
    
    LiveOperationStatus stringToOperationStatus(const std::string& statusStr) {
        if (statusStr == "PENDING") return LiveOperationStatus::PENDING;
        if (statusStr == "IN_PROGRESS") return LiveOperationStatus::IN_PROGRESS;
        if (statusStr == "COMPLETED") return LiveOperationStatus::COMPLETED;
        if (statusStr == "FAILED") return LiveOperationStatus::FAILED;
        if (statusStr == "REVERTED") return LiveOperationStatus::REVERTED;
        if (statusStr == "PARTIAL_SUCCESS") return LiveOperationStatus::PARTIAL_SUCCESS;
        return LiveOperationStatus::PENDING;
    }
    
    std::string operationStatusToString(LiveOperationStatus status) {
        switch (status) {
            case LiveOperationStatus::PENDING: return "PENDING";
            case LiveOperationStatus::IN_PROGRESS: return "IN_PROGRESS";
            case LiveOperationStatus::COMPLETED: return "COMPLETED";
            case LiveOperationStatus::FAILED: return "FAILED";
            case LiveOperationStatus::REVERTED: return "REVERTED";
            case LiveOperationStatus::PARTIAL_SUCCESS: return "PARTIAL_SUCCESS";
            default: return "UNKNOWN";
        }
    }

private:
    LiveUpdateManager* m_parent;
    
    // Estado
    std::atomic<bool> m_running;
    bool m_connectedToServer = false;
    
    // Configuração
    LiveUpdateConfig m_config;
    
    // Threads
    std::thread m_fileWatcherThread;
    std::thread m_operationProcessorThread;
    std::thread m_serverConnectionThread;
    
    // Monitoramento de arquivos
    std::chrono::system_clock::time_point m_lastFileCheckTime;
    
    // Mapeamento de arquivos
    std::map<std::string, FileMapping> m_fileMappings;
    
    // Fila de operações
    std::queue<LiveOperationData> m_operationQueue;
    std::mutex m_queueMutex;
    std::condition_variable m_queueCondition;
    
    // Operações ativas e históricas
    std::map<std::string, LiveOperationData> m_activeOperations;
    std::vector<LiveOperationData> m_completedOperations;
    std::mutex m_operationsMutex;
};

// Implementação dos métodos da classe LiveUpdateManager

LiveUpdateManager::LiveUpdateManager(const std::shared_ptr<WYDStudioFacade>& facade)
    : m_facade(facade), m_running(false), m_nextCallbackId(0)
{
    pImpl = std::make_unique<Impl>(this);
}

LiveUpdateManager::~LiveUpdateManager()
{
    stop();
}

bool LiveUpdateManager::initialize(const LiveUpdateConfig& config)
{
    m_config = config;
    
    if (!initializeDirectories()) {
        return false;
    }
    
    if (!loadFileMappings()) {
        // Não é um erro crítico se os mapeamentos não puderam ser carregados
        logInfo("Nenhum mapeamento de arquivo encontrado. Será criado quando necessário.");
    }
    
    return true;
}

bool LiveUpdateManager::start()
{
    if (m_running) {
        return true;
    }
    
    m_running = true;
    
    // Inicia threads
    pImpl->startFileWatcher();
    pImpl->startOperationProcessor();
    pImpl->startServerConnection();
    
    return true;
}

void LiveUpdateManager::stop()
{
    if (!m_running) {
        return;
    }
    
    m_running = false;
    
    // Para threads
    pImpl->stopFileWatcher();
    pImpl->stopOperationProcessor();
    pImpl->stopServerConnection();
    
    // Salva mapeamentos
    saveFileMappings();
}

bool LiveUpdateManager::scanFiles()
{
    // Implementação simplificada
    return true;
}

bool LiveUpdateManager::isRunning() const
{
    return m_running;
}

std::string LiveUpdateManager::queueOperation(const LiveOperationData& operation)
{
    if (!m_running) {
        return "";
    }
    
    LiveOperationData newOperation = operation;
    newOperation.id = generateOperationId();
    newOperation.status = LiveOperationStatus::PENDING;
    newOperation.requestTime = std::chrono::system_clock::now();
    
    std::lock_guard<std::mutex> lock(m_queueMutex);
    m_operationQueue.push(newOperation);
    m_queueCondition.notify_one();
    
    return newOperation.id;
}

bool LiveUpdateManager::cancelOperation(const std::string& operationId)
{
    std::lock_guard<std::mutex> lock(m_operationsMutex);
    
    auto it = m_activeOperations.find(operationId);
    if (it != m_activeOperations.end() && it->second.status == LiveOperationStatus::PENDING) {
        it->second.status = LiveOperationStatus::FAILED;
        it->second.statusMessage = "Operação cancelada pelo usuário";
        
        // Move para histórico
        m_completedOperations.push_back(it->second);
        m_activeOperations.erase(it);
        
        return true;
    }
    
    return false;
}

LiveOperationStatus LiveUpdateManager::getOperationStatus(const std::string& operationId)
{
    std::lock_guard<std::mutex> lock(m_operationsMutex);
    
    auto it = m_activeOperations.find(operationId);
    if (it != m_activeOperations.end()) {
        return it->second.status;
    }
    
    // Procura nas operações concluídas
    for (const auto& op : m_completedOperations) {
        if (op.id == operationId) {
            return op.status;
        }
    }
    
    return LiveOperationStatus::FAILED;
}

std::optional<LiveOperationData> LiveUpdateManager::getOperationDetails(const std::string& operationId)
{
    std::lock_guard<std::mutex> lock(m_operationsMutex);
    
    auto it = m_activeOperations.find(operationId);
    if (it != m_activeOperations.end()) {
        return it->second;
    }
    
    // Procura nas operações concluídas
    for (const auto& op : m_completedOperations) {
        if (op.id == operationId) {
            return op;
        }
    }
    
    return std::nullopt;
}

std::vector<LiveOperationData> LiveUpdateManager::getRecentOperations(int32_t count, bool includeCompleted)
{
    std::lock_guard<std::mutex> lock(m_operationsMutex);
    
    std::vector<LiveOperationData> operations;
    
    // Adiciona operações ativas
    for (const auto& [_, operation] : m_activeOperations) {
        operations.push_back(operation);
    }
    
    // Adiciona operações concluídas
    if (includeCompleted && !m_completedOperations.empty()) {
        // Obtém as operações mais recentes (as últimas na lista)
        int32_t start = std::max(0, static_cast<int32_t>(m_completedOperations.size()) - count);
        operations.insert(operations.end(), 
                          m_completedOperations.begin() + start,
                          m_completedOperations.end());
    }
    
    // Limita o número de operações se necessário
    if (operations.size() > static_cast<size_t>(count)) {
        operations.resize(count);
    }
    
    return operations;
}

std::vector<UpdateNotification> LiveUpdateManager::getRecentNotifications(int32_t count)
{
    std::lock_guard<std::mutex> lock(m_notificationsMutex);
    
    if (count >= static_cast<int32_t>(m_recentNotifications.size())) {
        return m_recentNotifications;
    }
    
    return std::vector<UpdateNotification>(
        m_recentNotifications.end() - count,
        m_recentNotifications.end()
    );
}

DecompilationResult LiveUpdateManager::decompileFile(const std::string& filePath, FileType fileType, const std::string& forcedFormat)
{
    DecompilationResult result;
    result.success = false;
    result.originalPath = filePath;
    
    // Verifica se o arquivo existe
    if (!m_facade->fileExists(filePath)) {
        result.errorMessage = "Arquivo não encontrado: " + filePath;
        return result;
    }
    
    // Obtém o analisador binário
    auto analyzer = m_facade->getBinaryAnalyzer();
    if (!analyzer) {
        result.errorMessage = "Analisador binário não disponível";
        return result;
    }
    
    // Converte o tipo de arquivo
    decompilers::BinaryFileType binFileType;
    switch (fileType) {
        case FileType::ITEM_LIST: binFileType = decompilers::BinaryFileType::ITEM_LIST; break;
        case FileType::ITEM_NAME: binFileType = decompilers::BinaryFileType::ITEM_NAME; break;
        case FileType::ITEM_HELP: binFileType = decompilers::BinaryFileType::ITEM_HELP; break;
        case FileType::ITEM_PRICE: binFileType = decompilers::BinaryFileType::ITEM_PRICE; break;
        case FileType::MAP_DATA: binFileType = decompilers::BinaryFileType::MAP_DATA; break;
        case FileType::MAP_HEIGHT: binFileType = decompilers::BinaryFileType::MAP_HEIGHT; break;
        case FileType::MAP_ATTRIBUTE: binFileType = decompilers::BinaryFileType::MAP_ATTRIBUTE; break;
        case FileType::NPC_DATA: binFileType = decompilers::BinaryFileType::NPC_DATA; break;
        case FileType::MOB_DATA: binFileType = decompilers::BinaryFileType::MOB_DATA; break;
        case FileType::SKILL_DATA: binFileType = decompilers::BinaryFileType::SKILL_DATA; break;
        case FileType::QUEST_DATA: binFileType = decompilers::BinaryFileType::QUEST_DATA; break;
        case FileType::EVENT_DATA: binFileType = decompilers::BinaryFileType::EVENT_DATA; break;
        case FileType::UI_COMPONENT: binFileType = decompilers::BinaryFileType::UI_COMPONENT; break;
        case FileType::UI_TEXTURE: binFileType = decompilers::BinaryFileType::UI_TEXTURE; break;
        case FileType::UI_CONFIG: binFileType = decompilers::BinaryFileType::UI_CONFIG; break;
        case FileType::GAME_CONFIG: binFileType = decompilers::BinaryFileType::GAME_CONFIG; break;
        case FileType::SERVER_CONFIG: binFileType = decompilers::BinaryFileType::SERVER_CONFIG; break;
        case FileType::TEXT_RESOURCE: binFileType = decompilers::BinaryFileType::TEXT_RESOURCE; break;
        default: binFileType = decompilers::BinaryFileType::UNKNOWN; break;
    }
    
    // Decompila o arquivo
    auto binResult = analyzer->decompileFile(filePath, binFileType, forcedFormat);
    
    // Converte o resultado
    result.success = binResult.success;
    result.filePath = binResult.targetPath;
    result.format = binResult.format;
    result.errorMessage = binResult.errorMessage;
    
    // Copia os metadados
    for (const auto& [key, value] : binResult.metadata) {
        result.metadata[key] = value;
    }
    
    return result;
}

CompilationResult LiveUpdateManager::compileFile(const std::string& filePath, FileType fileType, const std::string& targetPath)
{
    CompilationResult result;
    result.success = false;
    result.sourcePath = filePath;
    
    // Verifica se o arquivo existe
    if (!m_facade->fileExists(filePath)) {
        result.errorMessage = "Arquivo não encontrado: " + filePath;
        return result;
    }
    
    // Obtém o analisador binário
    auto analyzer = m_facade->getBinaryAnalyzer();
    if (!analyzer) {
        result.errorMessage = "Analisador binário não disponível";
        return result;
    }
    
    // Converte o tipo de arquivo
    decompilers::BinaryFileType binFileType;
    switch (fileType) {
        case FileType::ITEM_LIST: binFileType = decompilers::BinaryFileType::ITEM_LIST; break;
        case FileType::ITEM_NAME: binFileType = decompilers::BinaryFileType::ITEM_NAME; break;
        case FileType::ITEM_HELP: binFileType = decompilers::BinaryFileType::ITEM_HELP; break;
        case FileType::ITEM_PRICE: binFileType = decompilers::BinaryFileType::ITEM_PRICE; break;
        case FileType::MAP_DATA: binFileType = decompilers::BinaryFileType::MAP_DATA; break;
        case FileType::MAP_HEIGHT: binFileType = decompilers::BinaryFileType::MAP_HEIGHT; break;
        case FileType::MAP_ATTRIBUTE: binFileType = decompilers::BinaryFileType::MAP_ATTRIBUTE; break;
        case FileType::NPC_DATA: binFileType = decompilers::BinaryFileType::NPC_DATA; break;
        case FileType::MOB_DATA: binFileType = decompilers::BinaryFileType::MOB_DATA; break;
        case FileType::SKILL_DATA: binFileType = decompilers::BinaryFileType::SKILL_DATA; break;
        case FileType::QUEST_DATA: binFileType = decompilers::BinaryFileType::QUEST_DATA; break;
        case FileType::EVENT_DATA: binFileType = decompilers::BinaryFileType::EVENT_DATA; break;
        case FileType::UI_COMPONENT: binFileType = decompilers::BinaryFileType::UI_COMPONENT; break;
        case FileType::UI_TEXTURE: binFileType = decompilers::BinaryFileType::UI_TEXTURE; break;
        case FileType::UI_CONFIG: binFileType = decompilers::BinaryFileType::UI_CONFIG; break;
        case FileType::GAME_CONFIG: binFileType = decompilers::BinaryFileType::GAME_CONFIG; break;
        case FileType::SERVER_CONFIG: binFileType = decompilers::BinaryFileType::SERVER_CONFIG; break;
        case FileType::TEXT_RESOURCE: binFileType = decompilers::BinaryFileType::TEXT_RESOURCE; break;
        default: binFileType = decompilers::BinaryFileType::UNKNOWN; break;
    }
    
    // Compila o arquivo
    auto binResult = analyzer->compileFile(filePath, binFileType, targetPath);
    
    // Converte o resultado
    result.success = binResult.success;
    result.filePath = binResult.targetPath;
    result.errorMessage = binResult.errorMessage;
    
    // Copia os metadados
    for (const auto& [key, value] : binResult.metadata) {
        result.metadata[key] = value;
    }
    
    return result;
}

std::optional<FileMapping> LiveUpdateManager::getFileMapping(const std::string& filePath)
{
    std::lock_guard<std::mutex> lock(m_mappingsMutex);
    
    auto it = m_fileMappings.find(filePath);
    if (it != m_fileMappings.end()) {
        return it->second;
    }
    
    // Verifica se é um arquivo decompilado
    for (const auto& [origPath, mapping] : m_fileMappings) {
        if (mapping.decompPath == filePath) {
            return mapping;
        }
    }
    
    return std::nullopt;
}

std::vector<FileMapping> LiveUpdateManager::getFileMappingsByType(FileType fileType)
{
    std::lock_guard<std::mutex> lock(m_mappingsMutex);
    
    std::vector<FileMapping> mappings;
    
    for (const auto& [_, mapping] : m_fileMappings) {
        if (mapping.fileType == fileType) {
            mappings.push_back(mapping);
        }
    }
    
    return mappings;
}

int32_t LiveUpdateManager::addNotificationCallback(std::function<void(const UpdateNotification&)> callback)
{
    std::lock_guard<std::mutex> lock(m_callbacksMutex);
    
    int32_t id = m_nextCallbackId++;
    m_notificationCallbacks[id] = callback;
    
    return id;
}

void LiveUpdateManager::removeNotificationCallback(int32_t callbackId)
{
    std::lock_guard<std::mutex> lock(m_callbacksMutex);
    
    m_notificationCallbacks.erase(callbackId);
}

bool LiveUpdateManager::applyToLiveServer(const std::string& operationId)
{
    // Implementação simplificada
    return true;
}

bool LiveUpdateManager::revertOperation(const std::string& operationId)
{
    // Implementação simplificada
    return false;
}

bool LiveUpdateManager::canEditCompiledFile(const std::string& filePath)
{
    auto mapping = getFileMapping(filePath);
    
    if (mapping) {
        return mapping->canEditCompiled;
    }
    
    // Se não temos informações, assumimos que não é editável diretamente
    return false;
}

bool LiveUpdateManager::requiresServerRestart(FileType fileType)
{
    // Implementação simplificada - mapear quais tipos requerem reinício
    switch (fileType) {
        case FileType::SERVER_CONFIG:
        case FileType::GAME_CONFIG:
            return true;
        default:
            return false;
    }
}

json LiveUpdateManager::getUpdateStatistics()
{
    // Implementação simplificada
    json stats;
    stats["operationsCount"] = m_completedOperations.size();
    stats["activeOperations"] = m_activeOperations.size();
    stats["mappedFiles"] = m_fileMappings.size();
    return stats;
}

bool LiveUpdateManager::forceServerResourceReload(const std::string& resourceType)
{
    // Implementação simplificada
    return false;
}

bool LiveUpdateManager::updateConfig(const LiveUpdateConfig& newConfig)
{
    m_config = newConfig;
    return true;
}

bool LiveUpdateManager::registerCustomFileType(const std::string& extension, FileType fileType, 
                                             bool canEditCompiled, const std::string& decompilerModule,
                                             const std::string& compilerModule)
{
    // Implementação simplificada
    return true;
}

bool LiveUpdateManager::initializeDirectories()
{
    // Verifica e cria os diretórios necessários
    if (!m_facade->createDirectory(m_config.backupDir, true)) {
        logError("Não foi possível criar diretório de backup: " + m_config.backupDir);
        return false;
    }
    
    if (!m_facade->createDirectory(m_config.tempDir, true)) {
        logError("Não foi possível criar diretório temporário: " + m_config.tempDir);
        return false;
    }
    
    return true;
}

bool LiveUpdateManager::loadFileMappings()
{
    std::string mappingsPath = m_config.dataDir + "/file_mappings.json";
    
    if (!m_facade->fileExists(mappingsPath)) {
        return false;
    }
    
    try {
        std::string content = m_facade->readTextFile(mappingsPath);
        if (content.empty()) {
            return false;
        }
        
        json data = json::parse(content);
        
        if (data.contains("mappings") && data["mappings"].is_array()) {
            std::lock_guard<std::mutex> lock(m_mappingsMutex);
            m_fileMappings.clear();
            
            for (const auto& item : data["mappings"]) {
                FileMapping mapping;
                mapping.originalPath = item.value("originalPath", "");
                mapping.decompPath = item.value("decompPath", "");
                mapping.fileType = stringToFileType(item.value("fileType", "UNKNOWN"));
                mapping.format = item.value("format", "json");
                mapping.canEditCompiled = item.value("canEditCompiled", false);
                mapping.needsServerReload = item.value("needsServerReload", true);
                mapping.needsClientReload = item.value("needsClientReload", true);
                mapping.decompilerModule = item.value("decompilerModule", "");
                mapping.compilerModule = item.value("compilerModule", "");
                
                if (item.contains("metadata") && item["metadata"].is_object()) {
                    for (auto it = item["metadata"].begin(); it != item["metadata"].end(); ++it) {
                        mapping.metadata[it.key()] = it.value().get<std::string>();
                    }
                }
                
                m_fileMappings[mapping.originalPath] = mapping;
            }
            
            return true;
        }
    } catch (const std::exception& e) {
        logError("Erro ao carregar mapeamentos de arquivo: " + std::string(e.what()));
    }
    
    return false;
}

void LiveUpdateManager::saveFileMappings()
{
    std::string mappingsPath = m_config.dataDir + "/file_mappings.json";
    
    try {
        json data;
        json mappings = json::array();
        
        std::lock_guard<std::mutex> lock(m_mappingsMutex);
        
        for (const auto& [_, mapping] : m_fileMappings) {
            json item;
            item["originalPath"] = mapping.originalPath;
            item["decompPath"] = mapping.decompPath;
            item["fileType"] = fileTypeToString(mapping.fileType);
            item["format"] = mapping.format;
            item["canEditCompiled"] = mapping.canEditCompiled;
            item["needsServerReload"] = mapping.needsServerReload;
            item["needsClientReload"] = mapping.needsClientReload;
            item["decompilerModule"] = mapping.decompilerModule;
            item["compilerModule"] = mapping.compilerModule;
            
            json metadata = json::object();
            for (const auto& [key, value] : mapping.metadata) {
                metadata[key] = value;
            }
            item["metadata"] = metadata;
            
            mappings.push_back(item);
        }
        
        data["mappings"] = mappings;
        
        m_facade->writeTextFile(mappingsPath, data.dump(4));
    } catch (const std::exception& e) {
        logError("Erro ao salvar mapeamentos de arquivo: " + std::string(e.what()));
    }
}

void LiveUpdateManager::addFileMapping(const FileMapping& mapping)
{
    std::lock_guard<std::mutex> lock(m_mappingsMutex);
    m_fileMappings[mapping.originalPath] = mapping;
}

void LiveUpdateManager::updateFileMapping(const FileMapping& mapping)
{
    std::lock_guard<std::mutex> lock(m_mappingsMutex);
    m_fileMappings[mapping.originalPath] = mapping;
}

void LiveUpdateManager::removeFileMapping(const std::string& filePath)
{
    std::lock_guard<std::mutex> lock(m_mappingsMutex);
    m_fileMappings.erase(filePath);
}

void LiveUpdateManager::addNotification(const UpdateNotification& notification)
{
    {
        std::lock_guard<std::mutex> lock(m_notificationsMutex);
        m_recentNotifications.push_back(notification);
        
        // Limita o número de notificações armazenadas
        const int32_t maxNotifications = 100;
        if (m_recentNotifications.size() > maxNotifications) {
            m_recentNotifications.erase(m_recentNotifications.begin());
        }
    }
    
    notifyCallbacks(notification);
}

void LiveUpdateManager::notifyCallbacks(const UpdateNotification& notification)
{
    std::lock_guard<std::mutex> lock(m_callbacksMutex);
    
    for (const auto& [_, callback] : m_notificationCallbacks) {
        callback(notification);
    }
}

std::string LiveUpdateManager::generateOperationId()
{
    return pImpl->generateOperationId();
}

bool LiveUpdateManager::createBackup(const std::string& filePath, std::string& backupPath)
{
    // Implementação simplificada
    return false;
}

bool LiveUpdateManager::detectFileType(const std::string& filePath, FileType& detectedType)
{
    // Implementação simplificada
    return false;
}

void LiveUpdateManager::fileWatcherLoop()
{
    // Delegado para a implementação interna
}

void LiveUpdateManager::processOperation(LiveOperationData& operation)
{
    // Delegado para a implementação interna
}

void LiveUpdateManager::operationProcessorLoop()
{
    // Delegado para a implementação interna
}

void LiveUpdateManager::serverConnectionLoop()
{
    // Delegado para a implementação interna
}

std::string LiveUpdateManager::fileTypeToString(FileType type)
{
    return pImpl->fileTypeToString(type);
}

FileType LiveUpdateManager::stringToFileType(const std::string& typeStr)
{
    return pImpl->stringToFileType(typeStr);
}

std::string LiveUpdateManager::operationTypeToString(LiveOperationType type)
{
    return pImpl->operationTypeToString(type);
}

LiveOperationType LiveUpdateManager::stringToOperationType(const std::string& typeStr)
{
    return pImpl->stringToOperationType(typeStr);
}

std::string LiveUpdateManager::operationStatusToString(LiveOperationStatus status)
{
    return pImpl->operationStatusToString(status);
}

LiveOperationStatus LiveUpdateManager::stringToOperationStatus(const std::string& statusStr)
{
    return pImpl->stringToOperationStatus(statusStr);
}

// Métodos auxiliares para logs
void LiveUpdateManager::logInfo(const std::string& message)
{
    if (m_facade) {
        m_facade->logInfo(message, "LiveUpdateManager");
    }
}

void LiveUpdateManager::logWarning(const std::string& message)
{
    if (m_facade) {
        m_facade->logWarning(message, "LiveUpdateManager");
    }
}

void LiveUpdateManager::logError(const std::string& message)
{
    if (m_facade) {
        m_facade->logError(message, "LiveUpdateManager");
    }
}

}}  // namespace wydstudio::core