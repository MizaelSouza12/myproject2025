# Documentação dos Novos Menus do WYDBR

## Visão Geral

Este documento descreve a implementação dos novos menus modernos para o WYDBR, incluindo as interfaces gráficas, as classes de integração e as funcionalidades implementadas. Estes novos menus seguem o padrão visual e funcional existente no WYDBR, mantendo a consistência da experiência do usuário.

## Estrutura de Arquivos

### Arquivos de Interface (UI)

Os novos menus foram implementados como arquivos de texto seguindo o formato padrão do WYDBR, localizados em:

```
WYDBR/clientee/srce/UI/moderna/
```

Os arquivos de menu incluem:

1. **ajustes_avancados.txt** - Menu de configurações gráficas e de controles
2. **compartilhamento.txt** - Menu para gerenciar o compartilhamento de tela
3. **acessar_tela.txt** - Menu para acessar a tela de outro jogador
4. **chat_voz.txt** - Menu para comunicação por voz (VOIP)
5. **amigos.txt** - Menu para gerenciar lista de amigos
6. **perfil.txt** - Menu de informações detalhadas do perfil
7. **teleporte.txt** - Menu para teleporte rápido entre cidades
8. **indicadores.txt** - Menu para configurar indicadores de desempenho

### Arquivos de Implementação

Os arquivos de implementação foram criados para integrar os novos menus com o sistema existente:

```
WYDBR/clientee/src/implementation/ModernMenuIntegration.h
WYDBR/clientee/src/implementation/ModernMenuIntegration.cpp
WYDBR/clientee/src/input/keyboard/modern_input_integration.cpp
WYDBR/server/source/include/database/UserSettingsDB.h
```

## Atalhos de Teclado

Foram implementados os seguintes atalhos de teclado:

- **ALT+A**: Abrir menu de Ajustes Avançados
- **ALT+C**: Abrir menu de Compartilhamento de Tela
- **ALT+R**: Abrir menu para Acessar Tela de outro jogador
- **ALT+V**: Ativar/desativar VOIP
- **ALT+G**: Abrir menu de Chat de Voz
- **D**: Abrir/fechar menu de Teleporte Rápido

## Interações Específicas

Além dos atalhos de teclado, foram implementadas interações específicas:

- **Clique na barra de vida**: Abre o Menu de Perfil
- **Clique direito no nome do usuário remoto**: Abre menu contextual com opções (Desconectar, Bloquear, Restringir)
- **Mouse sobre indicadores de desempenho**: Mostra tooltips detalhados

## Classes e Funcionalidades

### ModernMenuIntegration

A classe `ModernMenuIntegration` é responsável por gerenciar a integração dos novos menus com o sistema existente do WYDBR. Ela implementa o padrão Singleton para garantir uma única instância em todo o sistema.

```cpp
class ModernMenuIntegration {
public:
    static ModernMenuIntegration& getInstance();
    void initialize();
    bool processKeyEvent(int keyCode, bool altPressed, bool ctrlPressed, bool shiftPressed);
    bool processClickEvent(int x, int y, bool rightClick);
    // ... outros métodos ...
};
```

#### Principais Métodos

- **initialize()**: Inicializa a integração de menu, carregando arquivos e registrando atalhos
- **processKeyEvent()**: Processa eventos de teclado para os novos menus
- **processClickEvent()**: Processa eventos de clique para os novos menus
- **openAdvancedSettingsMenu()**: Abre o menu de ajustes avançados
- **openScreenSharingMenu()**: Abre o menu de compartilhamento de tela
- **openScreenAccessMenu()**: Abre o menu para acessar tela remota
- **toggleVoip()**: Ativa/desativa o VOIP
- **toggleTeleportMenu()**: Abre/fecha o menu de teleporte rápido

### Integração com o Sistema de Input

O arquivo `modern_input_integration.cpp` fornece funções C para integrar com o sistema de input existente do WYDBR:

```cpp
extern "C" {
    int wydbr_process_modern_key_event(int keyCode, int altPressed, int ctrlPressed, int shiftPressed);
    int wydbr_process_modern_click_event(int x, int y, int rightClick);
    void wydbr_initialize_modern_menu();
    // ... outras funções ...
}
```

### Persistência de Configurações

