#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <map>

namespace fs = std::filesystem;

class SystemImplementationGenerator {
public:
    virtual ~SystemImplementationGenerator() = default;
    virtual bool generateImplementation(const std::string& basePath) = 0;
    virtual std::string getSystemName() const = 0;
};

class FixGeneratorManager {
private:
    std::vector<std::unique_ptr<SystemImplementationGenerator>> generators;
    std::string outputBasePath;
    
    // Sistemas já gerados para evitar duplicação
    std::map<std::string, bool> generatedSystems;
    
    // Cria diretório se não existir
    bool ensureDirectoryExists(const std::string& dirPath) {
        if (fs::exists(dirPath)) {
            return true;
        }
        
        try {
            return fs::create_directories(dirPath);
        } catch (const std::exception& e) {
            std::cerr << "Erro ao criar diretório " << dirPath << ": " << e.what() << std::endl;
            return false;
        }
    }
    
public:
    explicit FixGeneratorManager(const std::string& basePath) : outputBasePath(basePath) {}
    
    void registerGenerator(std::unique_ptr<SystemImplementationGenerator> generator) {
        generators.push_back(std::move(generator));
    }
    
    bool generateAllImplementations() {
        std::cout << "\n=======================================================" << std::endl;
        std::cout << "INICIANDO GERAÇÃO DE IMPLEMENTAÇÕES PARA WYDBR 2.0" << std::endl;
        std::cout << "=======================================================" << std::endl;
        
        bool allSuccess = true;
        int systemCount = 0;
        int successCount = 0;
        
        // Verifica caminho base
        if (!ensureDirectoryExists(outputBasePath)) {
            std::cerr << "ERRO: Não foi possível garantir a existência do diretório base!" << std::endl;
            return false;
        }
        
        // Gera cada sistema registrado
        for (const auto& generator : generators) {
            systemCount++;
            std::string systemName = generator->getSystemName();
            
            // Verifica se já foi gerado
            if (generatedSystems[systemName]) {
                std::cout << "Sistema " << systemName << " já foi gerado anteriormente." << std::endl;
                continue;
            }
            
            std::cout << "\n[" << systemCount << "/" << generators.size() << "] Gerando sistema: " << systemName << std::endl;
            
            bool success = generator->generateImplementation(outputBasePath);
            if (success) {
                successCount++;
                generatedSystems[systemName] = true;
                std::cout << "✓ Implementação do " << systemName << " concluída com sucesso!" << std::endl;
            } else {
                allSuccess = false;
                std::cerr << "✗ Falha ao gerar implementação para " << systemName << std::endl;
            }
        }
        
        // Relatório final
        std::cout << "\n=======================================================" << std::endl;
        std::cout << "RESUMO DA GERAÇÃO DE IMPLEMENTAÇÕES" << std::endl;
        std::cout << "-------------------------------------------------------" << std::endl;
        std::cout << "Total de sistemas: " << systemCount << std::endl;
        std::cout << "Implementações bem-sucedidas: " << successCount << std::endl;
        std::cout << "Implementações com falha: " << (systemCount - successCount) << std::endl;
        std::cout << "=======================================================" << std::endl;
        
        return allSuccess;
    }
};

// Função principal
int main(int argc, char* argv[]) {
    // Define caminho base para saída
    std::string basePath = (argc > 1) ? argv[1] : "../src";
    
    // Inicializa o gerenciador
    FixGeneratorManager manager(basePath);
    
    // Aqui serão registrados os geradores de implementação específicos
    // Isso será feito nas próximas partes do script
    
    std::cout << "Gerador de Correções WYDBR 2.0" << std::endl;
    std::cout << "Caminho base para implementações: " << basePath << std::endl;
    
    // Executar a geração
    return manager.generateAllImplementations() ? 0 : 1;
} 