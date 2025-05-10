/**
 * @file AdminPanel.cpp
 * @brief Implementação do painel de administração unificado para WYDStudio
 */

#include "AdminPanel.h"
#include "integration/TMSrvIntegration.h"
#include "integration/DBSrvIntegration.h"
#include "../../../include/Common/Utils/Logger.h"
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>

namespace WYDBR {

//-------------------------------------------------------------------------
// AdminPanelBase
//-------------------------------------------------------------------------

AdminPanelBase::AdminPanelBase(const std::string& name)
    : m_name(name), m_active(false) {
}

const std::string& AdminPanelBase::GetName() const {
    return m_name;
}

bool AdminPanelBase::IsActive() const {
    return m_active;
}

void AdminPanelBase::SetActive(bool active) {
    m_active = active;
}

//-------------------------------------------------------------------------
// ServerStatusPanel
//-------------------------------------------------------------------------

ServerStatusPanel::ServerStatusPanel(TMSrvIntegration* tmsrv, DBSrvIntegration* dbsrv)
    : AdminPanelBase("Status do Servidor"), m_tmsrv(tmsrv), m_dbsrv(dbsrv) {
}

ServerStatusPanel::~ServerStatusPanel() {
}

bool ServerStatusPanel::Initialize() {
    // Registrar callbacks de status
    m_tmsrv->RegisterStatusCallback([this](ServerStatus status) {
        OnTMSrvStatusChanged(status);
    });
    
    m_dbsrv->RegisterStatusCallback([this](ServerStatus status) {
        OnDBSrvStatusChanged(status);
    });
    
    // Inicializar dados de status
    m_tmsrvStatus = ServerStatusToString(m_tmsrv->GetStatus());
    m_dbsrvStatus = ServerStatusToString(m_dbsrv->GetStatus());
    
    // Obter estatísticas iniciais
    RefreshStats();
    
    return true;
}

void ServerStatusPanel::Update() {
    // Atualizar estatísticas periodicamente
    static auto lastUpdateTime = std::chrono::steady_clock::now();
    auto currentTime = std::chrono::steady_clock::now();
    
    if (std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastUpdateTime).count() >= 5) {
        RefreshStats();
        lastUpdateTime = currentTime;
    }
}

