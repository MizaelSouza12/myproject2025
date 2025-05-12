/**
 * QuestTemplateManager.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/quest/templates/QuestTemplateManager.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef QUESTTEMPLATEMANAGER_H
#define QUESTTEMPLATEMANAGER_H

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <memory>
#include <functional>

#include "../../../../core/WYDTypes.h"
#include "../QuestTypes.h"

namespace wydbr {


/**
 * @file QuestTemplateManager.h
 * @brief Gerenciador de templates de quests
 * 
 * Este arquivo contém a definição do gerenciador de templates de quests,
 * responsável por carregar, armazenar e gerenciar todos os templates de quests.
 */

namespace wyd {
namespace server {
namespace quest {

/**
 * @brief Gerenciador de templates de quests
 * 
 * Esta classe gerencia os templates de quests, incluindo carregamento,
 * validação, busca e manipulação.
 */
/**
 * Classe WYDQuestTemplateManager
 * 
 * Esta classe implementa a funcionalidade WYDQuestTemplateManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class QuestTemplateManager {
public:
    /**
     * @brief Construtor
     */
    QuestTemplateManager();
    
    /**
     * @brief Destrutor
     */
    ~QuestTemplateManager();
    
    /**
     * @brief Inicializa o gerenciador
     * @return true se inicializado com sucesso
     */
    bool Initialize();
    
    /**
     * @brief Finaliza o gerenciador
     */
    void Shutdown();
    
    /**
     * @brief Carrega templates de quests
     * @param filePath Caminho do arquivo
     * @return true se carregado com sucesso
     */
    bool LoadTemplates(const std::string& filePath);
    
    /**
     * @brief Salva templates de quests
     * @param filePath Caminho do arquivo
     * @return true se salvo com sucesso
     */
    bool SaveTemplates(const std::string& filePath);
    
    /**
     * @brief Adiciona um template de quest
     * @param tmpl Template de quest
     * @return true se adicionado com sucesso
     */
    bool AddTemplate(const QuestTemplate& tmpl);
    
    /**
     * @brief Remove um template de quest
     * @param questID ID da quest
     * @return true se removido com sucesso
     */
    bool RemoveTemplate(QuestID questID);
    
    /**
     * @brief Obtém um template de quest
     * @param questID ID da quest
     * @return Template de quest, ou nullptr se não encontrado
     */
    const QuestTemplate* GetTemplate(QuestID questID) const;
    
    /**
     * @brief Obtém um template de quest para edição
     * @param questID ID da quest
     * @return Template de quest, ou nullptr se não encontrado
     */
    QuestTemplate* GetTemplateForEdit(QuestID questID);
    
    /**
     * @brief Atualiza um template de quest
     * @param tmpl Template de quest
     * @return true se atualizado com sucesso
     */
    bool UpdateTemplate(const QuestTemplate& tmpl);
    
    /**
     * @brief Obtém todos os templates de quest
     * @return Lista de templates
     */
    std::vector<const QuestTemplate*> GetAllTemplates() const;
    
    /**
     * @brief Obtém o número de templates
     * @return Número de templates
     */
    size_t GetTemplateCount() const;
    
    /**
     * @brief Obtém templates de quest por tipo
     * @param type Tipo de quest
     * @return Lista de templates
     */
    std::vector<const QuestTemplate*> GetTemplatesByType(QuestType type) const;
    
    /**
     * @brief Obtém templates de quest por categoria
     * @param category Categoria de quest
     * @return Lista de templates
     */
    std::vector<const QuestTemplate*> GetTemplatesByCategory(QuestCategory category) const;
    
    /**
     * @brief Obtém templates de quest por nível
     * @param minLevel Nível mínimo
     * @param maxLevel Nível máximo
     * @return Lista de templates
     */
    std::vector<const QuestTemplate*> GetTemplatesByLevel(BYTE minLevel, BYTE maxLevel) const;
    
    /**
     * @brief Obtém templates de quest por NPC inicial
     * @param npcID ID do NPC
     * @return Lista de templates
     */
    std::vector<const QuestTemplate*> GetTemplatesByStartNPC(WORD npcID) const;
    
    /**
     * @brief Obtém templates de quest por NPC final
     * @param npcID ID do NPC
     * @return Lista de templates
     */
    std::vector<const QuestTemplate*> GetTemplatesByEndNPC(WORD npcID) const;
    
    /**
     * @brief Obtém templates de quest por mapa inicial
     * @param mapID ID do mapa
     * @return Lista de templates
     */
    std::vector<const QuestTemplate*> GetTemplatesByStartMap(WORD mapID) const;
    
    /**
     * @brief Obtém templates de quest por mapa final
     * @param mapID ID do mapa
     * @return Lista de templates
     */
    std::vector<const QuestTemplate*> GetTemplatesByEndMap(WORD mapID) const;
    
