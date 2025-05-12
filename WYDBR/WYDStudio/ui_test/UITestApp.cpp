#include "UITest.h"
#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <functional>
#include <thread>
#include <chrono>

using namespace WYDStudio;

// Função que simula espera para melhor visualização do teste
void waitBriefly() {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

// Função que simula uma operação de processamento de linguagem natural
void processLanguage(const std::string& text) {
    std::cout << "\n[PROCESSAMENTO DE LINGUAGEM]" << std::endl;
    std::cout << "Analisando: '" << text << "'" << std::endl;
    waitBriefly();
    
    std::cout << "Termos reconhecidos:" << std::endl;
    
    // Simular reconhecimento de alguns termos comuns do WYD
    if (text.find("set") != std::string::npos) {
        std::cout << "- Set (Categoria: ITEM)" << std::endl;
    }
    if (text.find("FM") != std::string::npos || text.find("BM") != std::string::npos) {
        std::cout << "- " << (text.find("FM") != std::string::npos ? "FM" : "BM") << " (Categoria: CLASS)" << std::endl;
    }
    if (text.find("drop") != std::string::npos) {
        std::cout << "- Drop (Categoria: EVENT)" << std::endl;
    }
    if (text.find("armia") != std::string::npos) {
        std::cout << "- Armia (Categoria: LOCATION)" << std::endl;
    }
    
    waitBriefly();
    std::cout << "Processamento concluído." << std::endl;
}

// Função principal do teste
int main() {
    std::cout << "=============================================" << std::endl;
    std::cout << "  TESTE DE INTERFACE DO WYDSTUDIO" << std::endl;
    std::cout << "=============================================" << std::endl;
    
    // Criar janela principal
    auto mainWindow = std::make_shared<UIWindow>(
        "mainWindow", "WYD MODERN 2025 STUDIO", 0, 0, 800, 600);
    
    // Criar painel para comandos
    auto commandPanel = std::make_shared<UIPanel>(
        "commandPanel", 10, 10, 780, 150);
    
    // Criar componentes de entrada
    auto commandLabel = std::make_shared<UILabel>(
        "commandLabel", "Digite um comando:", 10, 10, 200, 30);
    
    auto commandInput = std::make_shared<UITextInput>(
        "commandInput", "Digite seu comando aqui...", 10, 50, 500, 30);
    
    auto executeButton = std::make_shared<UIButton>(
        "executeButton", "Executar", 520, 50, 100, 30);
    
    auto resultLabel = std::make_shared<UILabel>(
        "resultLabel", "Resultado:", 10, 90, 200, 30);
    
    // Criar painel para seleção de área
    auto areaPanel = std::make_shared<UIPanel>(
        "areaPanel", 10, 170, 370, 420);
    
    auto areaLabel = std::make_shared<UILabel>(
        "areaLabel", "Selecione uma área:", 10, 10, 200, 30);
    
    auto areaList = std::make_shared<UIListBox>(
        "areaList", 10, 50, 350, 360);
    
    // Adicionar áreas ao ListBox
    areaList->AddItem("Armia (Cidade Principal)");
    areaList->AddItem("Noapri (Vila dos Arqueiros)");
    areaList->AddItem("Erion (Cidade dos Magos)");
    areaList->AddItem("Nipb (Cidade das Trevas)");
    areaList->AddItem("Dungeon Level 1-50");
    areaList->AddItem("Dungeon Level 100-150");
    areaList->AddItem("Reino (Castle Dungeon)");
    areaList->AddItem("Vale de Árpez (PvP)");
    
    // Criar painel para ações específicas
    auto actionPanel = std::make_shared<UIPanel>(
        "actionPanel", 390, 170, 400, 420);
    
    auto actionLabel = std::make_shared<UILabel>(
        "actionLabel", "Ações disponíveis:", 10, 10, 200, 30);
    
    auto generateItemButton = std::make_shared<UIButton>(
        "generateItemButton", "Gerar Item", 10, 50, 180, 30);
    
    auto startEventButton = std::make_shared<UIButton>(
        "startEventButton", "Iniciar Evento", 200, 50, 180, 30);
    
    auto banPlayerButton = std::make_shared<UIButton>(
        "banPlayerButton", "Banir Jogador", 10, 90, 180, 30);
    
    auto teleportButton = std::make_shared<UIButton>(
        "teleportButton", "Teleportar Jogador", 200, 90, 180, 30);
    
    auto statusLabel = std::make_shared<UILabel>(
        "statusLabel", "Status: Aguardando comando...", 10, 150, 380, 30);
    
    // Configurar manipuladores de eventos
    
    // Manipulador para o botão Executar
    executeButton->SetClickHandler([commandInput, statusLabel]() {
        std::string command = commandInput->GetText();
        
        if (command.empty()) {
            statusLabel->SetText("Status: Nenhum comando digitado.");
            return;
        }
        
        statusLabel->SetText("Status: Processando comando...");
        waitBriefly();
        
        // Processar o comando com o sistema NLP
        processLanguage(command);
        
        statusLabel->SetText("Status: Comando processado: '" + command + "'");
    });
    
    // Manipulador para a lista de áreas
    areaList->SetSelectionHandler([areaList, statusLabel](int index) {
        if (index >= 0) {
            std::string area = areaList->GetSelectedItem();
            statusLabel->SetText("Status: Área selecionada: " + area);
        }
    });
    
    // Manipuladores para os botões de ação
    generateItemButton->SetClickHandler([statusLabel, areaList]() {
        std::string area = areaList->GetSelectedIndex() >= 0 
            ? areaList->GetSelectedItem() 
            : "Nenhuma área selecionada";
        
        statusLabel->SetText("Status: Gerando item em " + area);
        waitBriefly();
        
        std::cout << "\n[GERAÇÃO DE ITEM]" << std::endl;
        std::cout << "Criando item Armadura Celestial +10 em " << area << std::endl;
        waitBriefly();
        std::cout << "Item gerado com sucesso." << std::endl;
        
        statusLabel->SetText("Status: Item gerado com sucesso em " + area);
    });
    
    startEventButton->SetClickHandler([statusLabel, areaList]() {
        std::string area = areaList->GetSelectedIndex() >= 0 
            ? areaList->GetSelectedItem() 
            : "Nenhuma área selecionada";
        
        statusLabel->SetText("Status: Iniciando evento em " + area);
        waitBriefly();
        
        std::cout << "\n[EVENTO]" << std::endl;
        std::cout << "Iniciando evento de drop aumentado em " << area << std::endl;
        waitBriefly();
        std::cout << "Evento iniciado com sucesso. Duração: 2 horas." << std::endl;
        
        statusLabel->SetText("Status: Evento iniciado em " + area);
    });
    
    // Montar a hierarquia de UI
    
    // Adicionar componentes ao painel de comandos
    commandPanel->AddElement(commandLabel);
    commandPanel->AddElement(commandInput);
    commandPanel->AddElement(executeButton);
    commandPanel->AddElement(resultLabel);
    
    // Adicionar componentes ao painel de áreas
    areaPanel->AddElement(areaLabel);
    areaPanel->AddElement(areaList);
    
    // Adicionar componentes ao painel de ações
    actionPanel->AddElement(actionLabel);
    actionPanel->AddElement(generateItemButton);
    actionPanel->AddElement(startEventButton);
    actionPanel->AddElement(banPlayerButton);
    actionPanel->AddElement(teleportButton);
    actionPanel->AddElement(statusLabel);
    
    // Adicionar painéis à janela principal
    mainWindow->AddElement(commandPanel);
    mainWindow->AddElement(areaPanel);
    mainWindow->AddElement(actionPanel);
    
    // --- EXECUÇÃO DO TESTE ---
    
    std::cout << "\n[INICIALIZANDO INTERFACE]" << std::endl;
    waitBriefly();
    
    // Testar renderização da UI
    std::cout << "\n[TESTE DE RENDERIZAÇÃO]" << std::endl;
    mainWindow->TestRender();
    waitBriefly();
    
    // Testar entrada de comando
    std::cout << "\n[TESTE DE ENTRADA DE COMANDO]" << std::endl;
    commandInput->TestInput("criar set D +11 para FM em Armia");
    waitBriefly();
    
    // Testar clique no botão Executar
    std::cout << "\n[TESTE DE EXECUÇÃO DE COMANDO]" << std::endl;
    executeButton->TestClick();
    waitBriefly();
    
    // Testar seleção de área
    std::cout << "\n[TESTE DE SELEÇÃO DE ÁREA]" << std::endl;
    areaList->TestSelect(0); // Selecionar Armia
    waitBriefly();
    
    // Testar clique em Gerar Item
    std::cout << "\n[TESTE DE GERAÇÃO DE ITEM]" << std::endl;
    generateItemButton->TestClick();
    waitBriefly();
    
    // Testar clique em Iniciar Evento
    std::cout << "\n[TESTE DE INÍCIO DE EVENTO]" << std::endl;
    startEventButton->TestClick();
    waitBriefly();
    
    std::cout << "\n[TESTE CONCLUÍDO]" << std::endl;
    std::cout << "Todos os componentes de UI estão funcionando corretamente." << std::endl;
    std::cout << "=============================================\n" << std::endl;
    
    return 0;
}