void ServerStatusPanel::Render() {
    if (!IsActive()) return;
    
    ImGui::Begin("Status do Servidor", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    
    // Seção TMSrv
    ImGui::Text("TMSrv Status: %s", m_tmsrvStatus.c_str());
    
    // Botões de controle para TMSrv
    ImGui::BeginGroup();
    if (ImGui::Button("Iniciar TMSrv")) {
        if (m_tmsrv->Start()) {
            LOG_INFO("TMSrv iniciado pelo painel de administração");
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Parar TMSrv")) {
        m_tmsrv->Stop();
        LOG_INFO("TMSrv parado pelo painel de administração");
    }
    ImGui::SameLine();
    if (ImGui::Button("Reiniciar TMSrv")) {
        if (m_tmsrv->Restart()) {
            LOG_INFO("TMSrv reiniciado pelo painel de administração");
        }
    }
    ImGui::EndGroup();
    
    // Estatísticas do TMSrv
    ImGui::Separator();
    ImGui::Text("TMSrv Estatísticas:");
    ImGui::TextWrapped("%s", m_tmsrvStats.c_str());
    
    // Seção DBSrv
    ImGui::Separator();
    ImGui::Text("DBSrv Status: %s", m_dbsrvStatus.c_str());
    
    // Botões de controle para DBSrv
    ImGui::BeginGroup();
    if (ImGui::Button("Iniciar DBSrv")) {
        if (m_dbsrv->Start()) {
            LOG_INFO("DBSrv iniciado pelo painel de administração");
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Parar DBSrv")) {
        m_dbsrv->Stop();
        LOG_INFO("DBSrv parado pelo painel de administração");
    }
    ImGui::SameLine();
    if (ImGui::Button("Reiniciar DBSrv")) {
        if (m_dbsrv->Restart()) {
            LOG_INFO("DBSrv reiniciado pelo painel de administração");
        }
    }
    ImGui::EndGroup();
    
    // Estatísticas do DBSrv
    ImGui::Separator();
    ImGui::Text("DBSrv Estatísticas:");
    ImGui::TextWrapped("%s", m_dbsrvStats.c_str());
    
    // Botão de atualização manual
    ImGui::Separator();
    if (ImGui::Button("Atualizar Estatísticas")) {
        RefreshStats();
    }
    
    ImGui::End();
}

bool ServerStatusPanel::ProcessEvent(const Event& event) {
    // Processar eventos relevantes para este painel
    return false;
}

void ServerStatusPanel::OnTMSrvStatusChanged(ServerStatus status) {
    m_tmsrvStatus = ServerStatusToString(status);
    LOG_INFO("Status do TMSrv alterado para: {}", m_tmsrvStatus);
}

void ServerStatusPanel::OnDBSrvStatusChanged(ServerStatus status) {
    m_dbsrvStatus = ServerStatusToString(status);
    LOG_INFO("Status do DBSrv alterado para: {}", m_dbsrvStatus);
}

void ServerStatusPanel::RefreshStats() {
    // Obter estatísticas do TMSrv
    m_tmsrvStats = m_tmsrv->GetStats();
    
    // Obter estatísticas do DBSrv
    m_dbsrvStats = m_dbsrv->GetStats();
}

std::string ServerStatusPanel::ServerStatusToString(ServerStatus status) {
    switch (status) {
        case ServerStatus::OFFLINE:
            return "Offline";
        case ServerStatus::STARTING:
            return "Iniciando";
        case ServerStatus::RUNNING:
            return "Rodando";
        case ServerStatus::STOPPING:
            return "Parando";
        case ServerStatus::ERROR:
            return "Erro";
        default:
            return "Desconhecido";
    }
}

//-------------------------------------------------------------------------
// PlayerManagementPanel
//-------------------------------------------------------------------------

PlayerManagementPanel::PlayerManagementPanel(TMSrvIntegration* tmsrv, DBSrvIntegration* dbsrv)
    : AdminPanelBase("Gerenciamento de Jogadores"), m_tmsrv(tmsrv), m_dbsrv(dbsrv) {
}

PlayerManagementPanel::~PlayerManagementPanel() {
}

bool PlayerManagementPanel::Initialize() {
    // Inicializar dados de jogadores
    RefreshPlayers();
    
    return true;
}

void PlayerManagementPanel::Update() {
    // Atualizar lista de jogadores periodicamente
    static auto lastUpdateTime = std::chrono::steady_clock::now();
    auto currentTime = std::chrono::steady_clock::now();
    
    if (std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastUpdateTime).count() >= 5) {
        RefreshPlayers();
        lastUpdateTime = currentTime;
    }
}

void PlayerManagementPanel::Render() {
    if (!IsActive()) return;
    
    ImGui::Begin("Gerenciamento de Jogadores", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    
    // Botão de atualização
    if (ImGui::Button("Atualizar Lista")) {
        RefreshPlayers();
    }
    
    // Lista de jogadores online
    ImGui::Separator();
    ImGui::Text("Jogadores Online: %zu", m_onlinePlayers.size());
    
    if (ImGui::BeginTable("PlayersTable", 6, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("ID");
        ImGui::TableSetupColumn("Nome da Conta");
        ImGui::TableSetupColumn("Personagem");
        ImGui::TableSetupColumn("Nível");
        ImGui::TableSetupColumn("Localização");
        ImGui::TableSetupColumn("Ações");
        ImGui::TableHeadersRow();
        
        for (const auto& player : m_onlinePlayers) {
            ImGui::TableNextRow();
            
            // ID
            ImGui::TableNextColumn();
            ImGui::Text("%u", player.id);
            
            // Nome da Conta
            ImGui::TableNextColumn();
            ImGui::Text("%s", player.accountName.c_str());
            
            // Personagem
            ImGui::TableNextColumn();
            ImGui::Text("%s", player.characterName.c_str());
            
            // Nível
            ImGui::TableNextColumn();
            ImGui::Text("%u", player.level);
            
            // Localização
            ImGui::TableNextColumn();
            ImGui::Text("%s", player.location.c_str());
            
            // Ações
            ImGui::TableNextColumn();
            ImGui::PushID(static_cast<int>(player.id));
            
            if (ImGui::Button("Detalhes")) {
                ViewPlayerDetails(player.id);
            }
            
            ImGui::SameLine();
            
            if (ImGui::Button("Kickar")) {
                KickPlayer(player.id);
            }
            
            ImGui::SameLine();
            
            if (ImGui::Button("Banir")) {
                // Mostrar diálogo de banimento
                ImGui::OpenPopup("BanPopup");
            }
            
            // Popup de banimento
            if (ImGui::BeginPopup("BanPopup")) {
                static std::string banReason;
                static int banDuration = 24; // Horas
                
                ImGui::Text("Banir Jogador: %s", player.characterName.c_str());
                ImGui::Separator();
                
                ImGui::Text("Motivo:");
                ImGui::InputText("##BanReason", &banReason);
                
                ImGui::Text("Duração (horas):");
                ImGui::SliderInt("##BanDuration", &banDuration, 1, 720);
                
                if (ImGui::Button("Confirmar Ban")) {
                    BanPlayer(player.id, banReason, banDuration);
                    banReason.clear();
                    banDuration = 24;
                    ImGui::CloseCurrentPopup();
                }
                
                ImGui::SameLine();
                
                if (ImGui::Button("Cancelar")) {
                    ImGui::CloseCurrentPopup();
                }
                
                ImGui::EndPopup();
            }
            
            ImGui::PopID();
        }
        
        ImGui::EndTable();
    }
    
    ImGui::End();
}

bool PlayerManagementPanel::ProcessEvent(const Event& event) {
    // Processar eventos relevantes para este painel
    return false;
}

void PlayerManagementPanel::RefreshPlayers() {
    // Obter lista de jogadores online do TMSrv
    m_onlinePlayers = m_tmsrv->GetConnectedClients();
}

void PlayerManagementPanel::KickPlayer(uint32_t playerId) {
    // Executar comando para kickar jogador
    std::string command = "kick " + std::to_string(playerId);
    std::string result = m_tmsrv->ExecuteCommand(command);
    
    LOG_INFO("Kickando jogador {}: {}", playerId, result);
}

void PlayerManagementPanel::BanPlayer(uint32_t playerId, const std::string& reason, int durationHours) {
    // Executar comando para banir jogador
    std::string command = "ban " + std::to_string(playerId) + " " + 
                         std::to_string(durationHours) + " " + reason;
    std::string result = m_tmsrv->ExecuteCommand(command);
    
    LOG_INFO("Banindo jogador {}: {}", playerId, result);
}

void PlayerManagementPanel::ViewPlayerDetails(uint32_t playerId) {
    // TODO: Implementar visualização detalhada do jogador
    LOG_INFO("Visualizando detalhes do jogador {}", playerId);
}

//-------------------------------------------------------------------------
// AdminPanel
//-------------------------------------------------------------------------

AdminPanel::AdminPanel(TMSrvIntegration* tmsrv, DBSrvIntegration* dbsrv)
    : m_tmsrv(tmsrv), m_dbsrv(dbsrv), m_activePanel(0), 
      m_initialized(false), m_visible(false) {
}

AdminPanel::~AdminPanel() {
}

bool AdminPanel::Initialize() {
    if (m_initialized) {
        return true;
    }
    
    try {
        // Inicializar componentes
        InitializePanels();
        
        m_initialized = true;
        LOG_INFO("Painel de administração inicializado com sucesso");
        return true;
    }
    catch (const std::exception& e) {
        LOG_ERROR("Exceção ao inicializar painel de administração: {}", e.what());
        return false;
    }
}

void AdminPanel::InitializePanels() {
    // Criar painéis
    auto statusPanel = std::make_unique<ServerStatusPanel>(m_tmsrv, m_dbsrv);
    statusPanel->Initialize();
    m_panels.push_back(std::move(statusPanel));
    
    auto playerPanel = std::make_unique<PlayerManagementPanel>(m_tmsrv, m_dbsrv);
    playerPanel->Initialize();
    m_panels.push_back(std::move(playerPanel));
    
    // TODO: Adicionar mais painéis conforme necessário
    
    // Ativar o primeiro painel
    if (!m_panels.empty()) {
        m_panels[0]->SetActive(true);
    }
}

void AdminPanel::Update() {
    if (!m_initialized || !m_visible) {
        return;
    }
    
    // Atualizar painel ativo
    if (m_activePanel >= 0 && m_activePanel < m_panels.size()) {
        m_panels[m_activePanel]->Update();
    }
}

void AdminPanel::Render() {
    if (!m_initialized || !m_visible) {
        return;
    }
    
    // Barra principal
    ImGui::BeginMainMenuBar();
    if (ImGui::BeginMenu("Arquivo")) {
        if (ImGui::MenuItem("Sair", "Alt+F4")) {
            // Implementar saída
        }
        ImGui::EndMenu();
    }
    
    if (ImGui::BeginMenu("Servidores")) {
        if (ImGui::MenuItem("Iniciar Todos")) {
            m_tmsrv->Start();
            m_dbsrv->Start();
        }
        if (ImGui::MenuItem("Parar Todos")) {
            m_tmsrv->Stop();
            m_dbsrv->Stop();
        }
        ImGui::Separator();
        if (ImGui::MenuItem("Iniciar TMSrv")) {
            m_tmsrv->Start();
        }
        if (ImGui::MenuItem("Parar TMSrv")) {
            m_tmsrv->Stop();
        }
        ImGui::Separator();
        if (ImGui::MenuItem("Iniciar DBSrv")) {
            m_dbsrv->Start();
        }
        if (ImGui::MenuItem("Parar DBSrv")) {
            m_dbsrv->Stop();
        }
        ImGui::EndMenu();
    }
    
    if (ImGui::BeginMenu("Painéis")) {
        for (size_t i = 0; i < m_panels.size(); i++) {
            if (ImGui::MenuItem(m_panels[i]->GetName().c_str(), nullptr, m_activePanel == i)) {
                SwitchToPanel(i);
            }
        }
        ImGui::EndMenu();
    }
    
    if (ImGui::BeginMenu("Ajuda")) {
        if (ImGui::MenuItem("Sobre")) {
            // Mostrar diálogo sobre
        }
        ImGui::EndMenu();
    }
    
    ImGui::EndMainMenuBar();
    
    // Renderizar painel ativo
    if (m_activePanel >= 0 && m_activePanel < m_panels.size()) {
        m_panels[m_activePanel]->Render();
    }
}

bool AdminPanel::ProcessEvent(const Event& event) {
    if (!m_initialized || !m_visible) {
        return false;
    }
    
    // Processar evento no painel ativo
    if (m_activePanel >= 0 && m_activePanel < m_panels.size()) {
        return m_panels[m_activePanel]->ProcessEvent(event);
    }
    
    return false;
}

void AdminPanel::Show() {
    m_visible = true;
}

void AdminPanel::Hide() {
    m_visible = false;
}

bool AdminPanel::IsVisible() const {
    return m_visible;
}

void AdminPanel::SwitchToPanel(int panelIndex) {
    if (panelIndex < 0 || panelIndex >= m_panels.size()) {
        return;
    }
    
    // Desativar painel atual
    if (m_activePanel >= 0 && m_activePanel < m_panels.size()) {
        m_panels[m_activePanel]->SetActive(false);
    }
    
    // Ativar novo painel
    m_activePanel = panelIndex;
    m_panels[m_activePanel]->SetActive(true);
}

} // namespace WYDBR
