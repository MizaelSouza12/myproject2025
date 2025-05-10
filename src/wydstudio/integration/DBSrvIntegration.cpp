/**
 * @file DBSrvIntegration.cpp
 * @brief Implementação da integração do DBSrv no WYDStudio
 */

#include "DBSrvIntegration.h"
#include "../../../include/Common/Utils/Logger.h"
#include <chrono>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace WYDBR {

DBSrvIntegration::DBSrvIntegration()
    : m_status(ServerStatus::OFFLINE), m_stopMonitoring(false) {
}

DBSrvIntegration::~DBSrvIntegration() {
    // Parar thread de monitoramento
    m_stopMonitoring = true;
    if (m_monitorThread.joinable()) {
        m_monitorThread.join();
    }
    
    // Parar o servidor se estiver rodando
    if (m_status != ServerStatus::OFFLINE) {
        Stop();
    }
}

bool DBSrvIntegration::Initialize() {
    // Verificar se já inicializado
    if (m_dbsrv) {
        LOG_WARNING("DBSrv já inicializado");
        return false;
    }
    
    try {
        // Criar e inicializar DBSrv
        m_dbsrv = std::make_unique<DBSrv>();
        
        // Inicializar DBSrv
        if (!m_dbsrv->Initialize()) {
            LOG_ERROR("Falha ao inicializar DBSrv");
            m_dbsrv.reset();
            return false;
        }
        
        // Iniciar thread de monitoramento
        m_stopMonitoring = false;
        m_monitorThread = std::thread(&DBSrvIntegration::MonitorThread, this);
        
        // Atualizar status
        UpdateStatus(ServerStatus::OFFLINE);
        
        LOG_INFO("DBSrv inicializado com sucesso");
        return true;
    }
    catch (const std::exception& e) {
        LOG_ERROR("Exceção ao inicializar DBSrv: {}", e.what());
        m_dbsrv.reset();
        return false;
    }
    catch (...) {
        LOG_ERROR("Exceção desconhecida ao inicializar DBSrv");
        m_dbsrv.reset();
        return false;
    }
}

bool DBSrvIntegration::Start() {
    std::lock_guard<std::mutex> lock(m_statusMutex);
    
    // Verificar se o DBSrv está inicializado
    if (!m_dbsrv) {
        LOG_ERROR("DBSrv não inicializado");
        return false;
    }
    
    // Verificar se já está rodando
    if (m_status == ServerStatus::RUNNING) {
        LOG_WARNING("DBSrv já está rodando");
        return true;
    }
    
    try {
        // Atualizar status para iniciando
        UpdateStatus(ServerStatus::STARTING);
        
        // Iniciar DBSrv
        if (!m_dbsrv->Start()) {
            LOG_ERROR("Falha ao iniciar DBSrv");
            UpdateStatus(ServerStatus::ERROR);
            return false;
        }
        
        LOG_INFO("DBSrv iniciado com sucesso");
        UpdateStatus(ServerStatus::RUNNING);
        return true;
    }
    catch (const std::exception& e) {
        LOG_ERROR("Exceção ao iniciar DBSrv: {}", e.what());
        UpdateStatus(ServerStatus::ERROR);
        return false;
    }
    catch (...) {
        LOG_ERROR("Exceção desconhecida ao iniciar DBSrv");
        UpdateStatus(ServerStatus::ERROR);
        return false;
    }
}

void DBSrvIntegration::Stop() {
    std::lock_guard<std::mutex> lock(m_statusMutex);
    
    // Verificar se o DBSrv está inicializado
    if (!m_dbsrv) {
        LOG_ERROR("DBSrv não inicializado");
        return;
    }
    
    // Verificar se está rodando
    if (m_status != ServerStatus::RUNNING) {
        LOG_WARNING("DBSrv não está rodando");
        return;
    }
    
    try {
        // Atualizar status para parando
        UpdateStatus(ServerStatus::STOPPING);
        
        // Parar DBSrv
        m_dbsrv->Shutdown();
        
        LOG_INFO("DBSrv parado com sucesso");
        UpdateStatus(ServerStatus::OFFLINE);
    }
    catch (const std::exception& e) {
        LOG_ERROR("Exceção ao parar DBSrv: {}", e.what());
        UpdateStatus(ServerStatus::ERROR);
    }
    catch (...) {
        LOG_ERROR("Exceção desconhecida ao parar DBSrv");
        UpdateStatus(ServerStatus::ERROR);
    }
}

bool DBSrvIntegration::Restart() {
    // Parar o servidor
    Stop();
    
    // Pequena pausa para garantir que tudo foi limpo
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    // Iniciar o servidor novamente
    return Start();
}

