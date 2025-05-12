/**
 * @file minimal_test.cpp
 * @brief Teste simplificado do DatabaseManager
 * 
 * Este arquivo implementa um teste básico do DatabaseManager para verificar
 * sua funcionalidade básica sem depender de outros componentes.
 */

#include "../../include/database/DatabaseManager.h"
#include "../../include/GlobalDef.h"
#include <iostream>
#include <cassert>

using namespace wydbr::database;

/**
 * main
 * 
 * Implementa a funcionalidade main conforme especificação original.
 * @return Retorna int
 */


int main(){
    std::cout << "=== Teste simplificado do DatabaseManager ===" << std::endl;
    
    // Cria uma instância do DatabaseManager
    DatabaseManager& dbManager = DatabaseManager::getInstance();
    
    // Inicializa o manager
    std::cout << "DatabaseManager::initialize(0, test_db_simple, 2)" << std::endl;
    bool result = dbManager.initialize(DatabaseType::FILE, "test_db_simple", 2);
    std::cout << "Inicialização: " << (result ? "OK" : "FALHA") << std::endl;
    assert(result && "Falha na inicialização do DatabaseManager");
    
    // Cria uma tabela de teste
    std::cout << "DatabaseManager::executeNonQuery: CREATE TABLE TestTable (id INTEGER, name TEXT, value INTEGER)" << std::endl;
    DatabaseResult createResult = dbManager.executeNonQuery("CREATE TABLE TestTable (id INTEGER, name TEXT, value INTEGER)");
    std::cout << "Criação de tabela: " << (createResult == DatabaseResult::SUCCESS ? "OK" : "FALHA") << std::endl;
    assert(createResult == DatabaseResult::SUCCESS && "Falha na criação da tabela");
    
    // Insere dados de teste
    std::cout << "DatabaseManager::executeNonQuery: INSERT INTO TestTable (id, name, value) VALUES (1, 'Item1', 100)" << std::endl;
    DatabaseResult insertResult = dbManager.executeNonQuery("INSERT INTO TestTable (id, name, value) VALUES (1, 'Item1', 100)");
    std::cout << "Inserção de dados: " << (insertResult == DatabaseResult::SUCCESS ? "OK" : "FALHA") << std::endl;
    assert(insertResult == DatabaseResult::SUCCESS && "Falha na inserção de dados");
    
    // Consulta dados
    std::cout << "DatabaseManager::executeQuery: SELECT * FROM TestTable" << std::endl;
    auto queryResult = dbManager.executeQuery("SELECT * FROM TestTable");
    std::cout << "Consulta: " << (queryResult ? "OK" : "FALHA") << ", " << (queryResult ? std::to_string(queryResult->getRowCount()) : "0") << " linhas encontradas" << std::endl;
    assert(queryResult && "Falha na consulta");
    assert(queryResult->getRowCount() > 0 && "Nenhum resultado encontrado");
    
    // Exibe os dados de exemplo /**
 * for
 * 
 * Implementa a funcionalidade for conforme especificação original.
 * @param queryResult->getRowCount( Parâmetro queryResult->getRowCount(
 * @return Retorna obtidos
 */
 obtidos
    for(uint32_t i = 0; i < queryResult->getRowCount(); i++) {
        if (i > 0) {
            queryResult->nextRow();
        }
        std::cout << "Dados: id=" << queryResult->getInt("id") 
                 << ", name=" << queryResult->getString("name")
                 << ", value=" << queryResult->getInt("value") << std::endl;
    }
    
    // Finaliza o manager
    std::cout << "DatabaseManager::shutdown()" << std::endl;
    dbManager.shutdown();
    
    std::cout << "Teste concluído." << std::endl;
    return 0;
}