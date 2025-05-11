# Integração de Linguagem Expandida - WYD MODERN 2025 STUDIO

## Visão Geral

Este documento descreve como o sistema de processamento de linguagem natural (NLP) do WYD MODERN 2025 STUDIO foi expandido para incorporar um conjunto mais amplo de termos, gírias e expressões específicas dos jogadores de WYD. Esta expansão permite que o sistema compreenda e processe com maior precisão a linguagem natural utilizada pela comunidade de jogadores.

## Estrutura do Sistema Expandido

O sistema expandido consiste em:

1. **Banco de Dados de Termos Expandido**: Arquivo JSON contendo categorias, termos, sinônimos, descrições e exemplos de uso.
2. **Carregador de Termos**: Componente responsável por carregar e integrar os termos ao processador de linguagem.
3. **Processamento de Intenção Aprimorado**: Reconhecimento e interpretação de intenções com base no vocabulário expandido.
4. **Exemplos de Uso**: Demonstração prática do sistema com os novos termos.

## Banco de Dados de Termos

O arquivo `expanded_language_terms.json` contém uma estrutura organizada de termos em categorias:

```json
{
  "terms": [
    {
      "category": "CATEGORIA",
      "terms": [
        {
          "standardForm": "Forma Padrão",
          "synonyms": ["Sinônimo1", "Sinônimo2", "Gíria1"],
          "description": "Descrição do termo",
          "usageExamples": [
            "Exemplo de uso 1",
            "Exemplo de uso 2"
          ]
        }
      ]
    }
  ]
}
```

### Categorias Disponíveis

O sistema reconhece as seguintes categorias de termos:

- **ITEM**: Equipamentos, itens consumíveis, materiais, etc.
- **CLASS**: Classes de personagens, evoluções e transcendências
- **SKILL**: Habilidades e técnicas dos personagens
- **LOCATION**: Locais, mapas e áreas do jogo
- **MOB**: Monstros, NPCs e criaturas
- **QUEST**: Missões e tarefas
- **ATTRIBUTE**: Atributos de personagem e itens
- **ENHANCEMENT**: Refinamentos, aprimoramentos, composições
- **EVENT**: Eventos especiais do jogo
- **ECONOMY**: Termos relacionados à economia e comércio
- **SOCIAL**: Interação social, guildas, grupos
- **COMMAND**: Comandos administrativos do jogo
- **PVP**: Combate entre jogadores
- **GENERAL_SLANG**: Gírias gerais do jogo

## Integração com o Sistema NLP

### Classe LanguageTermsLoader

Esta classe é responsável por carregar os termos do arquivo JSON e integrá-los ao processador de linguagem natural:

```cpp
// Criar instância do carregador
auto termsLoader = std::make_shared<LanguageTermsLoader>();

// Carregar termos para o processador
termsLoader->LoadTermsToProcessor(
    nlProcessor,                      // Processador de linguagem
    "./data/expanded_language_terms.json",  // Caminho do arquivo
    DisplayProgressCallback);          // Callback opcional para progresso
```

### Estrutura Interna

Internamente, os termos são convertidos em objetos `LanguageTerm` e adicionados ao `NaturalLanguageProcessor`:

```cpp
struct LanguageTerm {
    std::string standardForm;          // Forma oficial
    std::vector<std::string> synonyms; // Sinônimos e gírias
    std::string description;           // Descrição
    TermCategory category;             // Categoria
    std::vector<std::string> usageExamples; // Exemplos de uso
    std::map<std::string, std::string> metaData; // Metadados adicionais
    bool isActive;                     // Status de ativação
};
```

## Processo de Reconhecimento

Quando um texto é processado, o sistema:

1. **Identifica Termos**: Reconhece termos do WYD e suas variantes no texto
2. **Normaliza o Texto**: Substitui gírias/variantes por formas padrão para processamento consistente
3. **Reconhece Padrões**: Identifica padrões de comando ou consulta
4. **Extrai Intenção**: Determina a intenção do usuário (criar item, evento, consulta, etc.)
5. **Extrai Parâmetros**: Identifica parâmetros relevantes (nome do item, jogador, etc.)

## Exemplo de Uso no Código

