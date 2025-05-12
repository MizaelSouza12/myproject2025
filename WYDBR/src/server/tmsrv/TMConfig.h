/**
 * TMConfig.h
 * 
 * Este arquivo faz parte do projeto WYDBR, uma recriação fiel do WYD (With Your Destiny)
 * com melhorias modernas de código e arquitetura.
 * 
 * @file src/server/tmsrv/TMConfig.h
 * @author WYDBR Team
 * @date 2025-05-07
 */

#ifndef TMCONFIG_H
#define TMCONFIG_H

#include <string>
#include "../../core/WYDTypes.h"
#include "../common/CommonConfig.h"

namespace wydbr {


/**
 * @file TMConfig.h
 * @brief Configurações do TM Server (Trade & Merchant Server)
 * 
 * Este arquivo contém definições de configurações para o TMSrv (servidor principal do WYD)
 * seguindo a estrutura original do WYD, mas com extensões e melhorias para facilitar
 * a configuração e manutenção.
 */

namespace wyd {
namespace server {

/**
 * @brief Modos de execução do servidor
 */
enum SERVER_MODE : BYTE {
    SERVER_MODE_NORMAL = 0,       // Modo normal (produção)
    SERVER_MODE_TEST = 1,         // Modo de teste (staging)
    SERVER_MODE_DEBUG = 2,        // Modo de debug (desenvolvimento)
    SERVER_MODE_MAINTENANCE = 3,  // Modo de manutenção (somente admin/GM)
    SERVER_MODE_READONLY = 4      // Modo somente leitura (sem DB write)
};

/**
 * @brief Estrutura de configuração do TMSrv (baseado no original do WYD)
 */
struct STRUCT_TMCONFIG {
    // Parâmetros de conexão
    char   szDBServerIP[16];        // Endereço IP do DB Server (original: g_pDBSrvIP)
    WORD   wDBServerPort;           // Porta do DB Server (original: g_wDBSrvPort)
    char   szClientVersion[12];     // Versão do cliente aceita (original: g_szClientVersion)
    
    // Configurações de servidor
    WORD   wTMServerPort;           // Porta do TMSrv (original: g_wTMSrvPort)
    BYTE   byServerNumber;          // Número do servidor (original: g_byServerNumber)
    BYTE   byServerMode;            // Modo de execução (original: g_byServerMode)
    char   szServerName[32];        // Nome do servidor (original: g_szServerName)
    DWORD  dwUserAcceptTimeout;     // Timeout para aceitar conexões em ms
    DWORD  dwUserDisconnectTimeout; // Timeout para desconectar usuários inativos em ms
    DWORD  dwMaxUsers;              // Número máximo de usuários (original: g_dwMaxServerUsers)
    WORD   wLimitIP;                // Limite de conexões por IP (obsoleto, mas mantido por compatibilidade)
    
    // Configurações de mundo
    char   szMapRoute[128];         // Diretório de mapas (original: g_szMapRoute)
    char   szDataRoute[128];        // Diretório de dados (original: g_szDataRoute)
    float  fExpRate;                // Taxa de experiência (original: g_fExpRate)
    float  fDropRate;               // Taxa de drop (original: g_fDropRate)
    float  fGoldRate;               // Taxa de gold (original: g_fGoldRate)
    BYTE   byRvRMode;               // Modo RvR ativo (original: g_byRvRMode, Realm vs Realm)
    BYTE   byAmityMode;             // Modo Amizade (original: g_byAmityMode)
    DWORD  dwInitItem;              // Item inicial para novos personagens (original: g_dwInitItem)
    
    // Extensões de configuração (não existiam no WYD original)
    bool   bUseSQL;                 // Usar SQL em vez de servidor DB próprio
    bool   bUseCrypto;              // Usar criptografia para comunicação com o cliente
    WORD   wEncryptionVersion;      // Versão da criptografia
    bool   bUseNPCQuests;           // Usar sistema de quests via NPC
    bool   bUseEventSystem;         // Sistema de eventos globais
    BYTE   byMaxCharactersPerAccount; // Máximo de personagens por conta
    bool   bUseInGameStore;         // Usar loja in-game (premium ou não)
    
    // Limites e taxas
    WORD   wMaxLevel;               // Nível máximo de personagem
    DWORD  dwMaxGold;               // Máximo de gold que um personagem pode ter
    float  fStaminarRegenRate;      // Taxa de regeneração de stamina
    bool   bEnableTrading;          // Permitir comércio entre jogadores
    
    // Servidor Guild
    char   szGuildServerIP[16];     // IP do servidor de guild (GS)
    WORD   wGuildServerPort;        // Porta do servidor de guild
    
    // Configurações avançadas
    bool   bEnableAI;               // Habilitar IA para NPCs
    BYTE   byAILevel;               // Nível de complexidade da IA (0-10)
    bool   bEnableAdvancedLogging;  // Habilitar logs avanç/**
 * STRUCT_TMCONFIG
 * 
 * Implementa a funcionalidade STRUCT_TMCONFIG conforme especificação original.
 * @return Retorna ados
 */
ados
    
