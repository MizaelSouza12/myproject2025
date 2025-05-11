#include "../../include/BaseAgent.h"
#include "../../include/AgentFileAccessProvider.h"
#include "../../include/GameFileManager.h"
#include <iostream>
#include <algorithm>

namespace WYDStudio {

BaseAgent::BaseAgent()
    : m_logCallback(nullptr)
    , m_fileAccessProvider(nullptr) {
}

BaseAgent::~BaseAgent() = default;

bool BaseAgent::HasCapability(AgentCapability capability) const {
    return std::find(m_capabilities.begin(), m_capabilities.end(), capability) != m_capabilities.end();
}

void BaseAgent::SetLogCallback(std::function<void(const std::string&)> logCallback) {
    m_logCallback = logCallback;
}

void BaseAgent::SetFileAccessProvider(std::shared_ptr<AgentFileAccessProvider> provider) {
    m_fileAccessProvider = provider;
}

void BaseAgent::Log(const std::string& message) {
    if (m_logCallback) {
        m_logCallback(message);
    } else {
        std::cout << "[BaseAgent] " << message << std::endl;
    }
}

// ----- Implementações dos métodos de manipulação de arquivos -----

FileOperationResult BaseAgent::CreateFile(const std::string& path, 
                                         const std::string& content,
                                         const std::string& description) {
    // Verificar se o agente tem capacidade de acesso a arquivos
    if (!HasCapability(AgentCapability::FILE_ACCESS)) {
        Log("Erro: Agente não tem capacidade de acessar arquivos.");
        
        FileOperationResult result;
        result.success = false;
        result.message = "Agente não tem capacidade de acesso a arquivos";
        result.timestamp = 0;
        result.originalPath = path;
        
        return result;
    }
    
    // Verificar se o provedor de acesso está disponível
    if (!m_fileAccessProvider) {
        Log("Erro: Provedor de acesso a arquivos não disponível.");
        
        FileOperationResult result;
        result.success = false;
        result.message = "Provedor de acesso a arquivos não disponível";
        result.timestamp = 0;
        result.originalPath = path;
        
        return result;
    }
    
    // Delegar a operação para o provedor
    return m_fileAccessProvider->AgentCreateFile(this, path, content, description);
}

FileOperationResult BaseAgent::ModifyFile(const std::string& path, 
                                         const std::string& newContent,
                                         const std::string& description) {
    // Verificar se o agente tem capacidade de acesso a arquivos
    if (!HasCapability(AgentCapability::FILE_ACCESS)) {
        Log("Erro: Agente não tem capacidade de acessar arquivos.");
        
        FileOperationResult result;
        result.success = false;
        result.message = "Agente não tem capacidade de acesso a arquivos";
        result.timestamp = 0;
        result.originalPath = path;
        
        return result;
    }
    
    // Verificar se o provedor de acesso está disponível
    if (!m_fileAccessProvider) {
        Log("Erro: Provedor de acesso a arquivos não disponível.");
        
        FileOperationResult result;
        result.success = false;
        result.message = "Provedor de acesso a arquivos não disponível";
        result.timestamp = 0;
        result.originalPath = path;
        
        return result;
    }
    
    // Delegar a operação para o provedor
    return m_fileAccessProvider->AgentModifyFile(this, path, newContent, description);
}

FileOperationResult BaseAgent::ModifyFilePart(const std::string& path, 
                                             const std::string& oldContent,
                                             const std::string& newContent,
                                             const std::string& description) {
    // Verificar se o agente tem capacidade de acesso a arquivos
    if (!HasCapability(AgentCapability::FILE_ACCESS)) {
        Log("Erro: Agente não tem capacidade de acessar arquivos.");
        
        FileOperationResult result;
        result.success = false;
        result.message = "Agente não tem capacidade de acesso a arquivos";
        result.timestamp = 0;
        result.originalPath = path;
        
        return result;
    }
    
    // Verificar se o provedor de acesso está disponível
    if (!m_fileAccessProvider) {
        Log("Erro: Provedor de acesso a arquivos não disponível.");
        
        FileOperationResult result;
        result.success = false;
        result.message = "Provedor de acesso a arquivos não disponível";
        result.timestamp = 0;
        result.originalPath = path;
        
        return result;
    }
    
    // Delegar a operação para o provedor
    return m_fileAccessProvider->AgentModifyFilePart(this, path, oldContent, newContent, description);
}

FileOperationResult BaseAgent::DeleteFile(const std::string& path,
                                         const std::string& description) {
    // Verificar se o agente tem capacidade de acesso a arquivos
    if (!HasCapability(AgentCapability::FILE_ACCESS)) {
        Log("Erro: Agente não tem capacidade de acessar arquivos.");
        
        FileOperationResult result;
        result.success = false;
        result.message = "Agente não tem capacidade de acesso a arquivos";
        result.timestamp = 0;
        result.originalPath = path;
        
        return result;
    }
    
    // Verificar se o provedor de acesso está disponível
    if (!m_fileAccessProvider) {
        Log("Erro: Provedor de acesso a arquivos não disponível.");
        
        FileOperationResult result;
        result.success = false;
        result.message = "Provedor de acesso a arquivos não disponível";
        result.timestamp = 0;
        result.originalPath = path;
        
        return result;
    }
    
    // Delegar a operação para o provedor
    return m_fileAccessProvider->AgentDeleteFile(this, path, description);
}

FileOperationResult BaseAgent::MoveFile(const std::string& sourcePath,
                                       const std::string& destinationPath,
                                       const std::string& description) {
    // Verificar se o agente tem capacidade de acesso a arquivos
    if (!HasCapability(AgentCapability::FILE_ACCESS)) {
        Log("Erro: Agente não tem capacidade de acessar arquivos.");
        
        FileOperationResult result;
        result.success = false;
        result.message = "Agente não tem capacidade de acesso a arquivos";
        result.timestamp = 0;
        result.originalPath = sourcePath;
        result.newPath = destinationPath;
        
        return result;
    }
    
    // Verificar se o provedor de acesso está disponível
    if (!m_fileAccessProvider) {
        Log("Erro: Provedor de acesso a arquivos não disponível.");
        
        FileOperationResult result;
        result.success = false;
        result.message = "Provedor de acesso a arquivos não disponível";
        result.timestamp = 0;
        result.originalPath = sourcePath;
        result.newPath = destinationPath;
        
        return result;
    }
    
    // Delegar a operação para o provedor
    return m_fileAccessProvider->AgentMoveFile(this, sourcePath, destinationPath, description);
}

FileOperationResult BaseAgent::CopyFile(const std::string& sourcePath,
                                       const std::string& destinationPath,
                                       const std::string& description) {
    // Verificar se o agente tem capacidade de acesso a arquivos
    if (!HasCapability(AgentCapability::FILE_ACCESS)) {
        Log("Erro: Agente não tem capacidade de acessar arquivos.");
        
        FileOperationResult result;
        result.success = false;
        result.message = "Agente não tem capacidade de acesso a arquivos";
        result.timestamp = 0;
        result.originalPath = sourcePath;
        result.newPath = destinationPath;
        
        return result;
    }
    
    // Verificar se o provedor de acesso está disponível
    if (!m_fileAccessProvider) {
        Log("Erro: Provedor de acesso a arquivos não disponível.");
        
        FileOperationResult result;
        result.success = false;
        result.message = "Provedor de acesso a arquivos não disponível";
        result.timestamp = 0;
        result.originalPath = sourcePath;
        result.newPath = destinationPath;
        
        return result;
    }
    
    // Delegar a operação para o provedor
    return m_fileAccessProvider->AgentCopyFile(this, sourcePath, destinationPath, description);
}

FileOperationResult BaseAgent::CompileJsonToBinary(const std::string& jsonPath,
                                                 const std::string& binPath,
                                                 const std::string& description) {
    // Verificar se o agente tem capacidade de acesso a arquivos
    if (!HasCapability(AgentCapability::FILE_ACCESS)) {
        Log("Erro: Agente não tem capacidade de acessar arquivos.");
        
        FileOperationResult result;
        result.success = false;
        result.message = "Agente não tem capacidade de acesso a arquivos";
        result.timestamp = 0;
        result.originalPath = jsonPath;
        result.newPath = binPath;
        
        return result;
    }
    
    // Verificar se o provedor de acesso está disponível
    if (!m_fileAccessProvider) {
        Log("Erro: Provedor de acesso a arquivos não disponível.");
        
        FileOperationResult result;
        result.success = false;
        result.message = "Provedor de acesso a arquivos não disponível";
        result.timestamp = 0;
        result.originalPath = jsonPath;
        result.newPath = binPath;
        
        return result;
    }
    
    // Delegar a operação para o provedor
    return m_fileAccessProvider->AgentCompileJsonToBinary(this, jsonPath, binPath, description);
}

FileOperationResult BaseAgent::DecompileBinaryToJson(const std::string& binPath,
                                                   const std::string& jsonPath,
                                                   const std::string& description) {
    // Verificar se o agente tem capacidade de acesso a arquivos
    if (!HasCapability(AgentCapability::FILE_ACCESS)) {
        Log("Erro: Agente não tem capacidade de acessar arquivos.");
        
        FileOperationResult result;
        result.success = false;
        result.message = "Agente não tem capacidade de acesso a arquivos";
        result.timestamp = 0;
        result.originalPath = binPath;
        result.newPath = jsonPath;
        
        return result;
    }
    
    // Verificar se o provedor de acesso está disponível
    if (!m_fileAccessProvider) {
        Log("Erro: Provedor de acesso a arquivos não disponível.");
        
        FileOperationResult result;
        result.success = false;
        result.message = "Provedor de acesso a arquivos não disponível";
        result.timestamp = 0;
        result.originalPath = binPath;
        result.newPath = jsonPath;
        
        return result;
    }
    
    // Delegar a operação para o provedor
    return m_fileAccessProvider->AgentDecompileBinaryToJson(this, binPath, jsonPath, description);
}

std::string BaseAgent::ReadFile(const std::string& path) {
    // Verificar se o agente tem capacidade de acesso a arquivos
    if (!HasCapability(AgentCapability::FILE_ACCESS)) {
        Log("Erro: Agente não tem capacidade de acessar arquivos.");
        return "";
    }
    
    // Verificar se o provedor de acesso está disponível
    if (!m_fileAccessProvider) {
        Log("Erro: Provedor de acesso a arquivos não disponível.");
        return "";
    }
    
    // Delegar a operação para o provedor
    return m_fileAccessProvider->AgentReadFile(this, path);
}

DirectoryInfo BaseAgent::ListDirectory(const std::string& directory,
                                     bool recursive,
                                     const std::string& filter) {
    // Verificar se o agente tem capacidade de acesso a arquivos
    if (!HasCapability(AgentCapability::FILE_ACCESS)) {
        Log("Erro: Agente não tem capacidade de acessar arquivos.");
        
        DirectoryInfo emptyInfo;
        emptyInfo.path = directory;
        return emptyInfo;
    }
    
    // Verificar se o provedor de acesso está disponível
    if (!m_fileAccessProvider) {
        Log("Erro: Provedor de acesso a arquivos não disponível.");
        
        DirectoryInfo emptyInfo;
        emptyInfo.path = directory;
        return emptyInfo;
    }
    
    // Delegar a operação para o provedor
    return m_fileAccessProvider->AgentListDirectory(this, directory, recursive, filter);
}

FileOperationResult BaseAgent::RestoreFromBackup(const std::string& backupPath,
                                               bool restoreToOriginalPath,
                                               const std::string& alternativePath,
                                               const std::string& description) {
    // Verificar se o agente tem capacidade de acesso a arquivos
    if (!HasCapability(AgentCapability::FILE_ACCESS)) {
        Log("Erro: Agente não tem capacidade de acessar arquivos.");
        
        FileOperationResult result;
        result.success = false;
        result.message = "Agente não tem capacidade de acesso a arquivos";
        result.timestamp = 0;
        result.originalPath = backupPath;
        result.newPath = restoreToOriginalPath ? "" : alternativePath;
        
        return result;
    }
    
    // Verificar se o provedor de acesso está disponível
    if (!m_fileAccessProvider) {
        Log("Erro: Provedor de acesso a arquivos não disponível.");
        
        FileOperationResult result;
        result.success = false;
        result.message = "Provedor de acesso a arquivos não disponível";
        result.timestamp = 0;
        result.originalPath = backupPath;
        result.newPath = restoreToOriginalPath ? "" : alternativePath;
        
        return result;
    }
    
    // Delegar a operação para o provedor
    return m_fileAccessProvider->AgentRestoreBackup(this, backupPath, restoreToOriginalPath, alternativePath, description);
}

std::vector<BackupInfo> BaseAgent::GetBackupsForFile(const std::string& path) {
    // Verificar se o agente tem capacidade de acesso a arquivos
    if (!HasCapability(AgentCapability::FILE_ACCESS)) {
        Log("Erro: Agente não tem capacidade de acessar arquivos.");
        return std::vector<BackupInfo>();
    }
    
    // Verificar se o provedor de acesso está disponível
    if (!m_fileAccessProvider) {
        Log("Erro: Provedor de acesso a arquivos não disponível.");
        return std::vector<BackupInfo>();
    }
    
    // Delegar a operação para o provedor
    return m_fileAccessProvider->GetBackupsForFile(path);
}

} // namespace WYDStudio