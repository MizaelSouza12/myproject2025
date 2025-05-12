/**
 * MobTemplate.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/world/generator/MobTemplate.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef MOBTEMPLATE_H
#define MOBTEMPLATE_H

#include <vector>
#include <map>
#include <string>
#include <mutex>
#include <memory>

#include "../../../../core/WYDTypes.h"
#include "../WorldTypes.h"

namespace wydbr {


/**
 * @file MobTemplate.h
 * @brief Gerenciador de templates de mob
 * 
 * Este arquivo contém a definição do gerenciador de templates de mob,
 * que carrega e gerencia os dados base para diferentes tipos de mobs.
 * No WYD original, isso era feito de forma rudimentar, com dados
 * estáticos. Esta implementação oferece um sistema mais flexível.
 */

namespace wyd {
namespace server {
namespace world {
namespace generator {

/**
 * @brief Gerenciador de templates de mob
 * 
 * Esta classe gerencia os templates de mob, incluindo carregamento,
 * busca, e manipulação.
 */
/**
 * Classe WYDMobTemplateManager
 * 
 * Esta classe implementa a funcionalidade WYDMobTemplateManager conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class CMobTemplateManager {
public:
    /**
     * @brief Obtém a instância singleton
     * @return Referência para a instância
     */
    static CMobTemplateManager& GetInstance();
    
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
     * @brief Carrega templates de mob
     * @param filePath Caminho do arquivo
     * @return true se carregado com sucesso
     */
    bool LoadTemplates(const std::string& filePath);
    
    /**
     * @brief Salva templates de mob
     * @param filePath Caminho do arquivo
     * @return true se salvo com sucesso
     */
    bool SaveTemplates(const std::string& filePath);
    
    /**
     * @brief Carrega tabelas de drop
     * @param filePath Caminho do arquivo
     * @return true se carregado com sucesso
     */
    bool LoadDropTables(const std::string& filePath);
    
    /**
     * @brief Salva tabelas de drop
     * @param filePath Caminho do arquivo
     * @return true se salvo com sucesso
     */
    bool SaveDropTables(const std::string& filePath);
    
    /**
     * @brief Adiciona um template de mob
     * @param templ Template de mob
     * @return true se adicionado com sucesso
     */
    bool AddMobTemplate(const MobTemplate& templ);
    
    /**
     * @brief Remove um template de mob
     * @param mobID ID do mob
     * @return true se removido com sucesso
     */
    bool RemoveMobTemplate(WORD mobID);
    
    /**
     * @brief Obtém um template de mob
     * @param mobID ID do mob
     * @return Template de mob, ou nullptr se não existir
     */
    const MobTemplate* GetMobTemplate(WORD mobID) const;
    
    /**
     * @brief Obtém todos os templates de mob
     * @return Lista de templates
     */
    std::vector<const MobTemplate*> GetAllMobTemplates() const;
    
    /**
     * @brief Adiciona uma tabela de drop
     * @param table Tabela de drop
     * @return true se adicionado com sucesso
     */
    bool AddDropTable(const DropTable& table);
    
    /**
     * @brief Remove uma tabela de drop
     * @param tableID ID da tabela
     * @return true se removido com sucesso
     */
    bool RemoveDropTable(WORD tableID);
    
    /**
     * @brief Obtém uma tabela de drop
     * @param tableID ID da tabela
     * @return Tabela de drop, ou nullptr se não existir
     */
    const DropTable* GetDropTable(WORD tableID) const;
    
    /**
     * @brief Obtém todas as tabelas de drop
     * @return Lista de tabelas
     */
    std::vector<const DropTable*> GetAllDropTables() const;
    
    /**
     * @brief Adiciona um item a uma tabela de drop
     * @param tableID ID da tabela
     * @param drop Configuração de drop
     * @return true se adicionado com sucesso
     */
    bool AddDropToTable(WORD tableID, const DropConfig& drop);
    
    /**
     * @brief Remove um item de uma tabela de drop
     * @param tableID ID da tabela
     * @param dropID ID do drop
     * @return true se removido com sucesso
     */
    bool RemoveDropFromTable(WORD tableID, WORD dropID);
    
    /**
     * @brief Obtém templates de mob por nível
     * @param minLevel Nível mínimo
     * @param maxLevel Nível máximo
     * @return Lista de templates
     */
    std::vector<const MobTemplate*> GetMobTemplatesByLevel(BYTE minLevel, BYTE maxLevel) const;
    
    /**
     * @brief Obtém templates de mob por raça
     * @param race Raça
     * @return Lista de templates
     */
    std::vector<const MobTemplate*> GetMobTemplatesByRace(MobRace race) const;
    
    /**
     * @brief Obtém templates de mob por classe
     * @param mobClass Classe
     * @return Lista de templates
     */
    std::vector<const MobTemplate*> GetMobTemplatesByClass(MobClass mobClass) const;
    
    /**
     * @brief Obtém o próximo ID de template disponível
     * @return ID de template
     */
    WORD GetNextMobTemplateID() const;
    
    /**
     * @brief Obtém o próximo ID de tabela de drop disponível
     * @return ID de tabela de drop
     */
    WORD GetNextDropTableID() const;
    
    /**
     * @brief Obtém o próximo ID de item de drop disponível
     * @param tableID ID da tabela
     * @return ID de item de drop
     */
    WORD GetNextDropID(WORD tableID) const;
    
    /**
     * @brief Obter templates de mob por nome
     * @param namePart Parte do nome
     * @return Lista de templates
     */
    std::vector<const MobTemplate*> SearchMobTemplatesByName(const std::string& namePart) const;
    
private:
    /**
     * @brief Construtor privado (singleton)
     */
    CMobTemplateManager();
    
    /**
     * @brief Destrutor
     */
    ~CMobTemplateManager();
    
    /**
     * @brief Desabilita o construtor de cópia
     */
    CMobTemplateManager(const CMobTemplateManager&) = delete;
    
    /**
     * @brief Desabilita o operador de atribuição
     */
    CMobTemplateManager& operator=(const CMobTemplateManager&) = delete;
    
    /**
     * @brief Inicializa templates padrão
     */
    void InitializeDefaultTemplates();
    
    /**
     * @brief Inicializa tabelas de drop padrão
     */
    void InitializeDefaultDropTables();
    
    // Templates de mob
    std::map<WORD, MobTemplate> mobTemplates_;
    mutable std::mutex templatesMutex_;
    
    // Tabelas de drop
    std::map<WORD, DropTable> dropTables_;
    mutable std::mutex tablesMutex_;
    
    // Estado
    bool initialized_;
};

// Acesso global
#define g_MobTemplateManager CMobTemplateManager::GetInstance()

} // namespace generator
} // namespace world
} // namespace server
} // namespace wyd

#endif // MOBTEMPLATE_H

} // namespace wydbr
