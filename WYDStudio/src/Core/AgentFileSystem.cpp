#include "../../include/AgentFileSystem.h"
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;
namespace WYDStudio {

AgentFileSystem::AgentFileSystem() {
}

AgentFileSystem::~AgentFileSystem() = default;

bool AgentFileSystem::Initialize(const std::string& rootPath, 
                               const std::string& backupPath,
                               const std::string& permissionsPath) {
    try {
        Log("Inicializando AgentFileSystem...");
        
        // Criar os componentes do sistema
        auto fsConnector = std::make_shared<FileSystemConnector>();
        auto serverConnector = std::make_shared<ServerConnector>();
        
        // Inicializar o gerenciador de arquivos
        m_fileManager = std::make_shared<GameFileManager>(fsConnector, serverConnector);
        bool fileManagerInitialized = m_fileManager->Initialize(rootPath, backupPath);
        
        if (!fileManagerInitialized) {
            Log("Erro: Falha ao inicializar GameFileManager");
            return false;
        }
        
        // Configurar log callback
        m_fileManager->SetLogCallback([this](const std::string& message) {
            Log("FileManager: " + message);
        });
        
        // Criar o provedor de acesso a arquivos
        m_fileAccessProvider = std::make_shared<AgentFileAccessProvider>(m_fileManager);
        m_fileAccessProvider->SetLogCallback([this](const std::string& message) {
            Log("FileAccessProvider: " + message);
        });
        
        // Criar o gerenciador de permissões
        m_permissionManager = std::make_shared<AgentPermissionManager>(m_fileAccessProvider);
        m_permissionManager->SetLogCallback([this](const std::string& message) {
            Log("PermissionManager: " + message);
        });
        
        // Inicializar o gerenciador de permissões
        bool permManagerInitialized = m_permissionManager->Initialize(permissionsPath);
        
        if (!permManagerInitialized) {
            Log("Aviso: Falha ao inicializar AgentPermissionManager com arquivo " + permissionsPath);
            
            // Criar permissões padrão
            bool defaultsCreated = m_permissionManager->CreateDefaultPermissions();
            if (!defaultsCreated) {
                Log("Aviso: Falha ao criar permissões padrão");
            }
        }
        
        Log("AgentFileSystem inicializado com sucesso");
        return true;
    }
    catch (const std::exception& e) {
        Log("Erro ao inicializar AgentFileSystem: " + std::string(e.what()));
        return false;
    }
}

bool AgentFileSystem::RegisterAgent(std::shared_ptr<BaseAgent> agent, const std::string& name) {
    try {
        if (!agent) {
            Log("Erro: Agente nulo fornecido para registro");
            return false;
        }
        
        if (!m_fileAccessProvider) {
            Log("Erro: FileAccessProvider não inicializado");
            return false;
        }
        
        // Configurar o FileAccessProvider no agente
        agent->SetFileAccessProvider(m_fileAccessProvider);
        
        // Registrar o agente no provedor de acesso
        bool registered = m_fileAccessProvider->RegisterAgent(agent, agent->GetAgentType(), name);
        
        if (!registered) {
            Log("Erro: Falha ao registrar agente no FileAccessProvider");
            return false;
        }
        
        Log("Agente '" + name + "' registrado com sucesso");
        return true;
    }
    catch (const std::exception& e) {
        Log("Erro ao registrar agente: " + std::string(e.what()));
        return false;
    }
}

bool AgentFileSystem::SetupDefaultPermissions() {
    try {
        if (!m_permissionManager) {
            Log("Erro: PermissionManager não inicializado");
            return false;
        }
        
        bool created = m_permissionManager->CreateDefaultPermissions();
        
        if (!created) {
            Log("Erro: Falha ao criar permissões padrão");
            return false;
        }
        
        Log("Permissões padrão configuradas com sucesso");
        return true;
    }
    catch (const std::exception& e) {
        Log("Erro ao configurar permissões padrão: " + std::string(e.what()));
        return false;
    }
}

bool AgentFileSystem::AllowDirectory(AgentType agentType, 
                                   const std::string& directory,
                                   const std::vector<FileOperation>& operations) {
    try {
        if (!m_permissionManager) {
            Log("Erro: PermissionManager não inicializado");
            return false;
        }
        
        bool added = m_permissionManager->AddAllowedDirectory(agentType, directory, operations);
        
        if (!added) {
            Log("Erro: Falha ao adicionar diretório permitido");
            return false;
        }
        
        Log("Diretório permitido adicionado para agente tipo " + 
           std::to_string(static_cast<int>(agentType)) + ": " + directory);
        return true;
    }
    catch (const std::exception& e) {
        Log("Erro ao permitir diretório: " + std::string(e.what()));
        return false;
    }
}

bool AgentFileSystem::AllowFileType(AgentType agentType, 
                                  const std::string& fileType,
                                  const std::vector<FileOperation>& operations) {
    try {
        if (!m_permissionManager) {
            Log("Erro: PermissionManager não inicializado");
            return false;
        }
        
        bool added = m_permissionManager->AddAllowedFileType(agentType, fileType, operations);
        
        if (!added) {
            Log("Erro: Falha ao adicionar tipo de arquivo permitido");
            return false;
        }
        
        Log("Tipo de arquivo permitido adicionado para agente tipo " + 
           std::to_string(static_cast<int>(agentType)) + ": " + fileType);
        return true;
    }
    catch (const std::exception& e) {
        Log("Erro ao permitir tipo de arquivo: " + std::string(e.what()));
        return false;
    }
}

bool AgentFileSystem::AllowOperations(AgentType agentType, const std::vector<FileOperation>& operations) {
    try {
        if (!m_permissionManager) {
            Log("Erro: PermissionManager não inicializado");
            return false;
        }
        
        bool set = m_permissionManager->SetAllowedOperations(agentType, operations);
        
        if (!set) {
            Log("Erro: Falha ao definir operações permitidas");
            return false;
        }
        
        Log("Operações permitidas definidas para agente tipo " + 
           std::to_string(static_cast<int>(agentType)));
        return true;
    }
    catch (const std::exception& e) {
        Log("Erro ao permitir operações: " + std::string(e.what()));
        return false;
    }
}

std::shared_ptr<GameFileManager> AgentFileSystem::GetFileManager() const {
    return m_fileManager;
}

std::shared_ptr<AgentFileAccessProvider> AgentFileSystem::GetFileAccessProvider() const {
    return m_fileAccessProvider;
}

std::shared_ptr<AgentPermissionManager> AgentFileSystem::GetPermissionManager() const {
    return m_permissionManager;
}

void AgentFileSystem::SetLogCallback(std::function<void(const std::string&)> logCallback) {
    m_logCallback = logCallback;
    
    // Propagar o callback para os componentes
    if (m_fileManager) {
        m_fileManager->SetLogCallback([this](const std::string& message) {
            Log("FileManager: " + message);
        });
    }
    
    if (m_fileAccessProvider) {
        m_fileAccessProvider->SetLogCallback([this](const std::string& message) {
            Log("FileAccessProvider: " + message);
        });
    }
    
    if (m_permissionManager) {
        m_permissionManager->SetLogCallback([this](const std::string& message) {
            Log("PermissionManager: " + message);
        });
    }
}

bool AgentFileSystem::ApplyPermissionsAndFinalize() {
    try {
        if (!m_permissionManager) {
            Log("Erro: PermissionManager não inicializado");
            return false;
        }
        
        bool applied = m_permissionManager->ApplyPermissions();
        
        if (!applied) {
            Log("Erro: Falha ao aplicar permissões");
            return false;
        }
        
        Log("Permissões aplicadas e sistema finalizado com sucesso");
        return true;
    }
    catch (const std::exception& e) {
        Log("Erro ao aplicar permissões e finalizar: " + std::string(e.what()));
        return false;
    }
}

bool AgentFileSystem::SavePermissions(const std::string& filePath) {
    try {
        if (!m_permissionManager) {
            Log("Erro: PermissionManager não inicializado");
            return false;
        }
        
        bool saved = m_permissionManager->SavePermissionsToFile(filePath);
        
        if (!saved) {
            Log("Erro: Falha ao salvar permissões no arquivo " + filePath);
            return false;
        }
        
        Log("Permissões salvas com sucesso em: " + filePath);
        return true;
    }
    catch (const std::exception& e) {
        Log("Erro ao salvar permissões: " + std::string(e.what()));
        return false;
    }
}

void AgentFileSystem::Log(const std::string& message) {
    if (m_logCallback) {
        m_logCallback(message);
    } else {
        std::cout << "[AgentFileSystem] " << message << std::endl;
    }
}

} // namespace WYDStudio