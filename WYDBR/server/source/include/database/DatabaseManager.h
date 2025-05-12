/**
 * DatabaseManager.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/include/database/DatabaseManager.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

/**
 * @file DatabaseManager.h
 * @brief Gerenciador de banco de dados
 * 
 * Este arquivo contém a definição da classe DatabaseManager, que fornece
 * uma interface unificada para operações de banco de dados.
 */

#pragma once

#include <string>
#include <memory>
#include <mutex>
#include <vector>
#include <map>
#include "DatabaseDef.h"

namespace wydbr {
namespace database {

// Forward declarations
class DatabaseConnection;
using DatabaseConnectionPtr = std::shared_ptr<DatabaseConnection>;

/**
 * @class DatabaseManager
 * @brief Gerenciador central de banco de dados
 * 
 * Classe Singleton que gerencia conexões e operações de banco de dados,
 * fornecendo uma interface comum independente do tipo de banco utilizado.
 */
/**
 * Classe WYDDatabaseManager
 * 
 * Esta classe implementa a funcionalidade WYDDatabaseManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class DatabaseManager {
public:
    /**
     * @brief Obtém a instância única do DatabaseManager
     * @return Referência para a instância do DatabaseManager
     */
    static DatabaseManager& getInstance();

    /**
     * @brief Inicializa o gerenciador de banco de dados
     * @param type Tipo de banco de dados a ser utilizado
     * @param connectionString String de conexão ou caminho para o arquivo
     * @param maxConnections Número máximo de conexões simultâneas
     * @param username Nome de usuário (opcional, para MySQL)
     * @param password Senha (opcional, para MySQL)
     * @return true se a inicialização foi bem-sucedida, false caso contrário
     */
    bool initialize(DatabaseType type, const std::string& connectionString, 
                    uint32_t maxConnections = 1, 
                    const std::string& username = "", 
                    const std::string& password = "");
    
    /**
     * @brief Finaliza o gerenciador de banco de dados
     */
    void shutdown();
    
    /**
     * @brief Executa uma consulta SQL que não retorna resultados
     * @param query String SQL a ser executada
     * @return Resultado da operação
     */
    DatabaseResult executeNonQuery(const std::string& query);
    
    /**
     * @brief Executa uma consulta SQL e retorna um conjunto de resultados
     * @param query String SQL a ser executada
     * @return Ponteiro para o conjunto de resultados, ou nullptr em caso de erro
     */
    DatabaseResultSetPtr executeQuery(const std::string& query);
    
    /**
     * @brief Inicia uma transação
     * @return Resultado da operação
     */
    DatabaseResult beginTransaction();
    
    /**
     * @brief Confirma uma transação
     * @return Resultado da operação
     */
    DatabaseResult commitTransaction();
    
    /**
     * @brief Cancela uma transação
     * @return Resultado da operação
     */
    DatabaseResult rollbackTransaction();
    
    /**
     * @brief Verifica se o gerenciador está inicializado
     * @return true se inicializado, false caso contrário
     */
    bool isInitialized() const;

private:
    // Construtor privado (padrão Singleton)
    DatabaseManager();
    
    // Impede cópia e atribuição
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;
    
    // Obtém uma conexão disponível
    /**
 * getConnection
 * 
 * Implementa a funcionalidade getConnection conforme especificação original.
 * @return Retorna DatabaseConnectionPtr
 */

    DatabaseConnectionPtr getConnection();
    
    // Libera uma conexão para reuso
    /**
 * releaseConnection
 * 
 * Implementa a funcionalidade releaseConnection conforme especificação original.
 * @param connection Parâmetro connection
 */

    void releaseConnection(DatabaseConnectionPtr connection);
    
    // Atributos privados
    bool initialized;                              // Flag de inicialização
    DatabaseConfig config;                         // Configuração do banco
    std::vector<DatabaseConnectionPtr> connections; // Pool de conexões
    std::mutex connectionMutex;                    // Mutex para acesso ao pool
    
    // Criação de conexão específica para cada tipo de banco
    /**
 * createConnection
 * 
 * Implementa a funcionalidade createConnection conforme especificação original.
 * @param config Parâmetro config
 * @return Retorna DatabaseConnectionPtr
 */

    DatabaseConnectionPtr createConnection(const DatabaseConfig& config);
};

} // namespace database
} // namespace wydbr

#endif // DATABASEMANAGER_H