```cpp
// Texto com gírias e termos específicos
std::string userInput = "Preciso de um set E4 +10 para meu FM que é zika do PvP";

// Normalizar o texto (identificar termos específicos)
std::string normalized = nlProcessor->NormalizeText(userInput);
// Resultado: "Preciso de um Equipamento de Evolução +10 para meu ForeMaster que é Jogador Habilidoso"

// Processar o texto normalizado
RecognizedIntent intent = nlProcessor->ProcessText(userInput);

// Acessar dados da intenção reconhecida
std::cout << "Intenção: " << intent.intentName << std::endl;
std::cout << "Confiança: " << intent.confidence << std::endl;

// Mostrar termos reconhecidos
for (const auto& term : intent.recognizedTerms) {
    std::cout << "Termo: " << term << std::endl;
}

// Acessar parâmetros extraídos
if (intent.HasParam("itemType")) {
    std::cout << "Tipo de item: " << intent.GetParam("itemType") << std::endl;
}
```

## Adicionando Novos Termos

Para adicionar novos termos ao sistema, você pode:

### 1. Atualizar o Arquivo JSON

Adicione novos termos ao arquivo `expanded_language_terms.json` seguindo a estrutura existente.

### 2. Adicionar Programaticamente

```cpp
// Criar um novo termo
LanguageTerm newTerm;
newTerm.standardForm = "Nome do Item";
newTerm.synonyms = {"Apelido1", "Gíria1", "Variação1"};
newTerm.description = "Descrição detalhada do item";
newTerm.category = TermCategory::ITEM;
newTerm.usageExamples = {"Exemplo de uso em frase"};

// Adicionar ao processador
nlProcessor->AddTerm(newTerm);
```

## Estatísticas e Análise

O sistema mantém estatísticas de uso que podem ser exportadas:

```cpp
// Exportar estatísticas de uso
nlProcessor->ExportUsageStatistics("./nlp_stats.json");
```

As estatísticas incluem:
- Contagem de termos mais usados
- Padrões mais frequentemente reconhecidos
- Taxa de sucesso de reconhecimento
- Intenções mais comuns

## Casos de Uso Avançados

### Processamento Contextual

O sistema pode usar contexto de conversas anteriores:

```cpp
// Primeira interação
RecognizedIntent intent1 = nlProcessor->ProcessText("Quero um set E4 de FM", "user123");

// Segunda interação usando contexto (referência "ele")
RecognizedIntent intent2 = nlProcessor->ProcessText("Quanto custa ele?", "user123");
// O sistema entende que "ele" se refere ao "set E4" mencionado anteriormente
```

### Sugestões Contextuais

```cpp
// Obter sugestões com base no input atual
std::vector<std::string> suggestions = nlProcessor->GenerateContextualSuggestions(
    "user123",       // ID de contexto
    "Quero melhorar", // Input atual
    5                 // Número máximo de sugestões
);
```

## Exemplos de Termos Reconhecidos

Alguns exemplos notáveis da expansão incluem:

1. **Equipment Evolution (E2/E3/E4)**: "Onde consigo set E4 para meu personagem?"
2. **Automatic Systems (AutoBuff/AutoHP)**: "Como configurar AutoHP para 50%?"
3. **Server Types (Rate x1000)**: "Este servidor é rate x1000?"
4. **PvP Terms (X1, BRK mode)**: "Vamos fazer X1 no modo BRK!"
5. **Player Types (Zika do PvP, Bot)**: "Cuidado, aquele cara é zika do PvP."
6. **Farm Methods (Upar dormindo)**: "Eles estão upando dormindo nesse mapa."
7. **Events (Boss Time, Evento ON/OFF)**: "Quando é o boss time hoje?"

## Conclusão

A expansão do sistema de processamento de linguagem natural com termos e gírias específicas da comunidade WYD representa um avanço significativo na capacidade do WYD MODERN 2025 STUDIO de entender e responder naturalmente à comunicação dos jogadores. Isso permite uma administração mais intuitiva e uma experiência mais fluida para todos os usuários do sistema.

---

**Nota**: Para contribuir com novos termos e expressões, por favor submeta uma solicitação ao repositório do projeto ou use a ferramenta de feedback integrada ao WYDStudio.