#include "../../include/AgentPermissionManager.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <filesystem>
#include <json/json.hpp> // Usando nlohmann/json para C++

using json = nlohmann::json;
namespace fs = std::filesystem;

namespace WYDStudio {

AgentPermissionManager::AgentPermissionManager(std::shared_ptr<AgentFileAccessProvider> fileAccessProvider)
    : m_fileAccessProvider(fileAccessProvider) {
}

AgentPermissionManager::~AgentPermissionManager() = default;

bool AgentPermissionManager::Initialize(const std::string& configFilePath) {
    Log("Inicializando Gerenciador de Permissões de Agentes...");
    
    // Verificar se o provedor de acesso a arquivos está disponível
    if (!m_fileAccessProvider) {
        Log("Erro: Provedor de acesso a arquivos não disponível");
        return false;
    }
    
    // Criar configurações de permissão padrão
    bool defaultCreated = CreateDefaultPermissions();
    if (!defaultCreated) {
        Log("Aviso: Não foi possível criar configurações de permissão padrão");
    }
    
    // Se um arquivo de configuração foi fornecido, carregar dele
    if (!configFilePath.empty()) {
        if (fs::exists(configFilePath)) {
            bool loaded = LoadPermissionsFromFile(configFilePath);
            if (!loaded) {
                Log("Erro: Falha ao carregar configurações de permissão do arquivo: " + configFilePath);
                return false;
            }
        } else {
            Log("Aviso: Arquivo de configuração não encontrado: " + configFilePath);
            
            // Salvar configurações padrão no arquivo
            bool saved = SavePermissionsToFile(configFilePath);
            if (!saved) {
                Log("Aviso: Não foi possível salvar configurações padrão em: " + configFilePath);
            }
        }
    }
    
    // Aplicar as configurações ao provedor de acesso
    bool applied = ApplyPermissions();
    if (!applied) {
        Log("Erro: Falha ao aplicar configurações de permissão");
        return false;
    }
    
    Log("Gerenciador de Permissões de Agentes inicializado com sucesso");
    return true;
}

bool AgentPermissionManager::ConfigurePermissions(AgentType agentType, const AgentPermissionConfig& config) {
    try {
        // Armazenar a configuração
        m_permissions[agentType] = config;
        
        Log("Configurações de permissão definidas para agente tipo " + std::to_string(static_cast<int>(agentType)));
        return true;
    }
    catch (const std::exception& e) {
        Log("Erro ao configurar permissões: " + std::string(e.what()));
        return false;
    }
}

bool AgentPermissionManager::ApplyPermissions() {
    try {
        if (!m_fileAccessProvider) {
            Log("Erro: Provedor de acesso a arquivos não disponível");
            return false;
        }
        
        Log("Aplicando configurações de permissão...");
        
        // Para cada tipo de agente com configurações
        for (const auto& permPair : m_permissions) {
            AgentType agentType = permPair.first;
            const AgentPermissionConfig& config = permPair.second;
            
            // Configurar permissões por tipo de arquivo
            if (!config.allowedFileTypes.empty()) {
                bool success = m_fileAccessProvider->SetFileTypePermissions(
                    agentType, 
                    config.allowedFileTypes, 
                    config.allowedOperations
                );
                
                if (!success) {
                    Log("Aviso: Falha ao definir permissões por tipo de arquivo para agente tipo " + 
                       std::to_string(static_cast<int>(agentType)));
                }
            }
            
            // Configurar permissões por diretório
            if (!config.allowedDirectories.empty()) {
                bool success = m_fileAccessProvider->SetDirectoryPermissions(
                    agentType, 
                    config.allowedDirectories, 
                    config.allowedOperations, 
                    config.includeSubdirectories
                );
                
                if (!success) {
                    Log("Aviso: Falha ao definir permissões por diretório para agente tipo " + 
                       std::to_string(static_cast<int>(agentType)));
                }
            }
            
            // Configurar permissões específicas por tipo de arquivo
            for (const auto& fileTypePerm : config.fileTypeSpecificPermissions) {
                const std::string& fileType = fileTypePerm.first;
                const std::vector<FileOperation>& operations = fileTypePerm.second;
                
                bool success = m_fileAccessProvider->SetFileTypePermissions(
                    agentType, 
                    {fileType}, 
                    operations
                );
                
                if (!success) {
                    Log("Aviso: Falha ao definir permissões específicas para tipo de arquivo " + 
                       fileType + " para agente tipo " + std::to_string(static_cast<int>(agentType)));
                }
            }
            
            // Configurar permissões específicas por diretório
            for (const auto& dirPerm : config.directorySpecificPermissions) {
                const std::string& directory = dirPerm.first;
                const std::vector<FileOperation>& operations = dirPerm.second;
                
                bool success = m_fileAccessProvider->SetDirectoryPermissions(
                    agentType, 
                    {directory}, 
                    operations, 
                    config.includeSubdirectories
                );
                
                if (!success) {
                    Log("Aviso: Falha ao definir permissões específicas para diretório " + 
                       directory + " para agente tipo " + std::to_string(static_cast<int>(agentType)));
                }
            }
        }
        
        Log("Configurações de permissão aplicadas com sucesso");
        return true;
    }
    catch (const std::exception& e) {
        Log("Erro ao aplicar permissões: " + std::string(e.what()));
        return false;
    }
}

bool AgentPermissionManager::LoadPermissionsFromFile(const std::string& filePath) {
    try {
        Log("Carregando configurações de permissão de: " + filePath);
        
        // Verificar se o arquivo existe
        if (!fs::exists(filePath)) {
            Log("Erro: Arquivo não encontrado: " + filePath);
            return false;
        }
        
        // Abrir o arquivo
        std::ifstream file(filePath);
        if (!file.is_open()) {
            Log("Erro: Não foi possível abrir o arquivo: " + filePath);
            return false;
        }
        
        // Ler o JSON
        json configJson;
        file >> configJson;
        file.close();
        
        // Limpar configurações existentes
        m_permissions.clear();
        
        // Converter FileOperation de string para enum
        auto stringToFileOperation = [](const std::string& opStr) -> FileOperation {
            if (opStr == "CREATE") return FileOperation::CREATE;
            if (opStr == "MODIFY") return FileOperation::MODIFY;
            if (opStr == "DELETE") return FileOperation::DELETE;
            if (opStr == "MOVE") return FileOperation::MOVE;
            if (opStr == "COPY") return FileOperation::COPY;
            if (opStr == "COMPILE") return FileOperation::COMPILE;
            if (opStr == "DECOMPILE") return FileOperation::DECOMPILE;
            
            return FileOperation::MODIFY; // Default
        };
        
        // Processar cada tipo de agente
        for (const auto& agentConfig : configJson.items()) {
            int agentTypeInt = std::stoi(agentConfig.key());
            AgentType agentType = static_cast<AgentType>(agentTypeInt);
            
            AgentPermissionConfig config;
            
            // Ler diretórios permitidos
            if (agentConfig.value().contains("allowedDirectories") && 
                agentConfig.value()["allowedDirectories"].is_array()) {
                for (const auto& dir : agentConfig.value()["allowedDirectories"]) {
                    config.allowedDirectories.push_back(dir.get<std::string>());
                }
            }
            
            // Ler diretórios negados
            if (agentConfig.value().contains("deniedDirectories") && 
                agentConfig.value()["deniedDirectories"].is_array()) {
                for (const auto& dir : agentConfig.value()["deniedDirectories"]) {
                    config.deniedDirectories.push_back(dir.get<std::string>());
                }
            }
            
            // Ler tipos de arquivo permitidos
            if (agentConfig.value().contains("allowedFileTypes") && 
                agentConfig.value()["allowedFileTypes"].is_array()) {
                for (const auto& fileType : agentConfig.value()["allowedFileTypes"]) {
                    config.allowedFileTypes.push_back(fileType.get<std::string>());
                }
            }
            
            // Ler tipos de arquivo negados
            if (agentConfig.value().contains("deniedFileTypes") && 
                agentConfig.value()["deniedFileTypes"].is_array()) {
                for (const auto& fileType : agentConfig.value()["deniedFileTypes"]) {
                    config.deniedFileTypes.push_back(fileType.get<std::string>());
                }
            }
            
            // Ler operações permitidas
            if (agentConfig.value().contains("allowedOperations") && 
                agentConfig.value()["allowedOperations"].is_array()) {
                for (const auto& op : agentConfig.value()["allowedOperations"]) {
                    config.allowedOperations.push_back(stringToFileOperation(op.get<std::string>()));
                }
            }
            
            // Ler permissões específicas por tipo de arquivo
            if (agentConfig.value().contains("fileTypeSpecificPermissions") && 
                agentConfig.value()["fileTypeSpecificPermissions"].is_object()) {
                for (const auto& filePerm : agentConfig.value()["fileTypeSpecificPermissions"].items()) {
                    std::string fileType = filePerm.key();
                    std::vector<FileOperation> operations;
                    
                    if (filePerm.value().is_array()) {
                        for (const auto& op : filePerm.value()) {
                            operations.push_back(stringToFileOperation(op.get<std::string>()));
                        }
                    }
                    
                    config.fileTypeSpecificPermissions[fileType] = operations;
                }
            }
            
            // Ler permissões específicas por diretório
            if (agentConfig.value().contains("directorySpecificPermissions") && 
                agentConfig.value()["directorySpecificPermissions"].is_object()) {
                for (const auto& dirPerm : agentConfig.value()["directorySpecificPermissions"].items()) {
                    std::string directory = dirPerm.key();
                    std::vector<FileOperation> operations;
                    
                    if (dirPerm.value().is_array()) {
                        for (const auto& op : dirPerm.value()) {
                            operations.push_back(stringToFileOperation(op.get<std::string>()));
                        }
                    }
                    
                    config.directorySpecificPermissions[directory] = operations;
                }
            }
            
            // Ler configurações gerais
            if (agentConfig.value().contains("includeSubdirectories")) {
                config.includeSubdirectories = agentConfig.value()["includeSubdirectories"].get<bool>();
            } else {
                config.includeSubdirectories = true; // Default
            }
            
            if (agentConfig.value().contains("defaultAllow")) {
                config.defaultAllow = agentConfig.value()["defaultAllow"].get<bool>();
            } else {
                config.defaultAllow = false; // Default
            }
            
            // Armazenar a configuração para este tipo de agente
            m_permissions[agentType] = config;
        }
        
        Log("Configurações de permissão carregadas com sucesso");
        return true;
    }
    catch (const std::exception& e) {
        Log("Erro ao carregar configurações de permissão: " + std::string(e.what()));
        return false;
    }
}

bool AgentPermissionManager::SavePermissionsToFile(const std::string& filePath) {
    try {
        Log("Salvando configurações de permissão em: " + filePath);
        
        // Criar diretório pai se necessário
        fs::path path(filePath);
        fs::path parentDir = path.parent_path();
        if (!parentDir.empty() && !fs::exists(parentDir)) {
            fs::create_directories(parentDir);
        }
        
        // Converter FileOperation de enum para string
        auto fileOperationToString = [](FileOperation op) -> std::string {
            switch (op) {
                case FileOperation::CREATE: return "CREATE";
                case FileOperation::MODIFY: return "MODIFY";
                case FileOperation::DELETE: return "DELETE";
                case FileOperation::MOVE: return "MOVE";
                case FileOperation::COPY: return "COPY";
                case FileOperation::COMPILE: return "COMPILE";
                case FileOperation::DECOMPILE: return "DECOMPILE";
                default: return "UNKNOWN";
            }
        };
        
        // Criar objeto JSON
        json configJson;
        
        // Processar cada tipo de agente
        for (const auto& permPair : m_permissions) {
            AgentType agentType = permPair.first;
            const AgentPermissionConfig& config = permPair.second;
            
            // Criar objeto para este tipo de agente
            json agentConfig;
            
            // Adicionar diretórios permitidos
            agentConfig["allowedDirectories"] = config.allowedDirectories;
            
            // Adicionar diretórios negados
            agentConfig["deniedDirectories"] = config.deniedDirectories;
            
            // Adicionar tipos de arquivo permitidos
            agentConfig["allowedFileTypes"] = config.allowedFileTypes;
            
            // Adicionar tipos de arquivo negados
            agentConfig["deniedFileTypes"] = config.deniedFileTypes;
            
            // Adicionar operações permitidas
            json allowedOps = json::array();
            for (const auto& op : config.allowedOperations) {
                allowedOps.push_back(fileOperationToString(op));
            }
            agentConfig["allowedOperations"] = allowedOps;
            
            // Adicionar permissões específicas por tipo de arquivo
            json fileTypePerms = json::object();
            for (const auto& filePerm : config.fileTypeSpecificPermissions) {
                const std::string& fileType = filePerm.first;
                const std::vector<FileOperation>& operations = filePerm.second;
                
                json fileOps = json::array();
                for (const auto& op : operations) {
                    fileOps.push_back(fileOperationToString(op));
                }
                
                fileTypePerms[fileType] = fileOps;
            }
            agentConfig["fileTypeSpecificPermissions"] = fileTypePerms;
            
            // Adicionar permissões específicas por diretório
            json dirPerms = json::object();
            for (const auto& dirPerm : config.directorySpecificPermissions) {
                const std::string& directory = dirPerm.first;
                const std::vector<FileOperation>& operations = dirPerm.second;
                
                json dirOps = json::array();
                for (const auto& op : operations) {
                    dirOps.push_back(fileOperationToString(op));
                }
                
                dirPerms[directory] = dirOps;
            }
            agentConfig["directorySpecificPermissions"] = dirPerms;
            
            // Adicionar configurações gerais
            agentConfig["includeSubdirectories"] = config.includeSubdirectories;
            agentConfig["defaultAllow"] = config.defaultAllow;
            
            // Adicionar ao objeto principal
            configJson[std::to_string(static_cast<int>(agentType))] = agentConfig;
        }
        
        // Salvar no arquivo
        std::ofstream file(filePath);
        if (!file.is_open()) {
            Log("Erro: Não foi possível abrir o arquivo para escrita: " + filePath);
            return false;
        }
        
        file << configJson.dump(4); // Pretty print com 4 espaços
        file.close();
        
        Log("Configurações de permissão salvas com sucesso");
        return true;
    }
    catch (const std::exception& e) {
        Log("Erro ao salvar configurações de permissão: " + std::string(e.what()));
        return false;
    }
}

bool AgentPermissionManager::AddAllowedDirectory(AgentType agentType, 
                                               const std::string& directory,
                                               const std::vector<FileOperation>& operations) {
    try {
        // Verificar se o tipo de agente tem configuração
        if (m_permissions.find(agentType) == m_permissions.end()) {
            // Criar configuração padrão
            AgentPermissionConfig config;
            config.includeSubdirectories = true;
            config.defaultAllow = false;
            
            m_permissions[agentType] = config;
        }
        
        // Adicionar o diretório à lista de permitidos
        if (std::find(m_permissions[agentType].allowedDirectories.begin(),
                    m_permissions[agentType].allowedDirectories.end(),
                    directory) == m_permissions[agentType].allowedDirectories.end()) {
            m_permissions[agentType].allowedDirectories.push_back(directory);
        }
        
        // Se operações específicas foram fornecidas, adicionar à configuração específica
        if (!operations.empty()) {
            m_permissions[agentType].directorySpecificPermissions[directory] = operations;
        }
        // Caso contrário, remover qualquer configuração específica existente
        else if (m_permissions[agentType].directorySpecificPermissions.find(directory) != 
                m_permissions[agentType].directorySpecificPermissions.end()) {
            m_permissions[agentType].directorySpecificPermissions.erase(directory);
        }
        
        Log("Diretório permitido adicionado para agente tipo " + 
           std::to_string(static_cast<int>(agentType)) + ": " + directory);
        
        return true;
    }
    catch (const std::exception& e) {
        Log("Erro ao adicionar diretório permitido: " + std::string(e.what()));
        return false;
    }
}

bool AgentPermissionManager::AddDeniedDirectory(AgentType agentType, const std::string& directory) {
    try {
        // Verificar se o tipo de agente tem configuração
        if (m_permissions.find(agentType) == m_permissions.end()) {
            // Criar configuração padrão
            AgentPermissionConfig config;
            config.includeSubdirectories = true;
            config.defaultAllow = false;
            
            m_permissions[agentType] = config;
        }
        
        // Adicionar o diretório à lista de negados
        if (std::find(m_permissions[agentType].deniedDirectories.begin(),
                    m_permissions[agentType].deniedDirectories.end(),
                    directory) == m_permissions[agentType].deniedDirectories.end()) {
            m_permissions[agentType].deniedDirectories.push_back(directory);
        }
        
        // Remover qualquer configuração específica existente
        if (m_permissions[agentType].directorySpecificPermissions.find(directory) != 
            m_permissions[agentType].directorySpecificPermissions.end()) {
            m_permissions[agentType].directorySpecificPermissions.erase(directory);
        }
        
        Log("Diretório negado adicionado para agente tipo " + 
           std::to_string(static_cast<int>(agentType)) + ": " + directory);
        
        return true;
    }
    catch (const std::exception& e) {
        Log("Erro ao adicionar diretório negado: " + std::string(e.what()));
        return false;
    }
}

bool AgentPermissionManager::AddAllowedFileType(AgentType agentType, 
                                              const std::string& fileType,
                                              const std::vector<FileOperation>& operations) {
    try {
        // Verificar se o tipo de agente tem configuração
        if (m_permissions.find(agentType) == m_permissions.end()) {
            // Criar configuração padrão
            AgentPermissionConfig config;
            config.includeSubdirectories = true;
            config.defaultAllow = false;
            
            m_permissions[agentType] = config;
        }
        
        // Padronizar o formato da extensão do arquivo
        std::string normalizedFileType = fileType;
        if (!normalizedFileType.empty() && normalizedFileType[0] != '.') {
            normalizedFileType = "." + normalizedFileType;
        }
        
        // Adicionar o tipo de arquivo à lista de permitidos
        if (std::find(m_permissions[agentType].allowedFileTypes.begin(),
                    m_permissions[agentType].allowedFileTypes.end(),
                    normalizedFileType) == m_permissions[agentType].allowedFileTypes.end()) {
            m_permissions[agentType].allowedFileTypes.push_back(normalizedFileType);
        }
        
        // Se operações específicas foram fornecidas, adicionar à configuração específica
        if (!operations.empty()) {
            m_permissions[agentType].fileTypeSpecificPermissions[normalizedFileType] = operations;
        }
        // Caso contrário, remover qualquer configuração específica existente
        else if (m_permissions[agentType].fileTypeSpecificPermissions.find(normalizedFileType) != 
                m_permissions[agentType].fileTypeSpecificPermissions.end()) {
            m_permissions[agentType].fileTypeSpecificPermissions.erase(normalizedFileType);
        }
        
        Log("Tipo de arquivo permitido adicionado para agente tipo " + 
           std::to_string(static_cast<int>(agentType)) + ": " + normalizedFileType);
        
        return true;
    }
    catch (const std::exception& e) {
        Log("Erro ao adicionar tipo de arquivo permitido: " + std::string(e.what()));
        return false;
    }
}

bool AgentPermissionManager::AddDeniedFileType(AgentType agentType, const std::string& fileType) {
    try {
        // Verificar se o tipo de agente tem configuração
        if (m_permissions.find(agentType) == m_permissions.end()) {
            // Criar configuração padrão
            AgentPermissionConfig config;
            config.includeSubdirectories = true;
            config.defaultAllow = false;
            
            m_permissions[agentType] = config;
        }
        
        // Padronizar o formato da extensão do arquivo
        std::string normalizedFileType = fileType;
        if (!normalizedFileType.empty() && normalizedFileType[0] != '.') {
            normalizedFileType = "." + normalizedFileType;
        }
        
        // Adicionar o tipo de arquivo à lista de negados
        if (std::find(m_permissions[agentType].deniedFileTypes.begin(),
                    m_permissions[agentType].deniedFileTypes.end(),
                    normalizedFileType) == m_permissions[agentType].deniedFileTypes.end()) {
            m_permissions[agentType].deniedFileTypes.push_back(normalizedFileType);
        }
        
        // Remover qualquer configuração específica existente
        if (m_permissions[agentType].fileTypeSpecificPermissions.find(normalizedFileType) != 
            m_permissions[agentType].fileTypeSpecificPermissions.end()) {
            m_permissions[agentType].fileTypeSpecificPermissions.erase(normalizedFileType);
        }
        
        Log("Tipo de arquivo negado adicionado para agente tipo " + 
           std::to_string(static_cast<int>(agentType)) + ": " + normalizedFileType);
        
        return true;
    }
    catch (const std::exception& e) {
        Log("Erro ao adicionar tipo de arquivo negado: " + std::string(e.what()));
        return false;
    }
}

bool AgentPermissionManager::SetAllowedOperations(AgentType agentType, const std::vector<FileOperation>& operations) {
    try {
        // Verificar se o tipo de agente tem configuração
        if (m_permissions.find(agentType) == m_permissions.end()) {
            // Criar configuração padrão
            AgentPermissionConfig config;
            config.includeSubdirectories = true;
            config.defaultAllow = false;
            
            m_permissions[agentType] = config;
        }
        
        // Definir as operações permitidas
        m_permissions[agentType].allowedOperations = operations;
        
        Log("Operações permitidas definidas para agente tipo " + 
           std::to_string(static_cast<int>(agentType)));
        
        return true;
    }
    catch (const std::exception& e) {
        Log("Erro ao definir operações permitidas: " + std::string(e.what()));
        return false;
    }
}

bool AgentPermissionManager::SetIncludeSubdirectories(AgentType agentType, bool include) {
    try {
        // Verificar se o tipo de agente tem configuração
        if (m_permissions.find(agentType) == m_permissions.end()) {
            // Criar configuração padrão
            AgentPermissionConfig config;
            config.includeSubdirectories = true;
            config.defaultAllow = false;
            
            m_permissions[agentType] = config;
        }
        
        // Definir a configuração
        m_permissions[agentType].includeSubdirectories = include;
        
        Log("Inclusão de subdiretórios " + std::string(include ? "ativada" : "desativada") + 
           " para agente tipo " + std::to_string(static_cast<int>(agentType)));
        
        return true;
    }
    catch (const std::exception& e) {
        Log("Erro ao definir inclusão de subdiretórios: " + std::string(e.what()));
        return false;
    }
}

bool AgentPermissionManager::SetDefaultAllow(AgentType agentType, bool allow) {
    try {
        // Verificar se o tipo de agente tem configuração
        if (m_permissions.find(agentType) == m_permissions.end()) {
            // Criar configuração padrão
            AgentPermissionConfig config;
            config.includeSubdirectories = true;
            config.defaultAllow = false;
            
            m_permissions[agentType] = config;
        }
        
        // Definir a configuração
        m_permissions[agentType].defaultAllow = allow;
        
        Log("Comportamento padrão definido como " + std::string(allow ? "permitir" : "negar") + 
           " para agente tipo " + std::to_string(static_cast<int>(agentType)));
        
        return true;
    }
    catch (const std::exception& e) {
        Log("Erro ao definir comportamento padrão: " + std::string(e.what()));
        return false;
    }
}

void AgentPermissionManager::SetLogCallback(std::function<void(const std::string&)> logCallback) {
    m_logCallback = logCallback;
}

void AgentPermissionManager::Log(const std::string& message) {
    if (m_logCallback) {
        m_logCallback(message);
    } else {
        std::cout << "[AgentPermissionManager] " << message << std::endl;
    }
}

std::vector<FileOperation> AgentPermissionManager::GetAllOperations() {
    return {
        FileOperation::CREATE,
        FileOperation::MODIFY,
        FileOperation::DELETE,
        FileOperation::MOVE,
        FileOperation::COPY,
        FileOperation::COMPILE,
        FileOperation::DECOMPILE
    };
}

bool AgentPermissionManager::CreateDefaultPermissions() {
    try {
        Log("Criando configurações de permissão padrão...");
        
        // Lista de todas as operações
        std::vector<FileOperation> allOperations = GetAllOperations();
        
        // 1. Configuração para Administrador (acesso total)
        AgentPermissionConfig adminConfig;
        adminConfig.allowedDirectories = {"./"}; // Diretório raiz
        adminConfig.allowedFileTypes = {".json", ".bin", ".dat", ".txt", ".csv", ".md", ".cpp", ".h"};
        adminConfig.allowedOperations = allOperations;
        adminConfig.includeSubdirectories = true;
        adminConfig.defaultAllow = true;
        
        m_permissions[AgentType::ADMIN] = adminConfig;
        
        // 2. Configuração para Game Master (acesso controlado)
        AgentPermissionConfig gmConfig;
        gmConfig.allowedDirectories = {
            "./WYD", 
            "./WYD MODERN 2025 STUDIO/Server", 
            "./logs"
        };
        gmConfig.allowedFileTypes = {".json", ".bin", ".dat", ".txt", ".csv"};
        gmConfig.allowedOperations = {
            FileOperation::MODIFY,
            FileOperation::COMPILE,
            FileOperation::DECOMPILE,
            FileOperation::COPY
        };
        gmConfig.includeSubdirectories = true;
        gmConfig.defaultAllow = false;
        
        // Permissões específicas para tipos de arquivo
        gmConfig.fileTypeSpecificPermissions[".txt"] = {
            FileOperation::CREATE,
            FileOperation::MODIFY,
            FileOperation::DELETE
        };
        
        // Permissões específicas para diretórios
        gmConfig.directorySpecificPermissions["./logs"] = {
            FileOperation::CREATE,
            FileOperation::MODIFY,
            FileOperation::DELETE
        };
        
        m_permissions[AgentType::GM] = gmConfig;
        
        // 3. Configuração para Desenvolvedor (acesso amplo ao código)
        AgentPermissionConfig devConfig;
        devConfig.allowedDirectories = {
            "./src", 
            "./WYD MODERN 2025 STUDIO/Client", 
            "./WYD MODERN 2025 STUDIO/Server",
            "./WYD MODERN 2025 STUDIO/WYDStudio",
            "./WYD_DESCOMPILADO"
        };
        devConfig.allowedFileTypes = {
            ".json", ".bin", ".dat", ".txt", ".csv", ".md", 
            ".cpp", ".h", ".js", ".ts", ".py", ".c"
        };
        devConfig.allowedOperations = allOperations;
        devConfig.includeSubdirectories = true;
        devConfig.defaultAllow = false;
        
        // Diretórios negados para o desenvolvedor
        devConfig.deniedDirectories = {"./logs/security"};
        
        m_permissions[AgentType::DEV] = devConfig;
        
        // 4. Configuração para Jogador (acesso muito limitado)
        AgentPermissionConfig playerConfig;
        playerConfig.allowedDirectories = {"./saved_games", "./screenshots"};
        playerConfig.allowedFileTypes = {".txt", ".jpg", ".png", ".log"};
        playerConfig.allowedOperations = {FileOperation::CREATE, FileOperation::MODIFY};
        playerConfig.includeSubdirectories = false;
        playerConfig.defaultAllow = false;
        
        m_permissions[AgentType::PLAYER] = playerConfig;
        
        // 5. Configuração para Suporte (acesso a logs e dados específicos)
        AgentPermissionConfig supportConfig;
        supportConfig.allowedDirectories = {"./logs", "./WYD MODERN 2025 STUDIO/Client/Config"};
        supportConfig.allowedFileTypes = {".log", ".txt", ".json", ".csv"};
        supportConfig.allowedOperations = {
            FileOperation::MODIFY,
            FileOperation::COPY
        };
        supportConfig.includeSubdirectories = true;
        supportConfig.defaultAllow = false;
        
        m_permissions[AgentType::SUPPORT] = supportConfig;
        
        Log("Configurações de permissão padrão criadas com sucesso");
        return true;
    }
    catch (const std::exception& e) {
        Log("Erro ao criar configurações de permissão padrão: " + std::string(e.what()));
        return false;
    }
}

} // namespace WYDStudio