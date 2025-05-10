
/**
 * @file WYDStudio.h
 * @brief Interface principal do WYDStudio com TMSrv/DBSrv integrados
 */

#ifndef WYDBR_WYDSTUDIO_H
#define WYDBR_WYDSTUDIO_H

#include <memory>
#include <string>
#include <vector>

namespace WYDBR {

// Forward declarations
class TMSrv;
class DBSrv;
class AdminUI;
class ConfigManager;
class LogManager;

/**
 * Classe principal do WYDStudio
 */
class WYDStudio {
public:
    WYDStudio();
    ~WYDStudio();
    
    /**
     * Inicializa o WYDStudio e seus componentes
     * @param configPath Caminho para o arquivo de configuração
     * @return true se a inicialização foi bem-sucedida, false caso contrário
     */
    bool Initialize(const std::string& configPath = "");
    
    /**
     * Inicia o WYDStudio e seus serviços
     * @return true se o início foi bem-sucedido, false caso contrário
     */
    bool Start();
    
    /**
     * Encerra o WYDStudio e seus serviços
     */
    void Shutdown();
    
    /**
     * Executa o loop principal do WYDStudio
     */
    void Run();
    
    /**
     * Verifica se o WYDStudio está em execução
     * @return true se estiver em execução, false caso contrário
     */
    bool IsRunning() const;
    
    // Getters para componentes internos
    TMSrv* GetTMSrv() const;
    DBSrv* GetDBSrv() const;
    
private:
    // Evitar cópias
    WYDStudio(const WYDStudio&) = delete;
    WYDStudio& operator=(const WYDStudio&) = delete;
    
    // Componentes principais
    std::unique_ptr<TMSrv> m_tmSrv;
    std::unique_ptr<DBSrv> m_dbSrv;
    std::unique_ptr<AdminUI> m_adminUI;
    std::unique_ptr<ConfigManager> m_configManager;
    std::unique_ptr<LogManager> m_logManager;
    
    // Estado
    bool m_initialized;
    bool m_running;
    
    // Métodos privados
    bool InitializeServers();
    bool InitializeUI();
};

} // namespace WYDBR

#endif // WYDBR_WYDSTUDIO_H
