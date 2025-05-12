# Sistema de IA do WYDStudio

## Visão Geral

O sistema de IA do WYDStudio é projetado para fornecer assistência inteligente na administração de servidores WYD. O sistema utiliza processamento de linguagem natural, análise de contexto e aprendizado de máquina para melhorar a experiência de administração.

## Componentes Principais

### 1. Base de Conhecimento (AIKnowledgeBase)

A base de conhecimento armazena e gerencia padrões de comandos, estados do servidor e ações administrativas. Principais características:

- Armazenamento em SQLite para persistência
- Cache em memória para acesso rápido
- Sistema de pontuação de confiança
- Aprendizado contínuo baseado em feedback

### 2. Tokenizador de Comandos (CommandTokenizer)

Responsável por analisar e estruturar comandos em linguagem natural. Recursos:

- Reconhecimento de padrões específicos do WYD
- Extração de parâmetros
- Análise de contexto de comandos
- Suporte a múltiplos formatos de entrada

### 3. Analisador de Contexto (ContextAnalyzer)

Avalia o contexto de execução dos comandos para melhorar a precisão e segurança. Analisa:

- Horário do dia
- Estado do servidor
- Histórico do usuário
- Complexidade dos comandos

## Uso do Sistema

### Inicialização

```cpp
auto knowledgeBase = std::make_unique<AIKnowledgeBase>();
auto tokenizer = std::make_unique<CommandTokenizer>();
auto analyzer = std::make_unique<ContextAnalyzer>();
```

### Processamento de Comandos

```cpp
// Tokenização
QString command = "/give @player rare_item +7";
auto tokens = tokenizer->tokenize(command);

// Extração de parâmetros
auto params = tokenizer->extractParameters(tokens);

// Análise de contexto
AIKnowledgeBase::CommandContext context;
context.serverState = "normal";
context.userRole = "admin";
context.timeOfDay = QDateTime::currentDateTime();

float contextScore = analyzer->analyzeContext(context);
float successPrediction = knowledgeBase->predictSuccess(command, context);

// Execução e aprendizado
if (successPrediction > 0.7f) {
    // Executar comando
    knowledgeBase->learnFromSuccess(command, context);
} else {
    // Solicitar confirmação
}
```

## Padrões de Comando Suportados

### Comandos Básicos
- `/comando [parâmetros]`
- `--flag` ou `-f`
- `@menção`
- `"texto entre aspas"`

### Padrões Específicos do WYD
- Items: `set/item/equip [nome]`
- Melhorias: `+N`
- Items Especiais: `anc/arc/cel/div [nome]`
- Ações: `ban/kick/mute [alvo]`
- Eventos: `event/drop/rate [tipo]`

## Análise de Contexto

### Fatores Considerados
1. **Temporal (20%)**
   - Horário do dia
   - Dia da semana
   - Eventos ativos

2. **Carga do Servidor (30%)**
   - Uso de CPU
   - Uso de memória
   - Jogadores online
   - Latência

3. **Histórico do Usuário (30%)**
   - Nível de permissão
   - Comandos anteriores
   - Taxa de sucesso

4. **Complexidade do Comando (20%)**
   - Número de parâmetros
   - Tipo de ação
   - Impacto potencial

## Aprendizado e Otimização

O sistema aprende continuamente através de:

1. **Feedback de Execução**
   - Sucesso/falha de comandos
   - Padrões de uso
   - Contextos favoráveis

2. **Análise de Padrões**
   - Combinação de comandos frequentes
   - Sequências de ações
   - Horários de maior eficácia

3. **Otimização Automática**
   - Ajuste de pesos de contexto
   - Limpeza de padrões obsoletos
   - Fusão de padrões similares

## Segurança

O sistema implementa várias camadas de segurança:

1. **Análise de Risco**
   - Avaliação de impacto de comandos
   - Verificação de permissões
   - Análise de contexto

2. **Prevenção de Erros**
   - Validação de parâmetros
   - Confirmação de ações críticas
   - Limites de execução

3. **Auditoria**
   - Registro detalhado de ações
   - Rastreamento de mudanças
   - Histórico de aprendizado

## Desenvolvimento

### Compilação
```bash
mkdir build && cd build
cmake ..
make
```

### Testes
```bash
ctest -V
```

### Dependências
- Qt 5.15+
- nlohmann::json
- SQLite 3
- GTest (para testes)
- spdlog

## Contribuição

1. Fork o repositório
2. Crie uma branch para sua feature
3. Adicione testes
4. Envie um pull request

## Licença

Este projeto é parte do WYDStudio e está sob a mesma licença do projeto principal. 
