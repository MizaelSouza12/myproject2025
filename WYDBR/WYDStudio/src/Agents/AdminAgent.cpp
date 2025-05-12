#include "AdminAgent.h"
#include "../AI/NLPProcessor.h"
#include <iostream>
#include <regex>
#include <algorithm>

namespace WYDStudio {

/**
 * @brief Classe de implementação privada para AdminAgent
 */
class AdminAgent::Impl {
public:
    Impl(std::shared_ptr<ServerConnector> serverConnector, 
         std::shared_ptr<DatabaseConnector> dbConnector,
         std::shared_ptr<FileSystemConnector> fsConnector) 
        : m_serverConnector(serverConnector)
        , m_dbConnector(dbConnector)
        , m_fsConnector(fsConnector)
        , m_nlpProcessor(std::make_unique<NLPProcessor>())
        , m_initialized(false) {
    }
    
    ~Impl() = default;
    
    // Referências para conectores
    std::shared_ptr<ServerConnector> m_serverConnector;
    std::shared_ptr<DatabaseConnector> m_dbConnector;
    std::shared_ptr<FileSystemConnector> m_fsConnector;
    
    // Processador de linguagem natural
    std::unique_ptr<NLPProcessor> m_nlpProcessor;
    
    // Status de inicialização
    bool m_initialized;
    
    // Função de log
    std::function<void(const std::string&)> m_logCallback;
    
    /**
     * @brief Logs a message if a log callback is set
     * @param message The message to log
     */
    void Log(const std::string& message) {
        if (m_logCallback) {
            m_logCallback(message);
        }
    }
    
    /**
     * @brief Extrai informações de item e preço de uma consulta
     * @param query Consulta em linguagem natural
     * @return Mapa de entidades extraídas
     */
    std::map<std::string, std::string> ExtractItemAndPrice(const std::string& query) {
        std::map<std::string, std::string> entities;
        
        // Converter para minúsculas para comparação
        std::string lowerQuery = query;
        std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(),
                      [](unsigned char c) { return std::tolower(c); });
        
        // Lista de itens comuns para detecção simples
        std::vector<std::string> commonItems = {
            "espada de aço", "poção de vida", "armadura de couro", 
            "gema mágica", "pergaminho de teleporte"
        };
        
        // Detectar nome do item
        for (const auto& item : commonItems) {
            if (lowerQuery.find(item) != std::string::npos) {
                entities["itemName"] = item;
                break;
            }
        }
        
        // Detectar preço com regex
        std::regex priceRegex("\\d+");
        std::smatch priceMatch;
        if (std::regex_search(lowerQuery, priceMatch, priceRegex)) {
            entities["newPrice"] = priceMatch[0];
        }
        
        return entities;
    }
    
    /**
     * @brief Extrai informações de taxa de guild de uma consulta
     * @param query Consulta em linguagem natural
     * @return Mapa de entidades extraídas
     */
    std::map<std::string, std::string> ExtractTaxRate(const std::string& query) {
        std::map<std::string, std::string> entities;
        
        // Regex para detectar percentual (10%, 10 por cento, 10 porcento)
        std::regex percentRegex("(\\d+)%|(\\d+)\\s+por\\s+cento|(\\d+)\\s+porcento");
        std::smatch percentMatch;
        
        if (std::regex_search(query, percentMatch, percentRegex)) {
            // Extrair apenas o número
            std::regex numericRegex("\\d+");
            std::string matchedStr = percentMatch[0];
            std::smatch numericMatch;
            
            if (std::regex_search(matchedStr, numericMatch, numericRegex)) {
                entities["taxRate"] = numericMatch[0];
            }
        }
        
        return entities;
    }
    
    /**
     * @brief Analisa a intenção de uma consulta
     * @param query Consulta em linguagem natural
     * @return Dados da intenção detectada
     */
    CommandIntent AnalyzeIntent(const std::string& query) {
        // Converter para minúsculas para comparação
        std::string lowerQuery = query;
        std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(),
                      [](unsigned char c) { return std::tolower(c); });
        
        CommandIntent intentData;
        
        // Detectar intenções relacionadas à economia
        if (lowerQuery.find("alterar preço") != std::string::npos || 
            lowerQuery.find("mudar preço") != std::string::npos || 
            lowerQuery.find("ajustar preço") != std::string::npos) {
            
            intentData.intent = "adjust_item_price";
            intentData.confidence = 0.95f;
            intentData.entities = ExtractItemAndPrice(query);
            return intentData;
        }
        
