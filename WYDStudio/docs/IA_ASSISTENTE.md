# Documentação do Sistema de IA Assistente - WYD MODERN 2025 STUDIO

## Visão Geral

O Sistema de IA Assistente do WYD MODERN 2025 STUDIO foi projetado para oferecer uma interface de linguagem natural para administração e gerenciamento do jogo. Este sistema permite que administradores, game masters e jogadores interajam com o jogo usando expressões naturais, gírias específicas e comandos comuns do universo do WYD.

## Componentes do Sistema

### 1. WYDLanguageDatabase

Este componente é responsável pelo processamento e reconhecimento da linguagem específica do WYD:

- **Reconhecimento de Termos**: Identifica termos específicos do jogo, incluindo itens, classes, habilidades, locais, etc.
- **Normalização de Gírias**: Padroniza variações e gírias para formas reconhecíveis pelo sistema
- **Extração Semântica**: Extrai o significado e intenção das expressões dos usuários
- **Expansão de Vocabulário**: Conecta sinônimos e termos relacionados

### 2. WYDAssistantIntegration

Este componente integra o processamento de linguagem com o sistema de IA e o jogo:

- **Interpretação de Intenções**: Converte expressões naturais em intenções estruturadas
- **Execução de Comandos**: Traduz intenções em comandos executáveis
- **Geração de Respostas**: Produz respostas naturais para consultas
- **Manutenção de Contexto**: Mantém o contexto da conversa para melhor compreensão

### 3. AIAssistantWidget

Este widget fornece a interface gráfica para interação:

- **Chat Interface**: Permite diálogo natural com o assistente
- **Sugestões de Comandos**: Oferece sugestões baseadas no contexto
- **Histórico de Interações**: Mantém registro de comandos anteriores
- **Exibição de Ações**: Mostra ações disponíveis baseadas na conversa

## Categorias de Linguagem

O sistema é capaz de compreender linguagem em várias categorias:

1. **Itens e Equipamentos**: Sets, armas, acessórios, consumíveis
2. **Classes e Personagens**: Classes base, evoluções, transcendências
3. **Habilidades**: Skills, buffs, debuffs, passivas
4. **Localizações**: Mapas, cidades, masmorras, áreas
5. **Monstros e NPCs**: Tipos de mobs, bosses, vendedores
6. **Ações de Jogo**: Upar, dropar, farmar, PVP
7. **Comandos Administrativos**: Teleportar, banir, criar itens, eventos
8. **Economia**: Comércio, gold, itens de valor
9. **Eventos**: Invasões, guerras, eventos especiais
10. **Sistema**: Configurações, manutenção, segurança

## Processamento de Linguagem Natural

### Fluxo de Processamento

1. **Entrada do Usuário**: O usuário digita ou fala um comando em linguagem natural
2. **Tokenização**: A entrada é dividida em tokens (palavras e frases)
3. **Normalização**: Gírias e variações são normalizadas para termos padrão
4. **Reconhecimento de Padrões**: O sistema identifica padrões de comandos
5. **Extração de Parâmetros**: Parâmetros relevantes são extraídos (itens, quantidades, etc.)
6. **Mapeamento para Intenções**: A entrada é mapeada para uma intenção específica
7. **Execução**: A intenção é traduzida em comandos do jogo e executada
8. **Resposta**: Uma resposta em linguagem natural é gerada

### Exemplos de Processamento

#### Exemplo 1: Criação de Item

**Entrada do usuário**: "cria um set D +11 de FM"

**Processamento**:
- Reconhece o padrão de criação de item
- Identifica "set D" como Armadura Mystia
- Extrai o nível de refinamento "+11"
- Identifica "FM" como classe ForeMaster
- Mapeia para a intenção "create_equipment"
- Extrai parâmetros: {itemType: "set", setType: "D", refineLevel: "11", playerClass: "FM"}

**Comando gerado**: Criar Armadura Mystia com refinamento +11 para classe ForeMaster

#### Exemplo 2: Evento de Drop

**Entrada do usuário**: "inicia um evento de drop de poeira de lac em trolls"

**Processamento**:
- Reconhece o padrão de evento de drop
- Identifica "poeira de lac" como item específico
- Identifica "trolls" como tipo de monstro alvo
- Mapeia para a intenção "create_drop_event"
- Extrai parâmetros: {eventType: "drop", itemName: "poeira de lac", targetLocation: "trolls"}

**Comando gerado**: Iniciar evento que aumenta taxa de drop de Poeira de Lac em monstros Troll

## Capacidades de Aprendizado

O sistema possui capacidades de aprendizado para melhorar constantemente:

- **Aprendizado de Novos Termos**: Adiciona novos termos e gírias que surgem na comunidade
- **Refinamento de Padrões**: Melhora o reconhecimento de padrões de linguagem
- **Personalização por Usuário**: Adapta-se ao estilo de linguagem de administradores específicos
- **Feedback Loop**: Aprende com correções e ajustes de interpretações incorretas

## Sugestões Contextuais

O sistema oferece sugestões baseadas no contexto:

