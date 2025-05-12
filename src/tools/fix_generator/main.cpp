#include <iostream>
#include <memory>
#include <filesystem>
#include "WYDBRFixGenerator.cpp"
#include "CombatSystemGenerator.cpp"
#include "AntiCheatGenerator.cpp"

namespace fs = std::filesystem;

// Implementação dos geradores de sistema que seguem a interface SystemImplementationGenerator

class CombatSystemImplementationGenerator : public SystemImplementationGenerator {
private:
    CombatSystemGenerator generator;
    
public:
    bool generateImplementation(const std::string& basePath) override {
        return generator.generate(basePath);
    }
    
    std::string getSystemName() const override {
        return "Sistema de Combate Avançado";
    }
};

class AntiCheatImplementationGenerator : public SystemImplementationGenerator {
private:
    AntiCheatGenerator generator;
    
public:
    bool generateImplementation(const std::string& basePath) override {
        return generator.generate(basePath);
    }
    
    std::string getSystemName() const override {
        return "Sistema Anti-Cheat";
    }
};

// Adicione aqui as implementações de outros geradores de sistema conforme necessário

int main(int argc, char* argv[]) {
    // Define caminho base para saída (pode ser passado como argumento)
    std::string basePath = (argc > 1) ? argv[1] : "./src";
    
    // Inicializa o gerenciador
    FixGeneratorManager manager(basePath);
    
    // Registra os geradores de implementação
    std::cout << "Registrando geradores de sistema..." << std::endl;
    
    // Registra o gerador de sistema de combate
    manager.registerGenerator(std::make_unique<CombatSystemImplementationGenerator>());
    
    // Registra o gerador do sistema anti-cheat
    manager.registerGenerator(std::make_unique<AntiCheatImplementationGenerator>());
    
    // TODO: Registrar outros geradores aqui
    
    std::cout << "Iniciando geração de correções para WYDBR 2.0..." << std::endl;
    std::cout << "Caminho base para implementações: " << basePath << std::endl;
    
    // Executar a geração
    bool success = manager.generateAllImplementations();
    
    if (success) {
        std::cout << "\nTodos os sistemas foram gerados com sucesso!" << std::endl;
        std::cout << "As implementações estão disponíveis em: " << fs::absolute(basePath).string() << std::endl;
    } else {
        std::cerr << "\nHouve falhas na geração de alguns sistemas." << std::endl;
        std::cerr << "Verifique as mensagens acima para detalhes." << std::endl;
    }
    
    return success ? 0 : 1;
} 