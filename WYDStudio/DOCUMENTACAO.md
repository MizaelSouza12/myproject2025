# WYD MODERN 2025 STUDIO - Documentação

## Visão Geral

O WYD Modern 2025 Studio é uma plataforma completa de desenvolvimento e gerenciamento para o jogo WYD (With Your Destiny). Criado como um software standalone, ele fornece ferramentas avançadas para administração, desenvolvimento e customização de servidores de WYD, usando tecnologias modernas e uma interface intuitiva orientada por IA.

## Estrutura do Projeto

O projeto está organizado da seguinte forma:

```
WYD MODERN 2025 STUDIO/
└── WYDStudio/
    ├── src/
    │   ├── ai/                     # Sistema de IA e processamento de linguagem natural
    │   ├── core/                   # Núcleo da aplicação e gerenciamento de recursos
    │   ├── reverse/                # Ferramentas de engenharia reversa para arquivos do WYD
    │   └── ui/                     # Interface gráfica do usuário
    │       ├── admin/              # Painéis administrativos
    │       │   ├── dialogs/        # Diálogos para o painel administrativo
    │       │   ├── panels/         # Painéis específicos (jogadores, mundo, economia, etc.)
    │       │   └── widgets/        # Widgets reutilizáveis para o painel administrativo
    │       ├── common/             # Componentes comuns de UI para todas as áreas
    │       ├── models/             # Modelos de dados para a UI
    │       └── visual/             # Componentes visuais avançados (editores, visualizadores)
    └── docs/                       # Documentação
```

## Componentes Principais

### 1. Sistema de IA (Pasta `src/ai/`)

#### WYDLanguageDatabase
Sistema que compreende a linguagem específica dos jogadores de WYD, incluindo:
- Reconhecimento de gírias, termos técnicos e expressões comuns
- Entendimento de comandos em linguagem natural
- Processamento de pedidos como "cria um set D +11 de FM" ou "inicia um evento de drop de poeira de lac em trolls"

#### WYDAssistantIntegration
Sistema de integração de IA para:
- Responder perguntas sobre o jogo
- Executar comandos a partir de linguagem natural
- Fornecer sugestões baseadas no contexto

### 2. Painel Administrativo (Pasta `src/ui/admin/`)

#### AdminPanel
Painel central de administração que permite:
- Gerenciamento completo do servidor
- Interface para comandos em linguagem natural
- Monitoramento em tempo real

#### WorldManagerPanel
Gerenciador de eventos e mundo do jogo, permitindo:
- Criar eventos especiais (invasões, taxas de drop, etc.)
- Gerenciar mapas e localidades
- Controlar eventos de guerra entre guildas

#### AIAssistantWidget
Widget que fornece:
- Interface para comandos em linguagem natural
- Sugestões de ações baseadas no contexto
- Histórico de interações

### 3. Núcleo da Aplicação (Pasta `src/core/`)

#### LiveUpdateManager
Sistema de atualização em tempo real que permite:
- Hot-reloading de arquivos do jogo
- Conversão automática entre formatos
- Atualização sem necessidade de reiniciar o servidor

#### MultitaskingCore
Sistema de processamento paralelo para:
- Execução simultânea de tarefas
- Agendamento e gerenciamento de recursos
- Otimização de desempenho

### 4. Ferramentas de UI (Pasta `src/ui/`)

#### FileBrowser
Navegador de arquivos especializado para:
- Detecção automática de tipos de arquivo do WYD
- Filtragem e categorização
- Interface de navegação intuitiva

#### UIDesigner
Editor visual para interfaces do jogo, permitindo:
- Edição de arquivos .wyt (interfaces do WYD)
- Preview em tempo real
- Edição drag-and-drop

## Categorias de Linguagem Implementadas

O sistema de linguagem do WYD foi organizado nas seguintes categorias:

1. **ITEM**: Equipamentos, itens consumíveis, materiais
2. **CLASS**: Classes de personagens, evoluções e transcendências
3. **SKILL**: Habilidades e técnicas dos personagens
4. **LOCATION**: Locais, mapas e áreas do jogo
5. **MOB**: Monstros, NPCs e criaturas
6. **QUEST**: Missões e tarefas
7. **ATTRIBUTE**: Atributos de personagem e itens
8. **ENHANCEMENT**: Refinamentos, aprimoramentos, composições
9. **EVENT**: Eventos especiais do jogo
10. **ECONOMY**: Termos relacionados à economia e comércio
11. **SOCIAL**: Interação social, guildas, grupos
12. **COMMAND**: Comandos administrativos do jogo
13. **PVP**: Combate entre jogadores
14. **GENERAL_SLANG**: Gírias gerais do jogo

## Termos e Gírias Suportadas

### Sets e Equipamentos
- **Sets de Armadura**: A/Sephira, B/Sapphira, C/Legendaria, D/Mystia, Celestial/CL, Ancient/ANC
- **Armas**: Espada/Sword, Machado/Axe, Arco/Bow, Cajado/Staff, Lança/Spear
- **Acessórios**: Anel/Ring, Colar/Necklace, Brinco/Earring

### Classes e Evoluções
- **Classes Base**: Guerreiro/Fighter, Mago/Mage, Arqueiro/Archer
- **Evoluções**: ForeMaster/FM, BloodMaster/BM, Huntress/HT
- **Transcendências**: DivineNight/DN, ArchMage/AM, SoulLinkage/SL

### Ações e Terminologia
- Subir de Nível: "Upar", "Levelar"
- Dropar: "Drop", "Dropou", "Caiu"
- Farmar: "Farmar", "Farm", "Farming"
- Refinamentos: "+11", "refino", "plus"

### Comandos e Ações Administrativas
- Teleportar: "TP", "Teleportar"
- Banir: "Ban", "Banimento"
- Anúncios: "Broadcast", "Aviso global"
- Criação de itens: "Criar", "Spawnar"

## Padrões de Linguagem Natural

O sistema reconhece os seguintes padrões principais de fala natural:

1. **Criação de Equipamento**:
   ```
   (cria|criar|gera|gerar|faz|fazer|quero|queria|preciso|adiciona) (um|uma|umas|uns)? (set|armadura|arma|equip|) ([A-Da-d]|cl|anc|celestial|ancient|foema|divine|arc|hunt) \+?(\d+)? (de|para|da|do)? ?([a-z]+)?
   ```
   Exemplos: "cria um set D +11 de FM", "quero uma arma celestial +10 para BM"

2. **Evento de Drop**:
   ```
   (inicia|iniciar|começa|começar|faz|fazer|criar|ativa|ativar) (um|uma)? (evento|drop|droprate|) (de|para|com)? ?(drop|chance|taxa|)? (de)? ?([a-z\s]+) (em|nos|nas|para|no)? ?([a-z\s]+)?
   ```
   Exemplos: "inicia um evento de drop de poeira de lac em trolls", "ativa drop de set celestial no deserto"

3. **Gerenciamento de Jogador**:
   ```
   (bane|ban|bani|banir|silencia|cala|silenciar|tp|teleporta|move|mover|teleportar|kick|expulsa|expulsar|kika|kikar) (o jogador|a conta|o char|o personagem|o nick|o)? ?([a-zA-Z0-9]+)
   ```
   Exemplos: "bane o jogador Testador123", "tp o personagem Hunter para armia"

4. **Broadcast**:
   ```
   (anuncia|broadcast|avisa|notifica|manda mensagem|envia mensagem) (que|sobre|para todos|global|no server)? ?(.+)
   ```
   Exemplos: "anuncia que vai ter evento em 5 minutos", "broadcast guerra de guildas às 20h"

## Tecnologias Utilizadas

- **C++**: Linguagem principal para acesso direto às estruturas do jogo
- **Qt**: Framework para interface gráfica
- **JSON**: Formato para armazenamento e intercâmbio de dados
- **IA**: Integração com modelos de linguagem para assistência e comandos em linguagem natural

## Recursos Futuros

- **PlayerManagerPanel**: Gerenciamento avançado de jogadores
- **EconomyManagerPanel**: Controle da economia do jogo
- **SecurityManagerPanel**: Segurança e anti-cheating
- **WYDAssistantImplementation**: Implementação completa do assistente com IA
- **EditorDeLixeiras**: Interface visual para edição de drops dos monstros