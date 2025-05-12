#pragma once

#include "GameStructs.h"
#include <string>
#include <vector>
#include <memory>
#include <map>

namespace WYDStudio {

/**
 * @class FileSystemConnector
 * @brief Classe para manipulação dos arquivos do jogo
 * 
 * Esta classe fornece funcionalidades para ler, escrever e converter
 * os diversos formatos de arquivo utilizados pelo jogo WYD.
 */
class FileSystemConnector {
public:
    /**
     * @brief Construtor padrão
     */
    FileSystemConnector();
    
    /**
     * @brief Destrutor virtual
     */
    virtual ~FileSystemConnector();
    
    /**
     * @brief Define o diretório raiz do jogo
     * @param rootPath Caminho para o diretório raiz
     * @return true se o diretório é válido
     */
    bool SetGameRootPath(const std::string& rootPath);
    
    /**
     * @brief Carrega o banco de dados de itens
     * @param path Caminho para o arquivo de itens
     * @return true se carregado com sucesso
     */
    bool LoadItemDatabase(const std::string& path);
    
    /**
     * @brief Carrega o banco de dados de monstros/NPCs
     * @param path Caminho para o arquivo de monstros
     * @return true se carregado com sucesso
     */
    bool LoadMobDatabase(const std::string& path);
    
    /**
     * @brief Salva o banco de dados de itens
     * @param path Caminho para o arquivo de saída
     * @return true se salvo com sucesso
     */
    bool SaveItemDatabase(const std::string& path);
    
    /**
     * @brief Salva o banco de dados de monstros/NPCs
     * @param path Caminho para o arquivo de saída
     * @return true se salvo com sucesso
     */
    bool SaveMobDatabase(const std::string& path);
    
    /**
     * @brief Converte um arquivo binário para JSON
     * @param binaryPath Caminho para o arquivo binário
     * @param jsonPath Caminho para o arquivo JSON de saída
     * @return true se a conversão foi bem-sucedida
     */
    bool ConvertBinaryToJson(const std::string& binaryPath, const std::string& jsonPath);
    
    /**
     * @brief Converte um arquivo JSON para binário
     * @param jsonPath Caminho para o arquivo JSON
     * @param binaryPath Caminho para o arquivo binário de saída
     * @return true se a conversão foi bem-sucedida
     */
    bool ConvertJsonToBinary(const std::string& jsonPath, const std::string& binaryPath);
    
    /**
     * @brief Analisa um arquivo de itens
     * @param filePath Caminho para o arquivo
     * @return Vetor de estruturas de itens
     */
    std::vector<STRUCT_ITEM> ParseItemFile(const std::string& filePath);
    
    /**
     * @brief Analisa um arquivo de monstros/NPCs
     * @param filePath Caminho para o arquivo
     * @return Vetor de estruturas de monstros
     */
    std::vector<STRUCT_MOB> ParseMobFile(const std::string& filePath);
    
    /**
     * @brief Compila dados de itens para o formato binário
     * @param items Vetor de estruturas de itens
     * @param outputPath Caminho para o arquivo de saída
     * @return true se a compilação foi bem-sucedida
     */
    bool CompileToItemBinary(const std::vector<STRUCT_ITEM>& items, const std::string& outputPath);
    
    /**
     * @brief Compila dados de monstros para o formato binário
     * @param mobs Vetor de estruturas de monstros
     * @param outputPath Caminho para o arquivo de saída
     * @return true se a compilação foi bem-sucedida
     */
    bool CompileToMobBinary(const std::vector<STRUCT_MOB>& mobs, const std::string& outputPath);
    
    /**
     * @brief Obtém um item específico pelo ID
     * @param itemId ID do item
     * @return Estrutura do item
     */
    STRUCT_ITEM GetItemById(int itemId);
    
    /**
     * @brief Obtém um monstro específico pelo ID
     * @param mobId ID do monstro
     * @return Estrutura do monstro
     */
    STRUCT_MOB GetMobById(int mobId);
    
    /**
     * @brief Modifica um item específico
     * @param itemId ID do item
     * @param newItem Nova estrutura do item
     * @return true se a modificação foi bem-sucedida
     */
    bool ModifyItem(int itemId, const STRUCT_ITEM& newItem);
    
    /**
     * @brief Modifica um monstro específico
     * @param mobId ID do monstro
     * @param newMob Nova estrutura do monstro
     * @return true se a modificação foi bem-sucedida
     */
    bool ModifyMob(int mobId, const STRUCT_MOB& newMob);
    
    /**
     * @brief Verifica a integridade dos arquivos do jogo
     * @return Mapa com status de integridade dos arquivos
     */
    std::map<std::string, bool> CheckGameFilesIntegrity();
    
    /**
     * @brief Faz backup dos arquivos do jogo
     * @param backupPath Caminho para o diretório de backup
     * @return true se o backup foi bem-sucedido
     */
    bool BackupGameFiles(const std::string& backupPath);
    
    /**
     * @brief Restaura arquivos do jogo a partir de um backup
     * @param backupPath Caminho para o diretório de backup
     * @return true se a restauração foi bem-sucedida
     */
    bool RestoreGameFiles(const std::string& backupPath);
    
private:
    class Impl;
    std::unique_ptr<Impl> m_pImpl; // Implementação privada (Pimpl idiom)
};

} // namespace WYDStudio