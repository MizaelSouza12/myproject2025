#include "../include/BaseAgent.h"
#include "../include/GameFileManager.h"
#include "../include/AgentFileAccessProvider.h"
#include "../include/AgentPermissionManager.h"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <filesystem>

using namespace WYDStudio;
namespace fs = std::filesystem;

// Classe fictícia para simular um agente administrador
class AdminAgent : public BaseAgent {
public:
    AdminAgent() {
        // Adicionar capacidade de acesso a arquivos
        m_capabilities.push_back(AgentCapability::FILE_ACCESS);
    }
    
    bool Initialize(const std::string& modelPath) override {
        std::cout << "AdminAgent inicializado com modelPath: " << modelPath << std::endl;
        return true;
    }
    
    std::string ProcessQuery(const std::string& query) override {
        return "AdminAgent responde: " + query;
    }
    
    std::string ExecuteAction(const std::string& actionName, 
                           const std::map<std::string, std::string>& parameters) override {
        return "AdminAgent executou: " + actionName;
    }
    
    AgentType GetAgentType() const override {
        return AgentType::ADMIN;
    }
    
    std::vector<AgentCapability> GetCapabilities() const override {
        return m_capabilities;
    }
};

// Classe fictícia para simular um agente GM
class GameMasterAgent : public BaseAgent {
public:
    GameMasterAgent() {
        // Adicionar capacidade de acesso a arquivos
        m_capabilities.push_back(AgentCapability::FILE_ACCESS);
    }
    
    bool Initialize(const std::string& modelPath) override {
        std::cout << "GameMasterAgent inicializado com modelPath: " << modelPath << std::endl;
        return true;
    }
    
    std::string ProcessQuery(const std::string& query) override {
        return "GameMasterAgent responde: " + query;
    }
    
    std::string ExecuteAction(const std::string& actionName, 
                           const std::map<std::string, std::string>& parameters) override {
        return "GameMasterAgent executou: " + actionName;
    }
    
    AgentType GetAgentType() const override {
        return AgentType::GM;
    }
    
    std::vector<AgentCapability> GetCapabilities() const override {
        return m_capabilities;
    }
};

// Função para logar mensagens
void LogCallback(const std::string& message) {
    std::cout << "[LOG] " << message << std::endl;
}

// Função para exibir resultado de operação com arquivo
void PrintOperationResult(const std::string& agentName, 
                        const std::string& operation,
                        const FileOperationResult& result) {
    std::cout << "\n===== " << agentName << " - " << operation << " =====" << std::endl;
    std::cout << "Sucesso: " << (result.success ? "Sim" : "Não") << std::endl;
    std::cout << "Mensagem: " << result.message << std::endl;
    if (!result.backupPath.empty()) {
        std::cout << "Backup: " << result.backupPath << std::endl;
    }
    std::cout << "==============================\n" << std::endl;
}