    STRUCT_TMCONFIG(){
        // Valores padrão /**
 * strcpy
 * 
 * Implementa a funcionalidade strcpy conforme especificação original.
 * @param szDBServerIP Parâmetro szDBServerIP
 * @param "127.0.0.1" Parâmetro "127.0.0.1"
 * @return Retorna otimizados
 */
 otimizados
        strncpy(szDBServerIP, "127.0.0.1");
        wDBServerPort = 7514;
        strncpy(szClientVersion, "1.414");
        
        wTMServerPort = 7515;
        byServerNumber = 1;
        byServerMode = SERVER_MODE_NORMAL;
        strncpy(szServerName, "WYDBRASIL");
        dwUserAcceptTimeout = 30000;
        dwUserDisconnectTimeout = 300000;
        dwMaxUsers = 1000;
        wLimitIP = 5;
        
        strncpy(szMapRoute, "./Maps/");
        strncpy(szDataRoute, "./Data/");
        fExpRate = 1.0f;
        fDropRate = 1.0f;
        fGoldRate = 1.0f;
        byRvRMode = 0;
        byAmityMode = 0;
        dwInitItem = 0;
        
        bUseSQL = true;
        bUseCrypto = true;
        wEncryptionVersion = 1;
        bUseNPCQuests = true;
        bUseEventSystem = true;
        byMaxCharactersPerAccount = 4;
        bUseInGameStore = true;
        
        wMaxLevel = 400;
        dwMaxGold = 2000000000;
        fStaminarRegenRate = 1.0f;
        bEnableTrading = true;
        
        strncpy(szGuildServerIP, "127.0.0.1");
        wGuildServerPort = 7516;
        
        bEnableAI = true;
        byAILevel = 5;
        bEnableAdvancedLogging = false;
    }
    
    /**
     * @brief Carrega configuração de um arquivo
     * @param fileName Nome do arquivo
     * @return true se carregou com sucesso, false caso contrário
     */
    bool LoadFromFile(const char* fileName);
    
    /**
     * @brief Salva configuração em um arquivo
     * @param fileName Nome do arquivo
     * @return true se salvou com sucesso, false caso contrário
     */
    bool SaveToFile(const char* fileName);
    
    /**
     * @brief Obtém o modo do servidor em formato de string
     * @return String representando o modo do servidor
     */
    const char* GetServerModeString() const {
        switch (byServerMode) {
            case SERVER_MODE_NORMAL:
                return "Normal";
            case SERVER_MODE_TEST:
                return "Test";
            case SERVER_MODE_DEBUG:
                return "Debug";
            case SERVER_MODE_MAINTENANCE:
                return "Maintenance";
            case SERVER_MODE_READONLY:
                return "ReadOnly";
            default:
                return "Unknown";
        }
    }
    
    /**
     * @brief Verifica se o servidor está no modo de manutenção
     * @return true se estiver em manutenção, false caso contrário
     */
    bool IsMaintenanceMode() const {
        return byServerMode == SERVER_MODE_MAINTENANCE;
    }
    
    /**
     * @brief Verifica se o cliente é compatível
     * @param clientVersion Versão do cliente
     * @return true se for compatível, false caso contrário
     */
    bool IsClientCompatible(const char* clientVersion) const {
        return strcmp(clientVersion, szClientVersion) == 0;
    }
};

// Singleton para acesso global à configuração
/**
 * Classe TMConfig
 * 
 * Esta classe implementa a funcionalidade TMConfig conforme a especificação
 * original do WYD, mantendo compatibilidade binária completa.
 */
class CTMConfig {
public:
    /**
     * @brief Obtém a instância do singleton
     * @return Referência para a instância
     */
    static CTMConfig& GetInstance() {
        static CTMConfig instance;
        return instance;
    }
    
    /**
     * @brief Carrega a configuração de um arquivo
     * @param fileName Nome do arquivo
     * @return true se carregou com sucesso, false caso contrário
     */
    bool Load(const char* fileName = "TMServer.conf") {
        return config_.LoadFromFile(fileName);
    }
    
    /**
     * @brief Salva a configuração em um arquivo
     * @param fileName Nome do arquivo
     * @return true se salvou com sucesso, false caso contrário
     */
    bool Save(const char* fileName = "TMServer.conf") {
        return config_.SaveToFile(fileName);
    }
    
    /**
     * @brief Obtém a estrutura de configuração
     * @return Referência para a estrutura de configuração
     */
    STRUCT_TMCONFIG& Get() {
        return config_;
    }
    
    /**
     * @brief Obtém a estrutura de configuração (const)
     * @return Referência const para a estrutura de configuração
     */
    const STRUCT_TMCONFIG& Get() const {
        return config_;
    }
    
private:
    /**
     * @brief Construtor privado (singleton)
     */
    CTMConfig() {}
    
    /**
     * @brief Desabilita o construtor de cópia
     */
    CTMConfig(const CTMConfig&) = delete;
    
    /**
     * @brief Desabilita o operador de atribuição
     */
    CTMConfig& operator=(const CTMConfig&) = delete;
    
    STRUCT_TMCONFIG config_;
};

// Definição global para facilitar o acesso
#define g_Config CTMConfig::GetInstance().Get()

} // namespace server
} // namespace wyd

#endif // TMCONFIG_H

} // namespace wydbr
