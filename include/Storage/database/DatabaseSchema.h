/**
 * @file DatabaseSchema.h
 * @brief Definições unificadas de esquema de banco de dados
 */

#ifndef WYDBR_DATABASE_SCHEMA_H
#define WYDBR_DATABASE_SCHEMA_H

#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <any>
#include <optional>
#include <variant>
#include <typeindex>
#include <typeinfo>

namespace WYDBR {
namespace Storage {

/**
 * Tipos de campo de banco de dados
 */
enum class FieldType {
    Integer,     // Inteiro
    BigInteger,  // Inteiro grande
    Real,        // Ponto flutuante
    Text,        // Texto
    Blob,        // Dados binários
    Boolean,     // Booleano
    Timestamp,   // Timestamp
    UUID,        // UUID
    JSON,        // JSON
    Enum         // Enumeração
};

/**
 * Flags de campo
 */
enum class FieldFlag {
    None         = 0x00,
    PrimaryKey   = 0x01,    // Chave primária
    NotNull      = 0x02,    // Não nulo
    Unique       = 0x04,    // Único
    Index        = 0x08,    // Indexado
    ForeignKey   = 0x10,    // Chave estrangeira
    AutoIncrement = 0x20,   // Autoincremento
    Generated    = 0x40     // Gerado
};

// Operador OR para FieldFlag
inline FieldFlag operator|(FieldFlag a, FieldFlag b) {
    return static_cast<FieldFlag>(
        static_cast<int>(a) | static_cast<int>(b)
    );
}

// Operador AND para FieldFlag
inline bool operator&(FieldFlag a, FieldFlag b) {
    return (static_cast<int>(a) & static_cast<int>(b)) != 0;
}

/**
 * Definição de campo de banco de dados
 */
struct FieldDef {
    std::string name;        // Nome do campo
    FieldType type;          // Tipo do campo
    FieldFlag flags;         // Flags do campo
    std::string defaultValue; // Valor padrão
    std::string check;       // Restrição de verificação
    std::string foreignKey;  // Referência de chave estrangeira
    std::string comment;     // Comentário
    int size;                // Tamanho (para text, blob)
    int precision;           // Precisão (para real)
    int scale;               // Escala (para real)
    std::vector<std::string> enumValues; // Valores para enum
};

/**
 * Definição de índice
 */
struct IndexDef {
    std::string name;        // Nome do índice
    std::vector<std::string> fields; // Campos indexados
    bool unique;             // Índice único
    std::string where;       // Cláusula WHERE
};

/**
 * Definição de tabela
 */
struct TableDef {
    std::string name;        // Nome da tabela
    std::vector<FieldDef> fields; // Campos
    std::vector<IndexDef> indices; // Índices
    std::string primaryKey;  // Campo de chave primária
    std::string comment;     // Comentário
    std::string engine;      // Engine de banco de dados
    std::string charset;     // Charset
    std::string collation;   // Collation
};

/**
 * Tipos para valores de campo
 */
using FieldValue = std::variant<
    int32_t,        // INTEGER
    int64_t,        // BIGINT
    double,         // REAL
    std::string,    // TEXT ou outros
    std::vector<uint8_t>, // BLOB
    bool,           // BOOLEAN
    std::nullptr_t  // NULL
>;

/**
 * Metadados de tabela do banco de dados
 */
class TableMetadata {
public:
    /**
     * Construtor
     * @param name Nome da tabela
     */
    explicit TableMetadata(const std::string& name);
    
    /**
     * Adiciona uma definição de campo
     * @param field Definição do campo
     * @return Referência para this (para encadeamento)
     */
    TableMetadata& AddField(const FieldDef& field);
    
    /**
     * Adiciona um índice
     * @param index Definição do índice
     * @return Referência para this (para encadeamento)
     */
    TableMetadata& AddIndex(const IndexDef& index);
    
    /**
     * Define a chave primária
     * @param fieldName Nome do campo de chave primária
     * @return Referência para this (para encadeamento)
     */
    TableMetadata& SetPrimaryKey(const std::string& fieldName);
    