A classe `UserSettingsDB` gerencia a persistência das configurações do usuário no banco de dados:

```cpp
class UserSettingsDB {
public:
    static UserSettingsDB& getInstance();
    bool initialize();
    bool loadUserSettings(int userId, UserSettings& settings);
    bool saveUserSettings(int userId, const UserSettings& settings);
    // ... outros métodos ...
};
```

## Estrutura de Dados

### UserSettings

A estrutura `UserSettings` armazena todas as configurações do usuário:

```cpp
struct UserSettings {
    // Configurações de gráficos
    int graphicsQuality;
    int viewDistance;
    int effectsQuality;
    
    // Configurações de VOIP
    int pushToTalkKey;
    bool useMouse;
    int mouseButton;
    bool voiceActivation;
    int inputVolume;
    int outputVolume;
    
    // Configurações de compartilhamento
    std::string sharingPassword;
    bool blockItemMovement;
    bool blockPasswordChange;
    bool blockCharDeletion;
    
    // Configurações de interface
    bool showPerformanceIndicators;
    int indicatorsPosition;
    
    // Teclas de atalho personalizadas
    std::vector<std::pair<std::string, int>> customHotkeys;
    
    // Amigos e bloqueados
    std::vector<std::string> friendsList;
    std::vector<std::string> blockedUsers;
};
```

## Compatibilidade e Manutenção

### Requisitos de Integração

Para integrar os novos menus com o código existente do WYDBR, é necessário:

1. Adicionar chamadas para `wydbr_initialize_modern_menu()` na inicialização do cliente
2. Encaminhar eventos de teclado para `wydbr_process_modern_key_event()`
3. Encaminhar eventos de clique para `wydbr_process_modern_click_event()`
4. Compilar os novos arquivos junto com o restante do código

### Considerações de Manutenção

- Os backups dos arquivos originais são criados com a extensão `.bak`
- Todas as configurações são salvas na conta do usuário e carregadas ao fazer login
- As modificações seguem o padrão visual e a estrutura do WYDBR original

## Exemplos de Uso

### Inicialização do Sistema de Menus

```cpp
#include "client/implementation/ModernMenuIntegration.h"

void initializeClient() {
    // ... código de inicialização existente ...
    
    // Inicializar os novos menus
    wydbr::ui::ModernMenuIntegration::getInstance().initialize();
    
    // ... resto do código de inicialização ...
}
```

### Processamento de Eventos de Teclado

```cpp
bool handleKeyEvent(int keyCode, bool alt, bool ctrl, bool shift) {
    // Verificar se os novos menus processaram o evento
    if (wydbr::ui::ModernMenuIntegration::getInstance().processKeyEvent(keyCode, alt, ctrl, shift)) {
        return true; // Evento foi processado pelos novos menus
    }
    
    // Continuar com o processamento normal do evento
    // ...
    
    return false;
}
```

### Salvamento de Configurações do Usuário

```cpp
void saveUserPreferences(int userId) {
    // Obter configurações atuais
    wydbr::database::UserSettingsDB::UserSettings settings;
    
    // Preencher configurações com valores atuais
    // ...
    
    // Salvar no banco de dados
    wydbr::database::UserSettingsDB::getInstance().saveUserSettings(userId, settings);
}
```

## Próximos Passos

1. Compilar o código-fonte com as novas classes de integração
2. Executar testes para verificar se todos os menus são carregados corretamente
3. Verificar a funcionalidade dos atalhos de teclado
4. Validar a persistência das configurações no banco de dados

---

## Apêndice: Formato dos Arquivos de Menu

Os arquivos de menu seguem o formato padrão do WYDBR com três cores principais:

- **FFFFFF**: Cabeçalhos de seção
- **FFFFAA**: Subtítulos ou descrições
- **AAFFAA**: Opções selecionáveis ou valores

Exemplo:

```
FFFFFF
FFFFFF Menu de Exemplo
FFFFAA
FFFFAA Descrição do menu
AAFFAA

FFFFFF Seção 1
FFFFAA
FFFFAA Descrição da seção 1
AAFFAA Opção 1
AAFFAA Opção 2
AAFFAA Opção 3
AAFFAA

FFFFFF
FFFFFF Fechar
FFFFAA
FFFFAA Fechar este menu
AAFFAA
```