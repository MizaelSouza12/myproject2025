#pragma once

#include "../../include/GameStructs.h"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

namespace WYDStudio {

/**
 * @struct GameDatabaseEntry
 * @brief Estrutura para armazenar informações de NPCs
 */
struct NPCData {
    int id;                                        // ID do NPC
    std::string name;                              // Nome do NPC
    std::string location;                          // Localização do NPC
    std::string description;                       // Descrição do NPC
    std::vector<STRUCT_ITEM> items;                // Itens vendidos pelo NPC
    std::vector<std::string> services;             // Serviços oferecidos
};

/**
 * @struct ItemInfo
 * @brief Informações adicionais sobre um item
 */
struct ItemInfo {
    int id;                                        // ID do item
    std::string name;                              // Nome do item
    std::string description;                       // Descrição do item
    std::map<std::string, int> attributes;         // Atributos do item
};

/**
 * @class GameAnalyzer
 * @brief Classe para analisar arquivos e estruturas do jogo WYD
 * 
 * Esta classe fornece funcionalidades para analisar arquivos binários do jogo,
 * extrair informações de estruturas de dados e fornecer contexto semântico.
 */
class GameAnalyzer {
public:
    /**
     * @brief Construtor padrão
     */
    GameAnalyzer();
    
    /**
     * @brief Destrutor virtual
     */
    virtual ~GameAnalyzer();
    
    /**
     * @brief Inicializa o analisador com o caminho base do jogo
     * @param gamePath Caminho para o diretório do jogo
     * @return true se inicializado com sucesso
     */
    bool Initialize(const std::string& gamePath);
    
    /**
     * @brief Analisa um arquivo binário do jogo
     * @param filePath Caminho para o arquivo
     * @return Informações extraídas em formato de texto
     */
    std::string AnalyzeBinaryFile(const std::string& filePath);
    
    /**
     * @brief Analisa um arquivo executável do jogo
     * @param exePath Caminho para o arquivo executável
     * @return Informações extraídas do executável
     */
    std::map<std::string, std::string> AnalyzeExecutable(const std::string& exePath);
    
    /**
     * @brief Analisa um arquivo de dados específico do jogo
     * @param filePath Caminho para o arquivo
     * @param fileType Tipo de arquivo (item, mob, npc, etc.)
     * @return Informações extraídas do arquivo
     */
    std::map<std::string, std::string> AnalyzeGameDataFile(const std::string& filePath, const std::string& fileType);
    
    /**
     * @brief Busca informações sobre um NPC pelo nome
     * @param npcName Nome do NPC
     * @return Dados do NPC ou nullptr se não encontrado
     */
    std::shared_ptr<NPCData> FindNPC(const std::string& npcName);
    
    /**
     * @brief Busca informações sobre um item pelo nome
     * @param itemName Nome do item
     * @return Informações do item ou nullptr se não encontrado
     */
    std::shared_ptr<ItemInfo> FindItem(const std::string& itemName);
    
    /**
     * @brief Busca informações sobre um item pelo ID
     * @param itemId ID do item
     * @return Informações do item ou nullptr se não encontrado
     */
    std::shared_ptr<ItemInfo> FindItemById(int itemId);
    
    /**
     * @brief Obtém informações sobre colheitas e ervas
     * @return Mapa com informações sobre colheitas
     */
    std::map<std::string, std::vector<ItemInfo>> GetHerbInformation();
    
    /**
     * @brief Obtém informações sobre preços de colheitas
     * @return Mapa com informações de preços
     */
    std::map<std::string, std::vector<std::pair<std::string, int>>> GetHarvestPrices();
    
    /**
     * @brief Analisa o caminho do jogo para validar a estrutura
     * @param gamePath Caminho para verificar
     * @return true se o caminho contém uma instalação válida do WYD
     */
    bool ValidateGamePath(const std::string& gamePath);
    
    /**
     * @brief Obtém resposta para uma pergunta específica sobre o jogo
     * @param question Pergunta em linguagem natural
     * @return Resposta para a pergunta ou string vazia se não souber
     */
    std::string GetAnswerForQuestion(const std::string& question);
    
    /**
     * @brief Registra um callback para log
     * @param logCallback Função de callback para log
     */
    void SetLogCallback(std::function<void(const std::string&)> logCallback);
    
private:
    class Impl;
    std::unique_ptr<Impl> m_pImpl; // Implementação privada (Pimpl idiom)
    
    // Funções auxiliares
    std::string AnalyzeDataFile(const std::string& filePath);
    std::string AnalyzeBinFile(const std::string& filePath);
    std::string AnalyzeMobFile(const std::string& filePath);
    std::string AnalyzeItemFile(const std::string& filePath);
    std::string AnalyzeNpcFile(const std::string& filePath);
    std::string AnalyzeModelFile(const std::string& filePath);
    
    // Base de dados interna (pré-carregada)
    std::map<std::string, NPCData> m_npcDatabase;
    std::vector<ItemInfo> m_itemDatabase;
    
    // Flag de inicialização
    bool m_initialized;
    
    // Caminho base do jogo
    std::string m_gamePath;
    
    // Callback para log
    std::function<void(const std::string&)> m_logCallback;
};

} // namespace WYDStudio