    /**
     * Define um comentário para a tabela
     * @param comment Comentário
     * @return Referência para this (para encadeamento)
     */
    TableMetadata& SetComment(const std::string& comment);
    
    /**
     * Obtém a definição da tabela
     * @return Definição da tabela
     */
    TableDef GetTableDef() const;
    
    /**
     * Obtém o nome da tabela
     * @return Nome da tabela
     */
    const std::string& GetName() const;
    
    /**
     * Verifica se um campo existe
     * @param fieldName Nome do campo
     * @return true se o campo existir
     */
    bool HasField(const std::string& fieldName) const;
    
    /**
     * Obtém a definição de um campo
     * @param fieldName Nome do campo
     * @return Definição do campo ou nullptr se não existir
     */
    const FieldDef* GetField(const std::string& fieldName) const;
    
    /**
     * Obtém a lista de campos
     * @return Lista de campos
     */
    const std::vector<FieldDef>& GetFields() const;
    
    /**
     * Obtém a lista de índices
     * @return Lista de índices
     */
    const std::vector<IndexDef>& GetIndices() const;
    
    /**
     * Obtém o nome do campo de chave primária
     * @return Nome do campo de chave primária
     */
    const std::string& GetPrimaryKey() const;
    
private:
    std::string m_name;                 // Nome da tabela
    std::vector<FieldDef> m_fields;     // Campos da tabela
    std::vector<IndexDef> m_indices;    // Índices da tabela
    std::string m_primaryKey;           // Campo de chave primária
    std::string m_comment;              // Comentário da tabela
    std::string m_engine;               // Engine
    std::string m_charset;              // Charset
    std::string m_collation;            // Collation
};

/**
 * Construtor estático para tabela
 */
class Table {
public:
    /**
     * Cria uma tabela
     * @param name Nome da tabela
     * @return Metadados da tabela
     */
    static TableMetadata Create(const std::string& name);
    
    /**
     * Cria um campo inteiro
     * @param name Nome do campo
     * @param flags Flags do campo
     * @return Definição do campo
     */
    static FieldDef Integer(const std::string& name, FieldFlag flags = FieldFlag::None);
    
    /**
     * Cria um campo inteiro grande
     * @param name Nome do campo
     * @param flags Flags do campo
     * @return Definição do campo
     */
    static FieldDef BigInteger(const std::string& name, FieldFlag flags = FieldFlag::None);
    
    /**
     * Cria um campo real
     * @param name Nome do campo
     * @param precision Precisão
     * @param scale Escala
     * @param flags Flags do campo
     * @return Definição do campo
     */
    static FieldDef Real(const std::string& name, int precision = 10, int scale = 2, FieldFlag flags = FieldFlag::None);
    
    /**
     * Cria um campo de texto
     * @param name Nome do campo
     * @param size Tamanho (0 = ilimitado)
     * @param flags Flags do campo
     * @return Definição do campo
     */
    static FieldDef Text(const std::string& name, int size = 0, FieldFlag flags = FieldFlag::None);
    
    /**
     * Cria um campo de dados binários
     * @param name Nome do campo
     * @param size Tamanho (0 = ilimitado)
     * @param flags Flags do campo
     * @return Definição do campo
     */
    static FieldDef Blob(const std::string& name, int size = 0, FieldFlag flags = FieldFlag::None);
    
    /**
     * Cria um campo booleano
     * @param name Nome do campo
     * @param flags Flags do campo
     * @return Definição do campo
     */
    static FieldDef Boolean(const std::string& name, FieldFlag flags = FieldFlag::None);
    
    /**
     * Cria um campo de timestamp
     * @param name Nome do campo
     * @param flags Flags do campo
     * @return Definição do campo
     */
    static FieldDef Timestamp(const std::string& name, FieldFlag flags = FieldFlag::None);
    
