#include "../include/BaseAgent.h"
#include "../include/AgentFileSystem.h"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <filesystem>

using namespace WYDStudio;
namespace fs = std::filesystem;

// Classe fictícia para simular um agente administrador
class SimpleAdminAgent : public BaseAgent {
public:
    SimpleAdminAgent() {
        // Adicionar capacidade de acesso a arquivos
        m_capabilities.push_back(AgentCapability::FILE_ACCESS);
    }
    
    bool Initialize(const std::string& modelPath) override {
        std::cout << "SimpleAdminAgent inicializado" << std::endl;
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

// Função para logar mensagens
void LogToConsole(const std::string& message) {
    std::cout << "[LOG] " << message << std::endl;
}

int main() {
    std::cout << "=== Exemplo Simplificado de Sistema de Arquivos para Agentes ===\n" << std::endl;
    
    try {
        // Criar diretórios de exemplo
        fs::create_directories("./simple_example");
        fs::create_directories("./simple_example/data");
        fs::create_directories("./simple_example/backups");
        
        // 1. Criar e inicializar o sistema de arquivos para agentes
        auto fileSystem = std::make_shared<AgentFileSystem>();
        fileSystem->SetLogCallback(LogToConsole);
        
        if (!fileSystem->Initialize("./simple_example", "./simple_example/backups")) {
            std::cerr << "Erro ao inicializar o sistema de arquivos" << std::endl;
            return 1;
        }
        
        // 2. Configurar permissões personalizadas
        fileSystem->AllowDirectory(AgentType::ADMIN, "./simple_example/data");
        fileSystem->AllowFileType(AgentType::ADMIN, ".txt");
        fileSystem->AllowFileType(AgentType::ADMIN, ".json");
        fileSystem->AllowOperations(AgentType::ADMIN, {
            FileOperation::CREATE,
            FileOperation::MODIFY,
            FileOperation::DELETE
        });
        
        // 3. Aplicar as configurações de permissão
        if (!fileSystem->ApplyPermissionsAndFinalize()) {
            std::cerr << "Erro ao aplicar configurações de permissão" << std::endl;
            return 1;
        }
        
        // 4. Criar e registrar um agente
        auto adminAgent = std::make_shared<SimpleAdminAgent>();
        adminAgent->Initialize("");
        
        if (!fileSystem->RegisterAgent(adminAgent, "SimpleAdminAgent")) {
            std::cerr << "Erro ao registrar agente" << std::endl;
            return 1;
        }
        
        // 5. Usar o agente para realizar operações
        std::cout << "\n=== Realizando operações com o agente ===\n" << std::endl;
        
        // Criar um arquivo
        auto createResult = adminAgent->CreateFile(
            "./simple_example/data/test.txt",
            "Este é um arquivo de teste criado pelo agente.",
            "Teste de criação de arquivo"
        );
        
        std::cout << "Criação de arquivo: " << (createResult.success ? "Sucesso" : "Falha") << std::endl;
        std::cout << "Mensagem: " << createResult.message << std::endl;
        
        // Modificar o arquivo
        auto modifyResult = adminAgent->ModifyFile(
            "./simple_example/data/test.txt",
            "Este arquivo foi modificado pelo agente.\nEsta é a segunda linha.",
            "Teste de modificação de arquivo"
        );
        
        std::cout << "\nModificação de arquivo: " << (modifyResult.success ? "Sucesso" : "Falha") << std::endl;
        std::cout << "Mensagem: " << modifyResult.message << std::endl;
        
        if (!modifyResult.backupPath.empty()) {
            std::cout << "Backup: " << modifyResult.backupPath << std::endl;
        }
        
        // Ler o conteúdo do arquivo
        std::string content = adminAgent->ReadFile("./simple_example/data/test.txt");
        std::cout << "\nConteúdo do arquivo:\n" << content << std::endl;
        
        // Listar backups
        auto backups = adminAgent->GetBackupsForFile("./simple_example/data/test.txt");
        std::cout << "\nBackups disponíveis: " << backups.size() << std::endl;
        
        // Restaurar backup se disponível
        if (!backups.empty()) {
            auto restoreResult = adminAgent->RestoreFromBackup(
                backups[0].backupPath,
                true,
                "",
                "Teste de restauração de backup"
            );
            
            std::cout << "\nRestauração de backup: " << (restoreResult.success ? "Sucesso" : "Falha") << std::endl;
            std::cout << "Mensagem: " << restoreResult.message << std::endl;
            
            // Ler o conteúdo restaurado
            content = adminAgent->ReadFile("./simple_example/data/test.txt");
            std::cout << "\nConteúdo restaurado:\n" << content << std::endl;
        }
        
        // Excluir o arquivo
        auto deleteResult = adminAgent->DeleteFile(
            "./simple_example/data/test.txt",
            "Teste de exclusão de arquivo"
        );
        
        std::cout << "\nExclusão de arquivo: " << (deleteResult.success ? "Sucesso" : "Falha") << std::endl;
        std::cout << "Mensagem: " << deleteResult.message << std::endl;
        
        // Salvar configurações de permissão
        fileSystem->SavePermissions("./simple_example/permissions.json");
        
        std::cout << "\n=== Exemplo concluído com sucesso ===\n" << std::endl;
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Erro durante a execução do exemplo: " << e.what() << std::endl;
        return 1;
    }
}