    /**
     * @brief Obtém templates de quest por item necessário
     * @param itemID ID do item
     * @return Lista de templates
     */
    std::vector<const QuestTemplate*> GetTemplatesByRequiredItem(WORD itemID) const;
    
    /**
     * @brief Obtém templates de quest por item de recompensa
     * @param itemID ID do item
     * @return Lista de templates
     */
    std::vector<const QuestTemplate*> GetTemplatesByRewardItem(WORD itemID) const;
    
    /**
     * @brief Obtém templates de quest por nome
     * @param name Nome ou parte do nome
     * @param caseSensitive Se a busca é sensível a caso
     * @return Lista de templates
     */
    std::vector<const QuestTemplate*> SearchTemplatesByName(const std::string& name, bool caseSensitive = false) const;
    
    /**
     * @brief Obtém templates de quest por descrição
     * @param description Descrição ou parte da descrição
     * @param caseSensitive Se a busca é sensível a caso
     * @return Lista de templates
     */
    std::vector<const QuestTemplate*> SearchTemplatesByDescription(const std::string& description, bool caseSensitive = false) const;
    
    /**
     * @brief Verifica se um template de quest existe
     * @param questID ID da quest
     * @return true se existir
     */
    bool TemplateExists(QuestID questID) const;
    
    /**
     * @brief Obtém o próximo ID de quest disponível
     * @return ID de quest
     */
    QuestID GetNextQuestID() const;
    
    /**
     * @brief Obtém uma definição de objetivo
     * @param questID ID da quest
     * @param objectiveID ID do objetivo
     * @return Definição de objetivo, ou nullptr se não encontrada
     */
    const QuestObjectiveDefinition* GetObjectiveDefinition(QuestID questID, WORD objectiveID) const;
    
    /**
     * @brief Obtém uma definição de recompensa
     * @param questID ID da quest
     * @param rewardID ID da recompensa
     * @return Definição de recompensa, ou nullptr se não encontrada
     */
    const QuestRewardDefinition* GetRewardDefinition(QuestID questID, WORD rewardID) const;
    
    /**
     * @brief Obtém uma definição de pré-requisito
     * @param questID ID da quest
     * @param prereqID ID do pré-requisito
     * @return Definição de pré-requisito, ou nullptr se não encontrada
     */
    const QuestPrereqDefinition* GetPrereqDefinition(QuestID questID, WORD prereqID) const;
    
    /**
     * @brief Obtém uma definição de estado
     * @param questID ID da quest
     * @param stateID ID do estado
     * @return Definição de estado, ou nullptr se não encontrada
     */
    const QuestStateDefinition* GetStateDefinition(QuestID questID, WORD stateID) const;
    
    /**
     * @brief Adiciona um objetivo a um template
     * @param questID ID da quest
     * @param objective Definição de objetivo
     * @return true se adicionado com sucesso
     */
    bool AddObjectiveToTemplate(QuestID questID, const QuestObjectiveDefinition& objective);
    
    /**
     * @brief Remove um objetivo de um template
     * @param questID ID da quest
     * @param objectiveID ID do objetivo
     * @return true se removido com sucesso
     */
    bool RemoveObjectiveFromTemplate(QuestID questID, WORD objectiveID);
    
    /**
     * @brief Adiciona uma recompensa a um template
     * @param questID ID da quest
     * @param reward Definição de recompensa
     * @return true se adicionada com sucesso
     */
    bool AddRewardToTemplate(QuestID questID, const QuestRewardDefinition& reward);
    
    /**
     * @brief Remove uma recompensa de um template
     * @param questID ID da quest
     * @param rewardID ID da recompensa
     * @return true se removida com sucesso
     */
    bool RemoveRewardFromTemplate(QuestID questID, WORD rewardID);
    
    /**
     * @brief Adiciona um pré-requisito a um template
     * @param questID ID da quest
     * @param prereq Definição de pré-requisito
     * @return true se adicionado com sucesso
     */
    bool AddPrereqToTemplate(QuestID questID, const QuestPrereqDefinition& prereq);
    
    /**
     * @brief Remove um pré-requisito de um template
     * @param questID ID da quest
     * @param prereqID ID do pré-requisito
     * @return true se removido com sucesso
     */
    bool RemovePrereqFromTemplate(QuestID questID, WORD prereqID);
    
    /**
     * @brief Adiciona um estado a um template
     * @param questID ID da quest
     * @param state Definição de estado
     * @return true se adicionado com sucesso
     */
    bool AddStateToTemplate(QuestID questID, const QuestStateDefinition& state);
    
    /**
     * @brief Remove um estado de um template
     * @param questID ID da quest
     * @param stateID ID do estado
     * @return true se removido com sucesso
     */
    bool RemoveStateFromTemplate(QuestID questID, WORD stateID);
    
    /**
     * @brief Verifica a validade de um template
     * @param tmpl Template de quest
     * @param errorReason Razão do erro (saída)
     * @return true se válido
     */
    bool ValidateTemplate(const QuestTemplate& tmpl, std::string& errorReason) const;
    