    /**
     * Cria um campo UUID
     * @param name Nome do campo
     * @param flags Flags do campo
     * @return Definição do campo
     */
    static FieldDef UUID(const std::string& name, FieldFlag flags = FieldFlag::None);
    
    /**
     * Cria um campo JSON
     * @param name Nome do campo
     * @param flags Flags do campo
     * @return Definição do campo
     */
    static FieldDef JSON(const std::string& name, FieldFlag flags = FieldFlag::None);
    
    /**
     * Cria um campo de enumeração
     * @param name Nome do campo
     * @param values Valores da enumeração
     * @param flags Flags do campo
     * @return Definição do campo
     */
    static FieldDef Enum(const std::string& name, const std::vector<std::string>& values, FieldFlag flags = FieldFlag::None);
    
    /**
     * Cria um índice
     * @param name Nome do índice
     * @param fields Campos indexados
     * @param unique Índice único
     * @return Definição do índice
     */
    static IndexDef Index(const std::string& name, const std::vector<std::string>& fields, bool unique = false);
};

/**
 * Registro de esquema de banco de dados
 */
class SchemaRegistry {
public:
    /**
     * Obtém a instância singleton
     * @return Referência para o registro
     */
    static SchemaRegistry& GetInstance();
    
    /**
     * Registra uma tabela
     * @param table Definição da tabela
     */
    void RegisterTable(const TableDef& table);
    
    /**
     * Verifica se uma tabela está registrada
     * @param tableName Nome da tabela
     * @return true se a tabela estiver registrada
     */
    bool HasTable(const std::string& tableName) const;
    
    /**
     * Obtém a definição de uma tabela
     * @param tableName Nome da tabela
     * @return Definição da tabela ou nullptr se não existir
     */
    const TableDef* GetTable(const std::string& tableName) const;
    
    /**
     * Obtém todas as tabelas registradas
     * @return Mapa de definições de tabela
     */
    const std::unordered_map<std::string, TableDef>& GetTables() const;
    
    /**
     * Limpa o registro
     */
    void Clear();
    
private:
    // Construtor e destrutor privados (singleton)
    SchemaRegistry();
    ~SchemaRegistry();
    
    // Não permitir cópias
    SchemaRegistry(const SchemaRegistry&) = delete;
    SchemaRegistry& operator=(const SchemaRegistry&) = delete;
    
    // Mapa de tabelas por nome
    std::unordered_map<std::string, TableDef> m_tables;
    
    // Mutex para thread-safety
    mutable std::mutex m_mutex;
};

/**
 * Declarações de esquema para as tabelas principais
 */
class Schema {
public:
    /**
     * Inicializa o esquema global
     */
    static void Initialize();
    
    /**
     * Define a tabela de contas
     * @return Metadados da tabela
     */
    static TableMetadata DefineAccountTable();
    
    /**
     * Define a tabela de personagens
     * @return Metadados da tabela
     */
    static TableMetadata DefineCharacterTable();
    
    /**
     * Define a tabela de itens
     * @return Metadados da tabela
     */
    static TableMetadata DefineItemTable();
    
    /**
     * Define a tabela de inventário
     * @return Metadados da tabela
     */
    static TableMetadata DefineInventoryTable();
    
    /**
     * Define a tabela de skills
     * @return Metadados da tabela
     */
    static TableMetadata DefineSkillTable();
    
    /**
     * Define a tabela de guildas
     * @return Metadados da tabela
     */
    static TableMetadata DefineGuildTable();
    
    /**
     * Define a tabela de membros de guildas
     * @return Metadados da tabela
     */
    static TableMetadata DefineGuildMemberTable();
    
    /**
     * Define a tabela de quests
     * @return Metadados da tabela
     */
    static TableMetadata DefineQuestTable();
    
    /**
     * Define a tabela de progresso de quests
     * @return Metadados da tabela
     */
    static TableMetadata DefineQuestProgressTable();
    
private:
    // Impedir instanciação
    Schema() = delete;
};

} // namespace Storage
} // namespace WYDBR

#endif // WYDBR_DATABASE_SCHEMA_H
