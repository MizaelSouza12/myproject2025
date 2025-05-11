# Sistema Avançado de Processamento de Linguagem Natural e Agentes IA

## Visão Geral

Este sistema implementa capacidades avançadas de processamento de linguagem natural (NLP) específicas para o universo do jogo WYD (With Your Destiny), permitindo que comandos administrativos e consultas sobre o jogo sejam processados em linguagem natural, considerando terminologia, gírias e padrões linguísticos específicos dos jogadores.

O sistema é composto por dois componentes principais:

1. **Processador de Linguagem Natural**: Reconhece termos específicos do WYD, padrões de linguagem, e extrai intenções e parâmetros de comandos em linguagem natural.

2. **Agente de Administração Avançado**: Utiliza o processador de linguagem natural para interpretar comandos e executar ações administrativas no jogo.

## Principais Recursos

### Processamento de Linguagem Natural

- **Reconhecimento de Termos Específicos**: Identifica itens, classes, habilidades, locais, etc. específicos do WYD, incluindo gírias e variações.
- **Normalização de Termos**: Converte diferentes formas de referência (gírias, abreviações) para formas padrão.
- **Detecção de Padrões de Comando**: Reconhece padrões de linguagem para comandos comuns como criação de itens, eventos de drop, gerenciamento de jogadores, etc.
- **Correspondência Fuzzy**: Identifica termos mesmo com pequenos erros de digitação ou variações.
- **Memória Contextual**: Mantém contexto da conversa para entender referências anafóricas ("ele", "ela", "isso").
- **Aprendizado Contínuo**: Melhora constantemente com base nas interações e correções.

### Agente de Administração

- **Processamento de Consultas**: Responde a perguntas sobre o jogo usando a base de conhecimento.
- **Execução de Comandos**: Traduz consultas em linguagem natural para ações concretas no sistema.
- **Gerenciamento de Segurança**: Controla autorizações e permissões para comandos administrativos.
- **Análise Proativa**: Monitora o estado do jogo e sugere ações administrativas.
- **Diagnóstico de Servidor**: Realiza verificações de saúde e performance do servidor.
- **Agendamento de Tarefas**: Permite agendar tarefas administrativas para execução futura.

## Estrutura do Sistema

```
NaturalLanguageProcessor
  ├── Reconhecimento de Termos (TermRecognition)
  ├── Extração de Intenções (IntentExtraction)
  ├── Geração de Sugestões (SuggestionEngine)
  └── Memória Contextual (ContextMemory)

AdvancedAdminAgent
  ├── Processamento de Consultas (QueryProcessor)
  ├── Execução de Operações (OperationExecutor)
  ├── Gerenciamento de Segurança (SecurityManager)
  ├── Monitoramento de Estado (GameStateMonitor)
  └── Aprendizado de Agentes (AgentTraining)
```

## Como Usar

### Compilação

Para compilar o sistema, execute o script de compilação:

```bash
./compile.sh
```

Isso criará os executáveis na pasta `bin/`.

### Exemplo de Uso

Execute o exemplo de agente NLP avançado:

```bash
./bin/AdvancedNLPAgentExample
```

Este exemplo demonstra:
- Reconhecimento de termos e padrões do WYD
- Memória contextual para conversas
- Interação com o agente administrativo através de linguagem natural

### Comandos de Exemplo

1. **Criar Item**:
   - "Criar um set D +11 de FM para o jogador TestUser"
   - "Gerar uma arma celestial +10 para BM"
   - "Adicionar um set Sapphira para o jogador MasterKnight"

2. **Eventos de Drop**:
   - "Iniciar evento de drop de poeira de lac em trolls"
   - "Começar um drop rate de armadura D em noatun"
   - "Criar um evento para pedras em masmorras"

3. **Gerenciar Jogador**:
   - "Banir o jogador HackerXYZ por 3 dias"
   - "Teleportar o jogador TestUser para Armia"
   - "Silenciar o jogador SpamBot por 1 hora"

4. **Broadcast**:
   - "Anunciar que haverá manutenção às 18h"
   - "Enviar mensagem para todos sobre o evento de invasão"
   - "Broadcast sobre o novo update"

5. **Verificação de Estado**:
   - "Verificar status do servidor"
   - "Executar diagnóstico do servidor"
   - "Analisar estado atual do jogo"

## Extensão do Sistema

### Adicionar Novos Termos

Para adicionar novos termos ao sistema de reconhecimento:

```cpp
// Criar e adicionar um novo termo
LanguageTerm newTerm = WYDLanguageFactory::CreateItemTerm(
    "Nova Armadura", // Forma padrão
    {"Armadura Nova", "Novíssima", "ArmaNew"}, // Sinônimos
    "Nova armadura lendária nível 500" // Descrição
);
nlProcessor->AddTerm(newTerm);
```

### Adicionar Novos Padrões de Comando

Para adicionar novos padrões de reconhecimento:

```cpp
// Criar e adicionar um novo padrão de comando
LanguagePattern newPattern = WYDLanguageFactory::CreateCommandPattern(
    "new_event_pattern", // ID do padrão
    "(cria|criar|fazer|faz) (evento|festa|celebração) (de|para) ([a-z]+)", // Regex
    "create_special_event", // Nome da intenção
    {{1, "action"}, {4, "eventType"}}, // Grupos de captura
    0.8f // Prioridade
);
nlProcessor->AddPattern(newPattern);
```

### Registrar Manipuladores de Intenção

Para associar ações a intenções reconhecidas:

```cpp
// Registrar um handler para a intenção
nlProcessor->RegisterIntentHandler({
    "create_special_event", // Nome da intenção
    [](const RecognizedIntent& intent) -> std::string {
        // Lógica para criar evento especial
        std::string eventType = intent.GetParam("eventType");
        return "Evento especial '" + eventType + "' criado com sucesso!";
    },
    true, // Requer parâmetros
    false, // Não é restrito apenas a admin
    {"eventType"} // Parâmetros necessários
});
```

## Arquitetura Técnica

### Componentes do NLP

- **LanguageTerm**: Define um termo específico do WYD e suas variações
- **LanguagePattern**: Define um padrão de expressão a ser reconhecido
- **RecognizedIntent**: Resultado da análise de uma entrada de texto
- **IntentAction**: Associa uma intenção a uma ação concreta
- **NLPMemoryItem**: Item de memória contextual para referência entre mensagens

### Componentes do Agente Admin

- **AdminOperation**: Define uma operação administrativa (criar item, banir jogador, etc.)
- **AdminActionHistory**: Registro histórico das ações administrativas executadas
- **ScheduledTask**: Tarefa administrativa agendada para execução futura
- **ContextMemory**: Memória de contexto específica para o agente administrativo

## Recursos Futuros

- **Análise de Sentimento**: Detectar emoções e sentimentos em mensagens de jogadores
- **Correção Ortográfica Avançada**: Corrigir erros de ortografia em consultas
- **Interfaces de Voz**: Adicionar entrada e saída de voz para interação por fala
- **Chat Multi-turno**: Melhorar capacidades de diálogo para conversas complexas
- **Integrações com APIs Externas**: Conectar com WikiFandom, fóruns e outras fontes 

## Notas Técnicas

- Implementado em C++17 moderno com padrões de design atuais
- Thread-safe para uso em ambiente de servidor com múltiplos usuários
- Baixo consumo de memória com índices eficientes para termos e padrões
- Capacidade de aprendizado offline sem necessidade de serviços externos
- 100% funcional sem conexão à internet, todos os dados armazenados localmente