ServerStatus DBSrvIntegration::GetStatus() const {
    std::lock_guard<std::mutex> lock(m_statusMutex);
    return m_status;
}

ServerType DBSrvIntegration::GetType() const {
    return ServerType::DBSRV;
}

std::string DBSrvIntegration::GetName() const {
    return "DBSrv";
}

std::string DBSrvIntegration::GetStats() const {
    // Verificar se o DBSrv está inicializado
    if (!m_dbsrv) {
        return "{}";
    }
    
    try {
        // Obter estatísticas do DBSrv
        json stats;
        
        stats["status"] = static_cast<int>(GetStatus());
        stats["uptime"] = m_dbsrv->GetUptime();
        stats["totalQueries"] = m_dbsrv->GetTotalQueries();
        stats["queriesPerSecond"] = m_dbsrv->GetQueriesPerSecond();
        stats["connectionsCount"] = m_dbsrv->GetDatabaseConnectionsCount();
        stats["memoryUsage"] = m_dbsrv->GetMemoryUsage();
        stats["cpuUsage"] = m_dbsrv->GetCPUUsage();
        
        // Outras estatísticas relevantes...
        
        return stats.dump();
    }
    catch (const std::exception& e) {
        LOG_ERROR("Exceção ao obter estatísticas do DBSrv: {}", e.what());
        return "{}";
    }
}

std::string DBSrvIntegration::ExecuteCommand(const std::string& command) {
    // Verificar se o DBSrv está inicializado
    if (!m_dbsrv) {
        return "DBSrv não inicializado";
    }
    
    try {
        // Executar comando no DBSrv
        return m_dbsrv->ExecuteAdminCommand(command);
    }
    catch (const std::exception& e) {
        LOG_ERROR("Exceção ao executar comando no DBSrv: {}", e.what());
        return std::string("Erro: ") + e.what();
    }
}

void DBSrvIntegration::RegisterStatusCallback(std::function<void(ServerStatus)> callback) {
    std::lock_guard<std::mutex> lock(m_callbacksMutex);
    m_statusCallbacks.push_back(std::move(callback));
}

std::vector<ClientInfo> DBSrvIntegration::GetConnectedClients() const {
    // DBSrv não tem clientes conectados diretamente
    return {};
}

DBSrv* DBSrvIntegration::GetDBSrv() const {
    return m_dbsrv.get();
}

std::string DBSrvIntegration::ExecuteQuery(const std::string& query, const std::vector<std::string>& params) {
    // Verificar se o DBSrv está inicializado
    if (!m_dbsrv) {
        return "DBSrv não inicializado";
    }
    
    try {
        // Executar query no DBSrv
        auto result = m_dbsrv->ExecuteQuery(query, params);
        
        // Converter resultado para JSON
        json jsonResult = json::array();
        
        for (const auto& row : result) {
            json jsonRow;
            for (const auto& [key, value] : row) {
                jsonRow[key] = value;
            }
            jsonResult.push_back(jsonRow);
        }
        
        return jsonResult.dump();
    }
    catch (const std::exception& e) {
        LOG_ERROR("Exceção ao executar query no DBSrv: {}", e.what());
        return std::string("Erro: ") + e.what();
    }
}

void DBSrvIntegration::UpdateStatus(ServerStatus newStatus) {
    // Atualizar status
    {
        std::lock_guard<std::mutex> lock(m_statusMutex);
        if (m_status == newStatus) {
            return;
        }
        m_status = newStatus;
    }
    
    // Notificar callbacks
    std::lock_guard<std::mutex> lock(m_callbacksMutex);
    for (const auto& callback : m_statusCallbacks) {
        callback(newStatus);
    }
}

void DBSrvIntegration::MonitorThread() {
    // Thread para monitorar o status do DBSrv
    while (!m_stopMonitoring) {
        // Verificar status atual do DBSrv
        if (m_dbsrv) {
            ServerStatus currentStatus = m_status;
            ServerStatus newStatus = currentStatus;
            
            switch (currentStatus) {
                case ServerStatus::RUNNING:
                    // Verificar se o DBSrv ainda está rodando
                    if (!m_dbsrv->IsRunning()) {
                        newStatus = ServerStatus::ERROR;
                    }
                    break;
                
                case ServerStatus::ERROR:
                    // Tentar recuperar do erro
                    if (m_dbsrv->IsRunning()) {
                        newStatus = ServerStatus::RUNNING;
                    }
                    break;
                
                default:
                    // Não fazer nada para outros estados
                    break;
            }
            
            // Atualizar status se necessário
            if (newStatus != currentStatus) {
                UpdateStatus(newStatus);
            }
        }
        
        // Dormir por um curto período
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}

} // namespace WYDBR