        // Detectar intenções relacionadas às configurações de guild
        if (lowerQuery.find("taxa de guild") != std::string::npos || 
            lowerQuery.find("imposto de guild") != std::string::npos) {
            
            intentData.intent = "adjust_guild_tax";
            intentData.confidence = 0.9f;
            intentData.entities = ExtractTaxRate(query);
            return intentData;
        }
        
        // Detectar intenções relacionadas a recarregar configurações
        if (lowerQuery.find("recarregar") != std::string::npos || 
            lowerQuery.find("reload") != std::string::npos) {
            
            if (lowerQuery.find("item") != std::string::npos || 
                lowerQuery.find("itens") != std::string::npos) {
                
                intentData.intent = "reload_items";
                intentData.confidence = 0.95f;
                return intentData;
            }
            
            if (lowerQuery.find("guild") != std::string::npos || 
                lowerQuery.find("guilda") != std::string::npos) {
                
                intentData.intent = "reload_guild_config";
                intentData.confidence = 0.95f;
                return intentData;
            }
        }
        
        // Se não for detectada uma intenção específica
        intentData.intent = "unknown";
        intentData.confidence = 0.3f;
        return intentData;
    }
};

// Implementação da classe AdminAgent

AdminAgent::AdminAgent(std::shared_ptr<ServerConnector> serverConnector,
                     std::shared_ptr<DatabaseConnector> dbConnector,
                     std::shared_ptr<FileSystemConnector> fsConnector)
    : m_pImpl(std::make_unique<Impl>(serverConnector, dbConnector, fsConnector))
    , m_serverConnector(serverConnector) {
}

AdminAgent::~AdminAgent() = default;

bool AdminAgent::Initialize(const std::string& modelsPath) {
    try {
        if (m_pImpl->m_initialized) {
            return true; // Já inicializado
        }
        
        m_pImpl->Log("Inicializando AdminAgent...");
        
        // Verificar se os conectores são válidos
        if (!m_pImpl->m_serverConnector || !m_pImpl->m_dbConnector || !m_pImpl->m_fsConnector) {
            m_pImpl->Log("Erro: Conectores inválidos fornecidos ao AdminAgent");
            return false;
        }
        
        // Inicializar o processador de linguagem natural
        if (!m_pImpl->m_nlpProcessor->Initialize(modelsPath)) {
            m_pImpl->Log("Erro: Falha ao inicializar processador NLP");
            return false;
        }
        
        m_pImpl->m_initialized = true;
        m_pImpl->Log("AdminAgent inicializado com sucesso");
        return true;
    }
    catch (const std::exception& e) {
        m_pImpl->Log("Erro ao inicializar AdminAgent: " + std::string(e.what()));
        return false;
    }
}

std::string AdminAgent::ProcessQuery(const std::string& query) {
    if (!m_pImpl->m_initialized) {
        // Tentar inicializar automaticamente
        if (!Initialize("models")) {
            return "Erro: AdminAgent não está inicializado.";
        }
    }
    
    m_pImpl->Log("AdminAgent processando consulta: " + query);
    
    try {
        // Analisar a intenção da consulta
        CommandIntent intentData = m_pImpl->AnalyzeIntent(query);
        
        // Executar a ação correspondente
        const auto result = ExecuteAction(intentData.intent, intentData.parameters);
        
        // Se a ação foi bem-sucedida, retornar uma mensagem de sucesso
        if (result) {
            return "Ação executada com sucesso: " + intentData.intent;
        }
        else {
            return "Falha ao executar ação: " + intentData.intent;
        }
    }
    catch (const std::exception& e) {
        m_pImpl->Log("Erro ao processar consulta: " + std::string(e.what()));
        return "Erro ao processar sua consulta: " + std::string(e.what());
    }
}

