#pragma once

#include "GameStructs.h"
#include <string>
#include <vector>
#include <memory>

namespace WYDStudio {

/**
 * @class DatabaseConnector
 * @brief Classe para conexão direta com o banco de dados do jogo
 * 
 * Esta classe permite acesso direto ao banco de dados utilizado pelo jogo,
 * permitindo operações de leitura e escrita em todas as tabelas relevantes.
 */
class DatabaseConnector {
public:
    /**
     * @brief Construtor padrão
     */
    DatabaseConnector();
    
    /**
     * @brief Destrutor virtual
     */
    virtual ~DatabaseConnector();
    
    /**
     * @brief Conecta ao banco de dados do jogo
     * @param connectionString String de conexão com o banco de dados
     * @return true se a conexão foi estabelecida com sucesso
     */
    bool Connect(const std::string& connectionString);
    
    /**
     * @brief Desconecta do banco de dados
     */
    void Disconnect();
    
    /**
     * @brief Verifica se está conectado ao banco de dados
     * @return true se está conectado
     */
    bool IsConnected() const;
    
    /**
     * @brief Obtém todas as contas de jogadores
     * @return Vetor com todas as contas
     */
    std::vector<PlayerAccount> GetAllAccounts();
    
    /**
     * @brief Obtém dados de um jogador específico
     * @param playerId ID do jogador
     * @return Dados do jogador
     */
    PlayerData GetPlayerData(int playerId);
    
    /**
     * @brief Atualiza o inventário de um jogador
     * @param playerId ID do jogador
     * @param inventory Novo inventário
     * @return true se a atualização foi bem-sucedida
     */
    bool UpdatePlayerInventory(int playerId, const std::vector<STRUCT_ITEM>& inventory);
    
    /**
     * @brief Cria um backup do banco de dados
     * @param backupPath Caminho para o arquivo de backup
     * @return true se o backup foi criado com sucesso
     */
    bool BackupDatabase(const std::string& backupPath);
    
    /**
     * @brief Restaura o banco de dados a partir de um backup
     * @param backupPath Caminho para o arquivo de backup
     * @return true se a restauração foi bem-sucedida
     */
    bool RestoreFromBackup(const std::string& backupPath);
    
    /**
     * @brief Executa uma consulta SQL personalizada
     * @param sqlQuery Consulta SQL
     * @return true se a consulta foi executada com sucesso
     */
    bool ExecuteCustomQuery(const std::string& sqlQuery);
    
    /**
     * @brief Obtém o resultado de uma consulta SQL como texto
     * @param sqlQuery Consulta SQL
     * @return Resultado da consulta em formato de texto
     */
    std::string GetQueryResultAsText(const std::string& sqlQuery);
    
    /**
     * @brief Adiciona uma nova conta de jogador
     * @param account Dados da conta
     * @return true se a conta foi adicionada com sucesso
     */
    bool AddPlayerAccount(const PlayerAccount& account);
    
    /**
     * @brief Atualiza uma conta existente
     * @param accountId ID da conta
     * @param account Novos dados da conta
     * @return true se a atualização foi bem-sucedida
     */
    bool UpdatePlayerAccount(int accountId, const PlayerAccount& account);
    
    /**
     * @brief Define o status de ban de uma conta
     * @param accountId ID da conta
     * @param isBanned true para banir, false para desbanir
     * @param reason Motivo do ban (opcional)
     * @return true se a operação foi bem-sucedida
     */
    bool SetAccountBanStatus(int accountId, bool isBanned, const std::string& reason = "");
    
private:
    class Impl;
    std::unique_ptr<Impl> m_pImpl; // Implementação privada (Pimpl idiom)
};

} // namespace WYDStudio