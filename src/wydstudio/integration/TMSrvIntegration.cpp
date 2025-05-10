/**
 * @file TMSrvIntegration.cpp
 * @brief Implementação da integração do TMSrv no WYDStudio
 */

#include "TMSrvIntegration.h"
#include "../../../include/Common/Utils/Logger.h"
#include <chrono>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace WYDBR {

TMSrvIntegration::TMSrvIntegration()
    : m_status(ServerStatus::OFFLINE), m_stopMonitoring(false) {
}

TMSrvIntegration::~TMSrvIntegration() {
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

bool TMSrvIntegration::Initialize() {
    // Verificar se já inicializado
    if (m_tmsrv) {
        LOG_WARNING("TMSrv já inicializado");
        return false;
    }
    
    try {
        // Criar e inicializar TMSrv
        m_tmsrv = std::make_unique<TMSrv>();
        
        // Inicializar TMSrv
        if (!m_tmsrv->Initialize()) {
            LOG_ERROR("Falha ao inicializar TMSrv");
            m_tmsrv.reset();
            return false;
        }
        
        // Iniciar thread de monitoramento
        m_stopMonitoring = false;
        m_monitorThread = std::thread(&TMSrvIntegration::MonitorThread, this);
        
        // Atualizar status
        UpdateStatus(ServerStatus::OFFLINE);
        
        LOG_INFO("TMSrv inicializado com sucesso");
        return true;
    }
    catch (const std::exception& e) {
        LOG_ERROR("Exceção ao inicializar TMSrv: {}", e.what());
        m_tmsrv.reset();
        return false;
    }
    catch (...) {
        LOG_ERROR("Exceção desconhecida ao inicializar TMSrv");
        m_tmsrv.reset();
        return false;
    }
}

bool TMSrvIntegration::Start() {
    std::lock_guard<std::mutex> lock(m_statusMutex);
    
    // Verificar se o TMSrv está inicializado
    if (!m_tmsrv) {
        LOG_ERROR("TMSrv não inicializado");
        return false;
    }
    
    // Verificar se já está rodando
    if (m_status == ServerStatus::RUNNING) {
        LOG_WARNING("TMSrv já está rodando");
        return true;
    }
    
    try {
        // Atualizar status para iniciando
        UpdateStatus(ServerStatus::STARTING);
        
        // Iniciar TMSrv
        if (!m_tmsrv->Start()) {
            LOG_ERROR("Falha ao iniciar TMSrv");
            UpdateStatus(ServerStatus::ERROR);
            return false;
        }
        
        LOG_INFO("TMSrv iniciado com sucesso");
        UpdateStatus(ServerStatus::RUNNING);
        return true;
    }
    catch (const std::exception& e) {
        LOG_ERROR("Exceção ao iniciar TMSrv: {}", e.what());
        UpdateStatus(ServerStatus::ERROR);
        return false;
    }
    catch (...) {
        LOG_ERROR("Exceção desconhecida ao iniciar TMSrv");
        UpdateStatus(ServerStatus::ERROR);
        return false;
    }
}

void TMSrvIntegration::Stop() {
    std::lock_guard<std::mutex> lock(m_statusMutex);
    
    // Verificar se o TMSrv está inicializado
    if (!m_tmsrv) {
        LOG_ERROR("TMSrv não inicializado");
        return;
    }
    
    // Verificar se está rodando
    if (m_status != ServerStatus::RUNNING) {
        LOG_WARNING("TMSrv não está rodando");
        return;
    }
    
    try {
        // Atualizar status para parando
        UpdateStatus(ServerStatus::STOPPING);
        
        // Parar TMSrv
        m_tmsrv->Shutdown();
        
        LOG_INFO("TMSrv parado com sucesso");
        UpdateStatus(ServerStatus::OFFLINE);
    }
    catch (const std::exception& e) {
        LOG_ERROR("Exceção ao parar TMSrv: {}", e.what());
        UpdateStatus(ServerStatus::ERROR);
    }
    catch (...) {
        LOG_ERROR("Exceção desconhecida ao parar TMSrv");
        UpdateStatus(ServerStatus::ERROR);
    }
}

bool TMSrvIntegration::Restart() {
    // Parar o servidor
    Stop();
    
    // Pequena pausa para garantir que tudo foi limpo
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    // Iniciar o servidor novamente
    return Start();
}

ServerStatus TMSrvIntegration::GetStatus() const {
    std::lock_guard<std::mutex> lock(m_statusMutex);
    return m_status;
}

ServerType TMSrvIntegration::GetType() const {
    return ServerType::TMSRV;
}

std::string TMSrvIntegration::GetName() const {
    return "TMSrv";
}

std::string TMSrvIntegration::GetStats() const {
    // Verificar se o TMSrv está inicializado
    if (!m_tmsrv) {
        return "{}";
    }
    
    try {
        // Obter estatísticas do TMSrv
        json stats;
        
        stats["status"] = static_cast<int>(GetStatus());
        stats["uptime"] = m_tmsrv->GetUptime();
        stats["connectedClients"] = m_tmsrv->GetConnectedClientCount();
        stats["memoryUsage"] = m_tmsrv->GetMemoryUsage();
        stats["cpuUsage"] = m_tmsrv->GetCPUUsage();
        
        // Outras estatísticas relevantes...
        
        return stats.dump();
    }
    catch (const std::exception& e) {
        LOG_ERROR("Exceção ao obter estatísticas do TMSrv: {}", e.what());
        return "{}";
    }
}

std::string TMSrvIntegration::ExecuteCommand(const std::string& command) {
    // Verificar se o TMSrv está inicializado
    if (!m_tmsrv) {
        return "TMSrv não inicializado";
    }
    
    try {
        // Executar comando no TMSrv
        return m_tmsrv->ExecuteAdminCommand(command);
    }
    catch (const std::exception& e) {
        LOG_ERROR("Exceção ao executar comando no TMSrv: {}", e.what());
        return std::string("Erro: ") + e.what();
    }
}

void TMSrvIntegration::RegisterStatusCallback(std::function<void(ServerStatus)> callback) {
    std::lock_guard<std::mutex> lock(m_callbacksMutex);
    m_statusCallbacks.push_back(std::move(callback));
}

std::vector<ClientInfo> TMSrvIntegration::GetConnectedClients() const {
    // Verificar se o TMSrv está inicializado
    if (!m_tmsrv) {
        return {};
    }
    
    try {
        // Obter clientes conectados do TMSrv
        const auto& originalClients = m_tmsrv->GetConnectedClients();
        std::vector<ClientInfo> clients;
        
        for (const auto& originalClient : originalClients) {
            ClientInfo client;
            client.id = originalClient.id;
            client.accountName = originalClient.accountName;
            client.characterName = originalClient.characterName;
            client.ipAddress = originalClient.ipAddress;
            client.connectTime = originalClient.connectTime;
            client.level = originalClient.level;
            client.location = originalClient.location;
            
            clients.push_back(client);
        }
        
        return clients;
    }
    catch (const std::exception& e) {
        LOG_ERROR("Exceção ao obter clientes conectados do TMSrv: {}", e.what());
        return {};
    }
}

TMSrv* TMSrvIntegration::GetTMSrv() const {
    return m_tmsrv.get();
}

void TMSrvIntegration::UpdateStatus(ServerStatus newStatus) {
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

void TMSrvIntegration::MonitorThread() {
    // Thread para monitorar o status do TMSrv
    while (!m_stopMonitoring) {
        // Verificar status atual do TMSrv
        if (m_tmsrv) {
            ServerStatus currentStatus = m_status;
            ServerStatus newStatus = currentStatus;
            
            switch (currentStatus) {
                case ServerStatus::RUNNING:
                    // Verificar se o TMSrv ainda está rodando
                    if (!m_tmsrv->IsRunning()) {
                        newStatus = ServerStatus::ERROR;
                    }
                    break;
                
                case ServerStatus::ERROR:
                    // Tentar recuperar do erro
                    if (m_tmsrv->IsRunning()) {
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
