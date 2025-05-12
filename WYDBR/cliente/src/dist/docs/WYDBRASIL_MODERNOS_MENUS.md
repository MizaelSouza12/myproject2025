# WYDBRASIL - Documentação de Menus Modernos

Este documento descreve a implementação dos novos menus modernos no WYDBRASIL, incluindo a estrutura dos arquivos, atalhos de teclado e interações especiais.

## Estrutura dos Arquivos

### Arquivos de Menu
Os arquivos de menu estão localizados em `WYDBR/clientee/srce/UI/moderna/` e seguem o formato padrão de menu do WYD.

- **ajustes_avancados.txt**: Menu para configurações gráficas, teclas e VOIP
- **compartilhamento.txt**: Menu para gerenciar compartilhamento de tela
- **acessar_tela.txt**: Menu para acessar tela de outro jogador
- **chat_voz.txt**: Menu para comunicação por voz (VOIP)
- **amigos.txt**: Lista de amigos do jogador
- **perfil.txt**: Informações detalhadas do personagem
- **teleporte.txt**: Menu de teleporte rápido entre cidades
- **indicadores.txt**: Configurações de indicadores de desempenho

### Arquivos de Implementação
A implementação dos menus é feita através dos seguintes arquivos:

- **ModernMenuIntegration.h/cpp**: Classe principal para a integração de menus
  - Localização: `WYDBR/clientee/src/implementation/`
  - Gerencia interações, atalhos e estado dos menus
  
- **modern_input_integration.cpp**: Integração com o sistema de input
  - Localização: `WYDBR/clientee/src/input/keyboard/`
  - Conecta os atalhos de teclado com as funções da classe principal
  
- **UserSettingsDB.h**: Gerenciamento de persistência de configurações
  - Localização: `WYDBR/server/source/include/database/`
  - Define a estrutura para armazenar configurações do usuário

## Atalhos de Teclado

A implementação inclui os seguintes atalhos de teclado:

| Tecla | Função |
|-------|--------|
| ALT+A | Abrir menu de Ajustes Avançados |
| ALT+C | Abrir menu de Compartilhamento de Tela |
| ALT+R | Abrir menu para Acessar Tela Remota |
| ALT+V | Ativar/desativar VOIP |
| ALT+G | Abrir menu de Chat de Voz |
| D     | Abrir/fechar menu de Teleporte Rápido |

## Interações Especiais

Além dos atalhos de teclado, a implementação inclui interações especiais com o mouse:

- **Clique na barra de vida**: Abre o menu de Perfil
- **Clique direito no nome do usuário remoto**: Abre menu de contexto com opções (Desconectar, Bloquear, Restringir)
- **Mouse sobre indicadores de desempenho**: Mostra tooltips detalhados

## Persistência de Configurações

Todas as configurações do usuário são armazenadas no banco de dados e carregadas ao fazer login. Isso inclui:

### Configurações de Gráficos
- Qualidade da renderização
- Distância de visualização
- Efeitos especiais

### Configurações de VOIP
- Tecla Push-to-Talk
- Volume de entrada/saída
- Uso do botão do mouse

### Configurações de Compartilhamento
- Senha de acesso
- Restrições de segurança
- Link de compartilhamento

### Configurações de Interface
- Posição dos indicadores
- Teclas personalizadas

## Integração com Cliente Existente

A implementação é completamente compatível com o cliente existente do WYD, utilizando:

- **Namespaces isolados**: Evita conflitos com o código existente
- **Exportação C**: Funções exportadas com extern "C" para compatibilidade
- **Menu de Sistema**: Integração completa com o menu existente

## Como Testar

1. Execute o script `verificar_integracao_menus.sh` para verificar se todos os arquivos foram criados corretamente
2. Execute o script `test_wydbr_menus.sh` para uma simulação do funcionamento dos menus
3. Inicie o servidor WYDBRASIL normalmente e teste os atalhos de teclado no cliente

## Próximos Passos

- Implementar efeitos visuais específicos para cada menu
- Adicionar suporte para temas personalizados
- Expandir funcionalidades de VOIP para salas de chat
- Melhorar sistema de compartilhamento com gravação de vídeo

---

© WYDBRASIL 2025