    /**
     * @brief Cria um template de quest vazio
     * @param name Nome da quest
     * @param type Tipo da quest
     * @return Template de quest
     */
    QuestTemplate CreateEmptyTemplate(const std::string& name, QuestType type);
    
    /**
     * @brief Importa templates de outro formato
     * @param filePath Caminho do arquivo
     * @param format Formato (json, xml, csv, etc.)
     * @return true se importado com sucesso
     */
    bool ImportTemplates(const std::string& filePath, const std::string& format);
    
    /**
     * @brief Exporta templates para outro formato
     * @param filePath Caminho do arquivo
     * @param format Formato (json, xml, csv, etc.)
     * @return true se exportado com sucesso
     */
    bool ExportTemplates(const std::string& filePath, const std::string& format);
    
    /**
     * @brief Importa um template
     * @param filePath Caminho do arquivo
     * @return Template importado, ou nullptr se erro
     */
    std::unique_ptr<QuestTemplate> ImportTemplate(const std::string& filePath);
    
    /**
     * @brief Exporta um template
     * @param questID ID da quest
     * @param filePath Caminho do arquivo
     * @return true se exportado com sucesso
     */
    bool ExportTemplate(QuestID questID, const std::string& filePath);
    
    /**
     * @brief Duplica um template
     * @param sourceID ID da quest fonte
     * @param newName Nome da nova quest
     * @return ID da nova quest, ou 0 se erro
     */
    QuestID DuplicateTemplate(QuestID sourceID, const std::string& newName);
    
private:
    /**
     * @brief Desabilita o construtor de cópia
     */
    QuestTemplateManager(const QuestTemplateManager&) = delete;
    
    /**
     * @brief Desabilita o operador de atribuição
     */
    QuestTemplateManager& operator=(const QuestTemplateManager&) = delete;
    
    /**
     * @brief Inicializa templates padrão
     */
    void InitializeDefaultTemplates();
    
    /**
     * @brief Atualiza os índices de busca
     * @param tmpl Template de quest
     * @param isAdd Se é uma adição (true) ou remoção (false)
     */
    void UpdateSearchIndices(const QuestTemplate& tmpl, bool isAdd);
    
    /**
     * @brief Carrega templates de arquivo JSON
     * @param filePath Caminho do arquivo
     * @return true se carregado com sucesso
     */
    bool LoadTemplatesFromJSON(const std::string& filePath);
    
    /**
     * @brief Salva templates para arquivo JSON
     * @param filePath Caminho do arquivo
     * @return true se salvo com sucesso
     */
    bool SaveTemplatesToJSON(const std::string& filePath);
    
    /**
     * @brief Carrega templates de arquivo XML
     * @param filePath Caminho do arquivo
     * @return true se carregado com sucesso
     */
    bool LoadTemplatesFromXML(const std::string& filePath);
    
    /**
     * @brief Salva templates para arquivo XML
     * @param filePath Caminho do arquivo
     * @return true se salvo com sucesso
     */
    bool SaveTemplatesToXML(const std::string& filePath);
    
    /**
     * @brief Carrega templates de arquivo binário
     * @param filePath Caminho do arquivo
     * @return true se carregado com sucesso
     */
    bool LoadTemplatesFromBinary(const std::string& filePath);
    
    /**
     * @brief Salva templates para arquivo binário
     * @param filePath Caminho do arquivo
     * @return true se salvo com sucesso
     */
    bool SaveTemplatesToBinary(const std::string& filePath);
    
    /**
     * @brief Converte um template para texto
     * @param tmpl Template de quest
     * @return Texto formatado
     */
    std::string TemplateToString(const QuestTemplate& tmpl) const;
    
    /**
     * @brief Converte texto para um template
     * @param text Texto formatado
     * @return Template de quest
     */
    std::unique_ptr<QuestTemplate> StringToTemplate(const std::string& text) const;
    
    // Templates
    std::map<QuestID, QuestTemplate> templates_;
    mutable std::mutex templatesMutex_;
    
    // Índices de busca
    std::map<QuestType, std::set<QuestID>> indicesByType_;
    std::map<QuestCategory, std::set<QuestID>> indicesByCategory_;
    std::map<BYTE, std::set<QuestID>> indicesByLevel_;
    std::map<WORD, std::set<QuestID>> indicesByStartNPC_;
    std::map<WORD, std::set<QuestID>> indicesByEndNPC_;
    std::map<WORD, std::set<QuestID>> indicesByStartMap_;
    std::map<WORD, std::set<QuestID>> indicesByEndMap_;
    std::map<WORD, std::set<QuestID>> indicesByRequiredItem_;
    std::map<WORD, std::set<QuestID>> indicesByRewardItem_;
    
    // Contador para novos IDs
    QuestID nextQuestID_;
    
    // Estado
    bool initialized_;
};

} // namespace quest
} // namespace server
} // namespace wyd

#endif // QUESTTEMPLATEMANAGER_H

} // namespace wydbr