int main() {
    std::cout << "=== Exemplo de Sistema de Permissões de Arquivos ===\n" << std::endl;
    
    try {
        // Criar diretórios de exemplo
        fs::create_directories("./example_data");
        fs::create_directories("./example_data/admin");
        fs::create_directories("./example_data/gm");
        fs::create_directories("./example_data/shared");
        fs::create_directories("./example_backups");
        
        // 1. Criar o gerenciador de arquivos
        auto fsConnector = std::make_shared<FileSystemConnector>();
        auto serverConnector = std::make_shared<ServerConnector>();
        auto fileManager = std::make_shared<GameFileManager>(fsConnector, serverConnector);
        
        // Inicializar o gerenciador de arquivos
        fileManager->Initialize("./", "./example_backups", 5);
        fileManager->SetLogCallback(LogCallback);
        
        // 2. Criar o provedor de acesso a arquivos
        auto fileAccessProvider = std::make_shared<AgentFileAccessProvider>(fileManager);
        fileAccessProvider->SetLogCallback(LogCallback);
        
        // 3. Criar o gerenciador de permissões
        auto permissionManager = std::make_shared<AgentPermissionManager>(fileAccessProvider);
        permissionManager->SetLogCallback(LogCallback);
        
        // 4. Configurar permissões específicas para este exemplo
        
        // Admin tem acesso completo a tudo
        permissionManager->AddAllowedDirectory(AgentType::ADMIN, "./example_data");
        permissionManager->SetAllowedOperations(AgentType::ADMIN, {
            FileOperation::CREATE,
            FileOperation::MODIFY,
            FileOperation::DELETE,
            FileOperation::MOVE,
            FileOperation::COPY
        });
        
        // GM tem acesso apenas a diretórios específicos e não pode excluir
        permissionManager->AddAllowedDirectory(AgentType::GM, "./example_data/gm");
        permissionManager->AddAllowedDirectory(AgentType::GM, "./example_data/shared");
        permissionManager->SetAllowedOperations(AgentType::GM, {
            FileOperation::CREATE,
            FileOperation::MODIFY,
            FileOperation::COPY
        });
        
        // 5. Aplicar as configurações de permissão
        permissionManager->ApplyPermissions();
        
        // 6. Criar agentes
        auto adminAgent = std::make_shared<AdminAgent>();
        auto gmAgent = std::make_shared<GameMasterAgent>();
        
        // Inicializar agentes
        adminAgent->Initialize("./models");
        gmAgent->Initialize("./models");
        
        // Configurar acesso a arquivos para os agentes
        adminAgent->SetFileAccessProvider(fileAccessProvider);
        gmAgent->SetFileAccessProvider(fileAccessProvider);
        
        // Registrar agentes no provedor de acesso
        fileAccessProvider->RegisterAgent(adminAgent, AgentType::ADMIN, "AdminAgent");
        fileAccessProvider->RegisterAgent(gmAgent, AgentType::GM, "GameMasterAgent");
        
        // 7. Testar operações com os agentes
        
        // Criar arquivos de teste
        std::cout << "\n=== Criando arquivos de teste ===\n" << std::endl;
        
        // Admin cria arquivo em seu diretório (deve funcionar)
        auto adminResult1 = adminAgent->CreateFile(
            "./example_data/admin/admin_file.txt",
            "Este arquivo foi criado pelo administrador.",
            "Teste de criação de arquivo pelo admin"
        );
        PrintOperationResult("Admin", "Criar arquivo em diretório admin", adminResult1);
        
        // Admin cria arquivo no diretório compartilhado (deve funcionar)
        auto adminResult2 = adminAgent->CreateFile(
            "./example_data/shared/shared_file.txt",
            "Este arquivo foi criado pelo administrador no diretório compartilhado.",
            "Teste de criação de arquivo pelo admin em diretório compartilhado"
        );
        PrintOperationResult("Admin", "Criar arquivo em diretório compartilhado", adminResult2);
        
        // GM tenta criar arquivo no diretório do admin (deve falhar)
        auto gmResult1 = gmAgent->CreateFile(
            "./example_data/admin/gm_file.txt",
            "Este arquivo foi criado pelo GM no diretório do admin.",
            "Teste de criação de arquivo pelo GM em diretório do admin"
        );
        PrintOperationResult("GM", "Criar arquivo em diretório do admin", gmResult1);
        
        // GM cria arquivo em seu próprio diretório (deve funcionar)
        auto gmResult2 = gmAgent->CreateFile(
            "./example_data/gm/gm_file.txt",
            "Este arquivo foi criado pelo GM.",
            "Teste de criação de arquivo pelo GM"
        );
        PrintOperationResult("GM", "Criar arquivo em diretório do GM", gmResult2);
        
        // GM cria arquivo no diretório compartilhado (deve funcionar)
        auto gmResult3 = gmAgent->CreateFile(
            "./example_data/shared/gm_shared_file.txt",
            "Este arquivo foi criado pelo GM no diretório compartilhado.",
            "Teste de criação de arquivo pelo GM em diretório compartilhado"
        );
        PrintOperationResult("GM", "Criar arquivo em diretório compartilhado", gmResult3);
        
        // 8. Testar modificação de arquivos
        std::cout << "\n=== Modificando arquivos ===\n" << std::endl;
        
        // Admin modifica arquivo no diretório compartilhado (deve funcionar)
        auto adminResult3 = adminAgent->ModifyFile(
            "./example_data/shared/shared_file.txt",
            "Este arquivo foi modificado pelo administrador.",
            "Teste de modificação de arquivo pelo admin"
        );
        PrintOperationResult("Admin", "Modificar arquivo em diretório compartilhado", adminResult3);
        
        // GM modifica arquivo no diretório compartilhado (deve funcionar)
        auto gmResult4 = gmAgent->ModifyFile(
            "./example_data/shared/shared_file.txt",
            "Este arquivo foi modificado pelo GM.",
            "Teste de modificação de arquivo pelo GM"
        );
        PrintOperationResult("GM", "Modificar arquivo em diretório compartilhado", gmResult4);
        
        // 9. Testar exclusão de arquivos
        std::cout << "\n=== Excluindo arquivos ===\n" << std::endl;
        
        // Admin exclui arquivo no diretório compartilhado (deve funcionar)
        auto adminResult4 = adminAgent->DeleteFile(
            "./example_data/shared/gm_shared_file.txt",
            "Teste de exclusão de arquivo pelo admin"
        );
        PrintOperationResult("Admin", "Excluir arquivo em diretório compartilhado", adminResult4);
        
        // GM tenta excluir arquivo no diretório compartilhado (deve falhar devido às permissões)
        auto gmResult5 = gmAgent->DeleteFile(
            "./example_data/shared/shared_file.txt",
            "Teste de exclusão de arquivo pelo GM"
        );
        PrintOperationResult("GM", "Excluir arquivo em diretório compartilhado", gmResult5);
        
        // 10. Testar restauração de backup
        if (!adminResult3.backupPath.empty()) {
            std::cout << "\n=== Restaurando backup ===\n" << std::endl;
            
            // Admin restaura o backup (deve funcionar)
            auto adminResult5 = adminAgent->RestoreFromBackup(
                adminResult3.backupPath,
                true,
                "",
                "Teste de restauração de backup pelo admin"
            );
            PrintOperationResult("Admin", "Restaurar backup", adminResult5);
        }
        
        // 11. Listar backups disponíveis
        std::cout << "\n=== Backups disponíveis ===\n" << std::endl;
        
        auto adminBackups = adminAgent->GetBackupsForFile("./example_data/shared/shared_file.txt");
        std::cout << "Total de backups para shared_file.txt: " << adminBackups.size() << std::endl;
        
        for (size_t i = 0; i < adminBackups.size(); ++i) {
            const auto& backup = adminBackups[i];
            std::cout << "Backup #" << (i + 1) << ":" << std::endl;
            std::cout << "  Caminho: " << backup.backupPath << std::endl;
            std::cout << "  Descrição: " << backup.description << std::endl;
            std::cout << std::endl;
        }
        
        // 12. Salvar configurações de permissão
        permissionManager->SavePermissionsToFile("./example_data/permissions.json");
        
        std::cout << "\n=== Exemplo concluído com sucesso ===\n" << std::endl;
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Erro durante a execução do exemplo: " << e.what() << std::endl;
        return 1;
    }
}