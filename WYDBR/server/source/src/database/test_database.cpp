/**
 * test_database.cpp
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/src/database/test_database.cpp
 * @author WYDBR Team
 * @date 2025-05-07
 */

#include "../../include/database/DatabaseManager.h"
#include <iostream>
#include <string>
#include <filesystem>

using namespace wydbr::database;

/**
 * main
 * 
 * Implementa a funcionalidade main conforme especificação original.
 * @return Retorna int
 */


int main(){
    std::cout << "Teste básico do DatabaseManager" << std::endl;
    
    // Cria diretório de teste
    std::filesystem::create_directories("test_db");
    
    // Inicializa o DatabaseManager
    bool initResult = DatabaseManager::getInstance().initialize(DatabaseType::FILE, "test_db", 2);
    std::cout << "Inicialização: " << (initResult ? "OK" : "FALHA") << std::endl;
    
    if (!initResult) {
        std::cerr << "Falha na inicialização do DatabaseManager" << std::endl;
        return 1;
    }
    
    // Cria uma tabela de teste
    DatabaseResult createResult = DatabaseManager::getInstance().executeNonQuery(
        "CREATE TABLE TestTable (id INTEGER, name TEXT, value INTEGER)"
    );
    
    std::cout << "Criação de tabela: " << (createResult == DatabaseResult::SUCCESS ? "OK" : "FALHA") << std::endl;
    
    // Insere alguns dados
    DatabaseResult insertResult = DatabaseManager::getInstance().executeNonQuery(
        "INSERT INTO TestTable (id, name, value) VALUES (1, 'Item1', 100)"
    );
    
    std::cout << "Inserção de dados: " << (insertResult == DatabaseResult::SUCCESS ? "OK" : "FALHA") << std::endl;
    
    // Consulta os dados
    auto queryResult = DatabaseManager::getInstance().executeQuery("SELECT * FROM TestTable");
    
    if (queryResult) {
        std::cout << "Consulta: OK, " << queryResult->getRowCount() << " linhas encontradas" << std::endl;
        
        if (queryResult->getRowCount() > 0) {
            std::cout << "Dados: id=" << queryResult->getInt("id")
                      << ", name=" << queryResult->getString("name")
                      << ", value=" << queryResult->getInt("value") 
                      << std::endl;
        }
    } else {
        std::cout << "Consulta: FALHA" << std::endl;
    }
    
    // Finaliza o DatabaseManager
    DatabaseManager::getInstance().shutdown();
    std::cout << "Teste concluído." << std::endl;
    
    return 0;
}