1. **Baseadas em Histórico**: Sugestões considerando comandos anteriores
2. **Baseadas em Estado**: Sugestões considerando o estado atual do jogo
3. **Baseadas em Função**: Sugestões específicas para a função atual do usuário
4. **Baseadas em Tempo**: Sugestões relevantes para eventos programados ou horários

## Arquitetura do Sistema

```
WYDAssistantIntegration
  ├── LanguageProcessor
  │    ├── WYDLanguageDatabase
  │    ├── TermRecognizer
  │    ├── PatternMatcher
  │    └── IntentExtractor
  │
  ├── AICore
  │    ├── ContextManager
  │    ├── ResponseGenerator
  │    ├── SuggestionEngine
  │    └── LearningModule
  │
  ├── GameIntegration
  │    ├── CommandTranslator
  │    ├── ActionExecutor
  │    ├── GameStateMonitor
  │    └── EventManager
  │
  └── UserInterface
       ├── AIAssistantWidget
       ├── ChatDisplay
       ├── SuggestionPanel
       └── FeedbackCollector
```

## Implementação do Sistema de Reconhecimento de Gírias

O sistema de reconhecimento de gírias e linguagem específica do WYD está implementado nas seguintes classes:

1. **LanguageTerm**: Estrutura que mapeia um termo padrão para suas variações e gírias
   ```cpp
   struct LanguageTerm {
       std::string standardForm;       // Forma padrão do termo
       std::vector<std::string> synonyms; // Sinônimos e variações
       std::string description;        // Descrição do termo
       TermCategory category;          // Categoria do termo
       std::vector<std::string> usageExamples; // Exemplos de uso
   };
   ```

2. **LanguagePattern**: Estrutura que define padrões de expressões comuns
   ```cpp
   struct LanguagePattern {
       std::string pattern;            // Padrão de expressão (regex)
       std::string interpretation;     // Interpretação do padrão
       std::string intentMapping;      // Mapeamento para intenção
       std::vector<std::string> examples; // Exemplos de frases
       std::map<std::string, std::string> parameterMappings; // Mapeamento de parâmetros
   };
   ```

3. **RecognizedIntent**: Resultado do reconhecimento de uma intenção
   ```cpp
   struct RecognizedIntent {
       std::string intent;            // Intenção reconhecida
       float confidence;              // Nível de confiança
       std::map<std::string, std::string> parameters; // Parâmetros extraídos
       std::string originalInput;     // Entrada original
   };
   ```

## Padrões de Expressões Suportados

O sistema suporta os seguintes padrões principais de expressões:

1. **Criação de Equipamento**:
   ```
   (cria|criar|gera|gerar|faz|fazer|quero|queria|preciso|adiciona) (um|uma|umas|uns)? (set|armadura|arma|equip|) ([A-Da-d]|cl|anc|celestial|ancient|foema|divine|arc|hunt) \+?(\d+)? (de|para|da|do)? ?([a-z]+)?
   ```

2. **Evento de Drop**:
   ```
   (inicia|iniciar|começa|começar|faz|fazer|criar|ativa|ativar) (um|uma)? (evento|drop|droprate|) (de|para|com)? ?(drop|chance|taxa|)? (de)? ?([a-z\s]+) (em|nos|nas|para|no)? ?([a-z\s]+)?
   ```

3. **Gerenciamento de Jogador**:
   ```
   (bane|ban|bani|banir|silencia|cala|silenciar|tp|teleporta|move|mover|teleportar|kick|expulsa|expulsar|kika|kikar) (o jogador|a conta|o char|o personagem|o nick|o)? ?([a-zA-Z0-9]+)
   ```

4. **Broadcast**:
   ```
   (anuncia|broadcast|avisa|notifica|manda mensagem|envia mensagem) (que|sobre|para todos|global|no server)? ?(.+)
   ```

## Integração com Componentes do Studio

O sistema de IA integra-se com outros componentes do WYD MODERN 2025 STUDIO:

1. **AdminPanel**: Fornece interface administrativa com comandos em linguagem natural
2. **WorldManagerPanel**: Permite gerenciamento de eventos e mundo através de linguagem natural
3. **PlayerManagerPanel**: Facilita ações relacionadas a jogadores via comandos naturais
4. **LiveUpdateManager**: Permite atualizações em tempo real através de comandos simples

## Feedback e Melhoria Contínua

O sistema inclui mecanismos para melhoria contínua:

- **Coleta de Feedback**: Registra interpretações incorretas ou falhas de reconhecimento
- **Análise de Padrões**: Identifica novos padrões de linguagem usados pelos administradores
- **Expansão de Vocabulário**: Adiciona automaticamente novos termos e variações ao banco de dados
- **Refinamento de Modelos**: Ajusta modelos de reconhecimento com base no uso real

## Próximas Etapas

O desenvolvimento futuro do sistema de IA inclui:

1. **Melhorias de NLP**: Aprimoramento dos algoritmos de processamento de linguagem natural
2. **Expansão de Domínios**: Adição de mais domínios específicos de terminologia
3. **Interfaces de Voz**: Adição de reconhecimento e síntese de voz
4. **Personalização Avançada**: Perfis de linguagem adaptados a diferentes tipos de usuários
5. **Análise Preditiva**: Antecipação de necessidades baseada em padrões históricos