bool AdminAgent::ExecuteAction(const std::string& intent, const std::vector<std::string>& params) {
    if (!m_pImpl->m_initialized) {
        m_pImpl->Log("Erro: AdminAgent não está inicializado");
        return false;
    }
    
    try {
        if (intent == "adjust_item_price") {
            // Extrair parâmetros
            if (params.size() < 2) {
                m_pImpl->Log("Erro: Parâmetros insuficientes para adjust_item_price");
                return false;
            }
            
            const std::string& itemName = params[0];
            int newPrice = std::stoi(params[1]);
            
            return AdjustItemPrice(itemName, newPrice);
        }
        else if (intent == "adjust_guild_tax") {
            // Extrair parâmetros
            if (params.size() < 1) {
                m_pImpl->Log("Erro: Parâmetros insuficientes para adjust_guild_tax");
                return false;
            }
            
            int taxRate = std::stoi(params[0]);
            
            return AdjustGuildTaxRate(taxRate);
        }
        else if (intent == "reload_items") {
            return ReloadItems();
        }
        else if (intent == "reload_guild_config") {
            return ReloadGuildConfig();
        }
        else {
            m_pImpl->Log("Aviso: Intenção desconhecida: " + intent);
            return false;
        }
    }
    catch (const std::exception& e) {
        m_pImpl->Log("Erro ao executar ação: " + std::string(e.what()));
        return false;
    }
}

bool AdminAgent::CanHandleIntent(const std::string& intent) const {
    // Lista de intenções que o AdminAgent pode lidar
    static const std::vector<std::string> supportedIntents = {
        "adjust_item_price",
        "adjust_guild_tax",
        "reload_items",
        "reload_guild_config"
    };
    
    return std::find(supportedIntents.begin(), supportedIntents.end(), intent) != supportedIntents.end();
}

AgentType AdminAgent::GetAgentType() const {
    return AgentType::ADMIN;
}

bool AdminAgent::HandleEconomyRequest(const std::string& request) {
    m_pImpl->Log("Processando requisição de economia: " + request);
    
    // Analisar a intenção
    CommandIntent intentData = m_pImpl->AnalyzeIntent(request);
    
    // Se a intenção for relacionada a economia, processar
    if (intentData.intent == "adjust_item_price") {
        std::string itemName;
        int newPrice = 0;
        
        // Extrair informações de entidades
        auto it = intentData.entities.find("itemName");
        if (it != intentData.entities.end()) {
            itemName = it->second;
        }
        
        it = intentData.entities.find("newPrice");
        if (it != intentData.entities.end()) {
            newPrice = std::stoi(it->second);
        }
        
        return AdjustItemPrice(itemName, newPrice);
    }
    
    // Se chegou aqui, não foi possível processar a requisição
    m_pImpl->Log("Não foi possível processar requisição de economia");
    return false;
}

bool AdminAgent::HandleBalancingRequest(const std::string& request) {
    m_pImpl->Log("Processando requisição de balanceamento: " + request);
    
    // Implementação simplificada - na versão completa, 
    // analisar a intenção e extrair parâmetros
    
    // Simulação de alteração de balanceamento
    try {
        // Enviar comando para o servidor
        if (m_serverConnector) {
            bool result = m_serverConnector->SendAdminCommand("update_balance");
            
            if (result) {
                m_pImpl->Log("Balanceamento atualizado com sucesso");
            } else {
                m_pImpl->Log("Falha ao atualizar balanceamento");
            }
            
            return result;
        }
    }
    catch (const std::exception& e) {
        m_pImpl->Log("Erro ao processar requisição de balanceamento: " + std::string(e.what()));
    }
    
    return false;
}

bool AdminAgent::HandleServerConfigRequest(const std::string& request) {
    m_pImpl->Log("Processando requisição de configuração de servidor: " + request);
    
    // Analisar a intenção
    CommandIntent intentData = m_pImpl->AnalyzeIntent(request);
    
    // Se a intenção for relacionada a taxa de guild, processar
    if (intentData.intent == "adjust_guild_tax") {
        int taxRate = 0;
        
        // Extrair taxa
        auto it = intentData.entities.find("taxRate");
        if (it != intentData.entities.end()) {
            taxRate = std::stoi(it->second);
        }
        
        return AdjustGuildTaxRate(taxRate);
    }
    
    // Se chegou aqui, não foi possível processar a requisição
    m_pImpl->Log("Não foi possível processar requisição de configuração de servidor");
    return false;
}

