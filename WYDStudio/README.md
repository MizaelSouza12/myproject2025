# WYD MODERN 2025 STUDIO

Um ambiente de desenvolvimento e administração avançado para o jogo WYD (With Your Destiny), com recursos de processamento de linguagem natural específicos para a terminologia do jogo.

## Visão Geral

O WYD MODERN 2025 STUDIO é uma ferramenta administrativa para o jogo WYD que permite:

- Processamento avançado de comandos em linguagem natural
- Reconhecimento de termos e gírias específicos dos jogadores
- Interpretação contextual de comandos administrativos
- Interface de usuário intuitiva para gerenciamento do jogo
- Sistema de conhecimento específico sobre o jogo

## Estrutura do Projeto

```
WYDStudio/
├── bin/                # Executáveis compilados
├── build/              # Arquivos de compilação temporários
├── data/               # Dados e recursos
│   └── expanded_language_terms.json   # Banco de termos expandido
├── docs/               # Documentação
│   └── EXPANDED_LANGUAGE_INTEGRATION.md  # Documentação da integração de linguagem
├── examples/           # Exemplos de uso
│   └── EnhancedLanguageExample.cpp   # Exemplo de processamento de linguagem
├── include/            # Arquivos de cabeçalho
│   ├── AdvancedAdminAgent.h          # Agente administrativo avançado
│   ├── GameKnowledgeBase.h           # Base de conhecimento genérica de jogos
│   ├── LanguageTermsLoader.h         # Carregador de termos de linguagem
│   ├── NaturalLanguageProcessor.h    # Processador de linguagem natural
│   └── WYDSpecificKnowledge.h        # Conhecimento específico do WYD
├── src/                # Código fonte
├── ui_test/            # Testes de interface
│   ├── Makefile           # Makefile para testes de UI
│   ├── UITest.h           # Definições de classes de UI
│   └── UITestApp.cpp      # Aplicativo de teste de UI
├── Makefile            # Makefile principal
├── compile.sh          # Script de compilação
├── verify_functionality.sh  # Script de verificação de funcionalidade
└── README.md           # Este arquivo
```

## Recursos Principais

### 1. Processamento de Linguagem Natural Avançado

O sistema inclui um processador de linguagem natural que pode entender comandos expressos em linguagem natural, incluindo gírias e termos específicos do jogo WYD.

```cpp
// Exemplo de uso:
auto processor = std::make_shared<NaturalLanguageProcessor>();
processor->Initialize();

// Processar um comando em linguagem natural
RecognizedIntent intent = processor->ProcessText("Criar set E4 +10 para FM");
```

### 2. Banco de Termos Expandido

Um extenso banco de termos específicos do WYD permite reconhecer e interpretar corretamente a linguagem utilizada pelos jogadores, incluindo:

- Termos de equipamentos (E2, E3, E4, FoF, FoW)
- Classes de personagens (FM, BM, TK, HT)
- Sistemas automáticos (AutoBuff, AutoHP)
- Eventos (Boss Time, ON/OFF)
- Gírias (zika do PvP, upar dormindo, telar, etc.)

### 3. Agente Administrativo Avançado

Um agente administrativo inteligente que pode:

- Interpretar comandos administrativos em linguagem natural
- Executar ações no jogo com base nos comandos
- Verificar a validade de ações antes de executá-las
- Fornecer sugestões contextuais

```cpp
// Exemplo de uso:
auto agent = std::make_shared<AdvancedAdminAgent>(processor, gameKnowledge);
agent->Initialize();

// Processar um comando administrativo
std::string response = agent->ProcessQuery("Banir jogador XYZ por uso de hack");
```

### 4. Base de Conhecimento do Jogo

Uma base de conhecimento especializada que contém informações sobre:

- Itens, suas propriedades e requisitos
- Jogadores e suas características
- Mapas, localizações e propriedades
- Eventos do jogo e regras

### 5. Interface de Usuário para Testes

Componentes de UI para testar o sistema, incluindo:

- Janelas, painéis, botões
- Campos de texto, rótulos e listas
- Sistema de manipulação de eventos

## Compilação e Uso

### Requisitos

- Compilador C++ com suporte a C++17
- GNU Make
- Bibliotecas padrão do C++

### Compilação

```bash
# Compilação básica
./compile.sh

# Compilação com opções
./compile.sh --debug --test
```

Opções disponíveis:
- `--debug`: Compila em modo de depuração
- `--no-examples`: Não compila exemplos
- `--test`: Executa testes após compilação
- `--ui-test`: Executa testes de UI após compilação

### Verificação de Funcionalidade

```bash
./verify_functionality.sh
```

Este script verifica se todos os componentes do sistema estão funcionando corretamente.

## Testes de Interface

```bash
cd ui_test
make
./UITestApp
```

## Integração de Novos Termos

Para adicionar novos termos ao sistema, edite o arquivo `data/expanded_language_terms.json` seguindo o formato existente. Consulte a documentação em `docs/EXPANDED_LANGUAGE_INTEGRATION.md` para mais detalhes.

## Exemplos

### Exemplo de Processamento de Linguagem

```bash
bin/EnhancedLanguageExample
```

Este exemplo demonstra o processamento de comandos em linguagem natural com termos específicos do WYD.

## Licença

Este projeto é parte do ambiente WYD MODERN 2025 STUDIO e é distribuído sob os termos de sua licença proprietária.

## Contato

Para mais informações, entre em contato com a equipe de desenvolvimento do WYD MODERN 2025 STUDIO.