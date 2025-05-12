/**
 * DatabaseDef.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file server/source/include/database/DatabaseDef.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef DATABASEDEF_H
#define DATABASEDEF_H

/**
 * @file DatabaseDef.h
 * @brief Definições para o sistema de banco de dados
 * 
 * Este arquivo contém definições e constantes usadas pelo sistema de banco de dados.
 */

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <cstdint>
#include <functional>

// Inclusões necessárias para estruturas de dados
#include "../TM_Item.h"
#include "../TM_Mob.h"
#include "../GlobalDef.h"

namespace wydbr {
namespace database {

// Tipo de banco de dados
enum /**
 * Classe WYDDatabaseType
 * 
 * Esta classe implementa a funcionalidade WYDDatabaseType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class DatabaseType {
    SQLITE = 0,  // SQLite (banco de dados em arquivo)
    MYSQL  = 1,  // MySQL
    FILE   = 2   // Arquivo de texto/binário
};

// Resultado de operações de banco de dados
enum /**
 * Classe WYDDatabaseResult
 * 
 * Esta classe implementa a funcionalidade WYDDatabaseResult conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class DatabaseResult {
    SUCCESS = 0,      // Operação bem-sucedida
    ERROR_GENERAL,    // Erro geral
    ERROR_NOTFOUND,   // Item não encontrado
    ERROR_DUPLICATE,  // Item duplicado
    ERROR_CONNECT,    // Erro de conexão
    ERROR_QUERY,      // Erro na query
    ERROR_EXECUTE,    // Erro na execução
    ERROR_PREPARE,    // Erro na preparação
    ERROR_BIND,       // Erro no bind de parâmetros
    ERROR_TRANSACTION // Erro em transação
};

// Enumeração para os tipos de dados suportados em uma consulta
enum /**
 * Classe WYDDatabaseFieldType
 * 
 * Esta classe implementa a funcionalidade WYDDatabaseFieldType conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class DatabaseFieldType {
    INTEGER,
    REAL,
    TEXT,
    BLOB,
    NULL_TYPE
};

// Forward declaration
class DatabaseResultSet;

// Alias para ponteiro de resultado de query
using DatabaseResultSetPtr = std::shared_ptr<DatabaseResultSet>;

// Interface para resultados de consulta
/**
 * Classe WYDDatabaseResultSet
 * 
 * Esta classe implementa a funcionalidade WYDDatabaseResultSet conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class DatabaseResultSet {
public:
    virtual ~DatabaseResultSet() = default;
    
    // Navegação
    virtual bool nextRow() = 0;
    virtual void reset() = 0;
    
    // Informações sobre o resultado
    virtual uint32_t getRowCount() const = 0;
    virtual uint32_t getColumnCount() const = 0;
    virtual std::string getColumnName(uint32_t columnIndex) const = 0;
    virtual DatabaseFieldType getColumnType(uint32_t columnIndex) const = 0;
    
    // Recuperação de valores
    virtual int32_t getInt(uint32_t columnIndex) const = 0;
    virtual int32_t getInt(const std::string& columnName) const = 0;
    
    virtual int64_t getInt64(uint32_t columnIndex) const = 0;
    virtual int64_t getInt64(const std::string& columnName) const = 0;
    
    virtual double getDouble(uint32_t columnIndex) const = 0;
    virtual double getDouble(const std::string& columnName) const = 0;
    
    virtual std::string getString(uint32_t columnIndex) const = 0;
    virtual std::string getString(const std::string& columnName) const = 0;
    
    virtual const uint8_t* getBlob(uint32_t columnIndex, uint32_t& size) const = 0;
    virtual const uint8_t* getBlob(const std::string& columnName, uint32_t& size) const = 0;
    
    virtual bool isNull(uint32_t columnIndex) const = 0;
    virtual bool isNull(const std::string& columnName) const = 0;
};

// Estrutura de configuração de banco de dados
struct DatabaseConfig {
    DatabaseType type;            // Tipo de banco de dados
    std::string connectionString; // String de conexão (ou caminho do arquivo)
    uint32_t maxConnections;      // Número máximo de conexões
    std::string username;         // Nome de usuário (para MySQL)
    std::string password;         // Senha (para MySQL)
    
    // Construtor padrão
    DatabaseConfig() 
        : type(DatabaseType::SQLITE), maxConnections(1) {}
    
    // Construtor com parâmetros
    DatabaseConfig(DatabaseType t, const std::string& conn, uint32_t maxConn = 1)
        : type(t), connectionString(conn), maxConnections(maxConn) {}
};

} // namespace database
} // namespace wydbr

#endif // DATABASEDEF_H