bool AdminAgent::ModifyItemProperty(int itemId, const std::string& property, int value) {
    if (!m_pImpl->m_initialized) {
        m_pImpl->Log("Erro: AdminAgent não está inicializado");
        return false;
    }
    
    m_pImpl->Log("Modificando propriedade de item: ID=" + std::to_string(itemId) + 
                ", Propriedade=" + property + ", Valor=" + std::to_string(value));
    
    try {
        // Buscar o item no banco de dados
        if (!m_pImpl->m_dbConnector) {
            m_pImpl->Log("Erro: Conector de banco de dados não disponível");
            return false;
        }
        
        // Implementação simplificada - na versão completa, buscar o item e modificar
        
        // Enviar comando para o servidor
        if (m_serverConnector) {
            std::string command = "modify_item " + std::to_string(itemId) + " " + 
                                 property + " " + std::to_string(value);
            
            bool result = m_serverConnector->SendAdminCommand(command);
            
            if (result) {
                m_pImpl->Log("Propriedade de item modificada com sucesso");
            } else {
                m_pImpl->Log("Falha ao modificar propriedade de item");
            }
            
            return result;
        }
    }
    catch (const std::exception& e) {
        m_pImpl->Log("Erro ao modificar propriedade de item: " + std::string(e.what()));
    }
    
    return false;
}

bool AdminAgent::AdjustGameParameter(const std::string& parameter, double value) {
    if (!m_pImpl->m_initialized) {
        m_pImpl->Log("Erro: AdminAgent não está inicializado");
        return false;
    }
    
    m_pImpl->Log("Ajustando parâmetro de jogo: " + parameter + " = " + std::to_string(value));
    
    try {
        // Enviar comando para o servidor
        if (m_serverConnector) {
            std::string command = "set_parameter " + parameter + " " + std::to_string(value);
            
            bool result = m_serverConnector->SendAdminCommand(command);
            
            if (result) {
                m_pImpl->Log("Parâmetro de jogo ajustado com sucesso");
            } else {
                m_pImpl->Log("Falha ao ajustar parâmetro de jogo");
            }
            
            return result;
        }
    }
    catch (const std::exception& e) {
        m_pImpl->Log("Erro ao ajustar parâmetro de jogo: " + std::string(e.what()));
    }
    
    return false;
}

bool AdminAgent::ScheduleServerMaintenance(const time_t& startTime, int durationMinutes) {
    if (!m_pImpl->m_initialized) {
        m_pImpl->Log("Erro: AdminAgent não está inicializado");
        return false;
    }
    
    m_pImpl->Log("Agendando manutenção do servidor: Duração=" + std::to_string(durationMinutes) + " minutos");
    
    try {
        // Formatar timestamp para string
        std::string timeStr = std::to_string(startTime);
        
        // Enviar comando para o servidor
        if (m_serverConnector) {
            std::string command = "schedule_maintenance " + timeStr + " " + std::to_string(durationMinutes);
            
            bool result = m_serverConnector->SendAdminCommand(command);
            
            if (result) {
                m_pImpl->Log("Manutenção do servidor agendada com sucesso");
            } else {
                m_pImpl->Log("Falha ao agendar manutenção do servidor");
            }
            
            return result;
        }
    }
    catch (const std::exception& e) {
        m_pImpl->Log("Erro ao agendar manutenção do servidor: " + std::string(e.what()));
    }
    
    return false;
}

bool AdminAgent::AdjustItemPrice(const std::string& itemName, int newPrice) {
    if (!m_pImpl->m_initialized) {
        m_pImpl->Log("Erro: AdminAgent não está inicializado");
        return false;
    }
    
    if (itemName.empty() || newPrice <= 0) {
        m_pImpl->Log("Erro: Nome de item vazio ou preço inválido");
        return false;
    }
    
    m_pImpl->Log("Ajustando preço do item " + itemName + " para " + std::to_string(newPrice));
    
    try {
        // Buscar o item no banco de dados
        if (!m_pImpl->m_dbConnector) {
            m_pImpl->Log("Erro: Conector de banco de dados não disponível");
            return false;
        }
        
        // Executar consulta SQL para encontrar o item
        std::string query = "SELECT * FROM items WHERE name LIKE '%" + itemName + "%' LIMIT 1";
        std::string result = m_pImpl->m_dbConnector->GetQueryResultAsText(query);
        
        if (result.empty()) {
            m_pImpl->Log("Erro: Item não encontrado: " + itemName);
            return false;
        }
        
        // Atualizar o preço do item
        query = "UPDATE items SET price = " + std::to_string(newPrice) + 
                " WHERE name LIKE '%" + itemName + "%'";
        
        bool success = m_pImpl->m_dbConnector->ExecuteCustomQuery(query);
        
        if (success) {
            m_pImpl->Log("Preço do item " + itemName + " atualizado para " + std::to_string(newPrice));
            
            // Recarregar dados de itens no servidor
            if (m_serverConnector) {
                m_serverConnector->SendAdminCommand("reload_items");
            }
        } else {
            m_pImpl->Log("Falha ao atualizar preço do item " + itemName);
        }
        
        return success;
    }
    catch (const std::exception& e) {
        m_pImpl->Log("Erro ao ajustar preço do item: " + std::string(e.what()));
        return false;
    }
}

bool AdminAgent::AdjustGuildTaxRate(int taxRate) {
    if (!m_pImpl->m_initialized) {
        m_pImpl->Log("Erro: AdminAgent não está inicializado");
        return false;
    }
    
    if (taxRate < 0 || taxRate > 100) {
        m_pImpl->Log("Erro: Taxa de guild inválida (deve estar entre 0 e 100): " + std::to_string(taxRate));
        return false;
    }
    
    m_pImpl->Log("Ajustando taxa de guild para " + std::to_string(taxRate) + "%");
    
    try {
        if (!m_pImpl->m_fsConnector) {
            m_pImpl->Log("Erro: Conector de sistema de arquivos não disponível");
            return false;
        }
        
        // Ler a configuração atual de guilds
        std::vector<STRUCT_ITEM> unusedItems; // Placeholder, não usado nesta função
        std::string guildConfigPath = "config/guild-settings.json";
        
        if (!m_pImpl->m_fsConnector->ParseItemFile(guildConfigPath).empty()) {
            // Configuração existe, atualizá-la
            
            // Em uma implementação real, modificaríamos o arquivo JSON
            // Aqui simulamos a modificação
            
            // Enviar comando para o servidor para recarregar configuração
            if (m_serverConnector) {
                // Simulação: definir taxa via comando do servidor
                std::string command = "set_guild_tax " + std::to_string(taxRate);
                bool result = m_serverConnector->SendAdminCommand(command);
                
                if (result) {
                    m_pImpl->Log("Taxa de guild ajustada com sucesso para " + std::to_string(taxRate) + "%");
                } else {
                    m_pImpl->Log("Falha ao ajustar taxa de guild");
                }
                
                return result;
            }
        } else {
            m_pImpl->Log("Erro: Arquivo de configuração de guild não encontrado");
            return false;
        }
    }
    catch (const std::exception& e) {
        m_pImpl->Log("Erro ao ajustar taxa de guild: " + std::string(e.what()));
        return false;
    }
    
    return false;
}

bool AdminAgent::ReloadItems() {
    if (!m_pImpl->m_initialized) {
        m_pImpl->Log("Erro: AdminAgent não está inicializado");
        return false;
    }
    
    m_pImpl->Log("Recarregando dados de itens...");
    
    try {
        if (!m_serverConnector) {
            m_pImpl->Log("Erro: Conector de servidor não disponível");
            return false;
        }
        
        // Enviar comando para recarregar itens
        bool result = m_serverConnector->SendAdminCommand("reload_items");
        
        if (result) {
            m_pImpl->Log("Dados de itens recarregados com sucesso");
        } else {
            m_pImpl->Log("Falha ao recarregar dados de itens");
        }
        
        return result;
    }
    catch (const std::exception& e) {
        m_pImpl->Log("Erro ao recarregar dados de itens: " + std::string(e.what()));
        return false;
    }
}

bool AdminAgent::ReloadGuildConfig() {
    if (!m_pImpl->m_initialized) {
        m_pImpl->Log("Erro: AdminAgent não está inicializado");
        return false;
    }
    
    m_pImpl->Log("Recarregando configurações de guild...");
    
    try {
        if (!m_serverConnector) {
            m_pImpl->Log("Erro: Conector de servidor não disponível");
            return false;
        }
        
        // Enviar comando para recarregar configurações de guild
        bool result = m_serverConnector->SendAdminCommand("reload_guild_config");
        
        if (result) {
            m_pImpl->Log("Configurações de guild recarregadas com sucesso");
        } else {
            m_pImpl->Log("Falha ao recarregar configurações de guild");
        }
        
        return result;
    }
    catch (const std::exception& e) {
        m_pImpl->Log("Erro ao recarregar configurações de guild: " + std::string(e.what()));
        return false;
    }
}

} // namespace WYDStudio